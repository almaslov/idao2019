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


// ----------- <so sorry> -----------

template<class... Args, std::size_t... I>
std::vector<Feature> tuple_to_vector_impl(const std::tuple<Args...>& t, std::index_sequence<I...>) {
    return {std::get<I>(t)...};
}

template<class... Args>
std::vector<Feature> tuple_to_vector(const std::tuple<Args...>& t) {
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

    BoosterHandle booster;
    if (XGBoosterCreate(NULL, 0, &booster) < 0) {
        throw std::runtime_error("XGBoosterCreate");
    }

    if (XGBoosterLoadModel(booster, xgb_model_path) < 0) {
        throw std::runtime_error("XGBoosterLoadModel");
    }
    if (XGBoosterSetParam(booster, "nthread", "1") < 0) {
        throw std::runtime_error("XGBoosterSetParam");
    }

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

        if (XGBoosterPredict(booster, matrix, 0, 0, &out_len, &out_result) < 0) {
            throw std::runtime_error("XGBoosterPredict");
        }
        assert(out_len == 1);

        std::cout << id << DELIMITER << out_result[0] << '\n';

        XGDMatrixFree(matrix);
    }
    XGBoosterFree(booster);
    return 0;
}
