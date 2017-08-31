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
    tree_->Branch("t_h4daq", t_h4daq, "t_h4daq[100]/l");
    tree_->Branch("t_tofpet", t_tofpet, "t_tofpet[100]/l");
}
