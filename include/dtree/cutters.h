#pragma once


#include "dtree/detail/cuttingAlgorithms.h"

namespace dtree {

using CutResult = detail::CutResult;

using OptimalCut = detail::OneDimensionalCut<detail::OptimalCutter>;
using RandomCut = detail::OneDimensionalCut<detail::RandomCutter>;
using MedianCut = detail::OneDimensionalCut<detail::MedianCutter>;
using AverageCut = detail::OneDimensionalCut<detail::AverageCutter>;

} // dtree
