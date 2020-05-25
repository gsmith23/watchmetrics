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

// get this file or comment out if using other TStyle settings
// https://github.com/Watchman-PMT/Wavedump_Wrapper/blob/master/Common_Tools/wmStyle.C
#include "wmStyle.C"

//------------------
// Globals
   
//----------
// Objects

// for use with wmStyle.C
TStyle * wmStyle = GetwmStyle();
gROOT->SetStyle("wmStyle");
gROOT->ForceStyle();

TCanvas * c1 = new TCanvas("c1", "c1",1220,63,700,500);
TH1I    * h1[nFiles];  

//----------
// Variables
static const int nFiles = 3;
string parent_path = "/Users/gsmith23/software/modern/pull94/watchmakers/";

// particle ID (dev with e+, n ,208Tl)
int PIDs[nFiles] = {11,2112,208}; 
// cocktail ID (dev with percentate WbLS)
int CIDs[nFiles] = {1,3,5}; 

Int_t colors[5] = {kRed,kOrange,kGreen+2,kCyan+1,kMagenta+1};

//----------
// Functions
string GetFileName(int genPID, int fileID);
string GetFilePath(int genPID, int fileID);
void   Draw_Multi_TH(string metric = "nhit");
void   SetIDs(int particleID, int cocktailID);

//-------------------

void   watchmetrics(string metric1 = "nhit",
		    bool   logY = false, 
		    int    particleID = 11, 
		    string metric2 = "",
		    int    cocktailID = 1){
  
  SetIDs(particleID,cocktailID);
  c1->SetLogy(logY);
  
  Draw_Multi_TH(metric1);
  
}

void SetIDs(int particleID, int cocktailID){
  
  for( int i = 0 ; i < nFiles ; i++){
    if  (particleID != 0) // compare cocktail
      PIDs[i] = particleID;
    else                 // compare particle
      CIDs[i] = cocktailID;
  }
}

void Draw_Multi_TH(string metric){
  
  TLatex * latex = new TLatex();
  string text = "test";
  latex->SetNDC();
  latex->SetTextSize(0.03);
  
  string option = "";
  for (int iFile = 0 ; iFile < nFiles ; iFile++ ){
    
    string file_path = GetFilePath(PIDs[iFile],CIDs[iFile]);
    string file_name = GetFileName(PIDs[iFile],CIDs[iFile]);
        
    TFile *f;
    
    string title = ";";
    title += metric;
    title += ";Counts";
    
    h1[iFile] = new TH1I("h1",title.c_str(),100,0,200);
    
    f = new TFile(file_path.c_str());
    
    if(!f){
      cout << " check file :" << file_path << endl;
      return;
    }
    
    // Variables to read in
    TTree *rat_tree;
    
    RAT::DS::Root *ds = new RAT::DS::Root();
    
    TVector3 mc_p3;
    double   mc_energy;
      
    rat_tree = (TTree *)f->Get("T");
    rat_tree->SetBranchAddress("ds", &ds);
    
    int    nhit = 0;
    double totPE = 0.;
    RAT::DS::PMT *pmt;
    
    int n_events = rat_tree->GetEntries();
    for (int event = 0; event < n_events; event++){
      rat_tree->GetEntry(event);
      
      for (int sub_event = 0; 
	   sub_event < ds->GetEVCount(); 
	   sub_event++) {
	
	// MC info
	mc_p3 = ds->GetMC()->GetMCParticle(0)->GetPosition();
	mc_energy = ds->GetMC()->GetMCParticle(0)->GetKE();
	
	// Event info
	nhit  = ds->GetEV(sub_event)->GetPMTCount();
	totPE = ds->GetEV(sub_event)->GetTotalCharge();
	
	for (int hit = 0; hit < nhit; hit++) {
	  pmt = ds->GetEV(sub_event)->GetPMT(hit);
	}
	
	if     (metric=="nhit")
	  h1[iFile]->Fill(nhit);
	else if(metric=="totPE"){
	  h1[iFile]->Fill(totPE); 
	}else{ 
	  cerr << " invalid metric: " << metric << endl;
	  return;
	}
	
      } // end of: for (sub_event
      
    } //end of:  for (int event = 0
    
    //----------
    
    text  = "#color[";
    text += std::to_string(colors[iFile]);
    text += "]{";
    text += GetFileName(PIDs[iFile],CIDs[iFile]);
    text += "}";

    h1[iFile]->SetLineColor(colors[iFile]);
    h1[iFile]->Draw(option.c_str());
    option = "same";

    latex->DrawLatex(0.6,0.8-0.05*iFile,text.c_str());
    
  }  
}

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
    return "";
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
    return "";
  }
  
  return file_name;
}

string GetFilePath(int genPID = 11, int fileID = 1){
  
  string file_path = parent_path;

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
    return "";
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
    return "";
  }
  
  string file_name = GetFileName(genPID,fileID);
  file_path += file_name;
  file_path += ".root";

  cout << endl;
  cout << " file: " << endl; 
  cout << " " << file_name << endl;
  
  return file_path;
}
