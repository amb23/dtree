#pragma once

#include <vector>


namespace dtree {

using LabelCounts = std::vector<std::size_t>;

using LabelDistribution = std::vector<double>;


class Labels
{
public:
    using label_t = std::size_t; // FIXME - make more compact


    template <typename... Args>
    explicit Labels(Args&&... args)
        : mData{std::forward<Args>(args)...}
        , mLabelCounts{CountLabels(mData)}
    {}
    
    auto begin() const { return mData.begin(); }
    auto end() const { return mData.end(); }

    label_t operator[] (std::size_t loc) const { return mData[loc]; }

    std::size_t NumberOfLabels() const { return mLabelCounts.size(); }

    const LabelCounts& GetLabelCounts() const { return mLabelCounts; }

    LabelDistribution CalculateDistribution() const
    {
        LabelDistribution labelDistribution(mLabelCounts.size(), 0.0);
        double totalCount = mData.size();

        for (std::size_t i = 0; i < mLabelCounts.size(); ++i)
        {
            labelDistribution[i] = static_cast<double>(mLabelCounts[i]) / totalCount;
        }

        return labelDistribution;
    }

    void push_back(label_t label)
    {
        mData.push_back(label);

        if (label >= mLabelCounts.size())
        {
            mLabelCounts.resize(label + 1, 0u);
        }

        mLabelCounts[label]++;
    }

    std::size_t size() const { return mData.size(); }

private:
    
    static LabelCounts CountLabels(const std::vector<label_t>&);

    std::vector<label_t> mData;

    LabelCounts mLabelCounts;
};


} // namespace dtree
