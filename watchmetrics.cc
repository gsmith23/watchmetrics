#include <iomanip>
#include <iostream>

using namespace std;

#include <RAT/DS/EV.hh>
#include <RAT/DS/Root.hh>
#include <RAT/DS/Run.hh>

#include <TCanvas.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraph.h>
#include <TProfile.h>
#include <TROOT.h>
#include <TTree.h>

#include <TRandom.h>
#include <TF1.h>

// dependency for default style
// see - SetGraphics()
// https://github.com/Watchman-PMT/Wavedump_Wrapper/blob/master/Common_Tools/wmStyle.C
#include "wmStyle.C"

//------------------
// Globals
   
//----------
// static const int nFiles = 6;

// string parent_path = "/Users/gsmith23/software/modern/pull94/watchmakers/";

// // particle ID (dev with e+, n ,208Tl)
// int PIDs[nFiles] = {-11,2112,208,0,0,0}; 
// // cocktail ID (dev with percentate WbLS)
// int CIDs[nFiles] = {1,3,5,101,301,0}; 


// OD study
//static const int nFiles = 15;
  
static const int nWidths = 3, nRefs = 1, nPhots = 1;
static const int nFiles = nWidths * nRefs * nPhots;

int minSize  = 1600000, sizeStep  = 200000;
int maxSize  = minSize + (nWidths*sizeStep); 

int minPhoto = 10000,   photoStep = 5000;
int maxPhoto = minPhoto + (nPhots*photoStep);  

int minRef   = 70,      refStep   = 10;
int maxRef   = minRef + (nRefs*refStep);
  
//string parent_path = "/Users/gsmith23/software/modern/OD/trade_study/mac/1_GeV/";
string parent_path = "/Users/gsmith23/software/modern/OD/trade_study/output/";

// particle ID 
int PIDs[nFiles] = {-13}; 
// cocktail ID 
//int CIDs[nFiles] = {-71,-72,-73,-81,-82,-83,-91,-92,-93,-101,-102,-103};
// int CIDs[nFiles] = {-1,  -2,  -3,
// 		    -51, -52, -53,
// 		    -81, -82, -83,
// 		    -91, -92, -93,
// 		    -101,-102,-103};

int CIDs[nFiles] = {-2020070};

//----------
// Plotting

TCanvas * c1 = new TCanvas("c1", "c1",1220,63,700,500);
TLatex  * latex = new TLatex();
TH1D    * h1[nFiles];  
TGraph  * g1[nWidths];
TH1D    * hT;
TH2D    * h2;  
bool      logY = false; 
bool      logZ = false; 
Int_t     colors[nFiles] = {kBlack};

double mean_nhit[nFiles] = {0};

//----------
// Functions
string GetFileName(int genPID, int fileID);
string GetFilePath(int genPID, int fileID);

void   Draw_Multi_TH(string metrics[2]);

void   Draw_Multi_TGraph(string metrics[2]);

void   Fill_TH2D(string metrics[2],
		 int    PID,
		 int    CID);

void   Fill_TH1D(string metrics[2],
		 int    iFile);

void   Init(int particleID, int cocktailID);
void   SetIDs(int particleID, int cocktailID);
void   SetGraphics();
void   Init_Histos();


float CIDToWid(int CID);
float CIDToPho(int CID);
float CIDToRef(int CID);
//-------------------

