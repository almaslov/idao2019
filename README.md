# IDAO 2019

## Links

- [Contest link](https://official.contest.yandex.ru/contest/10569/enter/)
- [The Muon Identification Procedure](https://core.ac.uk/download/pdf/44230824.pdf)
- [Muon system explained](https://cds.cern.ch/record/2063310/files/CERN-THESIS-2015-181.pdf)
- [isMuonTight+BDT](https://indico.cern.ch/event/491582/contributions/1168914/attachments/1236304/1815447/LHCC_Cogoni_v4.pdf)
- [fitting with energy loss and multiple scattering](https://www.nikhef.nl/~wouterh/topicallectures/TrackingAndVertexing/part4.pdf)
- [MULTIPLE SCATTERING ERROR PROPAGATION IN PARTICLE TRACK RECONSTRUCTION](https://arxiv.org/pdf/hep-ex/9406006.pdf)

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