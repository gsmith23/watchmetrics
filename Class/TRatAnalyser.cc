#define TRatAnalyser_cxx
#include "TRatAnalyser.h"
#include <TH2.h>
#include <TStyle.h>
#include <TVector3.h>

#include <iostream>

using namespace std;
using namespace TMath;

void TRatAnalyser::MakeMetricsFile()
{
  
  if (rat_tree == 0 ||
      run_tree == 0 ) return;
  
  run_tree->GetEntry(0);
  
  InitMetricsFile();

  RAT::DS::PMT *pmt;

  Long64_t n_events = rat_tree->GetEntries();
  
  TVector3 mc_p3;
  
  for (int event = 0; event < n_events; event++){
    rat_tree->GetEntry(event);
    
    pmtID.clear();
    charge.clear();
    
    for (int sub_event = 0; 
	 sub_event < ds->GetEVCount(); 
	 sub_event++) {
      
      if(sub_event!=0)
	continue;
      
      mc_p3 = ds->GetMC()->GetMCParticle(0)->GetPosition();
      mc_E  = ds->GetMC()->GetMCParticle(0)->GetKE();
      
      mc_x = mc_p3.x();
      mc_y = mc_p3.y();
      mc_z = mc_p3.z();
      mc_r = sqrt(mc_p3.x()*mc_p3.x() + mc_p3.y()*mc_p3.y());
      
      // Event info
      nhit  = ds->GetEV(sub_event)->GetPMTCount();
      totPE = ds->GetEV(sub_event)->GetTotalCharge();
      
      for (unsigned short hit = 0; hit < nhit; hit++) {
	pmt = ds->GetEV(sub_event)->GetPMT(hit);
	pmtID.push_back(run->GetPMTInfo()->GetType(pmt->GetID()));
	charge.push_back(pmt->GetCharge());
	time.push_back(pmt->GetTime());
      }
    
    } // end of: for (sub_event

    metrics_tree->Fill();
   
  } //end of:  for (int event = 0
  
  metrics_tree->Write();
  metrics_file->Write();

}

void TRatAnalyser::Make2DHisto(string metric1  = "nhit",
			       string metric2  = "totPE",
			       bool   useInner = true){
  string metrics[2];
  metrics[0] = metric1;
  metrics[1] = metric2;
  
  // for filling histogram
  float metric_variable[2];
  float totPE_inner = 0., totPE_veto = 0.;
  unsigned short nhit_inner = 0, nhit_veto = 0;
  
  h2->Reset();

  string title = ";";
  title += metrics[0];
  title += ";";
  title += metrics[1];
  
  h2->SetTitle(title.c_str());

  Long64_t n_events = rat_tree->GetEntries();
  
  for (Long64_t event = 0; event < n_events; event++){
    metrics_tree->GetEntry(event);
    
    nhit_inner = 0;
    nhit_veto  = 0;
    totPE_inner = 0.;
    totPE_veto  = 0.;

    for (unsigned short hit = 0; hit < nhit; hit++) {
      
      if     (pmtID[hit]==1){
	
	nhit_inner++;
	totPE_inner += charge[hit]; 
	
	if(useInner)
	  hT->Fill(time[hit]);
      }
      else if(pmtID[hit]==2){
	
	nhit_veto++;
	totPE_veto += charge[hit]; 

	if(!useInner)
	  hT->Fill(time[hit]);
      }  
      
    }// end of: for (int hit = 0; hit <
	
    if( (nhit_inner+nhit_veto)!=nhit ){
      cout << endl;
      cout << " (nhit_inner+nhit_veto)!=nhit " << endl;
    }
    
    if( Abs(totPE_inner+totPE_veto-totPE) > 0.1  ){
      cout << endl;
      cout << " (totPE_inner+totPE_veto)!=totPE " << endl;
    }
    
    if(useInner){
      nhit  = nhit_inner;
      totPE = totPE_inner;
    }
    else{
      nhit  = nhit_veto;
      //totPE = totPE_veto;
    }
    
    for ( int i = 0 ; i < 2 ; i++ ){
      if     ( metrics[i] == "nhit" )
	metric_variable[i] = (float)nhit; 
      else if( metrics[i] == "totPE" )
	metric_variable[i] = totPE; 
      else if( metrics[i] == "mc_E" )
	metric_variable[i] = mc_E; 
      else if( metrics[i] == "mc_r" )
	metric_variable[i] = mc_r; 
      else if( metrics[i] == "mc_r2" )
	metric_variable[i] = mc_r*mc_r; 
      else if( metrics[i] == "mc_z" )
	metric_variable[i] = mc_z; 
      else
	metric_variable[i] = 0.;
    }
    
    if(     mc_r  < 3000   && 
	Abs(mc_z) < 3000 ) 
      h2->Fill(metric_variable[0],
	       metric_variable[1]);
    
  } //end of:  for (int event = 0
  
  h2->Draw("colz");
  c1->Print("can.pdf");
  
}

