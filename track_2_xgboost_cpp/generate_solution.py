import json


all_features = [
    # 'id',
    'ncl[0]', 'ncl[1]', 'ncl[2]', 'ncl[3]',
    'avg_cs[0]', 'avg_cs[1]', 'avg_cs[2]', 'avg_cs[3]',
    'ndof',
    'MatchedHit_TYPE[0]', 'MatchedHit_TYPE[1]', 'MatchedHit_TYPE[2]', 'MatchedHit_TYPE[3]',
    'MatchedHit_X[0]', 'MatchedHit_X[1]', 'MatchedHit_X[2]', 'MatchedHit_X[3]',
    'MatchedHit_Y[0]', 'MatchedHit_Y[1]', 'MatchedHit_Y[2]', 'MatchedHit_Y[3]',
    'MatchedHit_Z[0]', 'MatchedHit_Z[1]', 'MatchedHit_Z[2]', 'MatchedHit_Z[3]',
    'MatchedHit_DX[0]', 'MatchedHit_DX[1]', 'MatchedHit_DX[2]', 'MatchedHit_DX[3]',
    'MatchedHit_DY[0]', 'MatchedHit_DY[1]', 'MatchedHit_DY[2]', 'MatchedHit_DY[3]',
    'MatchedHit_DZ[0]', 'MatchedHit_DZ[1]', 'MatchedHit_DZ[2]', 'MatchedHit_DZ[3]',
    'MatchedHit_T[0]', 'MatchedHit_T[1]', 'MatchedHit_T[2]', 'MatchedHit_T[3]',
    'MatchedHit_DT[0]', 'MatchedHit_DT[1]', 'MatchedHit_DT[2]', 'MatchedHit_DT[3]',
    'Lextra_X[0]', 'Lextra_X[1]', 'Lextra_X[2]', 'Lextra_X[3]',
    'Lextra_Y[0]', 'Lextra_Y[1]', 'Lextra_Y[2]', 'Lextra_Y[3]',
    'NShared',
    'Mextra_DX2[0]', 'Mextra_DX2[1]', 'Mextra_DX2[2]', 'Mextra_DX2[3]',
    'Mextra_DY2[0]', 'Mextra_DY2[1]', 'Mextra_DY2[2]', 'Mextra_DY2[3]',
    'FOI_hits_N', 'FOI_hits_X', 'FOI_hits_Y', 'FOI_hits_Z',
    'FOI_hits_DX', 'FOI_hits_DY', 'FOI_hits_DZ',
    'FOI_hits_T', 'FOI_hits_DT', 'FOI_hits_S',
    'PT', 'P'
]

computed_features = []

all_features_map = None


class Feature:
    def __init__(self, name, num, storage='all'):
        self.name = name
        self.storage = storage
        self.num = num

    def __str__(self):
        return self.name

    def c_repr(self):
        return '%s[%d]' % (self.storage, self.num)


def remap():
    global all_features_map
    all_features_map = {
        v: Feature(v, k, storage='v') for k, v in enumerate(computed_features)}
    all_features_map.update({
        v: Feature(v, k) for k, v in enumerate(all_features)})

remap()


compute_header = """
void add_features(std::vector<float>& v, const std::vector<float>& all) {
"""

compute_footer = """
}
"""


compute_feature_header = """
    {
        float %(var_names)s;
        std::tie(%(var_names)s) = %(c_func)s(%(params)s);
"""

compute_feature_header_simple = """
    {
        float %(var_names)s = %(c_func)s(%(params)s);
"""


compute_feature_body = """
        v.push_back(%(var)s);
"""
compute_feature_footer = """
    }
"""


def mangle(s: str):
    return '_var__' + s.replace('[', '_').replace(']', '_')


def write_feature_eval(c_func, input, output):
    params = ', '.join(all_features_map[p].c_repr() for p in input)
    mangled = [mangle(s) for s in output]
    var_names = ', '.join(mangled)
    header_template = compute_feature_header_simple if len(output) == 1 else compute_feature_header
    print(header_template % {
        'var_names': var_names,
        'c_func': c_func,
        'params': params,
    })

    for s in mangled:
        print(compute_feature_body % {'var': s})

    print(compute_feature_footer)

