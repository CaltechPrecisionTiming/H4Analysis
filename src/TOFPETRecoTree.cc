#include "interface/TOFPETRecoTree.h"

TOFPETRecoTree::TOFPETRecoTree(uint64* idx, TTree* tree)
{
    tree_ = tree ? tree : new TTree();
    
    index=idx;
}

void TOFPETRecoTree::Init()
{
    //---global branches
    tree_->Branch("index", &index, "index/l");
    //---tofpet branches
    tree_->Branch("isMatched", &isMatched, "isMatched/O");
    tree_->Branch("t_sipm", t_sipm, "t_sipm[64]/D");
    tree_->Branch("tot", tot, "tot[64]/D");
    tree_->Branch("energy", energy, "energy[64]/D");        
    tree_->Branch("tqT", tqT, "tqT[64]/D");        
    tree_->Branch("tqE", tqE, "tqE[64]/D");        
    tree_->Branch("x", x, "x[64]/D");        
    tree_->Branch("y", y, "y[64]/D");        
    tree_->Branch("z", z, "z[64]/D");        
    tree_->Branch("xi", xi, "xi[64]/I");        
    tree_->Branch("yi", yi, "yi[64]/I");        
    tree_->Branch("t_h4daq", &t_h4daq, "t_h4daq/D");
    tree_->Branch("t_tofpet", t_tofpet, "t_tofpet[64]/D");
}
