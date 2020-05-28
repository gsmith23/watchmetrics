#include <iomanip>
#include <iostream>

using namespace std;

#include <RAT/DS/EV.hh>
#include <RAT/DS/Root.hh>
#include <RAT/DS/Run.hh>

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

// dependency for default style
// see - SetGraphics()
// https://github.com/Watchman-PMT/Wavedump_Wrapper/blob/master/Common_Tools/wmStyle.C
#include "wmStyle.C"

//------------------
// Globals
   
//----------
// Files
static const int nFiles = 6;

string parent_path = "/Users/gsmith23/software/modern/pull94/watchmakers/";

// particle ID (dev with e+, n ,208Tl)
int PIDs[nFiles] = {11,2112,208,0,0,0}; 
// cocktail ID (dev with percentate WbLS)
int CIDs[nFiles] = {1,3,5,101,301,0}; 

//----------
// Plotting

TCanvas * c1 = new TCanvas("c1", "c1",1220,63,700,500);
TLatex  * latex = new TLatex();
TH1D    * h1[nFiles];  
TH1D    * hT;
TH2D    * h2;  
bool      logY = false; 
bool      logZ = false; 
Int_t colors[] = {kRed,kOrange,kGreen+2,kCyan+1,kMagenta+1,kBlack};
 
//----------
// Functions
string GetFileName(int genPID, int fileID);
string GetFilePath(int genPID, int fileID);

void   Draw_Multi_TH(string metrics[2]);

void   Fill_TH2D(string metrics[2],
		 int    PID,
		 int    CID);

void   Fill_TH1D(string metrics[2],
		 int    iFile);

void   Init(int particleID, int cocktailID);
void   SetIDs(int particleID, int cocktailID);
void   SetGraphics();
void   Init_Histos();

//-------------------

void   watchmetrics(int    particleID = 2112,
		    string metric1 = "nhit",
		    string metric2 = "totPE",
		    int    cocktailID = 0){
  
  
  Init(particleID,cocktailID);
  
  string metrics[2];
  metrics[0] = metric1;
  metrics[1] = metric2;
  
  //----
  // 2D plot
  // Fill_TH2D(metrics,particleID,cocktailID);
  
  // 
  // TH2D *h2_1 = (TH2D*)h2->Clone();
  // h2_1->Draw("colz");

  //----
  // Timing  
  // TH1D *hT_2 = (TH1D*)hT->Clone();
  // hT_2->Draw();
  
  //-----
  // all cocktails
  Draw_Multi_TH(metrics);
  
}

void Init(int particleID,int cocktailID){
  
  // particleID != 0 - fixed particle
  // particleID == 0 - fixed cocktail
  
  SetIDs(particleID,cocktailID);

  SetGraphics();
  
  Init_Histos();
}

void Init_Histos(){

  string name = "h1_";;
  for (int iFile = 0 ; iFile < nFiles ; iFile++){
    name += std::to_string(iFile); 
    h1[iFile] = new TH1D(name.c_str(),";;Counts",100,0,200);
  }
  
  h2 = new TH2D("h2","h2",100,0,200,100,0,200);
  hT = new TH1D("hT","hT",100,-1000,1000);
  
}

void   SetGraphics(){

  c1->SetLogy(logY);
  c1->SetLogz(logZ);

  TStyle * wmStyle = GetwmStyle();
  
  const int NCont = 255;
  const int NRGBs = 5;
  
  // Color scheme for 2D plotting with a better defined scale 
  double stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
  double red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
  double green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
  double blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };          
  TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
  
  wmStyle->SetNumberContours(NCont);
  
  gROOT->SetStyle("wmStyle");

  gROOT->ForceStyle();
  
  latex->SetNDC();
  latex->SetTextSize(0.03);


}

void SetIDs(int particleID, int cocktailID){
  
  int initPIDs[] = {11,2112,208,0,0,0};
  int initCIDs[] = {1,3,5,101,301,0}; 
 
  for( int i = 0 ; i < nFiles ; i++){
    PIDs[i] = initPIDs[i];
    CIDs[i] = initCIDs[i];
    
    if  (particleID != 0)
      PIDs[i] = particleID;
    else
      CIDs[i] = cocktailID;
    
//     cout << " PIDs[" << i << "] = " << PIDs[i] << endl;
//     cout << " CIDs[" << i << "] = " << CIDs[i] << endl;
  }

}

void Fill_TH1D(string metrics[2],
	       int    iFile){

  Fill_TH2D(metrics,PIDs[iFile],CIDs[iFile]);
  h1[iFile] = (TH1D*)(h2->ProjectionX()->Clone());
  
}