void   watchmetrics(int    particleID = -13,
		    string metric1 = "nhit",
		    string metric2 = "totPE",
		    int    cocktailID = -2020100){
  
  
  Init(particleID,cocktailID);
  
  string metrics[2];
  metrics[0] = metric1;
  metrics[1] = metric2;
  
  //----
  // 2D plot
  // Fill_TH2D(metrics,particleID,cocktailID);
  
  // 
  // TH2D *h2_1 = (TH2D*)h2->Clone();
  // h2_1->Draw("colz ");
  
  // Timing  
  // TH1D *hT_2 = (TH1D*)hT->Clone();
  // hT_2->Draw();
  
  //-----
  // all cocktails
  Draw_Multi_TH(metrics);
  
  //Draw_Multi_TGraph(metrics);

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
    h1[iFile] = new TH1D(name.c_str(),";;Counts",100,0,500);
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
  
  int initPIDs[] = {-11,2112,208,0,0,0};
  int initCIDs[] = {1,3,5,101,301,0};   
  //  int muonCIDs[] = {-71,-72,-73,-81,-82,-83,-91,-92,-93,-101,-102,-103};
//   int muonCIDs[] = {-1,-2,-3,
// 		    -51, -52, -53,
// 		    -81, -82, -83,
// 		    -91, -92, -93,
// 		    -101,-102,-103};

  int muonCIDs[nFiles] = {2002100};
  
  int colorIDs[7]= {kBlue,kRed,kOrange,kGreen+2,kCyan+1,kMagenta+1,kBlack};

  int i = 0, j = 0;
  if(particleID==-13){
    for( int isize = minSize ; isize < maxSize ; isize = isize + sizeStep){
      for ( int iref = minRef ; iref < maxRef ; iref = iref + refStep){
	for ( int iphoto = minPhoto ; iphoto < maxPhoto ; iphoto = iphoto + photoStep){
	  muonCIDs[i] = - isize - iphoto - iref;
	  colors[i] = colorIDs[j];
	  i++;
	  j++;// change color for photocoverage
	}
	//j++;// change color for reflectivity
      }
      //j++; // change color for widths
    }// for( int isize
    
  }// end of: if(partic
  
  for( int i = 0 ; i < nFiles ; i++){
    
    PIDs[i] = initPIDs[i];
    CIDs[i] = initCIDs[i];
    
    // veto study
    if(particleID==-13){
      CIDs[i] = muonCIDs[i];
      PIDs[i] = -13;
    }
    else if  (particleID != 0)
      PIDs[i] = particleID;
    else
      CIDs[i] = cocktailID;
  }
  
  for( int i = 0 ; i < nFiles ; i++){
    cout << " PIDs[" << i << "] = " << PIDs[i] << endl;
    cout << " CIDs[" << i << "] = " << CIDs[i] << endl;
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
  
  hT->SetTitle(";pmt hit time (ns); Counts");

  int nBins[2];
  double min[2], max[2];
  
  for ( int i = 0 ; i < 2 ; i++ ){
        
    if     ( metrics[i] == "nhit" ||
	     metrics[i] == "totPE" ){
      nBins[i]= 100;
      min[i]  = 0.0;
      max[i]  = 200.;
      
      if(PID == -13){
	min[i] = 0.;//400
	max[i] = 500.;//2500.;
      }
    }
    else if( metrics[i] == "mc_energy" ){
      nBins[i]= 100;
      min[i]  = 0.0;
      max[i]  = 10.;
      
      if(PID == -13)
	max[i] = 1100;
    }
    else if( metrics[i] == "mc_r"){
      nBins[i]= 100;
      min[i]  = 0.0;
      max[i]  = 10000.;
    }
    else if( metrics[i] == "mc_z"){
      nBins[i]= 100;
      min[i]  = -10000.0;
      max[i]  = 10000.;
    }
    else if( metrics[i] == "mc_r2"){
      nBins[i]= 100;
      min[i]  = 0.0;
      max[i]  = 10000*10000.;
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

      if(sub_event!=0)
	continue;
       
      // MC info
      mc_p3 = ds->GetMC()->GetMCParticle(0)->GetPosition();
      mc_energy = ds->GetMC()->GetMCParticle(0)->GetKE();

      // Event info
      nhit  = (double)ds->GetEV(sub_event)->GetPMTCount();
      nhit_inner = 0;
      nhit_veto  = 0;
      //ds->GetEV(sub_event)->GetTotalCharge();
      totPE = ds->GetEV(sub_event)->GetTotalCharge();
      
      for (int hit = 0; hit < nhit; hit++) {
	pmt = ds->GetEV(sub_event)->GetPMT(hit);

	if(run->GetPMTInfo()->GetType(pmt->GetID())==1){
	  //  hT->Fill(pmt->GetTime());
	  
	  //if(pmt->GetTime() < 50.)
	  nhit_inner++;
	  
	  // 	    if( pmt->GetTime() < 0){
// 	      cout << " sub_event      = " << sub_event      << endl;      
// 	      cout << " pmt hit time   = " << pmt->GetTime() << endl;      
//	    }
	    
	}
	else if(run->GetPMTInfo()->GetType(pmt->GetID())==2){
	  hT->Fill(pmt->GetTime());
	  nhit_veto++;
	}  
	
	// 	  cout << " sub_event      = " << sub_event      << endl;      
// 	  cout << " pmt hit time   = " << pmt->GetTime() << endl;
      }
      
      nhit = nhit_veto;
      //nhit = nhit_inner;
      
//       if(nhit == 0)
// 	cout << " nhit is zero " << endl;
      
      mc_r = sqrt(mc_p3.x()*mc_p3.x() + mc_p3.y()*mc_p3.y());
      
      for ( int i = 0 ; i < 2 ; i++ ){
	if     ( metrics[i] == "nhit" )
	  metric_variable[i] = (double)nhit; 
	else if( metrics[i] == "totPE" )
	  metric_variable[i] = totPE; 
	else if( metrics[i] == "mc_energy" )
	  metric_variable[i] = mc_energy; 
	else if( metrics[i] == "mc_r" )
	  metric_variable[i] = mc_r; 
	else if( metrics[i] == "mc_r2" )
	  metric_variable[i] = mc_r*mc_r; 
	else if( metrics[i] == "mc_z" )
	  metric_variable[i] = mc_p3.z(); 
	else
	  metric_variable[i] = 0.;
      }
      
      // if(nhit > 8)
      // if(nhit < 5 && mc_energy > 2.0)
      h2->Fill(metric_variable[0],
	       metric_variable[1]);
      
    } // end of: for (sub_event
    
  } //end of:  for (int event = 0
  
  return;
}

void Draw_Multi_TGraph(string metrics[2]){

  double means[nWidths][nRefs][nPhots];

  double widths[] = {1.3,2.3,3.3};
  double refls[]  = {70.,80.,90.,100.};
  double phots[]  = {1.0,1.5,2.0,2.5,3.0};  

  double mean_max = 0.;
  int iref_gr = 3;
  int ipho_gr = 4;
  int iFile = 0;
  
  for ( int iwid = 0 ; iwid < nWidths ; iwid++){
    for ( int iref = 0 ; iref < nRefs ; iref++){
      for ( int ipho = 0 ; ipho < nPhots ; ipho++){
	means[iwid][iref][ipho] = 0.;
	h1[iFile]->Reset();
	
	// only get mean for those used
	if( ( ipho == ipho_gr && nPhots == 1) || 
	    ( iref == iref_gr && nRefs  == 1) ){
	  Fill_TH1D(metrics,iFile);
	  means[iwid][iref][ipho] = h1[iFile]->GetMean();
	  
	  if( means[iwid][iref][ipho] > mean_max )
	    mean_max = means[iwid][iref][ipho];
	  
	}
	else{
	  cout << " check settings for TGraphing " << endl;
	  return;
	}
	
	iFile++;
      }
    }
  }
  
  int nBins = 0;
  double means_wid[nWidths];
  double x_values[nWidths];

  for ( int iwid = 0 ; iwid < nWidths ; iwid++){

    if     ( nPhots==1 ){
      nBins = nRefs;
      for ( int iref = 0 ; iref < nRefs ; iref++){
	means_wid[iref] = means[iwid][iref][ipho_gr];
	x_values[iref]  = refls[iref];
      }
    }
    else if( nRefs==1 ) {
      nBins = nPhots;
      for ( int ipho = 0 ; ipho < nPhots ; ipho++){
	means_wid[ipho] = means[iwid][iref_gr][ipho];
	x_values[ipho]  = phots[ipho];
      }
    }
  }
  
  TH1F *hr;
  hr = c1->DrawFrame(65.,0.,105.,mean_max*1.2);
  //hr->GetXaxis()->SetTitle(" veto photocathode coverage (%)");
  hr->GetXaxis()->SetTitle(" reflectivity (%)");
  hr->GetYaxis()->SetTitle(" nhit mean ");
  hr->Draw();

  for ( int igr = 0 ; igr < nWidths ; igr++){
    g1[igr] = new TGraph(nBins,x_values,means_wid);
    g1[igr]->SetLineColor(colors[igr]);
    g1[igr]->Draw("same LP");  
  }

  TLegend * leg;
  leg = new TLegend(0.2,0.675,0.5,0.875);
  leg->SetHeader(" Veto width");
  leg->AddEntry(g1[2],"3.3 m","lp");
  leg->AddEntry(g1[1],"2.3 m","lp");
  leg->AddEntry(g1[0],"1.3 m","lp");
  leg->Draw();
}


float CIDToWid(int CID){

  int tank_diam    = -CID/100000;
  return (tank_diam - 13.4)/2.;
}

float CIDToPho(int CID){

  CID = -CID;
  int tank_diam    = CID/100000;
  CID     = CID - tank_diam*100000;
  
  return (CID/1000)/10. ;
}

float CIDToRef(int CID){

  CID = -CID;
  
  int tank_diam    = CID/100000;
  CID     = CID - tank_diam*100000;
  
  int phot    = CID/10000;
  int phot_p  = CID/1000 - phot*10;
  
  return(float)(CID - 1000*(phot*10 + phot_p));
}

void Draw_Multi_TH(string metrics[2]){
  
  string option = "";
  string title  = ";";
  string text   = "";

  //TF1 *f1 = new TF1("f1","gaus(0)",0,10);
  
  title += metrics[0];
  title += ";";
  title += "Counts";

  float means[nWidths][nRefs][nPhots];
  
  int iFile = 0;
  for ( int iwid = 0 ; iwid < nWidths ; iwid++){
    for ( int iref = 0 ; iref < nRefs ; iref++){
      for ( int ipho = 0 ; ipho < nPhots ; ipho++){
	h1[iFile]->Reset();
	// Fill histogram
	Fill_TH1D(metrics,iFile);
	h1[iFile]->SetTitle(title.c_str());
	
	means[iwid][iref][ipho] = h1[iFile]->GetMean();
	
	cout << " means[" << iwid << "][" << iref << "][" << ipho << "] = " 
	     << means[iwid][iref][ipho] << endl; 

	iFile++;
      }
    }
  }
  
  float   x_min = 0., x_max = 500.;
  float   x_range[2] = {500.,0.};
  int     max_bin = 0;
  float   maximum = 0.;
  TAxis * x_axis  = nullptr; 
  float   peak_x  = 0;


  for (int iFile = 0 ; iFile < nFiles ; iFile++ ){

    max_bin = h1[iFile]->GetMaximumBin();
    x_axis  = h1[iFile]->GetXaxis();
    peak_x  = x_axis->GetBinCenter(max_bin);
    
    x_min = 0.6*peak_x;
    x_max = 1.3*peak_x;
    
    if( x_min < x_range[0])
      x_range[0] = x_min;
    
    if( x_max > x_range[1])
      x_range[1] = x_max;
    
    if(h1[iFile]->GetMaximum() > maximum)
      maximum = h1[iFile]->GetMaximum();
	
  }

  cout << endl;
  cout << " x_range[0] = " << x_range[0] << endl;
  cout << " x_range[1] = " << x_range[1] << endl;
  cout << " maximum    = " << maximum << endl;

  h1[0]->SetAxisRange(x_range[0],x_range[1]);	
  h1[0]->SetMaximum(maximum*1.3);

  TLegend * leg;
  
  float y1 = 0.89-(float)nFiles/3*0.1;

  leg = new TLegend(0.2,y1,0.8,0.89);
  //leg->SetHeader("width, coverage, reflectivity, nhit mean");
  
  float mean;
  int   mean_int;

  for (int iFile = 0 ; iFile < nFiles ; iFile++ ){
    
    mean = h1[iFile]->GetMean();
    mean_int = (int)round(mean);
    
    text = "";

    //text  = "#color[";
    //text += std::to_string(colors[iFile]);
    //text += "]{";
    //text += GetFileName(PIDs[iFile],CIDs[iFile]);
    //text += "}";
    
    text += " W = ";
    int wid10 = 10*CIDToWid(CIDs[iFile]);
    int wid   = wid10/10;
    int wid_p = wid10-wid*10;
    
    text += std::to_string(wid);
    text += ".";
    text += std::to_string(wid_p);
    text += " m, ";
    
    text += ", P = ";
    int pho10 = 10*CIDToPho(CIDs[iFile]);
    int pho   = pho10/10;
    int pho_p = pho10 - pho*10;
    
    text += std::to_string(pho);
    text += ".";
    text += std::to_string(pho_p);
    text += " %, ";
	
    text += ", R = ";
    text += std::to_string((int)CIDToRef(CIDs[iFile]));
    text += " %";
    
    text += ", M = ";
    text += std::to_string(mean_int);

    cout << " mean = " << mean << endl;
    
    h1[iFile]->SetLineColor(colors[iFile]);
    h1[iFile]->SetLineWidth(2);
    //h1[iFile]->SetLineStyle(iFile/3+1);

    //h1[iFile]->SetFillColor(colors[iFile]);
    //h1[iFile]->SetFillStyle(3000+iFile/3+1);
    
    h1[iFile]->Draw(option.c_str());
    
    option = "same";

    // L - line ,P - marker ,F - fill
    leg->AddEntry(h1[iFile],text.c_str(),"L");
    
    //latex->DrawLatex(0.5,0.85-0.03*iFile,text.c_str());
  }  

  leg->Draw();
}

string GetFileName(int genPID = -11, int fluidID = 1){
  
  string file_name = "wbls_1pct_208Tl_PMT";
  
  switch(fluidID){
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
    
    if(fluidID < 0){
      
      // -2010100
      if(fluidID < -1000000){
	 
	fluidID = -fluidID;
	int size    = fluidID/100000;
	fluidID     = fluidID - size*100000;
	int veto    = fluidID/10000;
	int veto_p  = fluidID/1000 - veto*10;
	int reflect = fluidID - 1000*(veto*10 + veto_p);
	
	//file_name =  "muon_1_GeV_";
	file_name =  "muon_500_MeV_";
	file_name += std::to_string(size);
	file_name += "_m_";
	file_name += std::to_string(veto);
	file_name += "p";
	file_name += std::to_string(veto_p);
	file_name += "_pct_ref_";
	if(reflect < 100)
	  file_name += "0";
	if(reflect < 10 )
	  file_name += "0";
	file_name += std::to_string(reflect);
	file_name += "_pct";

	fluidID = -1;
      }  
      else{
	int reflect   = -fluidID/10*10;
	int photo = -fluidID - reflect;
	file_name =  "OD_";
	file_name += std::to_string(photo);
	file_name += "_pct_ref_";
	if(reflect < 100)
	  file_name += "0";
	if(reflect < 10 )
	  file_name += "0";
	file_name += std::to_string(reflect);    
	file_name +=  "_pct";
      }
      break;
    }
    else{
      cerr << " Error: no matching file found with this fluidID " << fluidID << endl;
      return "";
    }
  }
  
  switch(genPID){
  case(-13):
    break;
  case(-11):
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

string GetFilePath(int genPID = -11, int fluidID = 1){
  
  string file_path = parent_path;
  
  cout << " genPID   = " << genPID  << endl;
  cout << " fluidPID = " << fluidID << endl;

  switch(fluidID){
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
    if(fluidID<0)
      break;
    else{
      cerr << " Error: no matching file found with this fluidID " << endl;
      return "";
    }
  }
  
  switch(genPID){
  case(-13):
    break;
  case(-11):
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
  
  string file_name = GetFileName(genPID,fluidID);
  file_path += file_name;
  file_path += ".root";

  cout << endl;
  cout << " file: " << endl; 
  cout << " " << file_name << endl;
  
  return file_path;
}
