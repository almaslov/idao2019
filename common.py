N_STATIONS = 4

# train cols
unused_train_cols = ['particle_type', 'kinWeight', 'sWeight']
train_cols = ['label', 'weight']
ALL_TRAIN_COLS = train_cols + unused_train_cols

# original cols
x_cols = ['MatchedHit_X[%i]' % i for i in range(N_STATIONS)]
y_cols = ['MatchedHit_Y[%i]' % i for i in range(N_STATIONS)]
z_cols = ['MatchedHit_Z[%i]' % i for i in range(N_STATIONS)]
xy_cols = x_cols + y_cols
xyz_cols = x_cols + y_cols + z_cols
dx_cols = ['MatchedHit_DX[%i]' % i for i in range(N_STATIONS)]
dy_cols = ['MatchedHit_DY[%i]' % i for i in range(N_STATIONS)]
dz_cols = ['MatchedHit_DZ[%i]' % i for i in range(N_STATIONS)]
dxyz_cols = dx_cols + dy_cols + dz_cols

ex_cols = ['Lextra_X[%i]' % i for i in range(N_STATIONS)]
ey_cols = ['Lextra_Y[%i]' % i for i in range(N_STATIONS)]
exy_cols = ex_cols + ey_cols
edx_cols = ['Mextra_DX2[%i]' % i for i in range(N_STATIONS)]
edy_cols = ['Mextra_DY2[%i]' % i for i in range(N_STATIONS)]
edxy_cols = edx_cols + edy_cols

t_cols = ['MatchedHit_T[%i]' % i for i in range(N_STATIONS)]
dt_cols = ['MatchedHit_DT[%i]' % i for i in range(N_STATIONS)]

hit_type_cols = ['MatchedHit_TYPE[%i]' % i for i in range(N_STATIONS)]
mom_cols = ['P', 'PT']
hit_stats_cols = ['FOI_hits_N', 'NShared', 'ndof']

ncl_cols = ['ncl[%i]' % i for i in range(N_STATIONS)]
avg_cs_cols = ['avg_cs[%i]' % i for i in range(N_STATIONS)]

# foi cols
foi_xyz_cols = ["FOI_hits_X", "FOI_hits_Y", "FOI_hits_Z"]
foi_dxyz_cols = ["FOI_hits_DX", "FOI_hits_DY", "FOI_hits_DZ"]
foi_ts_cols = ["FOI_hits_T", "FOI_hits_DT", "FOI_hits_S"]
foi_cols = foi_xyz_cols + foi_dxyz_cols + foi_ts_cols

SIMPLE_FEATURE_COLS = xyz_cols + dxyz_cols + exy_cols + edxy_cols + t_cols + dt_cols + hit_type_cols + mom_cols + hit_stats_cols + ncl_cols + avg_cs_cols
ARR_FEATURE_COLS = foi_cols