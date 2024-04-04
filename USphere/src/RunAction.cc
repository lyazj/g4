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
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4ParticleGun.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

#include <TFile.h>
#include <TTree.h>
#include <stdexcept>
#include <vector>
#include <utility>
#include <algorithm>

namespace B1
{

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

  InitializeTree();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run* run)
{
  DestroyTree();

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
  fTree->Fill();

  // Reset stacking controller.
  fStackingAction->ResetRecords();

  // Clean up.
  fNeutronGeneration.clear();
  fNeutronGlobalTime.clear();
}

void RunAction::SaveTree()
{
  fTree->AutoSave("SaveSelf, Overwrite");
}

void RunAction::InitializeTree()
{
  fFile = new TFile(fFileName, "RECREATE");
  fTree = new TTree(fTreeName, fTreeName);
  fTree->Branch("NeutronGeneration", &fNeutronGeneration);
  fTree->Branch("NeutronGlobalTime", &fNeutronGlobalTime);
}

void RunAction::DestroyTree()
{
  fFile->cd();
  fTree->Write(fTreeName, fTree->kOverwrite);
  delete fTree;
  delete fFile;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
