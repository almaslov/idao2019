#pragma once

#include <functional>
#include <array>

#include "index.h"


using Features = std::vector<float>;
using feature_func = std::function<float(const Features&)>;

struct ComplexFeature {
    feature_func f;
    std::string func_name;
    std::vector<std::string> param_names;

    float operator()(const Features& v) {
        return f(v);
    }
};


template<typename Func, std::size_t... I>
feature_func _make_func(Func f, Index index, std::index_sequence<I...>) {
    return [=](const Features& v) {
        return f(v[index[I]]...);
    };
}

template<typename Func, class... Args>
ComplexFeature _make_complex_feature(Func f, const std::string& name, Args... args) {
    Index index({args...});
    ComplexFeature feature;

    feature.f = _make_func(f, index, std::make_index_sequence<sizeof...(args)>());
    feature.func_name = name;
    feature.param_names = std::vector<std::string>({args...});

    return feature;
}

#define make_complex_feature(f, ...) _make_complex_feature(f, #f, __VA_ARGS__)

template<class... Args, std::size_t... I>
void add_features_impl(Features& v, Features& params, std::tuple<Args...> f, std::index_sequence<I...>) {
    std::array<float, sizeof...(I)> l = {std::get<I>(f)(params) ...};
    v.insert(v.end(), l.begin(), l.end());
}

template<class... Args>
void add_features(Features& v, Features& params, std::tuple<Args...> f) {
    add_features_impl(v, params, f, std::make_index_sequence<sizeof...(Args)>());
}
