
#include "dtree/labels.h"


namespace dtree {

LabelCounts Labels::CountLabels(const std::vector<label_t>& labels)
{
    LabelCounts labelCounts;

    for (auto label : labels)
    {
        if (label >= labelCounts.size())
        {
            labelCounts.resize(label + 1, 0u);
        }

        labelCounts[label]++;
    }

    return labelCounts;
}

} // namespace dtree
