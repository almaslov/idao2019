# IDAO 2019

## Links

- [Contest link](https://official.contest.yandex.ru/contest/10569/enter/)
- [Muon system explained](https://cds.cern.ch/record/2063310/files/CERN-THESIS-2015-181.pdf)
- [isMuonTight+BDT](https://indico.cern.ch/event/491582/contributions/1168914/attachments/1236304/1815447/LHCC_Cogoni_v4.pdf)

## FAQ

- `data/` - folder for input train/test data.
- `out/` - folder for output results.
- data file extensions're ignored.
- track #1 notebooks are prefixed w/ 00-49
- track #2 notebooks are prefixed w/ 50-..

## Ideas

- [ ] read in parallel
- [x] normalize classes
- [ ] ensembles

## Experiments

### 100k rand from 400k (30588/369412)

| \    | coord    | e_m      | pca      | cos      | cos,e_m  | pca,e_m  | cos,pca  | cos,pca,e_m |
| ---- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | ----------- |
| mean | 0.522238 | 0.522174 | 0.476468 | 0.651596 | 0.594934 | 0.508300 | 0.615967 | 0.604534    |
| std  | 0.089591 | 0.063560 | 0.068526 | 0.126795 | 0.074542 | 0.030846 | 0.114739 | 0.072358    |
| time | 17 s     | 18.1 s   | 18.5 s   | 23.3 s   | 22.9 s   | 19 s     | 23.8 s   | 25 s        |

### 100k 50/50 from 400k (30588/369412)

| \    | coord    | e_m      | pca      | cos      | cos,e_m  | pca,e_m  | cos,pca  | cos,pca,e_m |
| ---- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | ----------- |
| mean | 0.496092 | 0.478049 | 0.469875 | 0.653774 | 0.666584 | 0.505335 | 0.670746 | 0.640206    |
| std  | 0.026036 | 0.028816 | 0.041083 | 0.032817 | 0.028323 | 0.025181 | 0.037231 | 0.044023    |
| time | 18.4 s   | 16.6 s   | 20.5 s   | 23.2 s   | 23.6 s   | 18.7 s   | 28.5 s   | 24.3 s      |

### 100k 30/70 from 400k (30588/369412)

| \    | coord    | e_m      | pca      | cos      | cos,e_m  | pca,e_m  | cos,pca  | cos,pca,e_m |
| ---- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | ----------- |
| mean | 0.510292 | -------- | -------- | -------- | -------- | -------- | 0.618931 | -           |
| std  | 0.052114 | -        | -        | -        | -        | -        | 0.053795 | -           |
| time | 16.5 s   | -        | -        | -        | -        | -        | 25.8 s   | -           |

### 100k even from 400k (30588/369412) continue

...

## Submissions

| descr             | id       | score   |
| ----------------- | -------- | ------- |
| baseline          | 19064098 | 4173.63 |
| e_m               | 19064099 | 4277.98 |
| 25/75             | 19064110 | 4083.36 |
| 50/50             | 19064113 | 4186.43 |
| cos               | 19064118 | 5400.8  |
| pca               | 19064120 | 4342.94 |
| 25/75 fr800k      | 19064124 | 3910.59 |
| 50/50 fr800k      | 19064127 | 4482.2  |
| baseline head     | 19064144 | 4277.82 |
| 50/50 head        | 19064174 | 4497.48 |
| 80/20 fr800k      | 19064210 | 4251.84 |
| 80/20 head fr800k | 19064211 | 4490.42 |