#pragma once


class Index {
    typedef std::vector<size_t> PermType;
public:
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

    size_t operator[](size_t i) const {
        return perm[i];
    }

    void select(std::vector<float>& data, std::vector<float>* result) const;
};
