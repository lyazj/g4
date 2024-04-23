#pragma once

class LiseHelper;

class LiseDetector {

public:
  LiseDetector(LiseHelper *helper, double depth);
  ~LiseDetector();
  LiseDetector(const LiseDetector &) = delete;
  LiseDetector(LiseDetector &&);

  LiseHelper *GetHelper() const { return helper_; }
  double GetDepth() const { return depth_; }
  void SetDepth(double depth) { depth_ = depth; }

  double GetEnergyLoss(double energy) const;

private:
  LiseHelper *helper_;  // Not owned.
  double depth_;

};