select_header = '''
void select(const std::vector<float>& all, std::vector<float>* v) {
    v->reserve(%(size)d);
'''

select_body = '''
    v->push_back(all[%d]);
'''

select_footer = '''
}
'''


def write_select(features):
    print(select_header % {'size': len(features)})
    for f in features:
        print(select_body % all_features_map[f].num)
    print(select_footer)


print_header = '''
void dump_features(std::vector<float>& v) {
    size_t i = 0;
'''

print_body = R'''
    std::cout << "%(name)s" << "\t" << v[i++] << "\n";
'''

print_footer = '''
}
'''


def write_print():
    print(print_header % {'size': len(computed_features)})
    for f in computed_features:
        print(print_body % {'name': f})
    print(select_footer)


def generate(info):
    print('#include <vector>')
    print('#include <iostream>')
    print('const char xgb_model_path[] = "%s";' % info['model'])

    write_select(info["features"])
    global computed_features
    computed_features += list(info["features"])
    remap()

    print(compute_header)
    for e in info["eval_features"]:
        input_features = e["input"]
        c_func = e["c_func"]
        output_features = e["output"]

        write_feature_eval(c_func, input_features, output_features)

        computed_features.extend(list(output_features))
        remap()
    print(compute_footer)

    write_print()


generate({
    'model': './my.xgb',
    'eval_features': [
        {
            'c_func': 'refraction_angles',
            'input': ("Lextra_X[0]", "Lextra_Y[0]",
                      "Lextra_X[1]", "Lextra_Y[1]",
                      "MatchedHit_X[0]", "MatchedHit_Y[0]", "MatchedHit_Z[0]",
                      "MatchedHit_X[1]", "MatchedHit_Y[1]", "MatchedHit_Z[1]",
                      "MatchedHit_X[2]", "MatchedHit_Y[2]", "MatchedHit_Z[2]",
                      "MatchedHit_X[3]", "MatchedHit_Y[3]", "MatchedHit_Z[3]"),
            'output': ('DA[0]', 'DA[1]', 'DA[2]')
        },
        {
            'c_func': 'mse',
            'input': ("MatchedHit_X[0]", "MatchedHit_Y[0]", "Lextra_X[0]", "Lextra_Y[0]", "MatchedHit_DX[0]", "MatchedHit_DY[0]",
                      "MatchedHit_X[1]", "MatchedHit_Y[1]", "Lextra_X[1]", "Lextra_Y[1]", "MatchedHit_DX[1]", "MatchedHit_DY[1]",
                      "MatchedHit_X[2]", "MatchedHit_Y[2]", "Lextra_X[2]", "Lextra_Y[2]", "MatchedHit_DX[2]", "MatchedHit_DY[2]",
                      "MatchedHit_X[3]", "MatchedHit_Y[3]", "Lextra_X[3]", "Lextra_Y[3]", "MatchedHit_DX[3]", "MatchedHit_DY[3]"),
            'output': ('MSE',)
        },
        {
            'c_func': 'normed_err',
            'input': ("MatchedHit_X[0]", "Lextra_X[0]", "Mextra_DX2[0]"),
            'output': ('NORM_ERR_X[0]',)
        },
        {
            'c_func': 'normed_err',
            'input': ("MatchedHit_X[1]", "Lextra_X[1]", "Mextra_DX2[1]"),
            'output': ('NORM_ERR_X[1]',)
        },
        {
            'c_func': 'normed_err',
            'input': ("MatchedHit_X[2]", "Lextra_X[2]", "Mextra_DX2[2]"),
            'output': ('NORM_ERR_X[2]',)
        },
        {
            'c_func': 'normed_err',
            'input': ("MatchedHit_X[3]", "Lextra_X[3]", "Mextra_DX2[3]"),
            'output': ('NORM_ERR_X[3]',)
        },
        {
            'c_func': 'normed_err',
            'input': ("MatchedHit_Y[0]", "Lextra_Y[0]", "Mextra_DY2[0]"),
            'output': ('NORM_ERR_Y[0]',)
        },
        {
            'c_func': 'normed_err',
            'input': ("MatchedHit_Y[1]", "Lextra_Y[1]", "Mextra_DY2[1]"),
            'output': ('NORM_ERR_Y[1]',)
        },
        {
            'c_func': 'normed_err',
            'input': ("MatchedHit_Y[2]", "Lextra_Y[2]", "Mextra_DY2[2]"),
            'output': ('NORM_ERR_Y[2]',)
        },
        {
            'c_func': 'normed_err',
            'input': ("MatchedHit_Y[3]", "Lextra_Y[3]", "Mextra_DY2[3]"),
            'output': ('NORM_ERR_Y[3]',)
        },
        {
            'c_func': 'dll',
            'input': ("MSE",),
            'output': ('DLL', )
        },
        {
            'c_func': 'err',
            'input': ("MatchedHit_X[0]", "Lextra_X[0]"),
            'output': ('ERR_X[0]',)
        },
        {
            'c_func': 'err',
            'input': ("MatchedHit_X[1]", "Lextra_X[1]"),
            'output': ('ERR_X[1]',)
        },
        {
            'c_func': 'err',
            'input': ("MatchedHit_X[2]", "Lextra_X[2]"),
            'output': ('ERR_X[2]',)
        },
        {
            'c_func': 'err',
            'input': ("MatchedHit_X[3]", "Lextra_X[3]"),
            'output': ('ERR_X[3]',)
        },
        {
            'c_func': 'err',
            'input': ("MatchedHit_Y[0]", "Lextra_Y[0]"),
            'output': ('ERR_Y[0]',)
        },
        {
            'c_func': 'err',
            'input': ("MatchedHit_Y[1]", "Lextra_Y[1]"),
            'output': ('ERR_Y[1]',)
        },
        {
            'c_func': 'err',
            'input': ("MatchedHit_Y[3]", "Lextra_Y[3]"),
            'output': ('ERR_Y[3]',)
        },
        {
            'c_func': 'err_z',
            'input': ("MatchedHit_Z[0]", "MatchedHit_Z[1]", "MatchedHit_Z[2]", "MatchedHit_Z[3]"),
            'output': ('ERR_Z[0]', 'ERR_Z[1]', 'ERR_Z[2]', 'ERR_Z[3]')
        },
        {
            'c_func': 'mass',
            'input': ("Lextra_X[0]", "Lextra_Y[0]",
                      "Lextra_X[1]", "Lextra_Y[1]",
                      "MatchedHit_X[0]", "MatchedHit_Y[0]", "MatchedHit_Z[0]", "MatchedHit_T[0]",
                      "MatchedHit_X[1]", "MatchedHit_Y[1]", "MatchedHit_Z[1]", "MatchedHit_T[1]",
                      "MatchedHit_X[2]", "MatchedHit_Y[2]", "MatchedHit_Z[2]", "MatchedHit_T[2]",
                      "MatchedHit_X[3]", "MatchedHit_Y[3]", "MatchedHit_Z[3]", "MatchedHit_T[3]",
                      "P", "PT"),
            'output': ('V', 'VT', 'M', 'MT')
        },
    ],
    'features': ['P', 'PT', 'MatchedHit_TYPE[0]', 'MatchedHit_TYPE[1]', 'MatchedHit_TYPE[2]',
                 'Lextra_X[0]', 'Lextra_X[1]', 'Lextra_X[2]', 'Lextra_X[3]',
                 'Lextra_Y[0]', 'Lextra_Y[1]', 'Lextra_Y[2]', 'Lextra_Y[3]',
                 'Mextra_DX2[0]', 'Mextra_DX2[1]', 'Mextra_DX2[2]', 'Mextra_DX2[3]',
                 'Mextra_DY2[0]', 'Mextra_DY2[1]', 'Mextra_DY2[3]',
                 'FOI_hits_N', 'NShared',
                 'MatchedHit_T[0]', 'MatchedHit_T[2]',
                 'ncl[0]', 'ncl[1]', 'ncl[2]', 'ncl[3]',
                 'avg_cs[0]', 'avg_cs[1]', 'avg_cs[2]', 'avg_cs[3]',
                 'MatchedHit_DX[1]', 'MatchedHit_DX[2]', 'MatchedHit_DX[3]',
                 'MatchedHit_DY[0]', 'MatchedHit_DY[3]', 'MatchedHit_DZ[2]']
})

