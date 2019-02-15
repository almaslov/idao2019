// Copyright 2019, Nikita Kazeev, Higher School of Economics
#include <iostream>
#include <iomanip>
#include <vector>
#include <limits>
#include <cassert>

#include <xgboost/c_api.h>

#include "parser.h"


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

int main(int argc, char* argv[]) {
    if (argc > 1 && std::string(argv[1]) == "--model-info") {
//        std::cout << "NO include features. Model file name " << xgb_model_path << std::endl;
        return 0;
    }
    size_t n = 0;
    if (argc > 1 && std::string(argv[1]) == "-n") {
        n = std::atoi(argv[2]);
        (void)n;
    }

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << std::setprecision(std::numeric_limits<float>::max_digits10);
    Parser parser(std::cin);

    Model model(xgb_model_path);

    std::cout << "id,prediction\n";
    std::vector<float> all_features(N_FEATURES);
    while (std::cin.good() && std::cin.peek() != EOF) {
        size_t id;
        parser.read_one(id, all_features);

        std::vector<float> features;
        select(all_features, &features);
        add_features(features, all_features);

        DMatrixHandle matrix;
        if (XGDMatrixCreateFromMat(features.data(), 1, features.size(), NAN, &matrix) < 0) {
            throw std::runtime_error("XGDMatrixCreateFromMat");
        }
        bst_ulong out_len;
        const float* out_result;
        model.predict(matrix, &out_len, &out_result);
        assert(out_len == 1);

        if (n) {
            if (id < n) continue;
            dump_features(features);
            break;
        }
        std::cout << id << DELIMITER << out_result[0] << '\n';
//        std::cout << id << DELIMITER  << "0\n";

        XGDMatrixFree(matrix);
    }
    return 0;
}
