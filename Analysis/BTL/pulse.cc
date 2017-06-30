#define pulse_cxx
#include "pulse.hh"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include "EfficiencyUtils.hh"

using namespace std;

void pulse::Loop()
{
//   In a ROOT session, you can do:
//      root> .L pulse.C
//      root> pulse t
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


void pulse::MakeEfficiencyVsXY(int channelNumber) {

  //declare histograms
  TH1F *histY_den = new TH1F("histX_den",";Y [mm];Number of Events", 50, 25,29);
  TH1F *histY_num = new TH1F("histX_num",";Y [mm];Number of Events", 50, 25,29);
  TH1F *histX_den = new TH1F("histY_den",";X [mm];Number of Events", 50, 14,18);
  TH1F *histX_num = new TH1F("histY_num",";X [mm];Number of Events", 50, 14,18);
  
  if (fChain == 0) return;
   Long64_t nentries = fChain->GetEntriesFast();
   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      if (jentry % 100 == 0) cout << "Processing Event " << jentry << " of " << nentries << "\n";

      nb = fChain->GetEntry(jentry);   nbytes += nb;

      //cuts
   
      //require photek to show MIP signal
      if (!(amp[0] > 0.1 && amp[0] < 0.3)) continue;

      //reject events with more than 1 track
      if ( !(ntracks == 1 && chi2 < 10 )) continue;
      if ( !(fabs(xSlope) < 5e-4 && fabs(ySlope) < 5e-4)) continue;
      if ( !(amp[channelNumber] < 0.3 )) continue;

      // if (!(x1 > 15000 && x1 < 18000 && y1 > 21000 && y1 < 24000)) continue; //CNM irradiated
      //if (!(x1 > 11300 && x1 < 14600 && y1 > 25000 && y1 < 28500)) continue; //HPK KU 2 channel
      //if (!(x1 > 15000 && x1 < 17000 && y1 > 25600 && y1 < 28200)) continue; //HPK KU 2 channel

      if (y1 > 25800 && y1 < 28100 ) {
	histX_den->Fill( 0.001*x1);
	if (amp[channelNumber] > 0.05) {
	    histX_num->Fill( 0.001*x1 );
	}
      }
      
      if ( x1 > 15000 && x1 < 17100) {
	histY_den->Fill( 0.001*y1 );
	if (amp[channelNumber] > 0.05) {
	  histY_num->Fill( 0.001*y1 );       
	} 
      }
    
	      
   }

   vector<double> xbins; 
   vector<double> ybins;
   TGraphAsymmErrors* effX = createEfficiencyGraph(histX_num, histX_den,
						   Form("EfficiencyVsX_Channel%d",channelNumber),
						   xbins,
						   14, 18,
						   0.0, 1.0,
						   false
						   );
   TGraphAsymmErrors* effY = createEfficiencyGraph(histY_num, histY_den,
						   Form("EfficiencyVsY_Channel%d",channelNumber),
						   ybins,
						   25.0, 29.0,
						   0.0, 1.0,
						   false
						   );
   

   TFile *file = new TFile("eff.root","UPDATE");
   file->cd();
   file->WriteTObject(effX, "Efficiency_X", "WriteDelete");
   file->WriteTObject(effY, "Efficiency_Y", "WriteDelete");
   file->WriteTObject(histX_num, "histX_num", "WriteDelete");
   file->WriteTObject(histX_den, "histX_den", "WriteDelete");
   file->WriteTObject(histY_num, "histY_num", "WriteDelete");
   file->WriteTObject(histY_den, "histY_den", "WriteDelete");
   file->Close();
   delete file; 
   
};


