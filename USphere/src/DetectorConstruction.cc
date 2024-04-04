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

#include "G4Isotope.hh"
#include "G4Element.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Sphere.hh"
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
  G4Material *sphereMaterial = GetUMaterial();
  double radius = 8.7407 * cm;

  /*
   * World construction.
   */
  auto solidWorld = new G4Box("World", radius * 1.2, radius * 1.2, radius * 1.2);
  auto logicalWorld = new G4LogicalVolume(solidWorld, worldMaterial, "World");
  auto physicalWorld = new G4PVPlacement(0, { }, logicalWorld, "World", 0, false, 0, checkOverlaps);

  /*
   * Sphere construction.
   */
  auto solidSphere = new G4Sphere("Sphere", 0.0, radius, 0, CLHEP::twopi, 0, CLHEP::pi);
  auto logicalSphere = new G4LogicalVolume(solidSphere, sphereMaterial, "Sphere");
  new G4PVPlacement(0, { }, logicalSphere, "Sphere", logicalWorld, false, 0, checkOverlaps);

  /*
   * Initialize fields.
   */
  fScoringVolume = logicalSphere;
  return physicalWorld;
}

G4Material *DetectorConstruction::GetUMaterial() const
{
  G4Element *U_std_elem = new G4Element("U_std", "U", 3);
  U_std_elem->AddIsotope(new G4Isotope("U235", 92, 235, 235.04 * (g / mole)), 93.71);
  U_std_elem->AddIsotope(new G4Isotope("U238", 92, 238, 238.05 * (g / mole)),  5.27);
  U_std_elem->AddIsotope(new G4Isotope("U234", 92, 234, 234.05 * (g / mole)),  1.02);
  G4Material *U_std = new G4Material("U_std", 18.74 * (g / cm3), 1);
  U_std->AddElement(U_std_elem, 1);
  return U_std;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}