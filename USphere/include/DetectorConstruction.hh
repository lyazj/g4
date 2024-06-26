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
/// \file B1/include/DetectorConstruction.hh
/// \brief Definition of the B1::DetectorConstruction class

#ifndef B1DetectorConstruction_h
#define B1DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4Material;

namespace B1
{

class DetectorMessenger;

/// Detector construction class to define materials and geometry.

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction();
    ~DetectorConstruction();

    G4VPhysicalVolume *Construct() override;

    void SetRadius(G4double radius);
    G4double GetRadius() const { return fRadius; }
    void SetU235Enrichment(G4double enrichment);
    G4double GetU235Enrichment() const { return fU235Enrichment; }

  protected:
    DetectorMessenger *fMessenger;

    G4double fRadius = 8.7407 * cm;
    G4double fU235Enrichment = 93.71;

    G4LogicalVolume *fWorld = nullptr;
    G4LogicalVolume *fSphere = nullptr;

    G4Material *GetUMaterial(G4double U235Enrichment) const;
};

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
