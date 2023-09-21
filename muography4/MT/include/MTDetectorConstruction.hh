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
/// \file MTDetectorConstruction.hh
/// \brief Definition of the MTDetectorConstruction class

#ifndef MTDetectorConstruction_h
#define MTDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"

#include <vector>

class G4VPhysicalVolume;
class G4GlobalMagFieldMessenger;
class G4UserLimits;

class MTDetectorMessenger;

/// Detector construction class to define materials and geometry.
/// The calorimeter is a box made of a given number of layers. A layer consists
/// of an absorber plate and of a detection gap. The layer is replicated.
///
/// Four parameters define the geometry of the calorimeter :
///
/// - the thickness of an absorber plate,
/// - the thickness of a gap,
/// - the number of layers,
/// - the transverse size of the calorimeter (the input face is a square).
///
/// In addition a transverse uniform magnetic field is defined 
/// via G4GlobalMagFieldMessenger class.

class MTDetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    MTDetectorConstruction();
    virtual ~MTDetectorConstruction();

  public:
    virtual G4VPhysicalVolume* Construct();
    virtual void ConstructSDandField();

    // get methods
    //
    const G4VPhysicalVolume* GetScPV() const;
    std::vector<double>& GetScPosVector(G4int xyz);
  
    // Set methods
    void SetDistance (G4double);
    void SetMaxStep (G4double);
     
  private:
    // methods
    //
    void DefineMaterials();
    G4VPhysicalVolume* DefineVolumes();
  
    // data members
    //
    std::vector<double> fposXSc;
    std::vector<double> fposYSc;
    std::vector<double> fposZSc;

    static G4ThreadLocal G4GlobalMagFieldMessenger*  fMagFieldMessenger; 
                                      // magnetic field messenger
     
    G4VPhysicalVolume*   fScPV;      // the scintillator physical volume
    
    G4bool  fCheckOverlaps; // option to activate checking of volumes overlaps

    G4double fDistance; // distance between modules
    G4UserLimits* fStepLimit; // pointer to user step limits

    MTDetectorMessenger* fMessenger; // messenger
};

// inline functions

inline std::vector<double>& MTDetectorConstruction::GetScPosVector(G4int xyz) {
  if (xyz == 0) {
    return fposXSc;
  } else if (xyz == 1) {
    return fposYSc;
  } else if (xyz == 2) {
    return fposZSc;
  }
}

inline const G4VPhysicalVolume* MTDetectorConstruction::GetScPV() const  { 
  return fScPV;
}
     

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

