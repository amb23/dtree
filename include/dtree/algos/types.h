#pragma once

namespace dtree::algos {

struct one_dimensional_split_result {
    double cost;
    double split;
};

struct multi_dimensional_split_result {
    double cost;
    std::vector<double> normal;
    double split;
};

} // dtree::algos
