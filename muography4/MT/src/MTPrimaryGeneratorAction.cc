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
/// \file MTPrimaryGeneratorAction.cc
/// \brief Implementation of the MTPrimaryGeneratorAction class

#include "MTPrimaryGeneratorAction.hh"
#include "MTEventAction.hh"

#include "G4RunManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MTPrimaryGeneratorAction::MTPrimaryGeneratorAction(MTEventAction* eventAction)
 : G4VUserPrimaryGeneratorAction(),
   fParticleGun(nullptr),
   fEventAction(eventAction)
{
  G4int nofParticles = 1;
  fParticleGun = new G4ParticleGun(nofParticles);

  // default particle kinematic
  //
  auto particleDefinition 
    = G4ParticleTable::GetParticleTable()->FindParticle("e-");
  fParticleGun->SetParticleDefinition(particleDefinition);
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
  fParticleGun->SetParticleEnergy(50.*MeV);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MTPrimaryGeneratorAction::~MTPrimaryGeneratorAction()
{
  delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MTPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  // This function is called at the begining of event

  // In order to avoid dependence of PrimaryGeneratorAction
  // on DetectorConstruction class we get world volume 
  // from G4LogicalVolumeStore
  //
  G4double worldZHalfLength = 0.;
  G4double worldXHalfLength = 0.;
  G4double worldYHalfLength = 0.;
  auto worldLV = G4LogicalVolumeStore::GetInstance()->GetVolume("World");

  // Check that the world volume has box shape
  G4Box* worldBox = nullptr;
  if (  worldLV ) {
    worldBox = dynamic_cast<G4Box*>(worldLV->GetSolid());
  }

  if ( worldBox ) {
    worldZHalfLength = worldBox->GetZHalfLength();  
    worldXHalfLength = worldBox->GetXHalfLength();  
    worldYHalfLength = worldBox->GetYHalfLength();  
  }
  else  {
    G4ExceptionDescription msg;
    msg << "World volume of box shape not found." << G4endl;
    msg << "Perhaps you have changed geometry." << G4endl;
    msg << "The gun will be place in the center.";
    G4Exception("MTPrimaryGeneratorAction::GeneratePrimaries()",
      "MyCode0002", JustWarning, msg);
  } 
  
  // Set gun position
  G4ThreeVector xyzPrim;
  xyzPrim.setX(worldXHalfLength*(2*G4UniformRand() - 1.));
  xyzPrim.setY(worldYHalfLength*(2*G4UniformRand() - 1.));
  xyzPrim.setZ(-worldZHalfLength);
  fParticleGun->SetParticlePosition(xyzPrim);
  // Set gun direction
  G4ThreeVector xyzExit;
  xyzExit.setX(worldXHalfLength*(2*G4UniformRand() - 1.));
  xyzExit.setY(worldYHalfLength*(2*G4UniformRand() - 1.));
  xyzExit.setZ(worldZHalfLength);
  G4ThreeVector dxyz;
  dxyz.setX((xyzExit - xyzPrim).x());
  dxyz.setY((xyzExit - xyzPrim).y());
  dxyz.setZ((xyzExit - xyzPrim).z());
  fParticleGun->SetParticleMomentumDirection(dxyz.unit());
  
  fParticleGun->GeneratePrimaryVertex(anEvent);

  G4double energy = fParticleGun->GetParticleEnergy();
  G4ThreeVector pos = fParticleGun->GetParticlePosition();
  G4ThreeVector dir = fParticleGun->GetParticleMomentumDirection();
  fEventAction->SetPrim(energy,pos,dir);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

