//C++ INCLUDES
#include <vector>
#include <iostream>
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
//LOCAL INCLUDES
#include "TimeReso.hh"
#include "digiSettings.hh"


int main( int argc, char* argv[])
{
	std::string inputfile = argv[1];

	if(inputfile == "")
	{
	  std::cerr << "[ERROR]: please provide an input file " << std::endl;
          return -1;
	}
	std::cout<<"using ntuple: "<<inputfile<<std::endl;

	//TFile* f = new TFile(inputfile.c_str() , "READ" );
	//TTree* tree = (TTree*)f->Get("t1065");

	TString cut = "amp[16]>0.01 && amp[17] >0.01";

	std::cout<<getTimeReso(inputfile, cut, 16, 17)<<std::endl;

	return 0;
}
