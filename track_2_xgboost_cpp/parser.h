// Copyright 2019, Nikita Kazeev, Higher School of Economics
#pragma once
#include <ctype.h>
#include <cstdint>
#include <cmath>

#include <iostream>
#include <vector>
#include <array>
#include <limits>

const size_t N_STATIONS = 4;
//const size_t FOI_FEATURES_PER_STATION = 6;
// The structure of .csv is the following:
// id, <62 float features>, number of hits in FOI, <9 arrays of FOI hits features>, <2 float features>
const size_t N_FEATURES = 74;
const size_t N_RAW_FEATURES_HEAD = 63;
const size_t N_FOI_FEATURES = 9;
const size_t N_RAW_FEATURES_TAIL = 2;

const size_t FOI_HITS_N_INDEX = 62;
//const size_t LEXTRA_X_INDEX = 45;
//const size_t LEXTRA_Y_INDEX = 49;
//const size_t FOI_FEATURES_START = N_RAW_FEATURES;
//const float EMPTY_FILLER = 1000;

const char DELIMITER = ',';


class Index {
    typedef std::vector<size_t> PermType;
private:
    const std::vector<std::string>& names;
    const PermType perm;

public:
    Index(const std::vector<std::string>& columns);

    size_t size() const {
        return perm.size();
    }

    PermType::const_iterator perm_begin() const {
        return perm.begin();
    }

    PermType::const_iterator perm_end() const {
        return perm.end();
    }
};


class Parser {
    const Index& index;
    std::istream& stream;

public:
    Parser(const Index& index, std::istream& stream)
        : index(index)
        , stream(stream)
    {
    }

    void read_one(size_t& id, std::vector<float>& result);
};
