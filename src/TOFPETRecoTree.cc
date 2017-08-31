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
    tree_->Branch("t_sipm", t_sipm, "t_sipm[100]/D");
    tree_->Branch("tot", tot, "tot[100]/D");
    tree_->Branch("energy", energy, "energy[100]/D");        
    tree_->Branch("tqT", tqT, "tqT[100]/D");        
    tree_->Branch("tqE", tqE, "tqE[100]/D");        
    tree_->Branch("x", x, "x[100]/D");        
    tree_->Branch("y", y, "y[100]/D");        
    tree_->Branch("z", z, "z[100]/D");        
    tree_->Branch("xi", xi, "xi[100]/I");        
    tree_->Branch("yi", yi, "yi[100]/I");        
    tree_->Branch("t_h4daq", t_h4daq, "t_h4daq[100]/l");
    tree_->Branch("t_tofpet", t_tofpet, "t_tofpet[100]/l");
}
