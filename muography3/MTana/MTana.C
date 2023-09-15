#define MTana_cxx
#include "MTana.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

#include <TFile.h>
#include <TH2D.h>

int main() {
   TChain *ch = new TChain("MT");
   ch->Add("MT.root");
   MTana t(ch);
   t.Loop();

   return 0;
}

void MTana::Loop()
{
//   In a ROOT session, you can do:
//      root> .L MTana.C
//      root> MTana t
//      root> t.GetEntry(12); // Fill t data members with entry number 12
//      root> t.Show();       // Show values of entry 12
//      root> t.Show(16);     // Read and show values of entry 16
//      root> t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   TFile tf("MTana.root","RECREATE"); // a file where results are saved.
   TH2D* h1 = new TH2D("hitmap1", "hitmap in the front module", 32, 0., 32., 32, 0., 32.); // a hit map

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
      // to take coincidence betwee the 1st and 2nd layers
      for (int i = 0; i < 32; ++i) { // loop over scintillators in the 1st layer
         if (ScEdep->at(i) > 0.) { // Do Scintillators have energy deposit?
            for (int j = 32; j < 64; ++j) { // loop over scintillators in the 2nd layer
               if (ScEdep->at(j) > 0.) { // Do Scintillators have energy deposit?
                  h1->Fill(i, j-32); // fill events.
               }
            }
         }
      }
   }
   tf.Write(); // to write results(histograms) to the file
   tf.Close(); // to close the file
}
