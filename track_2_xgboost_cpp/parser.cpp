#include <algorithm>
#include <cassert>
#include <sstream>

#include "parser.h"
#include "reference_header.h"

inline bool not_number(const char pen) {
    return !isdigit(pen) && !(pen == '.') && !(pen == '-');
}

void skip_to_number_pointer(const char *& pen) {
    while ((*pen) && not_number(*pen)) ++pen;
}

inline float square(const float x) {
    return x*x;
}

// https://stackoverflow.com/questions/5678932/fastest-way-to-read-numerical-values-from-text-file-in-c-double-in-this-case
template<class T>
T rip_uint_pointer(const char *&pen, T val = 0) {
    // Will return val if *pen is not a digit
    // WARNING: no overflow checks
    for (char c; (c = *pen ^ '0') <= 9; ++pen)
        val = val * 10 + c;
    return val;
}

template<class T>
T rip_float_pointer(const char *&pen) {
    static double const exp_lookup[]
        = {1, 0.1, 1e-2, 1e-3, 1e-4, 1e-5, 1e-6, 1e-7, 1e-8, 1e-9, 1e-10,
           1e-11, 1e-12, 1e-13, 1e-14, 1e-15, 1e-16, 1e-17};
    T sign = 1.;
    if (*pen == '-') {
        ++pen;
        sign = -1.;
    }
    uint64_t val = rip_uint_pointer<uint64_t>(pen);
    unsigned int neg_exp = 0;
    if (*pen == '.') {
        const char* const fracs = ++pen;
        val = rip_uint_pointer(pen, val);
        neg_exp  = pen - fracs;
    }
    return std::copysign(val*exp_lookup[neg_exp], sign);
}

// Warning: this is not a general-puropse parser, you have
// std::istream for that. As a rule, in the interest of speed, it
// doesn't check for input correctness and will have undefined
// behavior at incorrect input
class BufferedStream {
 public:
    explicit BufferedStream(std::istream& stream)
        :stream(stream)
    {
        next_line();
    }
    // Discards data from the stream until ecountering a digit, "." or "-"
    void skip_to_number() {
        skip_to_number_pointer(pen);
        while ((*pen) == 0) {
            next_line();
            skip_to_number_pointer(pen);
            // The skip stops either at 0-byte or
            // a number part
        }
    }
    // Reads a float from the stream, starting from the current character
    // and has undefined behaviour if there is no number at the
    // current position
    template<class T>
    T rip_float() {
        return rip_float_pointer<T>(pen);
    }
    // Reads an unsigned integer from stream, starting from the
    // current character and has undefined behaviour if there is no
    // number at the current position
    template<class T>
    T rip_uint() {
        return rip_uint_pointer<T>(pen);
    }
    // Reads a vector of floats of the given size from the stream,
    // skipping everything as needed
    template<class T>
    std::vector<T> fill_vector_float(const size_t size) {
        std::vector<T> result(size);
        fill_iterator_float<std::vector<float>::iterator>(result.begin(), size);
        return result;
    }

    // Reads a vector of unsigned ints of the given size from the stream,
    // skipping as needed. In principle, should be templated from
    // fill_vector_float, but if constexpr is C++17 :(
    template<class T>
    std::vector<T> fill_vector_uint(const size_t size) {
        std::vector<T> result(size);
        for (auto& value : result) {
            skip_to_number();
            value = rip_uint<T>();
        }
        return result;
    }
    // Reads count floating point numbers and stores them into the
    // container pointed to by the iterator
    template<class IteratorType>
    void fill_iterator_float(const IteratorType& iterator, const size_t count) {
        for (IteratorType value = iterator; value != iterator + count; ++value) {
            skip_to_number();
            *value = rip_float<typename std::iterator_traits<IteratorType>::value_type>();
        }
    }
    // Discards data from the stream until encountering the delimiter
    void skip_to_char(const char delimiter) {
        while ((*pen) != delimiter) {
            while ((*pen) && (*(++pen)) != delimiter) {}
            if (!(*pen)) next_line();
        }
    }
    // Discrads data from the stream until twice encountering the delimiter
    void skip_record(const char delimiter) {
        skip_to_char(delimiter);
        ++pen;
        skip_to_char(delimiter);
    }

 private:
    void next_line() {
        stream.getline(buffer, BUFFER_SIZE);
        pen = buffer;
    }
    // Buffer size is measured to fit the longest line in the test dataset
    // but the code doesn't rely on it
    static const size_t BUFFER_SIZE = 1016;
    char buffer[BUFFER_SIZE];
    std::istream& stream;
    const char* pen;
};

static std::vector<std::string> init_all_columns() {
    std::vector<std::string> result;
    std::istringstream stream(REFERENCE_HEADER);
    std::string s;
    while (getline(stream, s, DELIMITER)) {
        if (s != "id") {
            result.push_back(s);
        }
    }
    assert(result.size() == N_FEATURES);

    return result;
}

static std::vector<std::string> all_columns = init_all_columns();

static std::vector<size_t> calc_perm(const std::vector<std::string>& names) {
    std::vector<size_t> result;
    for (const auto& name : names) {
        if (name.substr(0, 3) == "FOI" && name != "FOI_hits_N") {
            throw std::invalid_argument("FOI columns not implemented");
        }

        auto it = std::find(all_columns.begin(), all_columns.end(), name);
        if (it != all_columns.end()) {
            result.push_back(it - all_columns.begin());
        } else {
            throw std::invalid_argument("Unknown name " + name);
        }
    }
    return result;
}


Index::Index(const std::vector<std::string>& columns)
    : names(columns)
    , perm(calc_perm(columns))
{
    (void)names;
}

void Parser::read_one(size_t& id, std::vector<float>& result)
{
    static std::vector<float> data(N_FEATURES);
    BufferedStream buffered_stream(stream);
    id = buffered_stream.rip_uint<size_t>();
    buffered_stream.fill_iterator_float(&data[0], N_RAW_FEATURES_HEAD);
    // No need to skip, fill_vector takes care of it
    // const size_t FOI_hits_N = data[FOI_HITS_N_INDEX];
    buffered_stream.skip_record(DELIMITER); // FOI_hits_X
    buffered_stream.skip_record(DELIMITER); // FOI_hits_Y
    buffered_stream.skip_record(DELIMITER); // FOI_hits_Z
    buffered_stream.skip_record(DELIMITER); // FOI_hits_DX
    buffered_stream.skip_record(DELIMITER); // FOI_hits_DY
    buffered_stream.skip_record(DELIMITER); // FOI_hits_DZ
    buffered_stream.skip_record(DELIMITER); // FOI_hits_T
    buffered_stream.skip_record(DELIMITER); // FOI_hits_DT
    buffered_stream.skip_record(DELIMITER); // FOI_hits_S
    buffered_stream.fill_iterator_float(
            &data[0] + N_FEATURES - N_RAW_FEATURES_TAIL, N_RAW_FEATURES_TAIL);

    result.reserve(index.size());
    for (auto it = index.perm_begin(); it != index.perm_end(); ++it) {
        result.push_back(data[*it]);
    }
}
