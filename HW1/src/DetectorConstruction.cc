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
/// \file B1/src/DetectorConstruction.cc
/// \brief Implementation of the B1::DetectorConstruction class

#include "DetectorConstruction.hh"

#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"

namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  /*
   * Tunable options and outer dependencies.
   */
  G4bool checkOverlaps = true;
  G4NistManager *nist = G4NistManager::Instance();
  G4Material *worldMaterial = nist->FindOrBuildMaterial("G4_AIR");
  G4Material *bodyMaterial = nist->FindOrBuildMaterial("G4_WATER");
  double distance = 2.0 * m, radius = 0.12 * m, height = 1.74 * m;

  /*
   * World construction. Typical configuration:
   *
   *   meter
   *       x
   *     2 |-----------------------*
   *     1 |                       |
   *   0 0 | * ----------------> @ |
   *     1 |                       |
   *     2 O------------------------ z
   *       2101234567890123456789012
   *         0         1         2   meter
   *
   * O: coordinate origin
   * *: beam source
   * @: target (body as a detector)
   */
  auto solidWorld = new G4Box("World", radius * 1.5, height * 0.6, (distance + radius) * 1.2);
  auto logicalWorld = new G4LogicalVolume(solidWorld, worldMaterial, "World");
  auto physicalWorld = new G4PVPlacement(0, { }, logicalWorld, "World", 0, false, 0, checkOverlaps);

  /*
   * Body construction.
   */
  auto bodyRotation = new G4RotationMatrix;
  bodyRotation->rotateX(-90 * degree);
  auto solidBody = new G4Tubs("Body", 0.0, radius, height * 0.5, 0.0, CLHEP::twopi);
  auto logicalBody = new G4LogicalVolume(solidBody, bodyMaterial, "Body");
  new G4PVPlacement(bodyRotation, { 0.0, 0.0, distance * 1.1 }, logicalBody, "Body", logicalWorld, false, 0, checkOverlaps);

  fScoringVolume = logicalBody;
  return physicalWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
