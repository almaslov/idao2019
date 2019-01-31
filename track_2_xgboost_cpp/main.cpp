// Copyright 2019, Nikita Kazeev, Higher School of Economics
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <iterator>
#include <vector>
#include <limits>

#include <xgboost/c_api.h>

#include "parser.h"

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << std::setprecision(std::numeric_limits<float>::max_digits10);
    Index index({"ncl[0]", "ncl[1]", "ncl[2]", "ncl[3]", "avg_cs[0]",
        "avg_cs[1]", "avg_cs[2]", "avg_cs[3]", "ndof", "MatchedHit_TYPE[0]",
        "MatchedHit_TYPE[1]", "MatchedHit_TYPE[2]", "MatchedHit_TYPE[3]",
        "MatchedHit_X[0]", "MatchedHit_X[1]", "MatchedHit_X[2]",
        "MatchedHit_X[3]", "MatchedHit_Y[0]", "MatchedHit_Y[1]",
        "MatchedHit_Y[2]", "MatchedHit_Y[3]", "MatchedHit_Z[0]",
        "MatchedHit_Z[1]", "MatchedHit_Z[2]", "MatchedHit_Z[3]",
        "MatchedHit_DX[0]", "MatchedHit_DX[1]", "MatchedHit_DX[2]",
        "MatchedHit_DX[3]", "MatchedHit_DY[0]", "MatchedHit_DY[1]",
        "MatchedHit_DY[2]", "MatchedHit_DY[3]", "MatchedHit_DZ[0]",
        "MatchedHit_DZ[1]", "MatchedHit_DZ[2]", "MatchedHit_DZ[3]",
        "MatchedHit_T[0]", "MatchedHit_T[1]", "MatchedHit_T[2]",
        "MatchedHit_T[3]", "MatchedHit_DT[0]", "MatchedHit_DT[1]",
        "MatchedHit_DT[2]", "MatchedHit_DT[3]", "Lextra_X[0]", "Lextra_X[1]",
        "Lextra_X[2]", "Lextra_X[3]", "Lextra_Y[0]", "Lextra_Y[1]",
        "Lextra_Y[2]", "Lextra_Y[3]", "NShared", "Mextra_DX2[0]",
        "Mextra_DX2[1]", "Mextra_DX2[2]", "Mextra_DX2[3]", "Mextra_DY2[0]",
        "Mextra_DY2[1]", "Mextra_DY2[2]", "Mextra_DY2[3]", "FOI_hits_N", "PT", "P"});
//    Index index({"MatchedHit_X[0]", "MatchedHit_Y[0]", "MatchedHit_Z[0]"});
    Parser parser(index, std::cin);

    BoosterHandle booster;
    if (XGBoosterCreate(NULL, 0, &booster) < 0) {
        throw std::runtime_error("XGBoosterCreate");
    }

    if (XGBoosterLoadModel(booster, "track_2_model.xgb") < 0) {
        throw std::runtime_error("XGBoosterLoadModel");
    }

    std::cout << "id,prediction\n";
    size_t n = 0;
    while (std::cin.good() && std::cin.peek() != EOF) {
        std::vector<float> features(N_FEATURES);
        size_t id;
        parser.read_one(id, features);

        DMatrixHandle matrix;
        if (XGDMatrixCreateFromMat(features.data(), 1, features.size(), NAN, &matrix) < 0) {
            throw std::runtime_error("XGDMatrixCreateFromMat");
        }
        bst_ulong out_len;
        const float *out_result;

        if (XGBoosterPredict(booster, matrix, 1, 0, &out_len, &out_result) < 0) {
            throw std::runtime_error("XGBoosterPredict");
        }
        assert(out_len == 1);

        std::cout << id << DELIMITER << out_result[0] << '\n';
//        for (size_t i = 0; i < index.size(); ++i)
//            std::cout << features[i] << '\t';
//        std::cout << std::endl;
        ++n;
//        const float prediction = \
//            evaluator.Apply(features, NCatboostStandalone::EPredictionType::RawValue);
//        std::cout << id << DELIMITER << prediction  << '\n';
    }
    return 0;
}
