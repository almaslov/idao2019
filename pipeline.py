import numpy as np
import pandas as pd
import xgboost as xgb
from sklearn import model_selection as mdsel
from sklearn.metrics import accuracy_score, precision_score, recall_score, f1_score, roc_auc_score

import scoring
from common import train_cols

def split_classes(data):
    return [data.loc[data.label == i, :] for i in range(2)]

def count_classes(data):
    cnt_0 = np.count_nonzero(data.label == 0)
    return cnt_0, len(data.index) - cnt_0

def sample(data, nrows):
    return data.sample(frac=1).iloc[:nrows, :]


def fit(train, n_estimators, transformer_cls):
    target_train = train[train_cols]
    
    # defined much later
    transformer = transformer_cls().fit(train)
    train = transformer.transform(train)
    
    estimator = xgb.XGBClassifier(n_estimators=n_estimators, n_jobs=3)
    estimator.fit(train.values, target_train.label.values, sample_weight=target_train.weight.values, eval_metric=scoring.rejection90_sklearn)
    return transformer, estimator
    
def predict(fitted_state, test):
    transformer, estimator = fitted_state
    
    test = transformer.transform(test)
    predictions = estimator.predict_proba(test.values)[:, 1]
    return predictions

def score(fitted_state, test):
    target_test = test.loc[:, train_cols]
    predictions = predict(fitted_state, test)
    return scoring.rejection90(target_test.label.values, predictions, sample_weight=target_test.weight.values)

def fit_predict_save(train, test, filename, n_estimators, transformer_cls):
    fitted_state = fit(train, n_estimators, transformer_cls)
    predictions = predict(fitted_state, test)
    
    pd.DataFrame(data={"prediction": predictions}, index=test.index).to_csv(
        filename, index_label='id'
    )
    
    model = fitted_state[1]
    model.save_model(to_model_filename(filename))

def fit_save_model(train, filename, n_estimators, transformer_cls):
    _, model = fit(train, n_estimators, transformer_cls)
    model.save_model(to_model_filename(filename))

def cross_validate(train, n_estimators, n_splits, n_rows, transformer_cls):
    train = sample(train, n_rows)
    
    splitter = mdsel.StratifiedKFold(n_splits=n_splits, shuffle=True)
    scores = []
    for train_indices, test_indices in splitter.split(train, train.label):
        train_subset = train.iloc[train_indices, :]
        test_subset = train.iloc[test_indices, :]
        
        fit_state = fit(train_subset, n_estimators, transformer_cls)
        
        target_test = test_subset[train_cols]
        predictions = predict(fit_state, test_subset)
        
        y_true = target_test.label.values
        l, r, _ = scoring.get_threshold_details(y_true, predictions, sample_weight=target_test.weight.values)
        threshold = (l + r) / 2
        y_pred = predictions >= threshold
                
        acc = accuracy_score(y_true, y_pred)
        prec = precision_score(y_true, y_pred)
        rec = recall_score(y_true, y_pred)
        f1 = f1_score(y_true, y_pred)
        roc_auc = roc_auc_score(y_true, predictions)
        scr = scoring.rejection90(y_true, predictions, sample_weight=target_test.weight.values)
        scores += [[acc, prec, rec, f1, roc_auc, scr, threshold, r - l]]

    return pd.DataFrame(scores, columns=['acc', 'prec', 'rec', 'f1', 'roc_auc', 'scr', 'th', 'dTh'])

def to_model_filename(filename):
    if filename.endswith('.'):
        return filename.replace('out/', 'models/').replace('.csv', '.xgb')

# =================== NOT WORKING =======================

def get_class(data, i):
    return data.index[data.label == i]

def get_cnts(nrows, prop_0, lens):
    if prop_0 is None:
        prop_0 = float(lens[0]) / (lens[0] + lens[1])
    cnt_0 = round(nrows * prop_0)
    cnt_1 = nrows - cnt_0
    return cnt_0, cnt_1


# Gets `n_rows` head samples from `data` with specified class proportions.
# If `prop_0` is None, it keeps `data`s natural proportions.
# If `data` samples is not enough to fulfil proportions for any `class_i` then random sampling from the `class_i` is applied.
def get_head_w_sampling(data, nrows, prop_0=None):
    def get_class_head_w_sampling(cnt, class_i, len_i):
        if cnt < len_i:
            return class_i[:cnt]
        class_indices = np.concatenate((np.arange(len_i),  np.random.randint(len_i, size=cnt-len_i)))
        return class_i[class_indices]
    
    classes = [get_class(i) for i in range(2)]
    cls_lens = list(map(len, classes))
    cnts = get_cnts(nrows, prop_0, cls_lens)
    indices = np.concatenate([get_class_head_w_sampling(*kargs) for kargs in zip(cnts, classes, cls_lens)])
    return data.loc[indices, :].sample(frac=1)

# ^^^^^^^^^^^^^^^^^^^^^^ NOT WORKING ^^^^^^^^^^^^^^^^^^^^^^^^


