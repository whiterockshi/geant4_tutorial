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
/// \file MTEventAction.hh
/// \brief Definition of the MTEventAction class

#ifndef MTEventAction_h
#define MTEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"

#include <vector>

/// Event action class
///
/// It defines data members to hold the energy deposit and track lengths
/// of charged particles in Absober and Gap layers:
/// - fEnergyAbs, fEnergyGap, fTrackLAbs, fTrackLGap
/// which are collected step by step via the functions
/// - AddAbs(), AddGap()

class MTEventAction : public G4UserEventAction
{
  public:
    MTEventAction();
    virtual ~MTEventAction();

    virtual void  BeginOfEventAction(const G4Event* event);
    virtual void    EndOfEventAction(const G4Event* event);
    
    void SetPrim(G4double energy, G4ThreeVector pos, G4ThreeVector dir);
    void AddSc(G4int scid, G4double edep);

    G4double GetPrimaryEnergy() {return fPrimE;}
    std::vector<double>& GetPrimaryPosVector() {return fPrimPos;}
    std::vector<double>& GetPrimaryDirVector() {return fPrimDir;}
    std::vector<double>& GetScEdepVector() {return fScEdep;}

  private:
    G4double fPrimE;
    std::vector<double> fPrimPos;
    std::vector<double> fPrimDir;
    std::vector<double> fScEdep;
};

// inline functions

inline void MTEventAction::SetPrim(G4double energy, G4ThreeVector pos, G4ThreeVector dir)
{
  fPrimE = energy;
  fPrimPos.at(0) = pos.x();
  fPrimPos.at(1) = pos.y();
  fPrimPos.at(2) = pos.z();
  fPrimDir.at(0) = dir.x();
  fPrimDir.at(1) = dir.y();
  fPrimDir.at(2) = dir.z();
}

inline void MTEventAction::AddSc(G4int scid, G4double edep)
{
  fScEdep.at(scid) += edep;
}
                     
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    
