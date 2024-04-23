#include "LiseDetector.hh"
#include "LiseHelper.hh"
#include <TGraph.h>

LiseDetector::LiseDetector(LiseHelper *helper, double depth)
{
  helper_ = helper;
  depth_ = depth;
}

LiseDetector::~LiseDetector()
{
  // Empty.
}

LiseDetector::LiseDetector(LiseDetector &&detector)
{
  helper_ = detector.helper_;
  detector.helper_ = nullptr;
  depth_ = detector.depth_;
}

double LiseDetector::GetEnergyLoss(double energy) const
{
  double init_energy = energy;
  double init_depth = helper_->GetE2x()->Eval(init_energy);
  double fini_depth = init_depth - depth_;
  if(fini_depth < 0.0) return energy;
  double fini_energy = helper_->GetX2E()->Eval(fini_depth);
  return init_energy - fini_energy;
}
