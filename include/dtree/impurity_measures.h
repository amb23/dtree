#pragma once

#include "dtree/labels.h"

namespace dtree {

double gini_index(const label_counts&);

double entropy(const label_counts&);

} // namespace dtree
