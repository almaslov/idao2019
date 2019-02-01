#pragma once

#include "index.h"


template<typename Func, std::size_t... I>
auto make_feature_impl(Func f, Index index, std::index_sequence<I...>) {
    return [=](const std::vector<float>& v) {
        return f(v[index[I]]...);
    };
}

template<typename Func, class... Args>
auto make_feature(Func f, Args... args) {
    Index index({args...});
    return make_feature_impl(f, index, std::make_index_sequence<sizeof...(args)>());
}

template<class... Args, std::size_t... I>
void add_features_impl(std::vector<float>& v, std::tuple<Args...> f, std::index_sequence<I...>) {
    std::initializer_list<float> l {std::get<I>(f)(v) ...};
    v.insert(v.end(), l.begin(), l.end());
}

template<class... Args>
void add_features(std::vector<float>& v, std::tuple<Args...> f) {
    add_features_impl(v, f, std::make_index_sequence<sizeof...(Args)>());
}
