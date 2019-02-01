import numpy as np
import pandas as pd
from sklearn.decomposition import PCA

from common import x_cols, y_cols, z_cols, N_STATIONS

pca_x_cols = ['PCA_X[%i]' % i for i in range(N_STATIONS)]
pca_y_cols = ['PCA_Y[%i]' % i for i in range(N_STATIONS)]
pca_z_cols = ['PCA_Z[%i]' % i for i in range(N_STATIONS)]
pca_xyz_cols = pca_x_cols + pca_y_cols + pca_z_cols

def fill_na(data):
    mask = data.isna()
    means = data.mean(skipna=True)
    data.fillna(means, inplace=True)
    return mask

def restore_na(data, mask):
    data.mask(mask, other=np.NaN, inplace=True)
    
def get_nth_detector_coords(i):
    return [x_cols[i], y_cols[i], z_cols[i]]

def get_nth_detector_coords_pca(i):
    return [pca_x_cols[i], pca_y_cols[i], pca_z_cols[i]]
    
def pca_fit(data):
    cols = get_nth_detector_coords(0)
    data = data[cols].copy()
    
    fill_na(data)
    pca_model = PCA(n_components=3)
    pca_model.fit(data)
    return pca_model

def pca_transform(pca_model, data, features):
    for i in range(4):
        cols = get_nth_detector_coords(i)
        new_cols = get_nth_detector_coords_pca(i)
        data_detector = data.loc[:, cols]
        
        mask = fill_na(data_detector)
        transformed_data = pca_model.transform(data_detector.values)
        restore_na(data_detector, mask)
        
        for j in range(3):
            data.loc[:, new_cols[j]] = transformed_data[:, j]

    features += pca_xyz_cols
    return data
