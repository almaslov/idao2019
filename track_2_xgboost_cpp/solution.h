#pragma once
#include <vector>
#include <tuple>
#include <cmath>

#include "feature.h"

std::vector<std::string> features_used({
   "MatchedHit_X[0]", "MatchedHit_X[1]", "MatchedHit_X[2]", "MatchedHit_X[3]",
   "MatchedHit_Y[0]", "MatchedHit_Y[1]", "MatchedHit_Y[2]", "MatchedHit_Y[3]",
   "MatchedHit_Z[0]", "MatchedHit_Z[1]", "MatchedHit_Z[2]", "MatchedHit_Z[3]",
   "P", "PT",
   "MatchedHit_TYPE[0]", "MatchedHit_TYPE[1]", "MatchedHit_TYPE[2]", "MatchedHit_TYPE[3]",
   "Lextra_X[0]", "Lextra_X[1]", "Lextra_X[2]", "Lextra_X[3]",
   "Lextra_Y[0]", "Lextra_Y[1]", "Lextra_Y[2]", "Lextra_Y[3]",
   "Mextra_DX2[0]", "Mextra_DX2[1]", "Mextra_DX2[2]", "Mextra_DX2[3]",
   "Mextra_DY2[0]", "Mextra_DY2[1]", "Mextra_DY2[2]", "Mextra_DY2[3]",
   "MatchedHit_DX[0]", "MatchedHit_DX[1]", "MatchedHit_DX[2]", "MatchedHit_DX[3]",
   "MatchedHit_DY[0]", "MatchedHit_DY[1]", "MatchedHit_DY[2]", "MatchedHit_DY[3]",
   "MatchedHit_DZ[0]", "MatchedHit_DZ[1]", "MatchedHit_DZ[2]", "MatchedHit_DZ[3]"
});


const char xgb_model_path[] = "08_mse_nan_mean_100.xgb";

float refraction_angle_2(float x0, float y0, float z0,
                       float x1, float y1, float z1) {
    float vx1 = x0, vy1 = y0, vz1 = z0;
    float vx2 = x1 - x0, vy2 = y1 - y0, vz2 = z1 - z0;
    float norm1 = sqrtf(vx1 * vx1 + vy1 * vy1 + vz1 * vz1);
    float norm2 = sqrtf(vx2 * vx2 + vy2 * vy2 + vz2 * vz2);
    float sc_pr = vx1 * vx2 + vy1 * vy2 + vz1 * vz2;
    return sc_pr / norm1 / norm2;
}

float refraction_angle_3(float x0, float y0, float z0,
                       float x1, float y1, float z1,
                       float x2, float y2, float z2) {
    float vx1 = x1 - x0, vy1 = y1 - y0, vz1 = z1 - z0;
    float vx2 = x2 - x1, vy2 = y2 - y1, vz2 = z2 - z1;
    float norm1 = sqrtf(vx1 * vx1 + vy1 * vy1 + vz1 * vz1);
    float norm2 = sqrtf(vx2 * vx2 + vy2 * vy2 + vz2 * vz2);
    float sc_pr = vx1 * vx2 + vy1 * vy2 + vz1 * vz2;
    return sc_pr / norm1 / norm2;
}

float mse(float x0, float y0, float ex0, float ey0, float dx0, float dy0,
          float x1, float y1, float ex1, float ey1, float dx1, float dy1,
          float x2, float y2, float ex2, float ey2, float dx2, float dy2,
          float x3, float y3, float ex3, float ey3, float dx3, float dy3) {
    float lx0 = (x0 - ex0) / dx0 / 2;
    float ly0 = (y0 - ey0) / dy0 / 2;
    float lx1 = (x1 - ex1) / dx1 / 2;
    float ly1 = (y1 - ey1) / dy1 / 2;
    float lx2 = (x2 - ex2) / dx2 / 2;
    float ly2 = (y2 - ey2) / dy2 / 2;
    float lx3 = (x3 - ex3) / dx3 / 2;
    float ly3 = (y3 - ey3) / dy3 / 2;
    return (lx0 * lx0 + ly0 * ly0 +
            lx1 * lx1 + ly1 * ly1 +
            lx2 * lx2 + ly2 * ly2 +
            lx3 * lx3 + ly3 * ly3) / 8;
}

float normed_err(float x, float ex, float edx) {
    return (x - ex) / sqrtf(edx);
}

template<size_t I>
float probability_hit_detector(float p)
{
    static float alpha[] = {0.0260, 0.0021, 0.0015, 0.0008};
    static float beta[] = {2040., 2387., 3320., 3903.};

    float t = powf(alpha[I] * (p - beta[I]), I + 1);
    return t / (1 + t);
}

#define MH(DIM, LAYER) ("MatchedHit_" #DIM "[" #LAYER "]")
#define E(DIM, LAYER)  ("Lextra_" #DIM "[" #LAYER "]")
#define ED(DIM, LAYER) ("Mextra_D" #DIM "2[" #LAYER "]")


auto calculated_features = std::make_tuple(
    make_feature(probability_hit_detector<0>, "P"),
    make_feature(probability_hit_detector<1>, "P"),
    make_feature(probability_hit_detector<2>, "P"),
    make_feature(probability_hit_detector<3>, "P"),

    make_feature(refraction_angle_2,
                 "MatchedHit_X[0]", "MatchedHit_Y[0]", "MatchedHit_Z[0]",
                 "MatchedHit_X[1]", "MatchedHit_Y[1]", "MatchedHit_Z[1]"),
    make_feature(refraction_angle_3,
                 "MatchedHit_X[0]", "MatchedHit_Y[0]", "MatchedHit_Z[0]",
                 "MatchedHit_X[1]", "MatchedHit_Y[1]", "MatchedHit_Z[1]",
                 "MatchedHit_X[2]", "MatchedHit_Y[2]", "MatchedHit_Z[2]"),
    make_feature(refraction_angle_3,
                 "MatchedHit_X[1]", "MatchedHit_Y[1]", "MatchedHit_Z[1]",
                 "MatchedHit_X[2]", "MatchedHit_Y[2]", "MatchedHit_Z[2]",
                 "MatchedHit_X[3]", "MatchedHit_Y[3]", "MatchedHit_Z[3]"),

    make_feature(mse,
                 "MatchedHit_X[0]", "MatchedHit_Y[0]", "Lextra_X[0]", "Lextra_Y[0]", "MatchedHit_DX[0]", "MatchedHit_DY[0]",
                 "MatchedHit_X[1]", "MatchedHit_Y[1]", "Lextra_X[1]", "Lextra_Y[1]", "MatchedHit_DX[1]", "MatchedHit_DY[1]",
                 "MatchedHit_X[2]", "MatchedHit_Y[2]", "Lextra_X[2]", "Lextra_Y[2]", "MatchedHit_DX[2]", "MatchedHit_DY[2]",
                 "MatchedHit_X[3]", "MatchedHit_Y[3]", "Lextra_X[3]", "Lextra_Y[3]", "MatchedHit_DX[3]", "MatchedHit_DY[3]"),

#define normed_err_feature(DIM, LAYER) \
        make_feature(normed_err, MH(DIM, LAYER), E(DIM, LAYER), ED(DIM, LAYER))

    normed_err_feature(X, 0),
    normed_err_feature(X, 1),
    normed_err_feature(X, 2),
    normed_err_feature(X, 3),
    normed_err_feature(Y, 0),
    normed_err_feature(Y, 1),
    normed_err_feature(Y, 2),
    normed_err_feature(Y, 3)
);
