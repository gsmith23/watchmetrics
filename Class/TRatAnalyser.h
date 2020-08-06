#ifndef TRatAnalyser_h
#define TRatAnalyser_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

#include <RAT/DS/Root.hh>
#include "TObject.h"
#include "utility"
#include <RAT/DS/MC.hh>
#include "TTimeStamp.h"
#include <RAT/DS/Calib.hh>
#include <RAT/DS/EV.hh>
#include <RAT/DS/Run.hh>

#include <TCanvas.h>
#include <TH2.h>

class TRatAnalyser {
public :

  // read from file
  
  TTree *rat_tree, *run_tree;
  
  RAT::DS::Root *ds = new RAT::DS::Root();
  RAT::DS::Run *run = new RAT::DS::Run();
  
  TVector3 mc_p3;
  double   mc_energy;

  
  // new members
  
  TCanvas * c1 = new TCanvas("c1", "c1",1220,63,700,500);
  //TLatex  * latex = new TLatex();
  //TH1D    * h1[nFiles];  
  //TGraph  * g1[nWidths];
  TH1D    * hT;
  TH2D    * h2;  
  bool      logY = false; 
  bool      logZ = false; 
  //Int_t     colors[nFiles] = {kBlack};
  //double mean_nhit[nFiles] = {0};
  
  
  TRatAnalyser(TTree *user_rat_tree = 0,
	       TTree *user_run_tree = 0);
  virtual ~TRatAnalyser();
  virtual Int_t    Cut(Long64_t entry);
  virtual Int_t    GetEntry(Long64_t entry);
  virtual void     Init(TTree *user_rat_tree,
			TTree *user_run_tree);
  virtual void     Loop();
  virtual Bool_t   Notify();
  virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef TRatAnalyser_cxx
TRatAnalyser::TRatAnalyser(TTree *user_rat_tree,TTree *user_run_tree) : rat_tree(0), run_tree(0){
  // if parameter user_rat_tree is not specified (or zero), connect the file
  // used to generate this class and read the Tree.
  if (user_rat_tree == 0 || user_run_tree == 0) {
    //TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("OD_1_pct_ref_000_pct.root");
    TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("Watchman_IBDNeutron_LIQUID_ibd_n.root");
    if (!f || !f->IsOpen()) {
      //  f = new TFile("OD_1_pct_ref_000_pct.root");
      f = new TFile("Watchman_IBDNeutron_LIQUID_ibd_n.root");
    }
    user_rat_tree = (TTree *)f->Get("T");
    user_run_tree = (TTree *)f->Get("runT");
  }
  
  Init(user_rat_tree,
       user_run_tree);
}

TRatAnalyser::~TRatAnalyser()
{
   if (!rat_tree) return;
   delete rat_tree->GetCurrentFile();
}

Int_t TRatAnalyser::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!rat_tree) return 0;
   return rat_tree->GetEntry(entry);
}

void TRatAnalyser::Init(TTree *user_rat_tree,
			TTree *user_run_tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
  if (!user_rat_tree) return;
  rat_tree = user_rat_tree;
  rat_tree->SetBranchAddress("ds", &ds);
  
  if (!user_run_tree) return;
  run_tree = user_run_tree;
  run_tree->SetBranchAddress("run", &run);
  
  h2 = new TH2D("h2","h2",100,0,200,100,0,200);
  hT = new TH1D("hT","hT",100,-1000,1000);
  
  Notify();
}

Bool_t TRatAnalyser::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void TRatAnalyser::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!rat_tree) return;
   rat_tree->Show(entry);
}

Int_t TRatAnalyser::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef TRatAnalyser_cxx
