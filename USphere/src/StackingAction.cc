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
/// \file B1/src/StackingAction.cc
/// \brief Implementation of the B1::StackingAction class

#include "StackingAction.hh"

#include "G4Track.hh"
#include "G4Neutron.hh"

namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ClassificationOfNewTrack
StackingAction::ClassifyNewTrack(const G4Track* track)
{
  // Select neutrons only.
  if(track->GetDefinition() != G4Neutron::Neutron()) return fUrgent;

  // Record time and generation first.
  G4double globalTime = GetAndRecordGlobalTime(track);
  G4int generation = GetAndRecordGeneration(track);

  // Either time or generation exceeding triggers a kill.
  if(fMaxGlobalTime && globalTime > fMaxGlobalTime) return fKill;
  if(fMaxGeneration && generation > fMaxGeneration) return fKill;
  return fUrgent;
}

void StackingAction::ResetRecords()
{
  fGenerationMap.clear();
  fGlobalTimeMap.clear();
}

G4int StackingAction::GetAndRecordGeneration(const G4Track *track)
{
  // Memoization.
  G4int ID = track->GetTrackID();
  auto it = fGenerationMap.find(ID);
  if(it != fGenerationMap.end()) return it->second;

  // Compute generation by adding 1 to the value of its parent.
  // The parent must have been recorded by an earlier call to this method.
  G4int parentID = track->GetParentID();
  G4int generation = 1;
  if(parentID) generation += fGenerationMap.at(parentID);
  fGenerationMap.emplace(ID, generation);
  return generation;
}

G4double StackingAction::GetAndRecordGlobalTime(const G4Track *track)
{
  return fGlobalTimeMap[track->GetTrackID()] = track->GetGlobalTime();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}