void pulse::MakeTimingPlotsVsXY(int channelNumber) {

  //declare histograms
  TH2F *histDeltaTVsX = new TH2F("histDeltaTVsX",";X [mm];Number of Events", 13, 11,24, 800, -10,10);
  TH2F *histDeltaTVsY = new TH2F("histDeltaTVsY",";Y [mm];Number of Events", 14, 16,30, 800, -10,10);
  

  if (fChain == 0) return;
   Long64_t nentries = fChain->GetEntriesFast();
   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      if (jentry % 100 == 0) cout << "Processing Event " << jentry << " of " << nentries << "\n";

      nb = fChain->GetEntry(jentry);   nbytes += nb;

      //cuts
   
      //require photek to show MIP signal
      if (!(amp[0] > 0.1 && amp[0] < 0.3)) continue;

      //reject events with more than 1 track
      // if ( !(ntracks == 1 && chi2 < 10 )) continue;
      // if ( !(fabs(xSlope) < 1e-3 && fabs(ySlope) < 1e-3)) continue;
      if ( !(amp[channelNumber] < 0.3 )) continue;
      if ( !(amp[channelNumber] > 0.05)) continue;

      histDeltaTVsX->Fill( x1*0.001, gauspeak[0] - linearTime45[channelNumber] );
      histDeltaTVsY->Fill( y1*0.001, gauspeak[0] - linearTime45[channelNumber] );        
	      
   }

   int nXBins = histDeltaTVsX->GetXaxis()->GetNbins();
   float meanDeltaTVsX[nXBins];
   float meanDeltaTErrVsX[nXBins];
   float timeResolutionVsX[nXBins];
   float timeResolutionErrVsX[nXBins];
   float xBins[nXBins];
   float xBinsErr[nXBins];
   for (int b=1; b<=nXBins; b++) {
     TH1D *tmphist = histDeltaTVsX->ProjectionY("tmpProjY",b,b+1);

     TF1* fgaus = new TF1("fgaus","gaus", tmphist->GetMean() - 2*tmphist->GetRMS(), tmphist->GetMean() + 2*tmphist->GetRMS());
     tmphist->Fit("fgaus","Q","", tmphist->GetMean() - 2*tmphist->GetRMS(), tmphist->GetMean() + 2*tmphist->GetRMS());     
     xBins[b-1] = histDeltaTVsX->GetXaxis()->GetBinCenter(b);
     xBinsErr[b-1] = histDeltaTVsX->GetXaxis()->GetBinUpEdge(b) - histDeltaTVsX->GetXaxis()->GetBinCenter(b);
     meanDeltaTVsX[b-1] = fgaus->GetParameter(1);
     meanDeltaTErrVsX[b-1] = fgaus->GetParError(1);
     timeResolutionVsX[b-1] = fgaus->GetParameter(2);
     timeResolutionErrVsX[b-1] = fgaus->GetParError(2); 
     cout << b-1 << " : " << xBins[b-1] << " " << meanDeltaTVsX[b-1] << "\n";
   }


   TGraphErrors* GraphDeltaTVsX = new TGraphErrors(nXBins, xBins, meanDeltaTVsX, xBinsErr, meanDeltaTErrVsX);
   TGraphErrors* GraphTimeResolutionVsX = new TGraphErrors(nXBins, xBins, timeResolutionVsX, xBinsErr, timeResolutionErrVsX);

   TFile *file = new TFile("timing.root","UPDATE");
   file->cd();
   file->WriteTObject(GraphDeltaTVsX, "GraphDeltaTVsX", "WriteDelete");  
   file->WriteTObject(GraphTimeResolutionVsX, "GraphTimeResolutionVsX", "WriteDelete");  
   file->WriteTObject(histDeltaTVsX, "histDeltaTVsX", "WriteDelete");  
   file->WriteTObject(histDeltaTVsY, "histDeltaTVsY", "WriteDelete");  
   file->Close();
   delete file; 
   
};


void pulse::MakeEfficiencyVsRun(int channelNumber) {

  //declare histograms
  TH2F *hist_den = new TH2F("hist_den",";X [mm];Number of Events", 10, 15, 17, 100, 0, 26000);
  TH2F *hist_num = new TH2F("hist_num",";X [mm];Number of Events", 10, 15, 17, 100, 0, 26000);
  
  if (fChain == 0) return;
   Long64_t nentries = fChain->GetEntriesFast();
   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      if (jentry % 100 == 0) cout << "Processing Event " << jentry << " of " << nentries << "\n";

      nb = fChain->GetEntry(jentry);   nbytes += nb;

      //cuts

      //require photek to show MIP signal
      if (!(amp[0] > 0.1 && amp[0] < 0.3)) continue;
      if (!(x1 > 15000 && x1 < 18000 && y1 > 21000 && y1 < 24000)) continue;

      hist_den->Fill( 0.001*x1, event );

      //cout << "Fill Den: " << 0.001*x2 << " " << 0.001*y2 << "\n";
      
      //numerator
      if (amp[channelNumber] > 0.01) {
	//cout << "Fill NUM: " << 0.001*x2 << " " << 0.001*y2 << "\n";
	hist_num->Fill( 0.001*x1, event );
      }
   }

   vector<double> xbins;
   vector<double> ybins;

   TH2F* eff = createEfficiencyHist2D(hist_num, hist_den,
				      Form("EfficiencyVsX_Channel%d",channelNumber),
				      xbins,ybins
				      );
   
   
   TFile *file = new TFile("eff_vs_run.root","UPDATE");
   file->cd();
   file->WriteTObject(eff, "Efficiency_X", "WriteDelete");
   file->WriteTObject(hist_num, "hist_num", "WriteDelete");
   file->WriteTObject(hist_den, "hist_den", "WriteDelete");
   file->Close();
   delete file; 
   
};


