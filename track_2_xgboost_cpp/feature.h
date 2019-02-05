#pragma once

#include <functional>

#include "index.h"

using feature_func = std::function<float(const std::vector<float>&)>;

struct Feature {
    feature_func f;
    std::string func_name;
    std::vector<std::string> param_names;

    float operator()(const std::vector<float>& v) {
        return f(v);
    }
};


template<typename Func, std::size_t... I>
feature_func _make_feature_impl(Func f, Index index, std::index_sequence<I...>) {
    return [=](const std::vector<float>& v) {
        return f(v[index[I]]...);
    };
}

template<typename Func, class... Args>
Feature _make_feature(Func f, const std::string& name, Args... args) {
    Index index({args...});
    Feature feature;

    feature.f = _make_feature_impl(f, index, std::make_index_sequence<sizeof...(args)>());
    feature.func_name = name;
    feature.param_names = std::vector<std::string>({args...});

    return feature;
}

#define make_feature(f, ...) _make_feature(f, #f, __VA_ARGS__)

template<class... Args, std::size_t... I>
void add_features_impl(std::vector<float>& v, std::vector<float>& params, std::tuple<Args...> f, std::index_sequence<I...>) {
    std::initializer_list<float> l {std::get<I>(f)(params) ...};
    v.insert(v.end(), l.begin(), l.end());
}

template<class... Args>
void add_features(std::vector<float>& v, std::vector<float>& params, std::tuple<Args...> f) {
    add_features_impl(v, params, f, std::make_index_sequence<sizeof...(Args)>());
}
