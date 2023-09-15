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
/// \file MTEventAction.cc
/// \brief Implementation of the MTEventAction class

#include "MTEventAction.hh"
#include "MTRunAction.hh"
#include "MTAnalysis.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"
#include <iomanip>
#include <vector>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MTEventAction::MTEventAction()
 : G4UserEventAction()
{
  for (G4int i = 0; i < 3; ++i) {
    fPrimPos.push_back(0.);
    fPrimDir.push_back(0.); 
  }

  for (G4int i = 0; i < 128; ++i)
    fScEdep.push_back(0.);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MTEventAction::~MTEventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MTEventAction::BeginOfEventAction(const G4Event* /*event*/)
{  
  // initialisation per event
  for (G4int i = 0; i < 128; ++i)
    fScEdep.at(i)= 0.;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MTEventAction::EndOfEventAction(const G4Event* event)
{
  // Accumulate statistics
  //

  // get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // fill ntuple
  analysisManager->FillNtupleDColumn(0, fPrimE);
  analysisManager->AddNtupleRow();

  // Print per event (modulo n)
  //
  auto eventID = event->GetEventID();
  auto printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
  for (G4int imodule = 0; imodule < 2; ++imodule ) {
    for (G4int ilayer = 0; ilayer < 2; ++ilayer ) {
      for (G4int isc = 0; isc < 32; ++isc ) {
        G4int scid = imodule*2*32 + ilayer*32 + isc;
        if (fScEdep.at(scid) > 0.){
          if ( ( printModulo > 0 ) && ( eventID % printModulo == 0 ) ) {
            G4cout  << "---> End of event: " << eventID << G4endl;     
            G4cout  << " module: " << imodule << " layer: " << ilayer << " sc: " << isc << " energy deposit: " << std::setw(7)
                    << G4BestUnit(fScEdep[scid],"Energy")
                    << G4endl;
          }
        }
      }
    }
  }

}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
