#pragma once
#include <vector>
#include <random>
#include <string>
#include <Rtypes.h>

class LiseDetector;
class TFile;
class TTree;

class LiseGenerator {

public:
  LiseGenerator(const char *path, double emin, double emax);
  ~LiseGenerator();
  LiseGenerator(const LiseGenerator &) = delete;

  void AddDetector(LiseDetector *detector);
  size_t GetNDetector() const { return detectors_.size(); }
  LiseDetector *GetDetector(size_t index) const { return detectors_.at(index); }

  void GenerateEvent();
  void GenerateEvents(size_t n);

private:
  std::uniform_real_distribution<double> edist_;
  std::normal_distribution<double> norm_;
  std::vector<LiseDetector *> detectors_;  // Owned.

  TFile *file_;  // Owned.
  TTree *tree_;  // Owned.
  Int_t Z_;
  Int_t A_;
  std::vector<Double_t> E_;

  void CreateTree(const char *path);
  void DestroyTree();
  void FillTree();

  void ParseBeam(const std::string &);
  static double GetEnergyUncertainty(double energy);

};
