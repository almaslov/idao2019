import numpy as np
from common import xy_cols, exy_cols, dx_cols, dy_cols, edxy_cols, N_STATIONS
from pipeline import split_classes

err_cols = ['ErrMSE', 'DLL'] #, 'Chi2Quantile']

nerr_x_cols = ['NErr_X[%i]' % i for i in range(N_STATIONS)]
nerr_y_cols = ['NErr_Y[%i]' % i for i in range(N_STATIONS)]
nerr_xy_cols = nerr_x_cols + nerr_y_cols


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

# NOT WORKING BECAUSE OF DTS SHOULD BE ALREADY CALCULATED FOR TEST
def add_dll(data, features):
    # fix here
    dts = [dt.loc[:, err_cols[0]] for dt in split_classes(data)]
    min_len = min(map(len, dts))
    nbins = int(round(np.sqrt(min_len) / np.pi))
    
    pdfs = []
    binses = []
    for i in range(2):
        pdf, bins = np.histogram(dts[i], bins=nbins)
        pdfs.append(pdf)
        binses.append(bins)

    def get_probs(bins, pdf, x):
        indices = np.digitize(x, bins) - 1
        indices = np.clip(indices, 0, len(pdf) - 1)
        return pdf[indices] / pdf.sum()

    def get_dll(x):
        probs = [get_probs(bins, pdf, x) for bins, pdf in zip(binses, pdfs)]
        probs = [np.clip(prob, 0.00000001, 1.) for prob in probs]
        DLL = np.log(probs[1]) - np.log(probs[0])
        return DLL

    data[err_cols[1]] = get_dll(data.loc[:, err_cols[0]])
    features += err_cols[1:2]
    return data