void Fill_TH2D(string metrics[2],
	       int    PID,
	       int    CID){
  
//   cout << " PID = " << PID << endl;
//   cout << " CID = " << CID << endl;

  string file_path = GetFilePath(PID,CID);
  string file_name = GetFileName(PID,CID);
  
  TFile *f;
  f = new TFile(file_path.c_str());
  
  if(!f){
    cout << " check file :" << file_path << endl;
    return;
  }

  string title = ";";
  title += metrics[0];
  title += ";";
  title += metrics[1];
    
  h2->Reset();
  h2->SetTitle(title.c_str());
  
  int nBins[2];
  double min[2], max[2];
  
  for ( int i = 0 ; i < 2 ; i++ ){
        
    if     ( metrics[i] == "nhit" ||
	     metrics[i] == "totPE" ){
      nBins[i]= 100;
      min[i]  = 0.0;
      max[i]  = 200.;
    }
    else if( metrics[i] == "mc_energy" ){
      nBins[i]= 100;
      min[i]  = 0.0;
      max[i]  = 10.;
    }
    else if( metrics[i] == "mc_r" ||
	     metrics[i] == "mc_z"){
      nBins[i]= 100;
      min[i]  = 0.0;
      max[i]  = 10000.;
    }
    else{
      nBins[i]= 100;
      min[i]  = 0.0;
      max[i]  = 200.;
    }
    
  }
  
  h2->SetBins(nBins[0],min[0],max[0],
	      nBins[1],min[1],max[1]);
  
  // Variables to read in
  TTree *rat_tree, *run_tree;
  
  RAT::DS::Root *ds = new RAT::DS::Root();
  RAT::DS::Run *run = new RAT::DS::Run();
    
  TVector3 mc_p3;
  double   mc_energy;
  
  rat_tree = (TTree *)f->Get("T");
  rat_tree->SetBranchAddress("ds", &ds);
  run_tree = (TTree *)f->Get("runT");
  run_tree->SetBranchAddress("run", &run);
  
  run_tree->GetEntry(0);
  
  int    nhit  = 0, nhit_inner = 0, nhit_veto = 0;
  double totPE = 0.;
  double mc_r  = 0;
  // for filling histogram
  double metric_variable[2];

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
      
      //cout << " mc_energy = " << mc_energy << endl;

      // Event info
      nhit  = (double)ds->GetEV(sub_event)->GetPMTCount();
      nhit_inner = 0;
      nhit_veto  = 0;
      ds->GetEV(sub_event)->GetTotalCharge();
      totPE = ds->GetEV(sub_event)->GetTotalCharge();
      
      for (int hit = 0; hit < nhit; hit++) {
	pmt = ds->GetEV(sub_event)->GetPMT(hit);

	if(run->GetPMTInfo()->GetType(pmt->GetID())==1){
	  hT->Fill(pmt->GetTime());
	  
	  //if(pmt->GetTime() < 50.)
	  nhit_inner++;
	  
	}
	else if(run->GetPMTInfo()->GetType(pmt->GetID())==2)
	  nhit_veto++;
	
      }
      
      nhit = nhit_inner;
      mc_r = sqrt(mc_p3.x()*mc_p3.x() + mc_p3.x()*mc_p3.x());

      for ( int i = 0 ; i < 2 ; i++ ){
	if     ( metrics[i] == "nhit" )
	  metric_variable[i] = (double)nhit; 
	else if( metrics[i] == "totPE" )
	  metric_variable[i] = totPE; 
	else if( metrics[i] == "mc_energy" )
	  metric_variable[i] = mc_energy; 
	else if( metrics[i] == "mc_r" )
	  metric_variable[i] = mc_r; 
	else if( metrics[i] == "mc_z" )
	  metric_variable[i] = mc_p3.z(); 
	else
	  metric_variable[i] = 0.;
      }

      if(nhit > 0)
	h2->Fill(metric_variable[0],
		 metric_variable[1]);
      
    } // end of: for (sub_event
    
  } //end of:  for (int event = 0
  
  return;
}

void Draw_Multi_TH(string metrics[2]){
  
  string option = "";
  string title  = ";";
  string text   = "test";
  
  title += metrics[0];
  title += ";";
  title += "Counts";
  
  for (int iFile = 0 ; iFile < nFiles ; iFile++ ){
    
    h1[iFile]->Reset();
    
    // Fill histogram
    Fill_TH1D(metrics,iFile);
    
    h1[iFile]->SetTitle(title.c_str());
    
  }
  
  for (int iFile = 0 ; iFile < nFiles ; iFile++ ){
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
  case(0):
    file_name =  "gd_";
    break;
  case(1):
    file_name =  "wbls_1pct_";
    break;
  case(3):
    file_name =  "wbls_3pct_";
    break;
  case(5):
    file_name =  "wbls_5pct_";
    break;
  case(101):
    file_name =  "wbls_1pct_gd_01pct_";
    break;
  case(301):
    file_name =  "wbls_3pct_gd_01pct_";
    break;
  default:
    cerr << " Error: no matching file found with this fileID " << fileID << endl;
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
  case(0):
    file_path += "test_gd/root_files_lightSim/";
    break;
  case(1):
    file_path += "test_wbls_1pct/root_files_detectorMedia_wbls_1pct_WM_0420_lightSim/";
    break;
  case(3):
    file_path += "test_wbls_3pct/root_files_detectorMedia_wbls_3pct_WM_0420_lightSim/";
    break;
  case(5):
    file_path += "test_wbls_5pct/root_files_detectorMedia_wbls_5pct_WM_0420_lightSim/";
    break;
  case(101):
    file_path += "test_wbls_1pct_gd_01pct/root_files_detectorMedia_wbls_1pct_gd_01pct_WM_0420_lightSim/";
    break;
  case(301):
    file_path += "test_wbls_3pct_gd_01pct/root_files_detectorMedia_wbls_3pct_gd_01pct_WM_0420_lightSim/";
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
