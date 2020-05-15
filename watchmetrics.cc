#include <iomanip>
#include <iostream>

using namespace std;

#include <RAT/DS/EV.hh>
#include <RAT/DS/Root.hh>

#include <TCanvas.h>
#include <TLatex.h>
#include <TStyle.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TProfile.h>
#include <TROOT.h>
#include <TTree.h>

#include <TRandom.h>

#include "wmStyle.C"

string GetFileName(int genPID = 11, int fileID = 1){
  
  string file_name = "wbls_1pct_208Tl_PMT";
  
  switch(fileID){
  case(1):
    file_name =  "wbls_1pct_";
    break;
  case(3):
    file_name =  "wbls_3pct_";
    break;
  case(5):
    file_name =  "wbls_5pct_";
    break;
  default:
    cerr << " Error: no matching file found with this fileID " << endl;
    break;
  }
  
  switch(genPID){
  case(11):
    file_name += "IBDPositron";
    break;
  case(2112):
    file_name += "IBDNeutron";
    break;
  case(208):
    file_name += "208Tl_PMT";
    break;
  default:
    cerr << " Error: no matching file found with this genID " << endl;
    break;
  }
  return file_name;
}

string GetFilePath(int genPID = 11, int fileID = 1,
		   string file_path = "/Users/gsmith23/software/modern/pull94/watchmakers/"){
  
  switch(fileID){
  case(1):
    file_path += "test_wbls_1pct/root_files_detectorMedia_wbls_1pct_WM_0420_lightSim/";
    break;
  case(3):
    file_path += "test_wbls_3pct/root_files_detectorMedia_wbls_3pct_WM_0420_lightSim/";
    break;
  case(5):
    file_path += "test_wbls_5pct/root_files_detectorMedia_wbls_5pct_WM_0420_lightSim/";
    break;
  default:
    cerr << " Error: no matching file found with this fileID " << endl;
    break;
  }
  
  switch(genPID){
  case(11):
    file_path += "Watchman_IBDPositron_LIQUID_ibd_p/";
    break;
  case(2112):
    file_path += "Watchman_IBDNeutron_LIQUID_ibd_n/";
    break;
  case(208):
    file_path += "Watchman_208Tl_PMT_CHAIN_232Th_NA/";
    break;
  default:
    cerr << " Error: no matching file found with this genID " << endl;
    break;
  }
  
  file_path += GetFileName(genPID,fileID);
  file_path += ".root";
  
  return file_path;
}

TH1I * Get_hNhits(int genPID = 11, int fileID = 1) {

  string file_path = GetFilePath(genPID,fileID);
  string file_name = GetFileName(genPID,fileID);

  TFile *f;
  f = new TFile(file_path.c_str());
  
  TTree *rat_tree;
  
  RAT::DS::Root *ds = new RAT::DS::Root();
  
  rat_tree = (TTree *)f->Get("T");
  rat_tree->SetBranchAddress("ds", &ds);
  
  RAT::DS::EV *ev;
  
  int nhit = 0;

  TStyle * wmStyle = GetwmStyle();
  gROOT->SetStyle("wmStyle");
  gROOT->ForceStyle();

  string title = ";Nhits;Counts";

  TH1I * hNhit = new TH1I("hNhit",title.c_str(),100,0,200);
  
  int n_events = rat_tree->GetEntries();
  for (int event = 0; event < n_events; event++){
    rat_tree->GetEntry(event);
  
    for (int sub_event = 0; 
	 sub_event < ds->GetEVCount(); 
	 sub_event++) {
      
      ev   = ds->GetEV(sub_event);
      nhit = ev->GetPMTCount();
      
      hNhit->Fill(nhit);
      
    } // end of: for (sub_event
  
  } //end of:  for (int event = 0
  
  return hNhit;
}

void Draw_Multi_hNhits(int genPID = 11,
		       int fileID = 3
		       ){
  
  // by default compare generators
  bool byCocktail = false;
    
  // if a particle ID is chosen 
  // compare by cocktail
  if(genPID!=0)
    byCocktail = true;
  
  static const int nFiles = 3;
  int genPIDs[nFiles] = {11,2112,208}; // e+,n,208Tl
  int fileIDs[nFiles] = {1,3,5}; // percentate WbLS

  TLatex * latex = new TLatex();
  string text = "test";
  latex->SetNDC();
  latex->SetTextSize(0.03);
  
  Int_t colors[5] = {kRed,kOrange,kGreen+2,kCyan+1,kMagenta+1};

  TH1I * hNhits[nFiles];  
  
  string option = "";
  for (int i = 0 ; i < nFiles ; i++ ){

    if  (byCocktail) // compare cocktail
      fileID = fileIDs[i];
    else              // compare particle
      genPID = genPIDs[i];
    
    cout << endl;
    cout << " fileID = " << fileID << endl;
    cout << " genPID = " << genPID << endl;

    hNhits[i] = Get_hNhits(genPID,fileID);
    text  = "#color[";
    text += std::to_string(colors[i]);
    text += "]{";
    text += GetFileName(genPID,fileID);
    text += "}";

    hNhits[i]->SetLineColor(colors[i]);
    hNhits[i]->Draw(option.c_str());
    option = "same";

    latex->DrawLatex(0.6,0.8-0.05*i,text.c_str());

  }
  
}


