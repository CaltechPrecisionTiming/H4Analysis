
void tofpet_trigger(const std::string& inFileName, unsigned short int trigger1 = 9999, unsigned short int trigger2 = 9999)
{

	TH1F *h_next = new TH1F("time_neighbor","time_neighbor",20000,-1e4,1e4);

	gStyle -> SetOptStat(0);

    	TFile* inFile = TFile::Open(inFileName.c_str(),"READ");
    	TTree* t = (TTree*)( inFile->Get("data") );

	long long int time;
	unsigned short int channelID;
	
	t->SetBranchAddress("time",&time);
	t->SetBranchAddress("channelID",&channelID);
	
	iint NEntries = t->GetEntries();

	for(i=0;i<NEntries;i++)
	{
		t->GetEntry(i);
		if(channelID==20) continue;
		if(i%10000==0) cout<<"event "<<i<<endl;
		if(channelID == trigger1 || channelID == trigger2)
		{
			long long int thisTime = time/1e3;
			//cout<<"find trigger in event "<<i;
			if(i>1)
			{
			//	cout<<"  time: "<<time/1e3;
				t->GetEntry(i-1);
				if(channelID == trigger1 || channelID == trigger2) t->GetEntry(i-2); 
				h_next->Fill(time/1e3 - thisTime);
			//	cout<<"  Dtime_before: "<<time/1e3 - thisTime;
			}
			if(i<NEntries-2)
			{
				t->GetEntry(i+1);
				if(channelID == trigger1 || channelID == trigger2) t->GetEntry(i+2); 
				h_next->Fill(time/1e3 - thisTime);
			//	cout<<"  Dtime_after: "<<time/1e3 - thisTime<<endl;
			}
			if(trigger2<999) i++;
		}
	}
	
	h_next->SetTitle("");
	h_next->GetXaxis()->SetTitle("#Delta T between trigger and neighbor events [ns]");
	h_next->Draw();
	
}
