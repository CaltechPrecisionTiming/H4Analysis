#include "TOFPETReco.h"


#define MAX_TOFPET_CHANNEL 64

#define TriggerChannelID 0

long long int matchTriggerWindow = 1e6; //pico-seconds

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
    while(rawTree_->channelID != TriggerChannelID) rawTree_->NextEntry();
    
    //---create a reco tree
    RegisterSharedData(new TTree("tofp", "tofpet_tree"), "tofpet_tree", true);
    recoTree_ = TOFPETRecoTree(index, (TTree*)data_.back().obj);
    recoTree_.Init();
    

    return true;
}

//----------ProcessEvent------------------------------------------------------------------
bool TOFPETReco::ProcessEvent(const H4Tree& h4Tree, map<string, PluginBase*>& plugins, CfgManager& opts)
{

    //cout<<" DEBUG TOFPET ProcessEvent  started..."<<endl;

    for(int i=0;i< MAX_TOFPET_CHANNEL; i++)
    {
	    recoTree_.t_sipm[i] = -9999;
	    recoTree_.tot[i] = -9999;
	    recoTree_.energy[i] = -9999;
	    recoTree_.tqT[i] = -9999;
	    recoTree_.tqE[i] = -9999;
	    recoTree_.x[i] = -9999;
	    recoTree_.y[i] = -9999;
	    recoTree_.z[i] = -9999;
	    recoTree_.xi[i] = -9999;
	    recoTree_.yi[i] = -9999;
	    recoTree_.t_tofpet[i] = -9999;
	    recoTree_.t_totrigger[i] = -9999;
    }
    recoTree_.isMatched = false;
    recoTree_.t_h4daq = -9999;
    recoTree_.spillNum = h4Tree.spillNumber;
    recoTree_.deltaT_h4daq_trigger = -9999;


    bool matched=false;
    bool foundThisTrigger=false;
    double h4daq_time = -1;
    spillAdjust_ = 0;  
    double time_diff_triggerh4 = rawTree_->time - tofpetRefTime_ - h4daq_time;
    double time_diff_triggerh4_best = time_diff_triggerh4;
    long int tofpet_triggerEntry = rawTree_->getCurrentEntry();
    long long int tofpet_triggertime = rawTree_->time;


    // if the starting event is not a trigger event, shift it to the next trigger event
    while(rawTree_->channelID != TriggerChannelID) rawTree_->NextEntry();
    // if the tofpetRefTime has never been set, then set it to the first trigger
    if(tofpetRefTime_ == -1) tofpetRefTime_ = rawTree_->time; 

    // get the h4daq time
    for(int iT=0; iT<h4Tree.nEvtTimes; ++iT)
    { 
        if(h4daqRefTime_ == -1 && h4Tree.evtTimeBoard[iT] == 16908289)
            h4daqRefTime_ = h4Tree.evtTime[iT]; 
        if(h4Tree.evtTimeBoard[iT] == 16908289)
            h4daq_time = h4Tree.evtTime[iT]-h4daqRefTime_; // time shifted to the first triger time
    }
    
   time_diff_triggerh4 = rawTree_->time - tofpetRefTime_ - h4daq_time;
   time_diff_triggerh4_best = time_diff_triggerh4;
   tofpet_triggerEntry = rawTree_->getCurrentEntry();


    // for every spill, reset h4daq_time, and tofpet time to zero  
    if(currentSpill_ != h4Tree.spillNumber)
    {

	    for(int iT=0; iT<h4Tree.nEvtTimes; ++iT)
    		{ 
        	if(h4Tree.evtTimeBoard[iT] == 16908289)
            		h4daqRefTime_ = h4Tree.evtTime[iT]; 
        	if(h4Tree.evtTimeBoard[iT] == 16908289)
            		h4daq_time = h4Tree.evtTime[iT]-h4daqRefTime_; // time shifted to the first triger time
    		}
 
	foundFirstTrigger_ = false;
	h4daqRefTimes_.clear();
	h4daqRefTimes_.push_back(h4daqRefTime_);
        currentSpill_ = h4Tree.spillNumber;

	// for each spill, need to find the first trigger appeared in the TOFPET data
	cout<<"processing spill "<<currentSpill_<<" h4daqtime: "<<h4daqRefTime_<<" first tofpet entry given : "<<rawTree_->getCurrentEntry();
	//first, find the first trigger in this spill: the one which is far away from the previous trigger - more than 1 second away
	//look around +- 10 triggers
	long int previousTriggerIndex = rawTree_->getCurrentEntry();
	double previousTriggerTime = rawTree_->time/1e6;	
	long int theFirstTriggerIndex = rawTree_->getCurrentEntry();
	bool findFirstTrigger = false;
	
	int Nattemp = -10;
	while(Nattemp < 0 && rawTree_->getCurrentEntry() > 0 && (!findFirstTrigger))
	{
		rawTree_->NextEntry(rawTree_->getCurrentEntry() - 1);	
		while(rawTree_->channelID != TriggerChannelID && rawTree_->getCurrentEntry() > 0) rawTree_->NextEntry(rawTree_->getCurrentEntry() - 1);	
		if(rawTree_->channelID == TriggerChannelID)
		{
			Nattemp++;
			if(abs(rawTree_->time/1e6 - previousTriggerTime) > 1e6) //1 second away
			{
				findFirstTrigger = true;
				theFirstTriggerIndex = previousTriggerIndex;
			}
			else
			{
				previousTriggerIndex = rawTree_->getCurrentEntry();
				previousTriggerTime = rawTree_->time/1e6;
			}
		}
	}
	rawTree_->NextEntry(theFirstTriggerIndex);

	while(Nattemp < 10 && rawTree_->getCurrentEntry() < rawTree_->getNEntries() - 1 && (!findFirstTrigger))
        {
                rawTree_->NextEntry();
                while(rawTree_->channelID != TriggerChannelID && rawTree_->getCurrentEntry() < rawTree_->getNEntries() - 1) rawTree_->NextEntry();
                if(rawTree_->channelID == TriggerChannelID)
                {
                        Nattemp++;
                        if(abs(rawTree_->time/1e6 - previousTriggerTime) > 1e6) //1 second away
                        {
                                findFirstTrigger = true;
                                theFirstTriggerIndex = rawTree_->getCurrentEntry();
                        }
                        else
                        {
                                previousTriggerIndex = rawTree_->getCurrentEntry();
                                previousTriggerTime = rawTree_->time/1e6;
                        }
                }
        }

	tofpet_triggerEntry = theFirstTriggerIndex;	
	rawTree_->NextEntry(tofpet_triggerEntry);
	tofpetRefTime_ = rawTree_->time;
    	cout<<", TOFPET first trigger: entry "<<tofpet_triggerEntry<<"  CH "<<rawTree_->channelID<<" h4daqRefTime_ "<<h4daqRefTime_<<"  tofpetRefTime_ "<<tofpetRefTime_<<endl;
    }
	
    else if(!foundFirstTrigger_)
    {
	//now we arrive at the second event in the spill in H4DAQ, then let's pair the first two events with first two triggers in TOFPET
	//go to the second trigger
	cout<<"trying to find the first trigger in the spill"<<endl;
	long long int this_h4daqtime;	
    	for(int iT=0; iT<h4Tree.nEvtTimes; ++iT)
  	{ 
        	if(h4Tree.evtTimeBoard[iT] == 16908289)
            		this_h4daqtime = h4Tree.evtTime[iT]; // time shifted to the first triger time
    	}
	
	rawTree_->NextEntry();
    	while(rawTree_->channelID != TriggerChannelID) rawTree_->NextEntry();
	
	cout<<"deltaT of the first two triggers: "<<rawTree_->time/1e6 - tofpetRefTime_/1e6<<endl;

	for(int ind_h4=0;ind_h4<h4daqRefTimes_.size();ind_h4++)
	{
		time_diff_triggerh4 = rawTree_->time/1e6 - tofpetRefTime_/1e6 - (this_h4daqtime - h4daqRefTimes_[ind_h4]);
		cout<<"pairing event "<<h4daqRefTimes_.size()<<" with "<<ind_h4<<" deltaT-h4 = "<<this_h4daqtime<<" - "<<h4daqRefTimes_[ind_h4]<<" = "<<this_h4daqtime - h4daqRefTimes_[ind_h4]<<"  deltaT-h4-trigger "<<time_diff_triggerh4<<endl;
		if(abs(time_diff_triggerh4)<2000) foundFirstTrigger_ = true;
	}

	if(foundFirstTrigger_) //matched!
	{
		cout<<"trigger paired! first entry in H4DAQ now set: "<<h4daqRefTimes_.size()<<endl;
		foundFirstTrigger_ = true;
		//can not go back to the first event, so abandon that one event...
		tofpetRefTime_ = rawTree_->time;	
		h4daqRefTime_ = this_h4daqtime;
		h4daq_time = 0;
		cout<<"in this spill, tofpetRefTime_ = "<<tofpetRefTime_<<"  h4daqRefTime_ = "<<h4daqRefTime_<<endl;
		h4daqRefTimes_.clear();
	}	

	else // not matched... this means the first trigger is missing 
	{
		cout<<"trigger pair failed for H4DAQ entry: "<<h4daqRefTimes_.size()<<endl;
		h4daqRefTimes_.push_back(this_h4daqtime);
		//go back to the first trigger and wait for next try
		if(rawTree_->getCurrentEntry() > 0) rawTree_->NextEntry(rawTree_->getCurrentEntry() - 1);	
		while(rawTree_->channelID != TriggerChannelID && rawTree_->getCurrentEntry() > 0) rawTree_->NextEntry(rawTree_->getCurrentEntry() - 1);	

	}
    }

    //abandon first event in the spill, since we need two neighboring events to pair with trigger
    if(!foundFirstTrigger_)
    {
	recoTree_.Fill();
	return false;	
    }

    //if you arrives here, it means that the first trigger is found, and TOFPET and H4DAQ ref times are all set properly

   time_diff_triggerh4 = rawTree_->time/1e6 - tofpetRefTime_/1e6 - h4daq_time;
   time_diff_triggerh4_best = time_diff_triggerh4;
   tofpet_triggerEntry = rawTree_->getCurrentEntry();

    while(rawTree_->channelID != TriggerChannelID) rawTree_->NextEntry();
    
    //cout<<"DEBUG TOFPET ProcessEvent - starting entry "<<rawTree_->getCurrentEntry()<<" ID: "<<rawTree_->channelID<<"  time_diff_triggerh4 "<<time_diff_triggerh4<<endl;

   //first, find the trigger which is closest to H4DAQ time (within 100000 micro-second window - about 10 triggers)
	while(rawTree_->time/1e6 - tofpetRefTime_/1e6 - h4daq_time > -100000 && rawTree_->getCurrentEntry() > 0) // keep searching to the left, until it reaches 1000 us window
	{
		rawTree_->NextEntry(rawTree_->getCurrentEntry() - 1);	
    		while(rawTree_->channelID != TriggerChannelID && rawTree_->getCurrentEntry() > 0) rawTree_->NextEntry(rawTree_->getCurrentEntry() - 1);
		if(abs(rawTree_->time/1e6 - tofpetRefTime_/1e6 - h4daq_time) < abs(time_diff_triggerh4_best) && rawTree_->channelID == TriggerChannelID)
		{
			time_diff_triggerh4_best = rawTree_->time/1e6 - tofpetRefTime_/1e6 - h4daq_time;
			tofpet_triggerEntry = rawTree_->getCurrentEntry();
		}
	}


	while(rawTree_->time/1e6 - tofpetRefTime_/1e6 - h4daq_time < 100000 && rawTree_->getCurrentEntry() < rawTree_->getNEntries() - 1)
	{
		rawTree_->NextEntry();	
    		while(rawTree_->channelID != TriggerChannelID && rawTree_->getCurrentEntry() < rawTree_->getNEntries() - 1) rawTree_->NextEntry();
		if(abs(rawTree_->time/1e6 - tofpetRefTime_/1e6 - h4daq_time) < abs(time_diff_triggerh4_best) && rawTree_->channelID == TriggerChannelID)
		{
			time_diff_triggerh4_best = rawTree_->time/1e6 - tofpetRefTime_/1e6 - h4daq_time;
			tofpet_triggerEntry = rawTree_->getCurrentEntry();
		}

	} 	

    rawTree_->NextEntry(tofpet_triggerEntry);
    time_diff_triggerh4 = rawTree_->time/1e6 - tofpetRefTime_/1e6 - h4daq_time;

    //if the closest trigger selected is far away from H4DAQ event, this means this trigger is missing

    //cout<<"DEBUG TOFPET ProcessEvent - closet trigger entry "<<rawTree_->getCurrentEntry()<<" ID: "<<rawTree_->channelID<<"  time_diff_triggerh4 "<<time_diff_triggerh4<<endl;

    if(abs(time_diff_triggerh4)>1000)
    {
	//cout<<"this event doesn't have tofpet trigger: "<<tofpet_triggerEntry<<endl;
	recoTree_.Fill();
	return false;	
    }

    // OK, now the trigger is selected...
    tofpet_triggertime = rawTree_->time;


    //then, around this trigger, claim that all the events within a given window is matched
    while(rawTree_->time - tofpet_triggertime > -1*matchTriggerWindow) rawTree_->NextEntry(rawTree_->getCurrentEntry() - 1);
    rawTree_->NextEntry();
 
    while(rawTree_->time - tofpet_triggertime < matchTriggerWindow)
    {
  	if(rawTree_->channelID != TriggerChannelID) matched = true;	
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
        	recoTree_.t_tofpet[rawTree_->channelID] = rawTree_->time-tofpetRefTime_;
        	recoTree_.t_totrigger[rawTree_->channelID] = rawTree_->time - tofpet_triggertime;
		//if(rawTree_->channelID!=20) cout<<"matched channelID "<<rawTree_->channelID<<" rawTree_->time "<<rawTree_->time<<"  tofpetRefTime_ "<<tofpetRefTime_<<"  tofpet_triggertime "<<tofpet_triggertime<<" t_tofpet "<<rawTree_->time-tofpetRefTime_<<" = "<<recoTree_.t_tofpet[rawTree_->channelID]<<" t_totrigger "<<rawTree_->time - tofpet_triggertime<<endl;
        }
	rawTree_->NextEntry();
    } 
//    rawTree_->NextEntry(rawTree_->getCurrentEntry() - 1);

    recoTree_.isMatched = matched;
    recoTree_.t_h4daq = h4daq_time;
    recoTree_.deltaT_h4daq_trigger = time_diff_triggerh4;
    
    if(matched) 
    {
       	//cout<<"Matched! "<<endl;
    }
    else
    {
	//cout<<"Not Matched..."<<endl;
    }

    //cout<<"\n"<<endl;
    while(rawTree_->channelID != TriggerChannelID) rawTree_->NextEntry();
	
    recoTree_.Fill();

    return true;
}
