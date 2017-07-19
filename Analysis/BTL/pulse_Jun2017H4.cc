#define pulse_Jun2017H4_cxx
#include "pulse_Jun2017H4.hh"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TProfile.h>
#include "Utils.hh"

using namespace std;

void pulse_Jun2017H4::Loop()
{
//   In a ROOT session, you can do:
//      root> .L pulse_Jun2017H4.C
//      root> pulse_Jun2017H4 t
//      root> t.GetEntry(12); // Fill t data members with entry number 12
//      root> t.Show();       // Show values of entry 12
//      root> t.Show(16);     // Read and show values of entry 16
//      root> t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
   }
}

void pulse_Jun2017H4::Time_Res(double lo, double hi, double xlo, double xhi, double ylo, double yhi){

  TFile* outputFile=TFile::Open("time_res.root","recreate");

  TH1D *h_deltaT = new TH1D("delta_T", "delta_T", 100, -1.0, 1.0);
  h_deltaT->Sumw2();
  TH2D *h_deltaT_amp1 = new TH2D("deltaTamp1", "deltaTamp1", 100, -1.0, 1.0, 10, lo, hi);
  h_deltaT_amp1->Sumw2();
  TH2D *h_deltaT_amp2 = new TH2D("deltaTamp2", "deltaTamp2", 100, -1.0, 1.0, 10, lo, hi);
  h_deltaT_amp2->Sumw2();

  TH1D *h_deltaT_cor = new TH1D("delta_T_cor", "delta_T_cor", 100, -1.0, 1.0);
  h_deltaT_cor->Sumw2();
  TH2D *h_deltaT_amp1_cor = new TH2D("deltaTamp1_cor", "deltaTamp1_cor", 100, -1.0, 1.0, 10, lo, hi);
  h_deltaT_amp1_cor->Sumw2();
  TH2D *h_deltaT_amp2_cor = new TH2D("deltaTamp2_cor", "deltaTamp2_cor", 100, -1.0, 1.0, 10, lo, hi);
  h_deltaT_amp2_cor->Sumw2();

  TH1D *h_dT  = new TH1D("h_dT", "h_dT", 100, -1.0, 1.0);
  h_dT->Sumw2();
  TH2D *h_dT_TDCx = new TH2D("h_dT_TDCx", "h_dT_TDCx", 100, -1.0, 1.0, 20, xlo, xhi);
  h_dT_TDCx->Sumw2();
  TH2D *h_dT_TDCy = new TH2D("h_dT_TDCy", "h_dT_TDCy", 100, -1.0, 1.0, 20, ylo, yhi);
  h_dT_TDCy->Sumw2();
  TH2D *h_dT_amp_posCor = new TH2D("h_dT_amp_posCor", "h_dT_amp_posCor", 100, -1.0, 1.0, 10, lo, hi);
  h_dT_amp_posCor->Sumw2();

  TH2D *h_dT_amp = new TH2D("h_dT_amp", "h_dT_amp", 100, -1.0, 1.0, 10, lo, hi);
  h_dT_amp->Sumw2();
  TH2D *h_dT_TDCx_ampCor = new TH2D("h_dT_TDCx_ampCor", "h_dT_TDCx_ampCor", 100, -1.0, 1.0, 20, xlo, xhi);
  h_dT_TDCx_ampCor->Sumw2();
  TH2D *h_dT_TDCy_ampCor = new TH2D("h_dT_TDCy_ampCor", "h_dT_TDCy_ampCor", 100, -1.0, 1.0, 20, ylo, yhi);
  h_dT_TDCy_ampCor->Sumw2();

  TH1D *h_dT_position_amp = new TH1D("h_dT_position_amp", "h_dT_position_amp", 100, -1.0, 1.0);
  h_dT_position_amp->Sumw2();
  TH1D *h_dT_amp_position = new TH1D("h_dT_amp_position", "h_dT_amp_position", 100, -1.0, 1.0);
  h_dT_amp_position->Sumw2();

  if (fChain == 0) return;
  Long64_t nentries = fChain->GetEntriesFast();
  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      if (jentry % 100 == 0) cout << "Processing Event " << jentry << " of " << nentries << "\n";

      nb = fChain->GetEntry(jentry);   nbytes += nb; 
  
      double t1 = linearTime45[11];
      double t2 = linearTime45[12];
      double Photek_amp = amp[0];
      double SiPM1_amp = amp[5];
      double SiPM2_amp = amp[6];
      double t0 = gauspeak[0];
      if(Photek_amp >0.08 && Photek_amp<0.2 && SiPM1_amp>lo && SiPM1_amp<hi && SiPM2_amp > lo && SiPM2_amp<hi && TDCy>ylo && TDCy<yhi && TDCx > xlo && TDCx < xhi){
         double deltaT = t1-t2;
         h_deltaT->Fill(deltaT);
         h_deltaT_amp1->Fill(deltaT, SiPM1_amp);
         h_deltaT_amp2->Fill(deltaT, SiPM2_amp);
         double dT = t1 - t0;
         h_dT->Fill(dT);
         h_dT_TDCx->Fill(dT, TDCx);//position x
         h_dT_TDCy->Fill(dT, TDCy);//position y
         h_dT_amp->Fill(dT,SiPM1_amp);
      }     
  }

  //amp1 correction for deltaT
  TProfile* h_deltaT_amp1_x = h_deltaT_amp1->ProfileY();
  h_deltaT_amp1_x->Fit("pol2");
  TF1 *fit1 = h_deltaT_amp1_x->GetFunction("pol2");
  Double_t* p1 = fit1->GetParameters();

  //amp2 correction for deltaT
  TProfile* h_deltaT_amp2_x = h_deltaT_amp2->ProfileY();
  h_deltaT_amp2_x->Fit("pol2");
  TF1 *fit2 = h_deltaT_amp2_x->GetFunction("pol2");
  Double_t* p2 = fit2->GetParameters();

  //fit time resolution for deltaT before correction
  h_deltaT->Fit("gaus");
  TF1 *fit = h_deltaT->GetFunction("gaus");
  Double_t* p_deltaT = fit->GetParameters();

  //fit time resolution for dT before correction
  h_dT->Fit("gaus");
  TF1 *fit_dT = h_dT->GetFunction("gaus");
  Double_t* p_dT = fit_dT->GetParameters();

  //fit position dependence for dT
  TProfile* h_dT_TDCx_x = h_dT_TDCx->ProfileY();
  h_dT_TDCx_x->Fit("pol8");
  TF1 *fit_dT_TDCx = h_dT_TDCx_x->GetFunction("pol8");
  Double_t* p_dT_TDCx = fit_dT_TDCx->GetParameters();

  TProfile* h_dT_TDCy_x = h_dT_TDCy->ProfileY();
  h_dT_TDCy_x->Fit("pol9");
  TF1 *fit_dT_TDCy = h_dT_TDCy_x->GetFunction("pol9");
  Double_t* p_dT_TDCy = fit_dT_TDCy->GetParameters();
 
  //fit amp dependency for dT
  TProfile* h_dT_amp_x = h_dT_amp->ProfileY();
  h_dT_amp_x->Fit("pol2");
  TF1 *fit_dT_amp = h_dT_amp_x->GetFunction("pol2");
  Double_t* p_dT_amp = fit_dT_amp->GetParameters();
  
  nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      if (jentry % 100 == 0) cout << "Processing Event " << jentry << " of " << nentries << "\n";

      nb = fChain->GetEntry(jentry);   nbytes += nb;

      double t1 = linearTime45[11];
      double t2 = linearTime45[12];
      double Photek_amp = amp[0];
      double SiPM1_amp = amp[5];
      double SiPM2_amp = amp[6];
      double t0 = gauspeak[0];
      if(Photek_amp >0.08 && Photek_amp<0.2 && SiPM1_amp>lo && SiPM1_amp<hi && SiPM2_amp > lo && SiPM2_amp<hi && TDCy>ylo && TDCy<yhi && TDCx > xlo && TDCx < xhi){

       double deltaT_cor = t1-t2 - p1[2]*SiPM1_amp*SiPM1_amp - p1[1]*SiPM1_amp - p1[0] - p2[2]*SiPM2_amp*SiPM2_amp - p2[1]*SiPM2_amp - p2[0];
       h_deltaT_cor->Fill(deltaT_cor);
       h_deltaT_amp1_cor->Fill(deltaT_cor, SiPM1_amp);
       h_deltaT_amp2_cor->Fill(deltaT_cor, SiPM2_amp);

       double dT_pos = t1-t0 - p_dT_TDCy[9]*TDCy*TDCy*TDCy*TDCy*TDCy*TDCy*TDCy*TDCy*TDCy - p_dT_TDCy[8]*TDCy*TDCy*TDCy*TDCy*TDCy*TDCy*TDCy*TDCy - p_dT_TDCy[7]*TDCy*TDCy*TDCy*TDCy*TDCy*TDCy*TDCy - p_dT_TDCy[6]*TDCy*TDCy*TDCy*TDCy*TDCy*TDCy - p_dT_TDCy[5]*TDCy*TDCy*TDCy*TDCy*TDCy - p_dT_TDCy[4]*TDCy*TDCy*TDCy*TDCy - p_dT_TDCy[3]*TDCy*TDCy*TDCy -  p_dT_TDCy[2]*TDCy*TDCy - p_dT_TDCy[1]*TDCy - p_dT_TDCy[0] - p_dT_TDCx[8]*TDCx*TDCx*TDCx*TDCx*TDCx*TDCx*TDCx*TDCx - p_dT_TDCx[7]*TDCx*TDCx*TDCx*TDCx*TDCx*TDCx*TDCx - p_dT_TDCx[6]*TDCx*TDCx*TDCx*TDCx*TDCx*TDCx - p_dT_TDCx[5]*TDCx*TDCx*TDCx*TDCx*TDCx - p_dT_TDCx[4]*TDCx*TDCx*TDCx*TDCx - p_dT_TDCx[3]*TDCx*TDCx*TDCx -  p_dT_TDCx[2]*TDCx*TDCx - p_dT_TDCx[1]*TDCx - p_dT_TDCx[0];
       h_dT_amp_posCor->Fill(dT_pos, SiPM1_amp);

       double dT_amp = t1-t0 - p_dT_amp[2]*SiPM1_amp*SiPM1_amp  - p_dT_amp[1]*SiPM1_amp - p_dT_amp[0];
       h_dT_TDCy_ampCor->Fill(dT_amp, TDCy);
       h_dT_TDCx_ampCor->Fill(dT_amp, TDCx);
    }
  }

  TProfile* h_deltaT_amp1_x_cor = h_deltaT_amp1_cor->ProfileY();
  TProfile* h_deltaT_amp2_x_cor = h_deltaT_amp2_cor->ProfileY();

  //deltaT after amp correction
  h_deltaT_cor->Fit("gaus");
  TF1 *fit_cor = h_deltaT_cor->GetFunction("gaus");
  Double_t* p_deltaT_cor = fit_cor->GetParameters();

  //double correction position->energy
  TProfile* h_dT_amp_posCor_x = h_dT_amp_posCor->ProfileY();
  h_dT_amp_posCor_x->Fit("pol2");
  TF1 *fit_dT_amp_2 = h_dT_amp_posCor_x->GetFunction("pol2");

  Double_t* p_dT_amp_2 = fit_dT_amp_2->GetParameters();

  //double correction energy->position
  TProfile* h_dT_TDCx_ampCor_x = h_dT_TDCx_ampCor->ProfileY();
  h_dT_TDCx_ampCor_x->Fit("pol8");
  TF1 *fit_dT_TDCx_2 = h_dT_TDCx_ampCor_x->GetFunction("pol8");
  Double_t* p_dT_TDCx_2 = fit_dT_TDCx_2->GetParameters();

  TProfile* h_dT_TDCy_ampCor_x = h_dT_TDCy_ampCor->ProfileY();
  h_dT_TDCy_ampCor_x->Fit("pol9");
  TF1 *fit_dT_TDCy_2 = h_dT_TDCy_ampCor_x->GetFunction("pol9");
  Double_t* p_dT_TDCy_2 = fit_dT_TDCy_2->GetParameters();

    nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      if (jentry % 100 == 0) cout << "Processing Event " << jentry << " of " << nentries << "\n";

      nb = fChain->GetEntry(jentry);   nbytes += nb;
      double t1 = linearTime45[11];
      double t2 = linearTime45[12];
      double Photek_amp = amp[0];
      double SiPM1_amp = amp[5];
      double SiPM2_amp = amp[6];
      double t0 = gauspeak[0];
      if(Photek_amp >0.08 && Photek_amp<0.2 && SiPM1_amp>lo && SiPM1_amp<hi && SiPM2_amp > lo && SiPM2_amp<hi && TDCy>ylo && TDCy<yhi && TDCx > xlo && TDCx < xhi){
       double dT_position_amp = t1-t0 - p_dT_TDCy[9]*TDCy*TDCy*TDCy*TDCy*TDCy*TDCy*TDCy*TDCy*TDCy - p_dT_TDCy[8]*TDCy*TDCy*TDCy*TDCy*TDCy*TDCy*TDCy*TDCy - p_dT_TDCy[7]*TDCy*TDCy*TDCy*TDCy*TDCy*TDCy*TDCy - p_dT_TDCy[6]*TDCy*TDCy*TDCy*TDCy*TDCy*TDCy - p_dT_TDCy[5]*TDCy*TDCy*TDCy*TDCy*TDCy - p_dT_TDCy[4]*TDCy*TDCy*TDCy*TDCy - p_dT_TDCy[3]*TDCy*TDCy*TDCy - p_dT_TDCy[2]*TDCy*TDCy - p_dT_TDCy[1]*TDCy - p_dT_TDCy[0] - p_dT_TDCx[8]*TDCx*TDCx*TDCx*TDCx*TDCx*TDCx*TDCx*TDCx - p_dT_TDCx[7]*TDCx*TDCx*TDCx*TDCx*TDCx*TDCx*TDCx - p_dT_TDCx[6]*TDCx*TDCx*TDCx*TDCx*TDCx*TDCx - p_dT_TDCx[5]*TDCx*TDCx*TDCx*TDCx*TDCx - p_dT_TDCx[4]*TDCx*TDCx*TDCx*TDCx - p_dT_TDCx[3]*TDCx*TDCx*TDCx -  p_dT_TDCx[2]*TDCx*TDCx - p_dT_TDCx[1]*TDCx - p_dT_TDCx[0] - p_dT_amp_2[2]*SiPM1_amp*SiPM1_amp - p_dT_amp_2[1]*SiPM1_amp - p_dT_amp_2[0];

       h_dT_position_amp->Fill(dT_position_amp, SiPM1_amp);

       double dT_amp_position = t1-t0 - p_dT_amp[2]*SiPM1_amp*SiPM1_amp  - p_dT_amp[1]*SiPM1_amp - p_dT_amp[0] - p_dT_TDCx_2[8]*TDCx*TDCx*TDCx*TDCx*TDCx*TDCx*TDCx*TDCx - p_dT_TDCx_2[7]*TDCx*TDCx*TDCx*TDCx*TDCx*TDCx*TDCx - p_dT_TDCx_2[6]*TDCx*TDCx*TDCx*TDCx*TDCx*TDCx - p_dT_TDCx_2[5]*TDCx*TDCx*TDCx*TDCx*TDCx - p_dT_TDCx_2[4]*TDCx*TDCx*TDCx*TDCx - p_dT_TDCx_2[3]*TDCx*TDCx*TDCx - p_dT_TDCx_2[2]*TDCx*TDCx - p_dT_TDCx_2[1]*TDCx - p_dT_TDCx_2[0] - p_dT_TDCy_2[9]*TDCy*TDCy*TDCy*TDCy*TDCy*TDCy*TDCy*TDCy*TDCy - p_dT_TDCy_2[8]*TDCy*TDCy*TDCy*TDCy*TDCy*TDCy*TDCy*TDCy - p_dT_TDCy_2[7]*TDCy*TDCy*TDCy*TDCy*TDCy*TDCy*TDCy - p_dT_TDCy_2[6]*TDCy*TDCy*TDCy*TDCy*TDCy*TDCy - p_dT_TDCy_2[5]*TDCy*TDCy*TDCy*TDCy*TDCy - p_dT_TDCy_2[4]*TDCy*TDCy*TDCy*TDCy - p_dT_TDCy_2[3]*TDCy*TDCy*TDCy - p_dT_TDCy_2[2]*TDCy*TDCy - p_dT_TDCy_2[1]*TDCy - p_dT_TDCy_2[0];
       h_dT_amp_position->Fill(dT_amp_position, SiPM1_amp);
    }
  }

  h_dT_position_amp->Fit("gaus");
  h_dT_amp_position->Fit("gaus");

  outputFile->cd();
  h_deltaT->Write();
  h_deltaT_amp1->Write();
  h_deltaT_amp2->Write();
  h_deltaT_amp1_x->Write();
  h_deltaT_amp2_x->Write();
  h_deltaT_cor->Write();
  h_deltaT_amp1_cor->Write();
  h_deltaT_amp2_cor->Write();
  h_deltaT_amp1_x_cor->Write();
  h_deltaT_amp2_x_cor->Write();

  h_dT->Write();
  h_dT_TDCy->Write();
  h_dT_TDCx->Write();
  h_dT_amp->Write();
  h_dT_TDCy_ampCor->Write();
  h_dT_TDCx_ampCor->Write();
  h_dT_TDCy_x->Write();
  h_dT_TDCx_x->Write();
  h_dT_amp_posCor->Write();
  h_dT_amp_posCor_x->Write();
  h_dT_position_amp->Write();

  h_dT_amp_x->Write();
  h_dT_TDCx_ampCor_x->Write();
  h_dT_TDCy_ampCor_x->Write();
  h_dT_amp_position->Write();
  outputFile->Close();

};
