#pragma once
#include <vector>
#include <tuple>
#include <cmath>

#include "feature.h"

std::vector<std::string> features_used({
    "ncl[0]", "ncl[1]", "ncl[2]", "ncl[3]", "avg_cs[0]",
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
    "Mextra_DY2[1]", "Mextra_DY2[2]", "Mextra_DY2[3]", "FOI_hits_N", "PT", "P"
});


const char xgb_model_path[] = "track_2_model.xgb";

float refraction_angle(float x0, float y0, float z0,
                       float x1, float y1, float z1,
                       float x2, float y2, float z2) {
    float vx1 = x1 - x0, vy1 = y1 - y0, vz1 = z1 - z0;
    float vx2 = x2 - x1, vy2 = y2 - y1, vz2 = z2 - z1;
    float norm1 = sqrtf(vx1 * vx1 + vy1 * vy1 + vz1 * vz1);
    float norm2 = sqrtf(vx2 * vx2 + vy2 * vy2 + vz2 * vz2);
    float sc_pr = vx1 * vx2 + vy1 * vy2 + vz1 * vz2;
    return acosf(sc_pr / norm1 / norm2) / M_PI * 180;
}

auto calculated_features = std::make_tuple(
    make_feature(refraction_angle,
                 "MatchedHit_X[0]", "MatchedHit_Y[0]", "MatchedHit_Z[0]",
                 "MatchedHit_X[1]", "MatchedHit_Y[1]", "MatchedHit_Z[1]",
                 "MatchedHit_X[2]", "MatchedHit_Y[2]", "MatchedHit_Z[2]"),
    make_feature(refraction_angle,
                 "MatchedHit_X[1]", "MatchedHit_Y[1]", "MatchedHit_Z[1]",
                 "MatchedHit_X[2]", "MatchedHit_Y[2]", "MatchedHit_Z[2]",
                 "MatchedHit_X[3]", "MatchedHit_Y[3]", "MatchedHit_Z[3]")
);
