import numpy as np
from common import xy_cols, xyz_cols, ex_cols, ey_cols, exy_cols, dx_cols, dy_cols, edxy_cols, z_cols, t_cols, mom_cols, N_STATIONS
from pipeline import split_classes

err_cols = ['ErrMSE', 'DLL'] #, 'Chi2Quantile']

nerr_x_cols = ['NErr_X[%i]' % i for i in range(N_STATIONS)]
nerr_y_cols = ['NErr_Y[%i]' % i for i in range(N_STATIONS)]
nerr_xy_cols = nerr_x_cols + nerr_y_cols

err_x_cols = ['Err_X[%i]' % i for i in range(N_STATIONS)]
err_y_cols = ['Err_Y[%i]' % i for i in range(N_STATIONS)]
err_z_cols = ['Err_Z[%i]' % i for i in range(N_STATIONS)]
err_xy_cols = err_x_cols + err_y_cols
err_xyz_cols = err_xy_cols + err_z_cols
ez = np.array([15270., 16470., 17670., 18870.])


def add_mse(data, features):
    dxy = (data.loc[:, xy_cols].values - data.loc[:, exy_cols].values) / data.loc[:, dx_cols + dy_cols].values / 2.
    D = np.nanmean(dxy**2, axis=1)
    
    data.loc[:, err_cols[0]] = D
    features += [err_cols[0]]
    return data

def add_normed_err(data, features):
    dxy = data.loc[:, xy_cols].values - data.loc[:, exy_cols].values
    normed_errors = dxy / np.sqrt(data.loc[:, edxy_cols].values)
    
    for i in range(4):
        data.loc[:, nerr_x_cols[i]] = normed_errors[:, i]
        data.loc[:, nerr_y_cols[i]] = normed_errors[:, i + 4]
    
    features += nerr_xy_cols
    return data

def add_errs(data, features):
    for err_col, e_col, col in zip (err_xy_cols, exy_cols, xy_cols):
        data.loc[:, err_col] = data[e_col].values - data[col].values
        
    for i in range(4):
        data.loc[:, err_z_cols[i]] = ez[i] - data[z_cols[i]].values
    
    features += err_xyz_cols

def create_distr(data):
    dts = [dt.loc[:, err_cols[0]] for dt in split_classes(data)]
    l, r = np.min(data[err_cols[0]]) - 1e-5, np.max(data[err_cols[0]]) + 1e-5
    bins = np.concatenate((
        np.arange(l, 1, .02),
        np.arange(1, 3, .04),
        np.arange(3, 10, .1),
        np.arange(10, 16, .4),
        np.arange(16, 34, 1.),
        np.arange(34, 66, 2),
        np.arange(66, 120, 5.),
        np.linspace(120, r, 3),
    ))
    pdfs = []
    
    for i in range(2):
        pdf, _ = np.histogram(dts[i], bins=bins)
        pdfs.append(pdf)
    
    cdfs = [np.cumsum(pdf) for pdf in pdfs]
    
    return cdfs, pdfs, bins

# выбираем опцию, как считать DLL - либо на основе pdf, либо на основе cdf
def get_dll_pdf(x, pdfs, cdfs, bins):
    def get_probs_pdf(pdf, x):
        indices = np.digitize(x, bins) - 1
        wbin = (bins[indices + 1] - bins[indices]) / (np.max(bins) - np.min(bins))
        prob = pdf[indices] / pdf.sum()
        return prob * wbin

    probs = [get_probs_pdf(pdf, x) for pdf in pdfs]
    DLL = np.log(probs[1]) - np.log(probs[0])
    return DLL

def get_dll_cdf(x, pdfs, cdfs, bins):
    def get_probs_cdf(cdf, x):
        indices = np.digitize(x, bins) - 1
        wbin = (bins[indices + 1] - bins[0]) / (np.max(bins) - np.min(bins))
        prob = cdf[indices] / cdf[-1]
        return prob * wbin
    probs = [get_probs_cdf(cdf, x) for cdf in cdfs]
    DLL = np.log(probs[1]) - np.log(probs[0])
    return DLL

vm_cols = ['V', 'VT', 'M', 'MT']

def add_velocity(data, features):
    def get_layer_coords(data, cols, i):
        return data[[cols[i], cols[i+4], cols[i+8]]].values
    def get_elayer_coords(data, i):
        exy = data.loc[:, [ex_cols[i], ey_cols[i]]].values
        ez_ = np.tile(ez[i], exy.shape[0]).reshape((-1, 1))
        return np.hstack((exy, ez_))
    def dot(x, y):
        return np.sum(x * y, axis=1, dtype=np.float32)
    def norm(x):
        return np.sqrt(dot(x, x))
    def get_zero_point(data):
        layers = [get_elayer_coords(data, i) for i in range(2)]
        r = layers[1] - layers[0]
        r = r / norm(r)[:, np.newaxis]
        p = get_elayer_coords(data, 0)
        alpha = - p[:, 2] / r[:, 2]
        
        xs = p[:, 0] + alpha * r[:, 0]
        ys = p[:, 1] + alpha * r[:, 1]
        zs = np.tile(0, len(xs))
        return np.vstack((xs, ys, zs)).T
    
    # radius-vector r_i = p_i - p_0: S x N x 3; 
    r = np.array([get_layer_coords(data, xyz_cols, i) for i in range(4)]) - get_zero_point(data)
    
    # time: S x N
    t = data.loc[:, t_cols].values.T
    
    # average velocity avg(r / t): N x 3    
    v_avg = np.nanmean(r / t[:, :, np.newaxis], axis=0)
    # average speed |v|: N
    speed = norm(v_avg)
    # transverse speed |v_xy|: N
    speed_tr = norm(v_avg * np.array([1., 1., 0.]))
    
    # momentum: N
    p = data.loc[:, mom_cols[0]].values
    # transverse momentum: N
    p_tr = data.loc[:, mom_cols[1]].values
    
    # mass: N
    m = p / speed
    # transverse mass: N
    m_tr = p_tr / speed_tr

    results = [speed, speed_tr, m, m_tr]
    for col, res in zip(vm_cols, results):
        data.loc[:, col] = res
    features += vm_cols
    return data
