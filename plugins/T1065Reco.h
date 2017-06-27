#ifndef __T1065Reco__
#define __T1065Reco__

#include <iostream>

#include "interface/PluginBase.h"
#include "interface/DigiTree.h"
#include "interface/T1065Tree.h"
#include "interface/WFTree.h"
#include "interface/WFClass.h"
#include "interface/WFClassNINO.h"
#include "interface/WFViewer.h"

#include "TGraphErrors.h"
#include "TString.h"
#include "TCanvas.h"
#include "TString.h"
#include "TLine.h"

class T1065Reco: public PluginBase
{
public:
    //---ctors---
    T1065Reco(){};

    //---dtor---
    ~T1065Reco(){};

    //---utils---
    bool Begin(CfgManager& opts, uint64* index);
    bool ProcessEvent(const H4Tree& event, map<string, PluginBase*>& plugins, CfgManager& opts);

    double GetAmplificationFactor ( double measuredAmplitude );
    TGraphErrors* GetTGraph(  float* channel, float* time );
    TGraphErrors GetTGraph(  short* channel, float* time );
    int FindMin( int n, short *a);
    int DigitalFindMin( int n, short *a);
    int FindMinAbsolute( int n, short *a);
    int FindRealMin( int n, short *a);
    int FindMinFirstPeakAboveNoise( int n, short *a);
    float GausFit_MeanTime(TGraphErrors* pulse, const float index_first, const float index_last);
    float GausFit_MeanTime(TGraphErrors* pulse, const float index_first, const float index_last, TString fname);
    float RisingEdgeFitTime(TGraphErrors * pulse, const float index_min, const float constantFraction, TString fname, bool makePlot );
    void RisingEdgeFitTime(TGraphErrors * pulse, const float index_min, const float fitLowEdge, const float fitHighEdge, float* tstamp, float &risetime, int event, TString fname, bool makePlot );
    void DigitalRisingEdgeFitTime(TGraphErrors * pulse, const float index_min, float* tstamp, float &risetime, int event, TString fname, bool makePlot );
    double GetGaussTime( TGraphErrors* pulse );
    float GetBaseline(TGraphErrors * pulse, int i_low, int i_high);
    float DigitalGetBaseline(TGraphErrors * pulse, int i_low, int i_high);
    float GetBaseline( int peak, short *a , int nbinsExcludedLeftOfPeak , int nbinsExcludedRightOfPeak );
    float GetPulseIntegral(int peak, short *a, std::string option);
    TGraphErrors* GetTGraphFilter( short* channel, float* time, TString pulseName, bool makePlot );

    float SigmoidTimeFit(TGraphErrors * pulse, const float index_min, int event, TString fname, bool makePlot = false );
    float FullFitScint( TGraphErrors * pulse, const float index_min, int event, TString fname, bool makePlot = false );
    float ConstantThresholdTime(TGraphErrors * pulse, const float threshold);

 
private:    
    //---internal data
    string                      srcInstance_;
    vector<string>              channelsNames_;
    vector<string>              timeRecoTypes_;
    map<string, vector<float> > timeOpts_;
    DigiTree                    digiTree_;
    T1065Tree                   t1065Tree_;
    WFTree                      outWFTree_;
    map<string, WFClass*>       WFs_;
    map<string, TH1*>           templates_;
    int                         eventCount_;
};

DEFINE_PLUGIN(T1065Reco);

#endif
