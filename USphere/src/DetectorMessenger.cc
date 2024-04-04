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
/// \file B1/src/DetectorMessenger.cc
/// \brief Implementation of the B1::DetectorMessenger class

#include "DetectorMessenger.hh"
#include "DetectorConstruction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::DetectorMessenger(DetectorConstruction *det)
 : fDetectorConstruction(det)
{
  fDetDirectory = new G4UIdirectory("/det/");
  fDetDirectory->SetGuidance("Detector construction control");

  auto setRadius = new G4UIcmdWithADoubleAndUnit("/det/setRadius", this);
  setRadius->SetGuidance("Specify Sphere radius.");
  setRadius->SetParameterName("radius", false);
  setRadius->AvailableForStates(G4State_PreInit, G4State_Idle);
  fSetRadius = setRadius;

  auto setU235Enrichment = new G4UIcmdWithADouble("/det/setU235Enrichment", this);
  setU235Enrichment->SetGuidance("Specify U235 enrichment.");
  setU235Enrichment->SetParameterName("enrichment", false);
  setU235Enrichment->AvailableForStates(G4State_PreInit, G4State_Idle);
  fSetU235Enrichment = setU235Enrichment;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::~DetectorMessenger()
{
  delete fSetU235Enrichment;
  delete fSetRadius;
  delete fDetDirectory;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorMessenger::SetNewValue(G4UIcommand *command, G4String newValue)
{
  if(command == fSetRadius) {
    fDetectorConstruction->SetRadius(fSetRadius->GetNewDoubleValue(newValue));
    return;
  }

  if(command == fSetU235Enrichment) {
    fDetectorConstruction->SetU235Enrichment(fSetU235Enrichment->GetNewDoubleValue(newValue));
    return;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
