#ifndef TimeReso_HH
#define TimeReso_HH
//C++ INCLUDES
#include <sstream>
#include <string>
//ROOT INCLUDES
#include <TTree.h>
#include <TString.h>
//ROOFIT INCLUDES
#include <RooRealVar.h>
#include <RooWorkspace.h>
#include <RooDataSet.h>
#include <RooAbsPdf.h>
//LOCAL INCLUDES
//

double getTimeReso(std::string fname, TString cut, int ch_ref, int ch_get);

#endif
