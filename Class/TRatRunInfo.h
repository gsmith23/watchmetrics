#ifndef TRatRunInfo_h
#define TRatRunInfo_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include <RAT/DS/Run.hh>
#include "TObject.h"
#include <RAT/DS/PMTInfo.hh>

class TRatRunInfo {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.
   static constexpr Int_t kMaxpmtinfo = 1;

   // Declaration of leaf types
   RAT::DS::Run    *run = new RAT::DS::Run();
   UInt_t          fUniqueID;
   UInt_t          fBits;
   Int_t           id;
   ULong64_t       type;
   Long_t          startTime;
   Int_t           pmtinfo_;
   UInt_t          pmtinfo_fUniqueID[kMaxpmtinfo];   //[pmtinfo_]
   UInt_t          pmtinfo_fBits[kMaxpmtinfo];   //[pmtinfo_]
   vector<TVector3> pmtinfo_pos[kMaxpmtinfo];
   vector<TVector3> pmtinfo_dir[kMaxpmtinfo];
   vector<int>     pmtinfo_type[kMaxpmtinfo];
   vector<int>     pmtinfo_modeltype[kMaxpmtinfo];
   vector<string>  pmtinfo_models[kMaxpmtinfo];

   // List of branches
   TBranch        *b_run_fUniqueID;   //!
   TBranch        *b_run_fBits;   //!
   TBranch        *b_run_id;   //!
   TBranch        *b_run_type;   //!
   TBranch        *b_run_startTime;   //!
   TBranch        *b_run_pmtinfo_;   //!
   TBranch        *b_pmtinfo_fUniqueID;   //!
   TBranch        *b_pmtinfo_fBits;   //!
   TBranch        *b_pmtinfo_pos;   //!
   TBranch        *b_pmtinfo_dir;   //!
   TBranch        *b_pmtinfo_type;   //!
   TBranch        *b_pmtinfo_modeltype;   //!
   TBranch        *b_pmtinfo_models;   //!

   TRatRunInfo(TTree *tree=0);
   virtual ~TRatRunInfo();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef TRatRunInfo_cxx
TRatRunInfo::TRatRunInfo(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("OD_1_pct_ref_000_pct.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("OD_1_pct_ref_000_pct.root");
      }
      f->GetObject("runT",tree);

   }
   Init(tree);
}

TRatRunInfo::~TRatRunInfo()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t TRatRunInfo::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t TRatRunInfo::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void TRatRunInfo::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("fUniqueID", &fUniqueID, &b_run_fUniqueID);
   fChain->SetBranchAddress("fBits", &fBits, &b_run_fBits);
   fChain->SetBranchAddress("id", &id, &b_run_id);
   fChain->SetBranchAddress("type", &type, &b_run_type);
   fChain->SetBranchAddress("startTime", &startTime, &b_run_startTime);
   fChain->SetBranchAddress("pmtinfo", &pmtinfo_, &b_run_pmtinfo_);
   fChain->SetBranchAddress("pmtinfo.fUniqueID", pmtinfo_fUniqueID, &b_pmtinfo_fUniqueID);
   fChain->SetBranchAddress("pmtinfo.fBits", pmtinfo_fBits, &b_pmtinfo_fBits);
   fChain->SetBranchAddress("pmtinfo.pos", pmtinfo_pos, &b_pmtinfo_pos);
   fChain->SetBranchAddress("pmtinfo.dir", pmtinfo_dir, &b_pmtinfo_dir);
   fChain->SetBranchAddress("pmtinfo.type", pmtinfo_type, &b_pmtinfo_type);
   fChain->SetBranchAddress("pmtinfo.modeltype", pmtinfo_modeltype, &b_pmtinfo_modeltype);
   fChain->SetBranchAddress("pmtinfo.models", pmtinfo_models, &b_pmtinfo_models);
   Notify();
}

Bool_t TRatRunInfo::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void TRatRunInfo::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t TRatRunInfo::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef TRatRunInfo_cxx
