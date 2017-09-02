#include "TOFPETReco.h"


#define MAX_TOFPET_CHANNEL 64

//**********Utils*************************************************************************
//----------Begin-------------------------------------------------------------------------
bool TOFPETReco::Begin(CfgManager& opts, uint64* index)
{
    //---inputs---
 
    //auto fileName = opts.GetOpt<string>(instanceName_+".inputFile");
    auto fileName = opts.GetOpt<string>(instanceName_+".path2data") + opts.GetOpt<string>(instanceName_+".run") + ".root";
    cout<<"opening TOFPET data: "<<fileName<<endl;
    inputFile_ = TFile::Open(fileName.c_str());
    inputTree_ = (TTree*)inputFile_->Get("data");
    rawTree_ = new TOFPETRawTree(inputTree_);
    rawTree_->NextEntry();
    while(rawTree_->channelID != 0)
        rawTree_->NextEntry();
    
    //---create a reco tree
    RegisterSharedData(new TTree("tofp", "tofpet_tree"), "tofpet_tree", true);
    recoTree_ = TOFPETRecoTree(index, (TTree*)data_.back().obj);
    recoTree_.Init();
    

    return true;
}

//----------ProcessEvent------------------------------------------------------------------
bool TOFPETReco::ProcessEvent(const H4Tree& h4Tree, map<string, PluginBase*>& plugins, CfgManager& opts)
{
    for(int i=0;i< MAX_TOFPET_CHANNEL; i++)
    {
	    recoTree_.t_sipm[i] = 9999;
	    recoTree_.tot[i] = 9999;
	    recoTree_.energy[i] = -1;
	    recoTree_.tqT[i] = -999;
	    recoTree_.tqE[i] = -999;
	    recoTree_.x[i] = -999;
	    recoTree_.y[i] = -999;
	    recoTree_.z[i] = -999;
	    recoTree_.xi[i] = -999;
	    recoTree_.yi[i] = -999;
	    recoTree_.t_tofpet[i] = 0;
    }
    recoTree_.isMatched = false;
    recoTree_.t_h4daq = 9999;

    double h4daq_time = -1;
    for(int iT=0; iT<h4Tree.nEvtTimes; ++iT)
    { 
        if(h4daqRefTime_ == -1 && h4Tree.evtTimeBoard[iT] == 16908289)
            h4daqRefTime_ = h4Tree.evtTime[iT]; 
        if(h4Tree.evtTimeBoard[iT] == 16908289)
            h4daq_time = h4Tree.evtTime[iT]-h4daqRefTime_; // time shifted to the first triger time
    }


    if(tofpetRefTime_ == -1)
        tofpetRefTime_ = rawTree_->time/1e6; // this will be the time of the first trigger sent to tofpet
    double tofpet_time = rawTree_->time/1e6-tofpetRefTime_;  // time shifted to the first trigger time  
   
    if(currentSpill_ != h4Tree.spillNumber)
    {
	while(rawTree_->channelID != 0)
		rawTree_->NextEntry();
	tofpetRefTime_ = rawTree_->time/1e6;
	tofpet_time = rawTree_->time/1e6-tofpetRefTime_;
	for(int iT=0; iT<h4Tree.nEvtTimes; ++iT)
    	{
        	if(h4Tree.evtTimeBoard[iT] == 16908289)
            		h4daqRefTime_ = h4Tree.evtTime[iT];
        	if(h4Tree.evtTimeBoard[iT] == 16908289)
            		h4daq_time = h4Tree.evtTime[iT]-h4daqRefTime_; // time shifted to the first triger time
    	}
	
        if(fabs(tofpet_time - h4daq_time -spillAdjust_)<100)
        {
            currentSpill_ = h4Tree.spillNumber;
            spillAdjust_ = tofpet_time - h4daq_time;
	    //cout<<"first event of spill "<<currentSpill_<<"; tofpet_time: "<<tofpet_time<<"  h4daq_time: "<<h4daq_time<<"  spillAdjust: "<<spillAdjust_<<endl;
        }
        else
        {
	    recoTree_.Fill();
            return false;                       
        }
    }
    
    //cout<<"DEBUG TOFPET ProcessEvent 0001 - spillAdjust  "<<spillAdjust_<<endl;

    double tofpet_rawtime = rawTree_->time/1e6;
    bool matched=false;
    double time_diff = tofpet_time - h4daq_time - spillAdjust_;
   
    while(time_diff <= -50)
    {
        rawTree_->NextEntry();
	tofpet_time = rawTree_->time/1e6-tofpetRefTime_;
	tofpet_rawtime = rawTree_->time/1e6;
	time_diff = tofpet_time - h4daq_time - spillAdjust_;
    }
 

    if(time_diff >= 50)
    {
    	recoTree_.Fill();
	//cout<<"NOT matched..."<<endl;
	return false;
    }
    
    //cout<<"Matched! ... tofpet_time: "<<tofpet_time<<"  h4daq_time: "<<h4daq_time<<endl;

    recoTree_.isMatched = true;

    recoTree_.t_h4daq = h4daq_time;

    while(abs(rawTree_->time/1e6 - tofpet_rawtime)<0.1)
    {
	if(rawTree_->channelID < MAX_TOFPET_CHANNEL)
	{
        	recoTree_.t_sipm[rawTree_->channelID] = rawTree_->time;//1-rawTree_->time2;
        	recoTree_.tot[rawTree_->channelID] = rawTree_->tot;
        	recoTree_.energy[rawTree_->channelID] = rawTree_->energy;
        	recoTree_.tqT[rawTree_->channelID] = rawTree_->tqT;
        	recoTree_.tqE[rawTree_->channelID] = rawTree_->tqE;
        	recoTree_.x[rawTree_->channelID] = rawTree_->x;
        	recoTree_.y[rawTree_->channelID] = rawTree_->y;
        	recoTree_.z[rawTree_->channelID] = rawTree_->z;
        	recoTree_.xi[rawTree_->channelID] = rawTree_->xi;
        	recoTree_.yi[rawTree_->channelID] = rawTree_->yi;
        	recoTree_.t_tofpet[rawTree_->channelID] = tofpet_time;
        	matched = true;
        	rawTree_->NextEntry();
        }
	else
	{
        	rawTree_->NextEntry();
	}
    }
 
    //while(rawTree_->channelID != 0)
    //    rawTree_->NextEntry();
    
    	
    recoTree_.Fill();

    return true;
}
