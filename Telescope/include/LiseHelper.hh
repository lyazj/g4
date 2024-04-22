#pragma once
#include <string>
#include <vector>

class TGraph;

class LiseHelper {

public:
  explicit LiseHelper(const std::string &path);
  ~LiseHelper();
  LiseHelper(const LiseHelper &) = delete;
  LiseHelper(LiseHelper &&);

  static std::vector<std::string> ListItem();
  static std::string GetEnergyUnit() { return "MeV/u"; }
  static std::string GetDepthUnit() { return "um"; }

  const std::string &GetPath() const { return path_; }
  const std::string &GetBeam() const { return beam_; }
  const std::string &GetTarget() const { return target_; }
  TGraph *GetE2x() const { return E2x_; }
  TGraph *GetX2E() const { return x2E_; }

private:
  std::string path_;
  std::string beam_;
  std::string target_;

  TGraph *E2x_;  // E: Energy per nucleon.
  TGraph *x2E_;  // x: Penetration depth.

};
