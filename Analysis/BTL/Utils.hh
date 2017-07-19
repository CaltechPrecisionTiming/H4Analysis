#include <TMath.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TH3F.h>
#include <TFile.h>
#include <string>
#include <vector>
#include <utility>
#include <TGraphAsymmErrors.h>
#include <TEfficiency.h>
#include <iostream>
#include <iomanip>
#include <assert.h>

using namespace std;

vector<TString> SplitString(const TString& theOpt, const char split )
{
   vector<TString> splitV;
   TString splitOpt(theOpt);
   splitOpt.ReplaceAll("\n"," ");
   splitOpt = splitOpt.Strip(TString::kBoth,split);
   while (splitOpt.Length()>0) {
      if ( !splitOpt.Contains(split) ) {
         splitV.push_back(splitOpt);
         break;
      }
      else {
         TString toSave = splitOpt(0,splitOpt.First(split));
         splitV.push_back(toSave);
         splitOpt = splitOpt(splitOpt.First(split),splitOpt.Length());
      }
      splitOpt = splitOpt.Strip(TString::kLeading,split);
   }
   return splitV;
}

