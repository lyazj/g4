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
#include "DetectorMessenger.hh"

#include "G4Isotope.hh"
#include "G4Element.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4UserLimits.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"

#include <stdexcept>
#include <string>

namespace B1
{

DetectorConstruction::DetectorConstruction()
{
  fMessenger = new DetectorMessenger(this);
}

DetectorConstruction::~DetectorConstruction()
{
  delete fMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  /*
   * Tunable options and outer dependencies.
   */
  G4bool checkOverlaps = true;
  G4NistManager *nist = G4NistManager::Instance();
  G4Material *worldMaterial = nist->FindOrBuildMaterial("G4_AIR");
  G4Material *sphereMaterial = GetUMaterial(fU235Enrichment);

  /*
   * World construction.
   */
  auto solidWorld = new G4Box("World", fRadius * 1.2, fRadius * 1.2, fRadius * 1.2);
  auto logicalWorld = new G4LogicalVolume(solidWorld, worldMaterial, "World");
  auto physicalWorld = new G4PVPlacement(0, { }, logicalWorld, "World", 0, false, 0, checkOverlaps);

  /*
   * Sphere construction.
   */
  auto solidSphere = new G4Sphere("Sphere", 0.0, fRadius, 0, CLHEP::twopi, 0, CLHEP::pi);
  auto logicalSphere = new G4LogicalVolume(solidSphere, sphereMaterial, "Sphere");
  logicalSphere->SetUserLimits(new G4UserLimits(0.01 * fRadius));
  new G4PVPlacement(0, { }, logicalSphere, "Sphere", logicalWorld, false, 0, checkOverlaps);

  /*
   * Initialize fields.
   */
  fWorld = logicalWorld;
  fSphere = logicalSphere;
  return physicalWorld;
}

G4Material *DetectorConstruction::GetUMaterial(G4double U235Enrichment) const
{
  if(!(U235Enrichment >= 0.0 && U235Enrichment <= 100.0)) {
    throw std::invalid_argument("unexpected U235 enrichment: " + std::to_string(U235Enrichment));
  }
  G4Element *U_elem = new G4Element("U", "U", 3);
  U_elem->AddIsotope(new G4Isotope("U235", 92, 235, 235.04 * (g / mole)), U235Enrichment);
  U_elem->AddIsotope(new G4Isotope("U238", 92, 238, 238.05 * (g / mole)), (100.0 - U235Enrichment) * (5.27 / 6.29));
  U_elem->AddIsotope(new G4Isotope("U234", 92, 234, 234.05 * (g / mole)), (100.0 - U235Enrichment) * (1.02 / 6.29));
  G4Material *U = new G4Material("U", 18.74 * (g / cm3), 1);
  U->AddElement(U_elem, 1);
  return U;
}

void DetectorConstruction::SetRadius(G4double radius)
{
  fRadius = radius;
  if(fWorld) ((G4Box *)fWorld->GetSolid())->SetXHalfLength(radius * 1.2);
  if(fWorld) ((G4Box *)fWorld->GetSolid())->SetYHalfLength(radius * 1.2);
  if(fWorld) ((G4Box *)fWorld->GetSolid())->SetZHalfLength(radius * 1.2);
  if(fSphere) ((G4Sphere *)fSphere->GetSolid())->SetOuterRadius(radius);
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

void DetectorConstruction::SetU235Enrichment(G4double enrichment)
{
  fU235Enrichment = enrichment;
  if(fSphere) fSphere->SetMaterial(GetUMaterial(enrichment));
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
