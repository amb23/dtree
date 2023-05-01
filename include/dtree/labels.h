#pragma once

#include "dtree/types.h"

namespace dtree {

// FIXME - do we want a labels type or do we want a
//         to just have methods around the labels?
class labels {
public:
    using label_t = std::size_t; // FIXME - make more compact

    using value_type = label_t;
    using reference = value_type; // FIXME

    template <typename... Args>
    explicit labels(Args&&... args)
        : m_data { std::forward<Args>(args)... }
        , m_label_counts { count_labels(m_data) }
    {
    }

    void reserve(std::size_t size) { m_data.reserve(size); }

    auto begin() const { return m_data.begin(); }
    auto end() const { return m_data.end(); }

    label_t operator[](std::size_t loc) const { return m_data[loc]; }

    std::size_t number_of_labels() const { return m_label_counts.size(); }

    const label_counts& get_label_counts() const { return m_label_counts; }

    label_distribution calculate_distribution() const
    {
        label_distribution out(m_label_counts.size(), 0.0);
        double total_count = m_data.size();

        for (std::size_t i = 0; i < m_label_counts.size(); ++i) {
            out[i] = static_cast<double>(m_label_counts[i]) / total_count;
        }

        return out;
    }

    void push_back(label_t label)
    {
        m_data.push_back(label);

        if (label >= m_label_counts.size()) {
            m_label_counts.resize(label + 1, 0u);
        }

        m_label_counts[label]++;
    }

    std::size_t size() const { return m_data.size(); }

private:
    static label_counts count_labels(const std::vector<label_t>&);

    std::vector<label_t> m_data;

    label_counts m_label_counts;
};

} // namespace dtree
