#ifndef TRatAnalyser_h
#define TRatAnalyser_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TTree.h>

#include <RAT/DS/Root.hh>
#include "TObject.h"
#include "utility"
#include <RAT/DS/MC.hh>
#include "TTimeStamp.h"
#include <RAT/DS/Calib.hh>
#include <RAT/DS/EV.hh>
#include <RAT/DS/Run.hh>

#include <vector>

#include <TCanvas.h>
#include <TH2.h>

class TRatAnalyser {
public :

  // read from file
  TTree *rat_tree, *run_tree;
  
  RAT::DS::Root *ds = new RAT::DS::Root();
  RAT::DS::Run *run = new RAT::DS::Run();
  
  // store metrics variables
  TFile *  metrics_file;
  TTree *  metrics_tree;
  
  // per event
  unsigned short nhit  = 0;
  float    mc_x  = 0., mc_y = 0., mc_z = 0.;
  float    totPE = 0., mc_E = 0., mc_r = 0.;  

  // per hit
  std::vector<unsigned short> pmtID;
  std::vector<float> time;
  std::vector<float> charge;
  
  // visualisation
  TCanvas * c1 = new TCanvas("c1", "c1",1220,63,700,500);
  //TLatex  * latex = new TLatex();
  //TH1F    * h1[nFiles];  
  //TGraph  * g1[nWidths];
  TH1F    * hT;
  TH2F    * h2;  
  bool      logY = false; 
  bool      logZ = false; 
  //Int_t     colors[nFiles] = {kBlack};
  //double mean_nhit[nFiles] = {0};
  
  TRatAnalyser(TTree *user_rat_tree = 0,
	       TTree *user_run_tree = 0);
  virtual ~TRatAnalyser();
  virtual Int_t    GetEntry(Long64_t entry);
  virtual void     Init(TTree *user_rat_tree,
			TTree *user_run_tree);
  
  void    InitMetricsFile();
  void    MakeMetricsFile();
  void    MakeHisto(string metric1,bool useInner);

  void    Make2DHisto(string metric1,string metric2, 
		      bool useInner);
};

#endif

#ifdef TRatAnalyser_cxx
TRatAnalyser::TRatAnalyser(TTree *user_rat_tree,
			   TTree *user_run_tree) : rat_tree(0), run_tree(0){
  
  if (user_rat_tree == 0 || user_run_tree == 0) {
    TFile *f;
    f = (TFile*)gROOT->GetListOfFiles()->FindObject("Watchman_IBDPositron_LIQUID_ibd_p.root");
    if (!f || !f->IsOpen()) {
      f = new TFile("Watchman_IBDPositron_LIQUID_ibd_p.root");
    }
    user_rat_tree = (TTree *)f->Get("T");
    user_run_tree = (TTree *)f->Get("runT");
  }
  
  Init(user_rat_tree,
       user_run_tree);

}

TRatAnalyser::~TRatAnalyser()
{
  
  metrics_tree->Delete();
  metrics_file->Close();

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
   // Set branch addresses and branch pointers
  if (!user_rat_tree) return;
  rat_tree = user_rat_tree;
  rat_tree->SetBranchAddress("ds", &ds);
  
  if (!user_run_tree) return;
  run_tree = user_run_tree;
  run_tree->SetBranchAddress("run", &run);
  

  //
  h2 = new TH2F("h2","h2",100,0,200,100,0,200);
  hT = new TH1F("hT","hT",100,-1000,1000);
  
}

void TRatAnalyser::InitMetricsFile(){
  
  metrics_file = new TFile("metrics_file.root","RECREATE","metrics_file");
  metrics_tree = new TTree("metrics_tree","metrics_tree");
  
  // per event variables
  metrics_tree->Branch("nhit",&nhit);
  metrics_tree->Branch("totPE",&totPE);
  metrics_tree->Branch("mc_E",&mc_E);
  metrics_tree->Branch("mc_x",&mc_x);
  metrics_tree->Branch("mc_y",&mc_y);
  metrics_tree->Branch("mc_z",&mc_z);
  metrics_tree->Branch("mc_r",&mc_r);
  // vectors for per hit variables
  metrics_tree->Branch("pmtID",&pmtID);  // unsigned short
  metrics_tree->Branch("time",&time);    // float
  metrics_tree->Branch("charge",&charge);// float 
}

#endif // #ifdef TRatAnalyser_cxx
