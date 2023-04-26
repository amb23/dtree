#pragma once

#include "dtree/labels.h"


namespace dtree {

double GiniIndex(const LabelCounts&);

double Entropy(const LabelCounts&);


} // namespace dtree
