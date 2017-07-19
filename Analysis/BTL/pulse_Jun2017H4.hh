//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Jul 19 16:14:38 2017 by ROOT version 6.06/06
// from TTree t1065/t1065_tree
// found on file: /eos/cms/store/group/phys_susy/razor/Timing/Jun2017CERN/skimmedNtuples/analysis_7803-7830_PhotekSkim.root
//////////////////////////////////////////////////////////

#ifndef pulse_Jun2017H4_h
#define pulse_Jun2017H4_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TString.h>

// Header file for the classes stored in the TTree if any.

class pulse_Jun2017H4 {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   UInt_t          event;
   UInt_t          ngroups;
   UInt_t          nsamples;
   UInt_t          nchannels;
   UShort_t        tc[4];
   UShort_t        b_c[36864];
   Short_t         raw[36][1024];
   Int_t           t[36864];
   Short_t         channel[36][1024];
   Short_t         channelCorrected[36][1024];
   Int_t           t0[1024];
   Float_t         time[4][1024];
   Float_t         xmin[36];
   Float_t         amp[36];
   Float_t         base[36];
   //Float_t         int[36];
   Float_t         intfull[36];
   Float_t         risetime[36];
   Float_t         gauspeak[36];
   Float_t         linearTime0[36];
   Float_t         linearTime15[36];
   Float_t         linearTime30[36];
   Float_t         linearTime45[36];
   Float_t         linearTime60[36];
   Float_t         sigmoidTime[36];
   Float_t         fullFitTime[36];
   Float_t         constantThresholdTime[36];
   Float_t         TDCx;
   Float_t         TDCy;

   // List of branches
   TBranch        *b_event;   //!
   TBranch        *b_ngroups;   //!
   TBranch        *b_nsamples;   //!
   TBranch        *b_nchannels;   //!
   TBranch        *b_tc;   //!
   TBranch        *b_b_c;   //!
   TBranch        *b_raw;   //!
   TBranch        *b_t;   //!
   TBranch        *b_channel;   //!
   TBranch        *b_channelCorrected;   //!
   TBranch        *b_t0;   //!
   TBranch        *b_time;   //!
   TBranch        *b_xmin;   //!
   TBranch        *b_amp;   //!
   TBranch        *b_base;   //!
   //TBranch        *b_int;   //!
   TBranch        *b_intfull;   //!
   TBranch        *b_risetime;   //!
   TBranch        *b_gauspeak;   //!
   TBranch        *b_linearTime0;   //!
   TBranch        *b_linearTime15;   //!
   TBranch        *b_linearTime30;   //!
   TBranch        *b_linearTime45;   //!
   TBranch        *b_linearTime60;   //!
   TBranch        *b_sigmoidTime;   //!
   TBranch        *b_fullFitTime;   //!
   TBranch        *b_constantThresholdTime;   //!
   TBranch        *b_TDCx;   //!
   TBranch        *b_TDCy;   //!

   pulse_Jun2017H4(TTree *tree=0);
   virtual ~pulse_Jun2017H4();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);

   //virtual std::pair<float,float> MPV_vs_Position( TString coor = "X", const int channel = -1, const float coorLow = 0, const float step = 25.,const float AmpLowCut = 0, const float AmpHighCut = 0.0);
   //virtual void CreateMPV_vs_PositionHisto(  );
   //void MakeEfficiencyVsXY(int channelNumber);
   //void MakeEfficiencyVsRun(int channelNumber);
   //void MakeTimingPlotsVsXY(int channelNumber);
   void Time_Res(double lo, double hi, double xlo, double xhi, double ylo, double yhi); 

};

#endif

#ifdef pulse_Jun2017H4_cxx
pulse_Jun2017H4::pulse_Jun2017H4(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/eos/cms/store/group/phys_susy/razor/Timing/Jun2017CERN/skimmedNtuples/analysis_7803-7830_PhotekSkim.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("/eos/cms/store/group/phys_susy/razor/Timing/Jun2017CERN/skimmedNtuples/analysis_7803-7830_PhotekSkim.root");
      }
      f->GetObject("t1065",tree);

   }
   Init(tree);
}

pulse_Jun2017H4::~pulse_Jun2017H4()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t pulse_Jun2017H4::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t pulse_Jun2017H4::LoadTree(Long64_t entry)
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

void pulse_Jun2017H4::Init(TTree *tree)
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

   fChain->SetBranchAddress("event", &event, &b_event);
   fChain->SetBranchAddress("ngroups", &ngroups, &b_ngroups);
   fChain->SetBranchAddress("nsamples", &nsamples, &b_nsamples);
   fChain->SetBranchAddress("nchannels", &nchannels, &b_nchannels);
   fChain->SetBranchAddress("tc", tc, &b_tc);
   fChain->SetBranchAddress("b_c", b_c, &b_b_c);
   fChain->SetBranchAddress("raw", raw, &b_raw);
   fChain->SetBranchAddress("t", t, &b_t);
   fChain->SetBranchAddress("channel", channel, &b_channel);
   fChain->SetBranchAddress("channelCorrected", channelCorrected, &b_channelCorrected);
   fChain->SetBranchAddress("t0", t0, &b_t0);
   fChain->SetBranchAddress("time", time, &b_time);
   fChain->SetBranchAddress("xmin", xmin, &b_xmin);
   fChain->SetBranchAddress("amp", amp, &b_amp);
   fChain->SetBranchAddress("base", base, &b_base);
   //fChain->SetBranchAddress("int", int, &b_int);
   fChain->SetBranchAddress("intfull", intfull, &b_intfull);
   fChain->SetBranchAddress("risetime", risetime, &b_risetime);
   fChain->SetBranchAddress("gauspeak", gauspeak, &b_gauspeak);
   fChain->SetBranchAddress("linearTime0", linearTime0, &b_linearTime0);
   fChain->SetBranchAddress("linearTime15", linearTime15, &b_linearTime15);
   fChain->SetBranchAddress("linearTime30", linearTime30, &b_linearTime30);
   fChain->SetBranchAddress("linearTime45", linearTime45, &b_linearTime45);
   fChain->SetBranchAddress("linearTime60", linearTime60, &b_linearTime60);
   fChain->SetBranchAddress("sigmoidTime", sigmoidTime, &b_sigmoidTime);
   fChain->SetBranchAddress("fullFitTime", fullFitTime, &b_fullFitTime);
   fChain->SetBranchAddress("constantThresholdTime", constantThresholdTime, &b_constantThresholdTime);
   fChain->SetBranchAddress("TDCx", &TDCx, &b_TDCx);
   fChain->SetBranchAddress("TDCy", &TDCy, &b_TDCy);
   Notify();
}

Bool_t pulse_Jun2017H4::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void pulse_Jun2017H4::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t pulse_Jun2017H4::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef pulse_Jun2017H4_cxx
