//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file B1/src/RunAction.cc
/// \brief Implementation of the B1::RunAction class

#include "RunAction.hh"
#include "StackingAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4AccumulableManager.hh"
#include "G4ParticleGun.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4Timer.hh"

#include <TFile.h>
#include <TTree.h>
#include <stdexcept>
#include <vector>
#include <utility>
#include <algorithm>

namespace B1
{

G4Mutex RunAction::fTreeMutex;
G4String RunAction::fFileName = "USphere";
G4String RunAction::fTreeName = "tree";
TFile *RunAction::fFile;
TTree *RunAction::fTree;
G4Timer *RunAction::fTimer;
G4double RunAction::fTimeElapsed;
G4double RunAction::fTimeElapsedTotal;
G4double RunAction::fAutoSaveTimeSpan = 10.0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction(StackingAction *stackingAction)
  : fStackingAction(stackingAction)
{
  // add new units for dose
  //
  const G4double milligray = 1.e-3*gray;
  const G4double microgray = 1.e-6*gray;
  const G4double nanogray  = 1.e-9*gray;
  const G4double picogray  = 1.e-12*gray;

  new G4UnitDefinition("milligray", "milliGy" , "Dose", milligray);
  new G4UnitDefinition("microgray", "microGy" , "Dose", microgray);
  new G4UnitDefinition("nanogray" , "nanoGy"  , "Dose", nanogray);
  new G4UnitDefinition("picogray" , "picoGy"  , "Dose", picogray);

  //// Register accumulable to the accumulable manager
  //G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  //accumulableManager->RegisterAccumulable(fEdep);
  //accumulableManager->RegisterAccumulable(fEdep2);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run*)
{
  // inform the runManager to save random number seed
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);

  // reset accumulables to their initial values
  G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  accumulableManager->Reset();

  if(G4Threading::IsMasterThread()) InitializeTree();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run* run)
{
  if(G4Threading::IsMasterThread()) DestroyTree();

  // Merge accumulables
  G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  accumulableManager->Merge();

  // Run conditions
  //  note: There is no primary generator action object for "master"
  //        run manager for multi-threaded mode.
  const auto generatorAction = static_cast<const PrimaryGeneratorAction*>(
    G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
  G4String runCondition;
  if (generatorAction)
  {
    const G4ParticleGun* particleGun = generatorAction->GetParticleGun();
    runCondition += particleGun->GetParticleDefinition()->GetParticleName();
    runCondition += " of ";
    G4double particleEnergy = particleGun->GetParticleEnergy();
    runCondition += G4BestUnit(particleEnergy, "Energy");
  }
  if(runCondition.empty()) runCondition = "<empty>";

  // Print
  //
  if (IsMaster()) {
    G4cout
     << G4endl
     << "--------------------End of Global Run-----------------------";
  }
  else {
    G4cout
     << G4endl
     << "--------------------End of Local Run------------------------";
  }

  G4cout
     << G4endl
     << " The run consists of " << run->GetNumberOfEvent() << " " << runCondition
     << G4endl
     << "------------------------------------------------------------"
     << G4endl
     << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::FillTree()
{
  // Adapt size.
  if(fStackingAction->fGenerationMap.size() != fStackingAction->fGlobalTimeMap.size()) {
    throw std::logic_error("bad stacking records");
  }
  fNeutronGeneration.reserve(fStackingAction->fGenerationMap.size());
  fNeutronGlobalTime.reserve(fStackingAction->fGenerationMap.size());

  // Sort data by generation and ID.
  std::vector<std::pair<G4int, G4int>> generationAndIDs;
  generationAndIDs.reserve(fStackingAction->fGenerationMap.size());
  for(auto [ID, generation] : fStackingAction->fGenerationMap) {
    generationAndIDs.emplace_back(generation, ID);
  }
  sort(generationAndIDs.begin(), generationAndIDs.end());

  // Transcript data.
  for(auto [generation, ID] : generationAndIDs) {
    G4double globalTime = fStackingAction->fGlobalTimeMap.at(ID);
    fNeutronGeneration.push_back(generation);
    fNeutronGlobalTime.push_back(globalTime / ns);
  }

  // Output data.
  {
    G4AutoLock lock(fTreeMutex);

    // Fill tree.
    *(void **)fTree->GetBranch("NeutronGeneration")->GetAddress() = (void *)&fNeutronGeneration;
    *(void **)fTree->GetBranch("NeutronGlobalTime")->GetAddress() = (void *)&fNeutronGlobalTime;
    fTree->Fill();

    // Save tree periodically.
    fTimer->Stop();
    fTimeElapsed += fTimer->GetRealElapsed();
    fTimer->Start();
    if(fTimeElapsed > fAutoSaveTimeSpan) SaveTree();
  }

  // Reset stacking controller.
  fStackingAction->ResetRecords();

  // Clean up.
  fNeutronGeneration.clear();
  fNeutronGeneration.shrink_to_fit();
  fNeutronGlobalTime.clear();
  fNeutronGlobalTime.shrink_to_fit();
}

void RunAction::InitializeTree()
{
  auto detectorConstruction = (DetectorConstruction *)
    G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  G4double radius = detectorConstruction->GetRadius() / cm;
  fFile = new TFile((fFileName + "-" + std::to_string(radius) + ".root").c_str(), "NEW");
  if(!fFile->IsOpen()) throw std::runtime_error("error opening output file: " + fFileName);
  fTree = new TTree(fTreeName, fTreeName);
  fTree->Branch("NeutronGeneration", &fNeutronGeneration);
  fTree->Branch("NeutronGlobalTime", &fNeutronGlobalTime);
  fTimer = new G4Timer;
  fTimer->Start();
}

void RunAction::SaveTree()
{
  fTimeElapsedTotal += fTimeElapsed;
  fTimeElapsed = 0.0;
  G4cout << "AutoSave: saving " << fTree->GetEntries() << " events ("
         << fTimeElapsedTotal << " secs elapsed)" << G4endl;
  fTree->AutoSave("SaveSelf, Overwrite");
}

void RunAction::DestroyTree()
{
  delete fTimer;
  fFile->cd();
  fTree->Write(fTreeName, fTree->kOverwrite);
  delete fTree;
  delete fFile;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
