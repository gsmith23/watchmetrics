#include "TRandom.h"
#include "TH1.h"
#include "TMath.h"
#include "TPad.h"

void noiseGen(double rate = 8000, double timeWindow = 400e-9, int npmt=400){

  TRandom ran;
  TH1F* h = new TH1F("h","h", 11, -0.5, 10.5);
  TH1F* h2 = new TH1F("h2","h2", 11, -0.5, 10.5);

  // one million time windows
  // 
  for (int i= 0; i < 1000000; ++i){
    int nhitpmt =0., nhitpmt2 =0;
    
    // increment total hits for all pmts in one time window
    // (nhitpmt2 is incremented only once per pmt hit))
    for (int ipmt = 0; ipmt < npmt ; ++ipmt){

      // use probability of time between hits 
      // to count number of hits in window
      int nhit =-1;  double t=0;
      while (t < timeWindow){
        ++nhit;
        double r = ran.Uniform(0., 1.);
	t += -TMath::Log(r)/rate;
	//  std::cout << t << std::endl;
      } // while
      
      nhitpmt += nhit; 

      if(nhit>0)
	nhitpmt2++;

    } // pmt
    h->Fill(nhitpmt);
    h2->Fill(nhitpmt2);
    // std::cout << nhitpmt << std::endl;
  }// event
  //gPad->SetLogy();
  h->Draw("HISTO");
  h2->SetLineColor(kBlue);
  h2->Draw("HISTO same");
}
