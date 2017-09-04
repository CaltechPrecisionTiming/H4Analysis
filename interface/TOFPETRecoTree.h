#ifndef __TOFPET_RECO_TREE__
#define __TOFPET_RECO_TREE__

#include "TFile.h"
#include "TTree.h"

typedef unsigned long long int uint64;

using namespace std;

class TOFPETRecoTree
{
public: 
    //---ctors---
    TOFPETRecoTree(){};
    TOFPETRecoTree(uint64* idx, TTree* tree=NULL);
    //---dtor---
    ~TOFPETRecoTree(){};
    
    //---utils---
    void Init();
    void Fill() {tree_->Fill();};

    TTree*  tree_; 

    uint64* index;
    bool   isMatched; 
    long long int t_sipm[64];
    double tot[64];
    double energy[64];
    double tqT[64];
    double tqE[64];
    double x[64];
    double y[64];
    double z[64];
    int xi[64];
    int yi[64];
    int spillNum;
    double t_h4daq;
    long long int t_totrigger[64];
    long long int t_tofpet[64];
    double deltaT_h4daq_trigger;
};

#endif

    
