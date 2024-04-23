#include "LiseGenerator.hh"
#include "LiseDetector.hh"
#include "LiseHelper.hh"
#include "random.hh"
#include <string>
#include <stdexcept>
#include <sstream>
#include <unordered_map>
#include <TFile.h>
#include <TTree.h>
#include <math.h>

using namespace std;

LiseGenerator::LiseGenerator(const char *path, double emin, double emax)
  : edist_(emin, emax)
{
  CreateTree(path);
}

LiseGenerator::~LiseGenerator()
{
  DestroyTree();
  for(LiseDetector *detector : detectors_) {
    delete detector;
  }
}

void LiseGenerator::AddDetector(LiseDetector *detector)
{
  ParseBeam(detector->GetHelper()->GetBeam());
  detectors_.push_back(detector);
}

void LiseGenerator::GenerateEvent()
{
  E_.reserve(detectors_.size());
  double energy = edist_(thread_random_engine);
  E_.push_back(energy);
  energy /= A_;
  for(LiseDetector *detector : detectors_) {
    double eloss = detector->GetEnergyLoss(energy);
    energy -= eloss;
    eloss *= A_;
    eloss += GetEnergyUncertainty(eloss) * norm_(thread_random_engine);
    E_.push_back(eloss);
  }
  FillTree();
}

void LiseGenerator::GenerateEvents(size_t n)
{
  while(n--) GenerateEvent();
}

void LiseGenerator::CreateTree(const char *path)
{
  file_ = new TFile(path, "NEW");
  if(!file_->IsOpen()) {
    throw runtime_error(string("Failed to open file: ") + path);
  }

  tree_ = new TTree("tree", "LISE simulation");
  tree_->Branch("Z", &Z_);
  tree_->Branch("A", &A_);
  tree_->Branch("E", &E_);

  Z_ = 0;  // Undetermined.
  A_ = 0;
}

void LiseGenerator::FillTree()
{
  tree_->Fill();
  E_.clear();
}

void LiseGenerator::DestroyTree()
{
  file_->cd();
  tree_->Write();
  delete tree_;
  delete file_;
}

void LiseGenerator::ParseBeam(const string &beam)
{
  static const unordered_map<string, int> name_to_Z = {
    {"Be", 4},
    {"B" , 5},
    {"C" , 6},
  };
  istringstream iss(beam);
  int A;
  string name;
  if(!(iss >> A >> name)) {
    throw runtime_error("Failed to parse beam: " + beam);
  }
  int Z = name_to_Z.at(name);
  if(Z_ == 0) {
    Z_ = Z, A_ = A;
  } else if(Z_ != Z || A_ != A) {
    throw runtime_error("Inconsistent beam: " + beam);
  }
}

double LiseGenerator::GetEnergyUncertainty(double energy)
{
  return 0.01 * sqrt(energy);
}
