//C++ INCLUDES
#include <vector>
#include <fstream>
#include <string>
//ROOT INCLUDES
//#include <TSYSTEM.h>
#include <TSystem.h>
#include <TTree.h>
#include <TLatex.h>
#include <TString.h>
#include <TFile.h>
#include <TH1D.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TRandom3.h>
#include <TLegend.h>
#include <TMath.h>
#include <TBox.h>
#include <TMath.h>
#include <TROOT.h>
#include <Math/GaussIntegrator.h>
#include <Math/IntegratorOptions.h>
//ROOFIT INCLUDES
#include <RooWorkspace.h>
#include <RooDataSet.h>
#include <RooRealVar.h>
#include <RooExponential.h>
#include <RooAddPdf.h>
#include <RooGaussian.h>
#include <RooMinimizer.h>
#include <RooFitResult.h>
#include <RooPlot.h>
#include <RooExtendPdf.h>
#include <RooStats/SPlot.h>
#include <RooStats/ModelConfig.h>
#include <RooGenericPdf.h>
#include <RooFormulaVar.h>
#include <RooBernstein.h>
#include <RooMinuit.h>
#include <RooNLLVar.h>
#include <RooRandom.h>
#include <RooDataHist.h>
#include <RooHistPdf.h>
//#include <RealVar.h>
//LOCAL INCLUDES
#include "TimeReso.hh"
#include "digiSettings.hh"


double getTimeReso(std::string fname, TString cut, int ch_ref, int ch_get)
{
	double time_reso = 0.0;
	TFile* f = new TFile(fname.c_str() , "READ" );
        TTree* tree = (TTree*)f->Get("t1065");

	TCanvas *cv = new TCanvas("cv","cv",800,600);
	
	TH1F *h_dT = new TH1F("deltaTime","deltaTime",100,-3,3);

//	string s_ref = to_string(ch_ref);
//	string s_get = to_string(ch_get);

	//string s_draw = "gauspeak["+(to_string(ch_ref)).c_str()+"] - gauspeak["+(to_string(ch_get)).c_str()+"]>>deltaTime";
	std::string s_draw = "gauspeak["+std::to_string(ch_ref)+"] - gauspeak["+std::to_string(ch_get)+"]>>deltaTime";

	tree->Draw(s_draw.c_str(),cut);
	
	time_reso = h_dT->GetStdDev();		
	return time_reso;	
};


