
#include "dtree/labels.h"

namespace dtree {

label_counts labels::count_labels(const std::vector<label_t>& labels)
{
    label_counts out;

    for (auto label : labels) {
        if (label >= out.size()) {
            out.resize(label + 1, 0u);
        }

        out[label]++;
    }

    return out;
}

} // namespace dtree
