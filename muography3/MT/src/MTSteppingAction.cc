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
/// \file MTSteppingAction.cc
/// \brief Implementation of the MTSteppingAction class

#include "MTSteppingAction.hh"
#include "MTEventAction.hh"
#include "MTDetectorConstruction.hh"

#include "G4Step.hh"
#include "G4RunManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MTSteppingAction::MTSteppingAction(
                      const MTDetectorConstruction* detectorConstruction,
                      MTEventAction* eventAction)
  : G4UserSteppingAction(),
    fDetConstruction(detectorConstruction),
    fEventAction(eventAction)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MTSteppingAction::~MTSteppingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MTSteppingAction::UserSteppingAction(const G4Step* step)
{
// Collect energy and track length step by step

  // get volume of the current step
  auto volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
  
  // energy deposit
  auto edep = step->GetTotalEnergyDeposit();
      
  if ( edep != 0. && volume == fDetConstruction->GetScPV() ) {
    G4int replicaid = step->GetPreStepPoint()->GetTouchableHandle()->GetReplicaNumber();
    G4int layerid = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume(1)->GetCopyNo();
    G4int moduleid = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume(2)->GetCopyNo();
    G4int scid = moduleid*2*32 + layerid*32 + replicaid;
    fEventAction->AddSc(scid,edep);
  }
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
