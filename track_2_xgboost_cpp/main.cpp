// Copyright 2019, Nikita Kazeev, Higher School of Economics
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <iterator>
#include <vector>
#include <limits>
#include <cassert>

#include <xgboost/c_api.h>

#include "parser.h"
#include "solution.h"
#include "feature.h"


class Model {
    BoosterHandle booster;

public:
    Model(const std::string& path) {
        if (XGBoosterCreate(NULL, 0, &booster) < 0) {
            throw std::runtime_error("XGBoosterCreate");
        }
        if (XGBoosterLoadModel(booster, path.data()) < 0) {
            throw std::runtime_error("XGBoosterLoadModel");
        }
        if (XGBoosterSetParam(booster, "nthread", "1") < 0) {
            throw std::runtime_error("XGBoosterSetParam");
        }
    }

    void predict(DMatrixHandle matrix, size_t *out_len, const float **out_result) {
        if (XGBoosterPredict(booster, matrix, 0, 0, out_len, out_result) < 0) {
            throw std::runtime_error("XGBoosterPredict");
        }
    }

    ~Model() {
        XGBoosterFree(booster);
    }
};

// ----------- <so sorry> -----------

template<class... Args, std::size_t... I>
std::vector<ComplexFeature> tuple_to_vector_impl(const std::tuple<Args...>& t, std::index_sequence<I...>) {
    return {std::get<I>(t)...};
}

template<class... Args>
std::vector<ComplexFeature> tuple_to_vector(const std::tuple<Args...>& t) {
    return tuple_to_vector_impl(t, std::make_index_sequence<sizeof...(Args)>());
}
// ----------- </so sorry> -----------

void dump_model_info() {
    std::cout << "Model file: " << xgb_model_path << std::endl
              << "Features: " << std::endl;
    size_t i = 0;
    for (const auto& s : features_used) {
        std::cout << i++ << ": " <<  s << std::endl;
    }

    std::cout << "Computed features: " << std::endl;
    i = 0;
    for (const auto& feature : tuple_to_vector(calculated_features)) {
        std::cout << i++ << ": " << feature.func_name << std::endl;
        for (const auto& s : feature.param_names) {
            std::cout << "\t" << s << std::endl;
        }
    }
}

void dump_features(const Index& index, const std::vector<float>& features) {
    size_t i = 0;
    for (float f: features) {
        if (i < index.names.size()) {
            std::cout << index.names[i++];
        } else {
            std::cout << '\t';
        }
        std::cout << '\t' << f << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc > 1 && std::string(argv[1]) == "--model-info") {
        dump_model_info();
        return 0;
    }

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << std::setprecision(std::numeric_limits<float>::max_digits10);
    Index index(features_used);
    Parser parser(index, std::cin);

    Model model(xgb_model_path);

    std::cout << "id,prediction\n";
    std::vector<float> all_features(N_FEATURES);
    while (std::cin.good() && std::cin.peek() != EOF) {
        size_t id;
        parser.read_one(id, all_features);

        std::vector<float> features;
        index.select(all_features, &features);

        add_features(features, all_features, calculated_features);

        DMatrixHandle matrix;
        if (XGDMatrixCreateFromMat(features.data(), 1, features.size(), NAN, &matrix) < 0) {
            throw std::runtime_error("XGDMatrixCreateFromMat");
        }
        bst_ulong out_len;
        const float *out_result;
        model.predict(matrix, &out_len, &out_result);
        assert(out_len == 1);

//        if (id < 527801) continue;
        std::cout << id << DELIMITER << out_result[0] << '\n';
//        dump_features(index, features);
//        break;

        XGDMatrixFree(matrix);
    }
    return 0;
}
