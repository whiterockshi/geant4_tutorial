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
/// \file MTDetectorConstruction.cc
/// \brief Implementation of the MTDetectorConstruction class

#include "MTDetectorConstruction.hh"
#include "MTDetectorMessenger.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4Orb.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "G4UserLimits.hh"
#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal 
G4GlobalMagFieldMessenger* MTDetectorConstruction::fMagFieldMessenger = nullptr; 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MTDetectorConstruction::MTDetectorConstruction()
 : G4VUserDetectorConstruction(),
   fScPV(nullptr),
   fDistance(0.),
   fStepLimit(nullptr),
   fCheckOverlaps(true)
{
  fMessenger = new MTDetectorMessenger(this);

  for (G4int isc = 0; isc < 128; ++isc) {
    fposXSc.push_back(-9999.);
    fposYSc.push_back(-9999.);
    fposZSc.push_back(-9999.);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MTDetectorConstruction::~MTDetectorConstruction()
{ 
  delete fStepLimit;
  delete fMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* MTDetectorConstruction::Construct()
{
  // Define materials 
  DefineMaterials();
  
  // Define volumes
  return DefineVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MTDetectorConstruction::DefineMaterials()
{ 
  // Lead material defined using NIST Manager
  auto nistManager = G4NistManager::Instance();
  nistManager->FindOrBuildMaterial("G4_AIR");
  nistManager->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
  nistManager->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
  
  // Liquid argon material
  G4double a;  // mass of a mole;
  G4double z;  // z=mean number of protons;  
  G4double density; 
  new G4Material("liquidArgon", z=18., a= 39.95*g/mole, density= 1.390*g/cm3);
         // The argon by NIST Manager is a gas with a different density

  // Vacuum
  new G4Material("Galactic", z=1., a=1.01*g/mole,density= universe_mean_density,
                  kStateGas, 2.73*kelvin, 3.e-18*pascal);

  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* MTDetectorConstruction::DefineVolumes()
{
  // Geometry perameters
  G4int nofModules = 2;
  G4int nofLayersperModule = 2;
  G4int nofScperLayer = 32;
  G4double scLength = 320.*mm;
  G4double scThickness = 40.*mm;
  G4double scHeight = 10.*mm;
  G4double distancebetweenModules = 320.*mm;

  if (fDistance != 0 ) distancebetweenModules = fDistance;

  auto worldSizeXY = 2. * scLength;
  auto worldSizeZ  = 10. * (nofModules * nofLayersperModule * scThickness + (nofModules-1)*distancebetweenModules); 
  
  // Get materials
  auto defaultMaterial = G4Material::GetMaterial("G4_AIR");
  auto scMaterial = G4Material::GetMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
  auto targetMaterial = G4Material::GetMaterial("G4_SILICON_DIOXIDE");
  
  if ( ! defaultMaterial || ! scMaterial || ! targetMaterial ) {
    G4ExceptionDescription msg;
    msg << "Cannot retrieve materials already defined."; 
    G4Exception("MTDetectorConstruction::DefineVolumes()",
      "MyCode0001", FatalException, msg);
  }
  //     
  // World
  //
  auto worldS 
    = new G4Box("World",           // its name
                 worldSizeXY/2, worldSizeXY/2, worldSizeZ/2); // its size
  auto worldLV
    = new G4LogicalVolume(worldS,           // its solid
                          defaultMaterial,  // its material
                          "World");         // its name
  auto worldPV
    = new G4PVPlacement(0,                // no rotation
                        G4ThreeVector(),  // at (0,0,0)
                        worldLV,          // its logical volume                         
                        "World",          // its name
                        0,                // its mother  volume
                        false,            // no boolean operation
                        0,                // copy number
                        fCheckOverlaps);  // checking overlaps 
  //
  // Target
  //
  auto targetS = new G4Orb("target", 10.*cm);
  auto targetLV = new G4LogicalVolume(targetS,
                                      targetMaterial,
                                      "target");
  new G4PVPlacement(0,
                    G4ThreeVector(0.,0.,-worldSizeZ/2.*0.8),
                    targetLV,
                    "Target",
                    worldLV,
                    false,
                    0,
                    fCheckOverlaps);
  //
  // Module
  //  
  G4ThreeVector posModule[nofModules];
  auto moduleS
    = new G4Box("Module",     // its name
                scLength/2, scLength/2, nofLayersperModule*scThickness/2); // its size
  auto moduleLV
      = new G4LogicalVolume(moduleS,     // its solid
                            defaultMaterial,  // its material
                            "Module");   // its name
  G4double x = 0., y=0., z;
  for (G4int iModule = 0; iModule < nofModules; ++iModule) {
    z = (iModule - (nofModules-1)/2.)*(nofLayersperModule*scThickness + distancebetweenModules);
    new G4PVPlacement(0,                // no rotation
                      G4ThreeVector(x,y,z),  // at (0,0,0)
                      moduleLV,          // its logical volume                         
                      "Module",    // its name
                      worldLV,          // its mother  volume
                      false,            // no boolean operation
                      iModule,                // copy number
                      fCheckOverlaps);  // checking overlaps 
    posModule[iModule].setX(x);
    posModule[iModule].setY(y);
    posModule[iModule].setZ(z);
  }
  //                                 
  // Layer
  //
  G4ThreeVector posLayer[nofLayersperModule];
  auto layerS 
    = new G4Box("Layer",           // its name
                 scLength/2, scLength/2, scThickness/2); // its size
  auto layerLV
    = new G4LogicalVolume(layerS,           // its solid
                          defaultMaterial,  // its material
                          "Layer");         // its name
  for (G4int iLayer = 0; iLayer < nofLayersperModule; ++iLayer){
    z = (iLayer - (nofLayersperModule-1)/2.)*scThickness;
    G4ThreeVector position(0.,0.,z);
    G4RotationMatrix rotMatrix=G4RotationMatrix();
    rotMatrix.rotateZ(iLayer*90.*deg);
    G4Transform3D transform = G4Transform3D(rotMatrix,position);
    //G4cout << rotMatrix << G4endl;
    new G4PVPlacement(transform,            // rotated by pi/2 w.r.t Z axis and positioned at (x,y,z)
                      layerLV,          // its logical volume                         
                      "Layer",    // its name
                      moduleLV,          // its mother  volume
                      false,            // no boolean operation
                      iLayer,                // copy number
                      fCheckOverlaps);  // checking overlaps 
    posLayer[iLayer].setX(x);
    posLayer[iLayer].setY(y);
    posLayer[iLayer].setZ(z);
  }
  //
  // Scintillator
  //
  auto scS 
    = new G4Box("Layer",           // its name
                 scHeight/2, scLength/2, scThickness/2); // its size
  auto scLV
    = new G4LogicalVolume(scS,           // its solid
                          scMaterial,  // its material
                          "Sc");         // its name
  fScPV = new G4PVReplica("Sc",          // its name
                          scLV,          // its logical volume
                          layerLV,          // its mother
                          kXAxis,           // axis of replication
                          nofScperLayer,        // number of replica
                          scHeight);  // witdth of replica
  //
  // save sc positions for ntuple
  //                          
  for (G4int iModule = 0; iModule < nofModules; ++iModule) {
    for (G4int iLayer = 0; iLayer < nofLayersperModule; ++iLayer) {
      for (G4int iSc = 0; iSc < nofScperLayer; ++iSc) {
        G4int scid = (iModule*nofLayersperModule + iLayer)*nofScperLayer + iSc;
        if (iLayer%2 == 0) {
          fposXSc.at(scid) = (iSc - (nofScperLayer-1)/2.)*scHeight;
        } else {
          fposYSc.at(scid) = (iSc - (nofScperLayer-1)/2.)*scHeight;
        }
        fposZSc.at(scid) = posModule[iModule].z()+posLayer[iLayer].z();
      }
    }
  }
  //
  // print parameters
  //
  G4cout
    << G4endl 
    << "------------------------------------------------------------" << G4endl
    << "---> The target is a sphare with radius of "
    << targetS->GetRadius() << " mm of " << targetMaterial->GetName() << G4endl
    << "------------------------------------------------------------" << G4endl;

  //                                        
  // Visualization attributes
  //
  worldLV->SetVisAttributes (G4VisAttributes::GetInvisible());

  auto simpleBoxVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,1.0));
  simpleBoxVisAtt->SetVisibility(true);
  moduleLV->SetVisAttributes(simpleBoxVisAtt);

  //
  // Always return the physical World
  //
  return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MTDetectorConstruction::ConstructSDandField()
{ 
  // Create global magnetic field messenger.
  // Uniform magnetic field is then created automatically if
  // the field value is not zero.
  G4ThreeVector fieldValue;
  fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
  fMagFieldMessenger->SetVerboseLevel(1);
  
  // Register the field messenger for deleting
  G4AutoDelete::Register(fMagFieldMessenger);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MTDetectorConstruction::SetDistance(G4double distance)
{
  fDistance = distance;
  G4cout
    << G4endl
    << "----> The distance is set to  " << fDistance << " mm" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MTDetectorConstruction::SetMaxStep(G4double maxStep)
{
  if ((fStepLimit)&&(maxStep>0.)) fStepLimit->SetMaxAllowedStep(maxStep);
}
