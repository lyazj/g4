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
/// \file B1/include/StackingAction.hh
/// \brief Definition of the B1::StackingAction class

#ifndef B1StackingAction_h
#define B1StackingAction_h 1

#include "G4UserStackingAction.hh"
#include "G4SystemOfUnits.hh"
#include "globals.hh"
#include <unordered_map>

namespace B1
{

class RunAction;

/// Stacking action class : manage the newly generated particles
///
/// One wishes do not track secondary neutrino.Therefore one kills it
/// immediately, before created particles will  put in a stack.

class StackingAction : public G4UserStackingAction
{
    friend class RunAction;

  public:
    StackingAction() = default;
    ~StackingAction() override = default;

    G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track*) override;

    void ResetRecords();

  private:
    std::unordered_map<G4int, G4int> fGenerationMap;
    std::unordered_map<G4int, G4double> fGlobalTimeMap;
    G4int fMaxGeneration = 100;  // 0: unset
    G4double fMaxGlobalTime = 6000 * ns;  // 0: unset

    G4int GetAndRecordGeneration(const G4Track *track);
    G4double GetAndRecordGlobalTime(const G4Track *track);
};

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

