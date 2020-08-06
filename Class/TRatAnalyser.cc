#define TRatAnalyser_cxx
#include "TRatAnalyser.h"
#include <TH2.h>
#include <TStyle.h>
#include <TVector3.h>

#include <iostream>

using namespace std;

void SetRegion(char userRegion = 'I');

void TRatAnalyser::Loop()
{
  if (rat_tree == 0 ||
      run_tree == 0 ) return;
  
  run_tree->GetEntry(0);
  
  int      nhit  = 0, nhit_inner = 0, nhit_veto = 0;
  double   totPE = 0., mc_r  = 0, mc_energy = 0;
  TVector3 mc_p3;
  
  // for filling histogram
  double metric_variable[2];
  
  // set here at temp measure
  string metrics[2];
  metrics[0] = "nhit";
  metrics[1] = "totPE";

  RAT::DS::PMT *pmt;
  
  //
  h2->Reset();
  //h2->SetTitle(title.c_str());
  h2->SetTitle("h2");

  Long64_t n_events = rat_tree->GetEntries();
  
  Long64_t nbytes = 0, nb = 0;
  
  for (int event = 0; event < n_events; event++){
    rat_tree->GetEntry(event);
    
    for (int sub_event = 0; 
	 sub_event < ds->GetEVCount(); 
	 sub_event++) {
      
      if(sub_event!=0)
	continue;
      
      // MC info
      mc_p3     = ds->GetMC()->GetMCParticle(0)->GetPosition();
      mc_energy = ds->GetMC()->GetMCParticle(0)->GetKE();

      // Event info
      nhit  = (double)ds->GetEV(sub_event)->GetPMTCount();
      nhit_inner = 0;
      nhit_veto  = 0;

      totPE = ds->GetEV(sub_event)->GetTotalCharge();
      
      for (int hit = 0; hit < nhit; hit++) {
	pmt = ds->GetEV(sub_event)->GetPMT(hit);
	
	//if(run->GetPMTInfo()->GetType(pmt->GetID())!=1)
	//cout << " id   = " << run->GetPMTInfo()->GetType(pmt->GetID()) << endl;
	
	if(run->GetPMTInfo()->GetType(pmt->GetID())==1){
	  //  hT->Fill(pmt->GetTime());
	  
	  //if(pmt->GetTime() < 50.)
	  nhit_inner++;
	  
// 	  if( pmt->GetTime() < 0){
// 	    cout << " sub_event      = " << sub_event      << endl;      
// 	    cout << " pmt hit time   = " << pmt->GetTime() << endl;      
// 	  }
	  
	}
	else if(run->GetPMTInfo()->GetType(pmt->GetID())==2){
	  //hT->Fill(pmt->GetTime());
	  nhit_veto++;

	}  
	
// 	cout << " sub_event      = " << sub_event      << endl;      
// 	cout << " pmt hit time   = " << pmt->GetTime() << endl;
	
      }// end of: for (int hit = 0; hit <
	
      // nhit = nhit_veto;
      cout << " nhit  = " << nhit << endl;
      cout << " totPE = " << totPE << endl;
      nhit = nhit_inner;
      
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

  h2->Draw("colz");
  c1->Print("can.pdf");

//   for (Long64_t jentry=0; jentry<nentries;jentry++) {
    
//     //Long64_t ientry = LoadTree(jentry);
//     //if (ientry < 0) break;
//     nb = rat_tree->GetEntry(jentry);   nbytes += nb;
    
//     cout << " entry = " << jentry << endl;
    
//     //cout << " ds->GetEVCount() = " << ds->GetEVCount() << endl; 
    
//     for (int sub_event = 0; sub_event < ds->GetEVCount(); sub_event++){
//       //ev = ds->GetEV(sub_event);
      
//       nhit = (double)ds->GetEV(sub_event)->GetPMTCount();
      
//       cout << " sub_event = " << sub_event << endl;
//       cout << " nhit      = " << nhit      << endl;
//       //cout << " ds->GetEVCount() " << ds->GetEVCount() << endl;
      
//     }     
//     // if (Cut(ientry) < 0) continue;
    
//   }

}
