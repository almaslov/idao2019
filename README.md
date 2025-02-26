# IDAO 2019

## Links

- [Contest link](https://official.contest.yandex.ru/contest/10569/enter/)
- [Feature clarification](https://docs.google.com/document/d/1ur3EdTo49PCYtbwN35IXVGV-0dR9X0U8TH3dZ4pT8JM/edit)
- [The Muon Identification Procedure](https://core.ac.uk/download/pdf/44230824.pdf)
- [Muon system explained](https://cds.cern.ch/record/2063310/files/CERN-THESIS-2015-181.pdf)
- [isMuonTight+BDT](https://indico.cern.ch/event/491582/contributions/1168914/attachments/1236304/1815447/LHCC_Cogoni_v4.pdf)
- [fitting with energy loss and multiple scattering](https://www.nikhef.nl/~wouterh/topicallectures/TrackingAndVertexing/part4.pdf)
- [MULTIPLE SCATTERING ERROR PROPAGATION IN PARTICLE TRACK RECONSTRUCTION](https://arxiv.org/pdf/hep-ex/9406006.pdf)
- [The Performance of the LHCb Muon Identification Procedure](http://cds.cern.ch/record/1093941/files/lhcb-2007-145.pdf)
- [Selection and processing of calibration samples to measure the particle identification performance of the LHCb experiment in Run 2](https://arxiv.org/pdf/1803.00824.pdf)

## FAQ

- `data/` - folder for input train/test data.
- `out/` - folder for output results.
- data file extensions're ignored.
- track #1 notebooks are prefixed w/ 00-49
- track #2 notebooks are prefixed w/ 50-..

## Ideas

- [x] angle change between detectors. Good
- [x] pca fitted on 1st detector, applied to the all detector MatchedHits. Mediocre or bad
- [ ] read in parallel
- [x] normalize classes. Mediocre
- [ ] ensembles
- [x] IsMuon. Data already filtered by it, so it adds no info

## Experiments

### 100k rand from 400k (30588/369412) n_estimators=120

| \    | coord    | e_m      | pca      | cos      | cos,e_m  | pca,e_m  | cos,pca  | cos,pca,e_m |
| ---- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | ----------- |
| mean | 0.522238 | 0.522174 | 0.476468 | 0.651596 | 0.594934 | 0.508300 | 0.615967 | 0.604534    |
| std  | 0.089591 | 0.063560 | 0.068526 | 0.126795 | 0.074542 | 0.030846 | 0.114739 | 0.072358    |
| time | 17 s     | 18.1 s   | 18.5 s   | 23.3 s   | 22.9 s   | 19 s     | 23.8 s   | 25 s        |

### 100k 50/50 from 400k (30588/369412) n_estimators=120

| \    | coord    | e_m      | pca      | cos      | cos,e_m  | pca,e_m  | cos,pca  | cos,pca,e_m |
| ---- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | ----------- |
| mean | 0.496092 | 0.478049 | 0.469875 | 0.653774 | 0.666584 | 0.505335 | 0.670746 | 0.640206    |
| std  | 0.026036 | 0.028816 | 0.041083 | 0.032817 | 0.028323 | 0.025181 | 0.037231 | 0.044023    |
| time | 18.4 s   | 16.6 s   | 20.5 s   | 23.2 s   | 23.6 s   | 18.7 s   | 28.5 s   | 24.3 s      |

### 100k 30/70 from 400k (30588/369412) n_estimators=120

| \    | coord    | e_m      | pca      | cos      | cos,e_m  | pca,e_m  | cos,pca  | cos,pca,e_m |
| ---- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | ----------- |
| mean | 0.510292 | -------- | -------- | -------- | -------- | -------- | 0.618931 | -           |
| std  | 0.052114 | -        | -        | -        | -        | -        | 0.053795 | -           |
| time | 16.5 s   | -        | -        | -        | -        | -        | 25.8 s   | -           |

### 100k even from 400k (30588/369412) n_estimators=60, n_splits=3

0=coord,em

| \    | 0        | cos      | +imt     | +mom     | cos,-c   | cos,-c,mom | cos,-c,ht | cos,mom,ht | cos,mom,ht,imt | prev-c   |
| ---- | -------- | -------- | -------- | -------- | -------- | ---------- | --------- | ---------- | -------------- | -------- |
| mean | 0.423082 | 0.610361 | 0.588911 | 0.619311 | 0.507526 | 0.574089   | 0.538018  | 0.616694   | 0.613318       | 0.601273 |
| std  | 0.070076 | 0.008110 | 0.025762 | 0.027870 | 0.019386 | 0.008427   | 0.020904  | 0.014155   | 0.019909       | 0.016490 |
| time | 4.86 s   | 6.8 s    | 7.27 s   | 8.43 s   | 3.73 s   | 4.85 s     | 4.35 s    | 8.51 s     | 8.63 s         | 5.7 s    |

Good ones:

- xyz_cols + mom_cols + hit_type_cols; coses, mse, normed_err

### Cleaning by DAngle

df_scores = cross_validate(train, n_estimators=60, n_splits=3, n_rows=100000, transformer_cls=DataTransformer)

| what  | mean     | std      |
| ----- | -------- | -------- |
| 0=xyz | 0.451799 | 0.054607 |
| + cos | 0.588528 | 0.015640 |

## Submissions

| descr                                           | id       | score   |
| ----------------------------------------------- | -------- | ------- |
| baseline                                        | 19064098 | 4173.63 |
| e_m                                             | 19064099 | 4277.98 |
| 25/75                                           | 19064110 | 4083.36 |
| 50/50                                           | 19064113 | 4186.43 |
| cos                                             | 19064118 | 5400.8  |
| pca                                             | 19064120 | 4342.94 |
| 25/75 fr800k                                    | 19064124 | 3910.59 |
| 50/50 fr800k                                    | 19064127 | 4482.2  |
| baseline head                                   | 19064144 | 4277.82 |
| 50/50 head                                      | 19064174 | 4497.48 |
| 80/20 fr800k                                    | 19064210 | 4251.84 |
| 80/20 head fr800k                               | 19064211 | 4490.42 |
| mse, ht, pht, ex                                | 19078590 | 6709.17 |
| normed deltas                                   | 19083303 | 6941.6  |
| normed d2                                       | 19087940 | 7106.62 |
| custom pads d2                                  | 19088889 | 6842.78 |
| x_dx_ex_edx_mom_hit_phit_da_mse_nerr_orig_1000k | 19090913 | 7195.64 |
| x_dx_ex_edx_mom_hit_phit_da_mse_nerr_30_1300k   | 19090918 | 7161.54 |
| x_dx_ex_edx_mom_hit_phit_da_mse_nerr_50_1700k   | 19090928 | 7205.29 |
| 07_full_0_label_1000k                           | 19108324 | 7234.54 |
| 08_all_nan_mean_400                             | 19177390 | 7187.28 |
| 09_cos_EAngle_100                               | 19178103 | 7145.69 |
| 09_cos_fix_NaN_100                              | 19178500 | 7137.69 |
| 09_cos_fix_NaN_100_60                           | 19178558 | 7062.98 |
| 09_plus_dll_wo_wbin_100                         | 19180172 | 7137.33 |
| 09_plus_all_orig_features_100                   | 19180184 | 7264.64 |
| 09_plus_all_orig_features_1000_120              | 19180186 | 7295.24 |
| 09_plus_all_orig_features_1000_200              | 19180244 | 7337.39 |
| 09_err_xyz_100                                  | 19213590 | 7315.29 |
| 09_importance_001_100                           | 19213636 | 7155.73 |
| 09_foi_closest_100                              | 19215506 | 7195.69 |
| 09_foi_closest_replaced_100                     | 19216704 | 7275.62 |
| 09_some_baseline_100                            | 19224628 | 7160.55 |
| 09_baseline_wo_wbins_importance_100             | 19224649 | 7235.39 |
| 09_baseline_cdf_100                             | 19224694 | 7263.3  |
| 09_baseline_cdf_wo_wbins_100                    | 19224712 | 7172.26 |
| 09_best_wo_score                                | 19224767 | 7219.85 |
| 09_best_wo_score_wo_weights                     | 19224828 | 7274.37 |
| 09_best_wo_weights                              | 19224914 | 7125.43 |
| 09_w_score_wo_custom_track_match                | 19224959 | 7273.95 |
| 09_naive_v_m_100                                | 19226060 | 7331.76 |
| 09_naive_v_m_w_cl                               | 19226134 | 7270.43 |
| 09_vm_100                                       | 19236776 | 7324.46 |
| 10_matched_hits_100                             | 19238691 | 7287.65 |
| 10_matched_hits_1seg_100                        | 19238723 | 7173.52 |
| 10_matched_hits_1seg_cos_100                    | 19238753 | 7232.04 |
| 10_matched_hits_1seg_cos_d2_100                 | 19238758 | 7194.55 |
| 10_wo_time_constraints_100                      | 19246684 | 7215.02 |
| 10_wo_time_constraints_mean_100                 | 19247316 | 7287.91 |
| 11_baseline_100                                 | 19249026 | 7177.62 |
| 11_baseline_wo_imp_100                          | 19249047 | 7174.55 |
| 11_no_wbin_100                                  | 19249087 | 7239.43 |
| 11_dumb_cos_100                                 | 19249164 | 7169.83 |
| 11_cat_boost_100_120                            | 19249577 | 7018.77 |
| 11_cat_boost_100_120 wo weights                 | 19249593 | 6815.17 |
| 11_cat_boost_100_120 pos weights                | 19249638 | 6944.37 |
| 11_weigths_pos_100_120                          | 19249661 | 6894.69 |
| 11_weigths_pos_400k_500                         | 19249699 | 7272.03 |
| 11_1m_1000                                      | 19249842 | 7385.34 |
| 12_baseline_1m                                  | 19262836 | 7323.49 |
| 12_baseline_2m_400                              | 19263823 | 7387.86 |
| 13_blending                                     | 19264752 | 7461.79 |