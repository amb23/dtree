#pragma once

#include <numeric>
#include <sstream>
#include <variant>

#include "dtree/concepts.h"
#include "dtree/types.h"

namespace dtree {

struct single_numeric_splitting {
    template <single_numeric_sample_c single_numeric_sample_t>
    bool operator()(single_numeric_sample_t val) const
    {
        return val <= split;
    }
    bool operator==(const single_numeric_splitting&) const = default;

    double split;
};

struct multi_numeric_splitting {
    template <multi_numeric_sample_c multi_numeric_sample_t>
    bool operator()(const multi_numeric_sample_t& vals) const
    {
        using namespace std;
        double norm_v = inner_product(begin(vals), end(vals), begin(normal), 0.0);
        return norm_v <= split;
    }
    bool operator==(const multi_numeric_splitting&) const = default;

    std::vector<double> normal;
    double split;
};

struct has_substring_splitting {
    bool operator()(const std::string& string) const
    {
        return string.find(substring) != std::string::npos;
    }

    bool operator==(const has_substring_splitting&) const = default;

    std::string substring;
};

struct string_length_splitting {
    bool operator()(const std::string& string) const { return string.size() <= length; }

    bool operator==(const string_length_splitting&) const = default;

    std::size_t length;
};

template <typename... splitting_ts> struct splitting_variant {
    template <typename... Ts> bool operator()(const std::variant<Ts...>& sample) const
    {
        return std::visit([this](const auto& t) -> bool { return apply<0>(t); }, sample);
    }

    bool operator==(const splitting_variant&) const = default;

    template <typename T> bool operator()(const T& val) const { return apply<0>(val); }

    std::variant<splitting_ts...> splittings;

private:
    template <std::size_t index, typename T> bool apply(const T& val) const
    {
        if constexpr (index < sizeof...(splitting_ts)) {
            using splitting_t = element_t<index, splitting_ts...>;
            if constexpr (std::is_invocable_v<splitting_t, T>) {
                if (index == splittings.index()) {
                    const auto& impl = std::get<index>(splittings);
                    return impl(val);
                }
                return apply<index + 1>(val);
            } else {
                return apply<index + 1>(val);
            }
        } else {
            std::stringstream msg;
            msg << "The set splitting at index[" << splittings.index()
                << "] cannot handle the passed sample type <" << typeid(T).name() << ">";
            throw std::runtime_error { msg.str() };
        }
    }
};

} // dtree
