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
    double t_sipm[100];
    double tot[100];
    double energy[100];
    uint64 t_h4daq[100];
    uint64 t_tofpet[100];
};

#endif

    