void pulse::CreateMPV_vs_PositionHisto( )
{
  const int npoints = 30;
  float x_pos[npoints];//x-positions
  float x_pos_un[npoints];//x-position uncertainty
  float mpv_x[npoints];//mpv amplitude for x
  float mpv_x_un[npoints];//uncertainty on mpv amplitude x
  float y_pos[npoints];//y-positions
  float y_pos_un[npoints];//y-position uncertainty
  float mpv_y[npoints];//mpv amplitude for x
  float mpv_y_un[npoints];//uncertainty on mpv amplitude y

  //------------------
  //Define initial positions and step size, all units are in microns
  //-------------------
  float x_init = 11000.;//microns
  float y_init = 13000.;//microns
  float step = 500.;//microns
  for ( int i = 0; i < npoints; i++ )
    {
      x_pos[i] = x_init + step*(float)i;
      x_pos_un[i] = 0;
      std::pair<float,float> MPVAndError_X = MPV_vs_Position( "X", 1, x_pos[i], step, 0.08, 0.2 );
      mpv_x[i] = MPVAndError_X.first;
      mpv_x_un[i] = MPVAndError_X.second;
      y_pos[i] = y_init + step*(float)i;
      y_pos_un[i] = 0;
      std::pair<float,float> MPVAndError_Y = MPV_vs_Position( "Y", 1, y_pos[i], step, 0.08, 0.2 );
      mpv_y[i] = MPVAndError_Y.first;
      mpv_y_un[i] = MPVAndError_Y.second;
    }

  TGraphErrors* gr_mpv_x = new TGraphErrors(npoints, x_pos, mpv_x, x_pos_un, mpv_x_un);
  TGraphErrors* gr_mpv_y = new TGraphErrors(npoints, y_pos, mpv_y, y_pos_un, mpv_y_un);

  TFile* fout = new TFile("mpv_tgraphs.root", "RECREATE");
  gr_mpv_x->Write("mpv_x");
  gr_mpv_y->Write("mpv_y");
  fout->Close();
};

std::pair<float,float> pulse::MPV_vs_Position( TString coor, const int channel, const float coorLow, const float step,
			      const float AmpLowCut, const float AmpHighCut)
{
  if ( channel < 0 ) return std::pair<float,float>(-999,0);
  
  fChain->SetBranchStatus("*", 0);
  fChain->SetBranchStatus("amp", 1);
  fChain->SetBranchStatus("x2", 1);
  fChain->SetBranchStatus("y2", 1);
  if (fChain == 0) return std::pair<float,float>(-999,0);
  Long64_t nentries = fChain->GetEntriesFast();
  Long64_t nbytes = 0, nb = 0;
  
  cout << "Running MPV_vs_Position Analysis\n";
  cout << "Total Events: " << nentries << "\n";
  TH1F* h_mpv = new TH1F("h_mpv", "h_mpv", 100, 0, 0.5);
  for (Long64_t jentry=0; jentry<nentries;jentry++)
    {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      if (ientry % 10000 == 0) cout << "Processing Event " << ientry << "\n";
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      
      if ( amp[channel] >= AmpLowCut && amp[channel] <= AmpHighCut )
	{
 	  if ( (coor == "x" || coor == "X") && x2 >= coorLow && x2 < coorLow + step ) h_mpv->Fill(amp[channel]);
	  if ( (coor == "y" || coor == "Y") && y2 >= coorLow && y2 < coorLow + step ) h_mpv->Fill(amp[channel]);
	}
    }
  
  TF1* landau = new TF1( "landau", "landau", AmpLowCut, AmpHighCut );
  h_mpv->Fit("landau","Q","", AmpLowCut, AmpHighCut);
  std::pair<float,float> result;
  result.first = landau->GetParameter(1);
  result.second = landau->GetParError(1);
  TFile* fout = new TFile("mpv_test.root", "recreate");
  h_mpv->Write();
  fout->Close();
  return result;
};
