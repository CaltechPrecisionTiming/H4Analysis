#include "T1065Reco.h"

//----------Utils-------------------------------------------------------------------------


// //*********************************************************
// // Get amplification factor used for the silicon sensor
// //*********************************************************
// double T1065Reco::GetAmplificationFactor ( double measuredAmplitude ) {
  
//   int index_firstBinAboveInput = -1;
//   for (int i=0; i < nPoints; ++i) {
//     index_firstBinAboveInput = i;
//     if (measuredAmplitude < outputAmplitude[i]) break;
//   }
  
//   double answer = 0; 

//   if (measuredAmplitude > outputAmplitude[21]) answer =amplificationFactor[21];
//   else if (index_firstBinAboveInput == 0) answer = amplificationFactor[0];
//   else {
    
//     //cout << "index_firstBinAboveInput = " << index_firstBinAboveInput << " : "
//     //	 << amplificationFactor[index_firstBinAboveInput-1] << " " << outputAmplitude[index_firstBinAboveInput]
//     //	 << "\n";
//     double x = measuredAmplitude - outputAmplitude[index_firstBinAboveInput-1];
//     double y = amplificationFactor[index_firstBinAboveInput-1] + x * (amplificationFactor[index_firstBinAboveInput] - amplificationFactor[index_firstBinAboveInput-1]) / (outputAmplitude[index_firstBinAboveInput] - outputAmplitude[index_firstBinAboveInput-1]);
//     //cout << "x = " << x << " , y = " << y << "\n";
//     answer = y;
//   }

//   //cout << measuredAmplitude << " " << answer << "\n";

//   return answer;
  
// }


TGraphErrors* T1065Reco::GetTGraph(  float* channel, float* time )
{		
  //Setting Errors
  float errorX[1024], errorY[1024], channelFloat[1024];
  float _errorY = 0.00; //5%error on Y
  for ( int i = 0; i < 1024; i++ )
    {
      errorX[i]       = .0;
      errorY[i]       = _errorY*channel[i];
      channelFloat[i] = -channel[i];
    }
  TGraphErrors* tg = new TGraphErrors( 1024, time, channelFloat, errorX, errorY );
  return tg;
};


TGraphErrors T1065Reco::GetTGraph(  short* channel, float* time )
{		
  //Setting Errors
  float errorX[1024], errorY[1024], channelFloat[1024];
  float _errorY = 0.00; //5%error on Y
  for ( int i = 0; i < 1024; i++ )
    {
      errorX[i]       = .0;
      errorY[i]       = _errorY*channel[i];
      channelFloat[i] = -channel[i];
    }
  //TGraphErrors* tg = new TGraphErrors( 1024, time, channelFloat, errorX, errorY );
  TGraphErrors tg( 1024, time, channelFloat, errorX, errorY );
  return tg;
};

////////////////////////////////////////////
// find minimum of the pulse
////////////////////////////////////////////
int T1065Reco::FindMin( int n, short *a) {
  return FindRealMin(n,a);
}

int T1065Reco::FindMinAbsolute( int n, short *a) {
  
  if (n <= 0 || !a) return -1;
  float xmin = a[5];
  int loc = 0;
  for  (int i = 5; i < n-10; i++) {
    if (xmin > a[i] && a[i+1] < 0.5*a[i] && a[i] < -40. )  
      {
	//std::cout << i << " " << a[i] << std::endl;
	xmin = a[i];
	loc = i;
	//if ( a[i+5]>a[i] && a[i+10]>a[i+5] ) {
	//break;
      }
  }
  //std::cout << "loc0: " << loc << std::endl;
  return loc;
}

int T1065Reco::FindRealMin( int n, short *a) {  
  if (n <= 0 || !a) return -1;
  float xmin = a[5];
  int loc = 0;
  
  float noise = 0;
  
  for ( int i = 5; i < 100; i++)
    {
      if( abs(a[i]) > noise ) 
	{
	  noise = abs(a[i]);
	}
    }

  for  (int i = 5; i < n-10; i++) {
    if (xmin > a[i] && a[i+1] < 0.5*a[i] && a[i] < -3*noise && a[i] < -50.)  
      {
	//std::cout << a[i] << std::endl;
	xmin = a[i];
	loc = i;
	//if ( a[i+5]>a[i] && a[i+10]>a[i+5] ) {
	//break;
      }
  }
 
  float xmin_init = xmin;
  float xmin_new = a[5];
  int loc_new = loc;

  bool stop = false;
  while( !stop )
    {
      for ( int i = 5; i < loc_new -25; i++ )
        {
          if ( a[i] < xmin_new && 0.5*a[i] > a[i+1] && a[i] < 0.15* xmin_init )
            {
              xmin_new = a[i];
              loc_new = i;
            }
        }

      xmin_init = xmin_new;

      if( loc_new == loc ) break;
      //std::cout << "new peak @ " << loc_new << ", ymin: " << xmin_new << std::endl;
      if ( xmin_new > -2*noise || xmin_new > -40 ) loc_new = 0;
      xmin_new = a[5];
      loc = loc_new;
    }

  //std::cout << "LOC2: " << loc << std::endl;                                                                                                                               
  /*                                                                
  while ( xmin_init != xmin_new ) {
    for (int i = 5; i < loc - 50; i++) {
      if (xmin_new > a[i] && a[i+1] < 0.5*a[i] && a[i] < xmin_init*2/3 )  {
        xmin_new = a[i];
        loc = i;
      }
    }
    xmin_init = xmin_new
    xmin_new = a[5]
  }
  */
  return loc_new;
}

int T1065Reco::FindMinFirstPeakAboveNoise( int n, short *a) {
  
  const float noise = 50;
  if (n <= 0 || !a) return -1;
  int loc = 0;
  
  for  (int i = 10; i < n-10; i++) {   
    if ( abs(a[i]) > noise 
	&& 
	(a[i] < a[i-1] && a[i] < a[i-2] && a[i] < a[i-3] && a[i] < a[i-4] && a[i] < a[i-5] )
	&&
	(a[i] < a[i+1] && a[i] < a[i+2] && a[i] < a[i+3] && a[i] < a[i+4] && a[i] < a[i+5] )
	)  {
      loc = i;
      break;
    }
  }  
  return loc;
}


// find the mean time from gaus fit
float T1065Reco::GausFit_MeanTime(TGraphErrors* pulse, const float index_first, const float index_last)
{
  TF1* fpeak = new TF1("fpeak","gaus", index_first, index_last);
  pulse->Fit("fpeak","Q","", index_first, index_last);
  
  float timepeak = fpeak->GetParameter(1);
  delete fpeak;
  
  return timepeak;
};

float T1065Reco::GausFit_MeanTime(TGraphErrors* pulse, const float index_first, const float index_last, TString fname)
{
  TF1* fpeak = new TF1("fpeak","gaus", index_first, index_last);
  //float max = pulse->GetMaximum();
  double max = -9999;
  double* yy = pulse->GetY();
  for ( int i = 0; i < 1024; i++ )
    {
      if ( yy[i] > max ) max = yy[i];
    }
  //std::cout << "max: " << max << std::endl;
  if( max < 42 || index_first < 10 || index_last > 1010 ) return -99999;
  pulse->Fit("fpeak","Q","", index_first, index_last);
  
  TCanvas* c = new TCanvas("canvas","canvas",800,400) ;
  float timepeak = fpeak->GetParameter(1);
  pulse->GetXaxis()->SetLimits( timepeak-10, timepeak+10);
  pulse->SetMarkerSize(0.5);
  pulse->SetMarkerStyle(20);
  pulse->Draw("AP");
  c->SaveAs(fname+".pdf");
  delete fpeak;
  
  return timepeak;
};

float T1065Reco::RisingEdgeFitTime(TGraphErrors * pulse, const float index_min, const float constantFraction, TString fname, bool makePlot )
{
  double x_low, x_high, y, dummy;
  //pulse->GetPoint(index_min-7, x_low, y);
  //pulse->GetPoint(index_min-2, x_high, y);
  pulse->GetPoint(index_min-12, x_low, y);
  pulse->GetPoint(index_min-7, x_high, y);
  pulse->GetPoint(index_min, dummy, y);  
  TF1* flinear = new TF1("flinear","[0]*x+[1]", x_low, x_high );
  
  pulse->Fit("flinear","Q","", x_low, x_high );
  double slope = flinear->GetParameter(0);
  double b     = flinear->GetParameter(1);
  
  if ( makePlot )
    {
      std::cout << "make plot" << std::endl;
      TCanvas* c = new TCanvas("canvas","canvas",800,400) ;
      pulse->GetXaxis()->SetLimits(x_low-3, x_high+3);
      pulse->SetMarkerSize(0.3);
      pulse->SetMarkerStyle(20);
      pulse->Draw("AP");
      c->SaveAs(fname+"LinearFit.pdf");
      //delete c;
    }
  delete flinear;
  
  return (constantFraction*y-b)/slope;
};


void T1065Reco::RisingEdgeFitTime(TGraphErrors * pulse, const float index_min, float* tstamp, int event, TString fname, bool makePlot )
{
  double x_low, x_high, y, dummy;
  double ymax;
  pulse->GetPoint(index_min, x_low, ymax);
  for ( int i = 1; i < 100; i++ )
    {
      pulse->GetPoint(index_min-i, x_low, y);
      if ( y < 0.2*ymax ) break;
    }
  for ( int i = 1; i < 100; i++ )
    {
      pulse->GetPoint(index_min-i, x_high, y);
      if ( y < 0.9*ymax ) break;
    }
  //pulse->GetPoint(index_min-8, x_low, y);
  //pulse->GetPoint(index_min-3, x_high, y);


  //pulse->GetPoint(index_min-12, x_low, y);
  //pulse->GetPoint(index_min-7, x_high, y);
  pulse->GetPoint(index_min, dummy, y);
  
  TF1* flinear = new TF1("flinear","[0]*x+[1]", x_low, x_high );
  float max = -9999;
  double* yy = pulse->GetY();
  
  for ( int i = 0; i < 1024; i++ )
    {
      if ( yy[i] > max ) max = yy[i];
    }
  //std::cout << "max: " << max << std::endl;

  /*if( max < 10 || index_min < 0 || index_min > 1023 )
    {
      std::cout << "DEB: skipping event--> " << event << std::endl;
      return;
    }
  */
  pulse->Fit("flinear","Q","", x_low, x_high );
  double slope = flinear->GetParameter(0);
  double b     = flinear->GetParameter(1);
  
  if ( makePlot )
    {
      std::cout << "make plot" << std::endl;
      TCanvas* c = new TCanvas("canvas","canvas",800,400) ;
      pulse->GetXaxis()->SetLimits(x_low-10, x_high+10);
      pulse->SetMarkerSize(0.3);
      pulse->SetMarkerStyle(20);
      pulse->Draw("AP");
      c->SaveAs(fname+"LinearFit.pdf");
      //delete c;
    }
  tstamp[0] = (0.0*y-b)/slope;
  //std::cout << "----" << tstamp[0]  << std::endl;
  tstamp[1] = (0.15*y-b)/slope;
  tstamp[2] = (0.30*y-b)/slope;
  tstamp[3] = (0.45*y-b)/slope;
  tstamp[4] = (0.60*y-b)/slope;
  
  delete flinear;
};


double T1065Reco::GetGaussTime( TGraphErrors* pulse )
{
  return 0;
};


float T1065Reco::GetBaseline(TGraphErrors * pulse, int i_low, int i_high, TString fname )
{
  double x_low, x_high, y, dummy;
  pulse->GetPoint(i_low, x_low, y);
  pulse->GetPoint(i_high, x_high, y);
  
  TF1* flinear = new TF1("flinear","[0]", x_low, x_high );
  
  pulse->Fit("flinear","RQ","", x_low, x_high );
  
  /* std::cout << "make plot" << std::endl;
  std::cout << x_low << x_high << fname << std::endl;
  TCanvas* c = new TCanvas("canvas","canvas",800,400) ;
  pulse->GetXaxis()->SetLimits(x_low-3, x_high+3);
  pulse->SetMarkerSize(1);
  pulse->SetMarkerStyle(20);
  pulse->Draw("AP");
  c->SaveAs(fname+"LinearFit.pdf"); */
  
  float a = flinear->GetParameter(0);
  delete flinear;
  
  return a;
}

float T1065Reco::GetBaseline( int peak, short *a ) {

  float tmpsum = 0;
  float tmpcount = 0;
  //std::cout << "GGG\n";
  if (peak < 300) {
    for  (int i = peak + 200; i < 1000; i++) {
      // std::cout << i << " : " << a[i] << "\n";
      tmpsum += a[i];
      tmpcount += 1.0;
    }
  } else {
    for  (int i = 5; i < peak-200; i++) {
      // std::cout << i << " : " << a[i] << "\n";
      tmpsum += a[i];
      tmpcount += 1.0;
    }
  }
  // std::cout << tmpsum / tmpcount << "\n";

  return tmpsum / tmpcount;
}


float T1065Reco::GetPulseIntegral(int peak, short *a, std::string option) 
{
  float integral = 0.;

  if (option == "full") {
    for (int i=5; i < 1020; i++) {
      integral += a[i] * 0.2 * 1e-9 * (1.0/4096.0) * (1.0/50.0) * 1e12; //in units of pC, for 50Ohm termination
    }
  }
  else {
    for (int i=peak-20; i < peak+25; i++) {
      integral += a[i] * 0.2 * 1e-9 * (1.0/4096.0) * (1.0/50.0) * 1e12; //in units of pC, for 50Ohm termination
    }
  }
  return -1.0 * integral;

}

TGraphErrors* T1065Reco::GetTGraphFilter( short* channel, float* time, TString pulseName, bool makePlot )
{
  float Gauss[1024];
  //Setting Errors
  float errorX[1024], errorY[1024], channelFloat[1024];
  float _errorY = 0.00; //5%error on Y
  
  for ( int i = 0; i < 1024; i++ )
    {
      
      errorX[i]       = .0;
      errorY[i]       = _errorY*channel[i];
      channelFloat[i] = -channel[i];
    }
  
  TF1 *fb = new TF1("fb","gaus(0)", 0.0, 204.6);
  fb->SetParameter(1, 100);
  float sigma =1.0;
  fb->SetParameter(2, sigma);
  fb->SetParameter(0, 1/(sqrt(3.1415*2.0)*sigma) );
  //eval Gaussian
  float step = 0.2;//200ps
  for ( int i = 0; i < 1024; i++ )
    {
      Gauss[i] = fb->Eval( float(i)*step );
    }
  
  float channelFloatFiltered[2048];
  for ( int i = 0; i < 2048; i++ )
    {
      float convolvedPoint = 0;
      for ( int j = 0; j <= i; j++ )
	{
	  if ( i < 1024 )
	    {
	      convolvedPoint += channelFloat[i-j]*Gauss[1023-j];
	    }
	  else
	    {
	      if ( 1023-(i-1023)-j >= 0 ) convolvedPoint += channelFloat[1023-j]*Gauss[1023-(i-1023)-j];
	    }
	}
      //if ( i < 1024 ) channelFloatFiltered[i] = convolvedPoint;
      channelFloatFiltered[i] = convolvedPoint;
    }
  
  float channelFloatFilteredFix[1024];
  for ( int i = 0; i < 1024; i++ )
    {
      channelFloatFilteredFix[i] = 0.2*channelFloatFiltered[i+523];
    }
  
  TGraphErrors* tg = new TGraphErrors( 1024, time, channelFloat, errorX, errorY );
  TGraphErrors* tg2 = new TGraphErrors( 1024, time, channelFloatFilteredFix, errorX, errorY );

  if (makePlot) {
    TCanvas* c = new TCanvas("canvas","canvas",800,400) ;         
    tg2->GetXaxis()->SetLimits(50, 70);
    tg->GetXaxis()->SetLimits(50, 70);
    //tg2->Fit("fb","","", 0.0, 204.6 );
    tg2->SetMarkerSize(0.5);
    tg->SetMarkerSize(0.5);
    tg2->SetMarkerStyle(20);
    tg->SetMarkerStyle(20);
    tg2->Draw("AP");
    tg2->SetMarkerColor(kBlue);
    tg->Draw("sameP");
    c->SaveAs(pulseName + "GausPulse.pdf");
  }
  return tg;
};





bool T1065Reco::Begin(CfgManager& opts, uint64* index)
{
    //---inputs---
    if(!opts.OptExist(instanceName_+".srcInstanceName"))
    {
        cout << ">>> FFTAnalyzer ERROR: no source plugin specified" << endl;
        return false;
    }
    srcInstance_ = opts.GetOpt<string>(instanceName_+".srcInstanceName");
    float nSamples = opts.OptExist(srcInstance_+".nSamples") ?
        opts.GetOpt<int>(srcInstance_+".nSamples") :
        opts.GetOpt<int>(instanceName_+".nSamples");
    channelsNames_ = opts.GetOpt<vector<string> >(instanceName_+".channelsNames");
    timeRecoTypes_ = opts.GetOpt<vector<string> >(instanceName_+".timeRecoTypes");

    //---channels setup
    string templateTag="prof";
    if(opts.OptExist(instanceName_+".templateTags"))
        for(auto& tag : opts.GetOpt<vector<string> >(instanceName_+".templateTags"))
            for(auto& run : opts.GetOpt<vector<string> >(tag+".runList"))
                if(run == opts.GetOpt<string>("h4reco.run"))
                    templateTag = tag;

    for(auto& channel : channelsNames_)
    {        
        if(opts.OptExist(channel+".templateFit.file"))
        {            
            TFile* templateFile = TFile::Open(opts.GetOpt<string>(channel+".templateFit.file", 0).c_str(), ".READ");
            TH1* wfTemplate=(TH1*)templateFile->Get((opts.GetOpt<string>(channel+".templateFit.file", 1)+
                                                     +"_"+templateTag).c_str());
	    templates_[channel] = (TH1F*) wfTemplate->Clone();
	    templates_[channel] -> SetDirectory(0);
            templateFile->Close();
        }
        //---keep track of all the possible time reco method requested
        for(auto type_name : timeRecoTypes_)
        {
            if(opts.OptExist(channel+"."+type_name))
                timeOpts_[channel+"."+type_name] = opts.GetOpt<vector<float> >(channel+"."+type_name);
        }
    }
    
    //---outputs---
    eventCount_ = 0;
    string digiTreeName = opts.OptExist(instanceName_+".digiTreeName") ?
        opts.GetOpt<string>(instanceName_+".digiTreeName") : "digi";
    bool storeTree = opts.OptExist(instanceName_+".storeTree") ?
        opts.GetOpt<bool>(instanceName_+".storeTree") : true;
    RegisterSharedData(new TTree(digiTreeName.c_str(), "digi_tree"), "digi_tree", storeTree);
    digiTree_ = DigiTree(index, (TTree*)data_.back().obj);
    digiTree_.Init(channelsNames_, timeRecoTypes_);

     string t1065TreeName = opts.OptExist(instanceName_+".t1065TreeName") ?
        opts.GetOpt<string>(instanceName_+".t1065TreeName") : "t1065";
    //bool storeTree = opts.OptExist(instanceName_+".storeTree") ?
    //    opts.GetOpt<bool>(instanceName_+".storeTree") : true;
    RegisterSharedData(new TTree(t1065TreeName.c_str(), "t1065_tree"), "t1065_tree", storeTree);
    t1065Tree_ = T1065Tree(index, (TTree*)data_.back().obj);
    t1065Tree_.Init(channelsNames_, timeRecoTypes_);


    if(opts.GetOpt<int>(instanceName_+".fillWFtree"))
    {
        string wfTreeName = opts.OptExist(instanceName_+".wfTreeName") ?
            opts.GetOpt<string>(instanceName_+".wfTreeName") : "wf";
        RegisterSharedData(new TTree(wfTreeName.c_str(), "wf_tree"), "wf_tree", true);
        outWFTree_ = WFTree(channelsNames_.size(), nSamples, index, (TTree*)data_.back().obj);
        outWFTree_.Init();
    }

    return true;
}

bool T1065Reco::ProcessEvent(const H4Tree& event, map<string, PluginBase*>& plugins, CfgManager& opts)
{

    //---setup options
    bool drawDebugPulses = false;
  
    //---setup output event 
    int outCh=0;
    bool fillWFtree=false;
    if(opts.GetOpt<int>(instanceName_+".fillWFtree"))
        fillWFtree = *digiTree_.index % opts.GetOpt<int>(instanceName_+".WFtreePrescale") == 0;


    t1065Tree_.event = eventCount_;
    eventCount_++;

    if (eventCount_ % 1000 == 0) cerr << "Processing Event " << eventCount_ << "\n";

    //---load WFs from source instance shared data
    for(auto& channel : channelsNames_)
    {
        auto shared_data = plugins[srcInstance_]->GetSharedData(srcInstance_+"_"+channel, "", false);
        if(shared_data.size() != 0)
            WFs_[channel] = (WFClass*)shared_data.at(0).obj;
    }
    
    //---compute reco variables
    for(auto& channel : channelsNames_)
    {
        //---skip dead channels
        if(WFs_.find(channel) == WFs_.end())
        {
            ++outCh;
            continue; 
        }
        
        //---subtract a specified channel if requested
        if(opts.OptExist(channel+".subtractChannel") && WFs_.find(opts.GetOpt<string>(channel+".subtractChannel")) != WFs_.end())
            *WFs_[channel] -= *WFs_[opts.GetOpt<string>(channel+".subtractChannel")];        
        WFs_[channel]->SetBaselineWindow(opts.GetOpt<int>(channel+".baselineWin", 0), 
                                        opts.GetOpt<int>(channel+".baselineWin", 1));
        WFs_[channel]->SetSignalWindow(opts.GetOpt<int>(channel+".signalWin", 0), 
                                      opts.GetOpt<int>(channel+".signalWin", 1));
        WFBaseline baselineInfo = WFs_[channel]->SubtractBaseline();
        WFFitResults interpolAmpMax = WFs_[channel]->GetInterpolatedAmpMax(-1,-1,opts.GetOpt<int>(channel+".signalWin", 2));
        digiTree_.b_charge[outCh] = WFs_[channel]->GetIntegral(opts.GetOpt<int>(channel+".baselineInt", 0), 
                                                             opts.GetOpt<int>(channel+".baselineInt", 1));        
        digiTree_.b_slope[outCh] = baselineInfo.slope;
        digiTree_.b_rms[outCh] = baselineInfo.rms;
        digiTree_.maximum[outCh] = WFs_[channel]->GetAmpMax();
        digiTree_.time_maximum[outCh] = WFs_[channel]->GetTimeCF(1).first;
        digiTree_.amp_max[outCh] = interpolAmpMax.ampl;
        digiTree_.time_max[outCh] = interpolAmpMax.time;
        digiTree_.chi2_max[outCh] = interpolAmpMax.chi2;
        digiTree_.charge_tot[outCh] = WFs_[channel]->GetModIntegral(opts.GetOpt<int>(channel+".baselineInt", 1), 
                                                                   WFs_[channel]->GetNSample());
        digiTree_.charge_sig[outCh] = WFs_[channel]->GetSignalIntegral(opts.GetOpt<int>(channel+".signalInt", 0), 
                                                                     opts.GetOpt<int>(channel+".signalInt", 1));
        //---compute time with all the requested time reconstruction method
        for(int iT=0; iT<timeRecoTypes_.size(); ++iT)
        {
            //---compute time with selected method or store default value (-99)
            if(timeOpts_.find(channel+"."+timeRecoTypes_[iT]) != timeOpts_.end())
            {
                pair<float, float> timeInfo = WFs_[channel]->GetTime(timeRecoTypes_[iT], timeOpts_[channel+"."+timeRecoTypes_[iT]]);
                digiTree_.time[outCh+iT*channelsNames_.size()] = timeInfo.first;
                digiTree_.time_chi2[outCh+iT*channelsNames_.size()] = timeInfo.second;
            }
            else
            {
                digiTree_.time[outCh+iT*channelsNames_.size()] = -99;
                digiTree_.time_chi2[outCh+iT*channelsNames_.size()] = -99;
            }
        }
        
        //---template fit (only specified channels)
        WFFitResults fitResults{-1, -1000, -1};
       /*
	 if(opts.OptExist(channel+".templateFit.file"))
        {
            WFs_[channel]->SetTemplate(templates_[channel]);
            fitResults = WFs_[channel]->TemplateFit(opts.GetOpt<float>(channel+".templateFit.fitWin", 0),
                                                   opts.GetOpt<int>(channel+".templateFit.fitWin", 1),
                                                   opts.GetOpt<int>(channel+".templateFit.fitWin", 2));
            digiTree_.fit_ampl[outCh] = fitResults.ampl;
            digiTree_.fit_time[outCh] = fitResults.time;
            digiTree_.fit_chi2[outCh] = fitResults.chi2;
        }
	*/
            
	//---calibration constant for each channel if needed
	if(opts.OptExist(channel+".calibration.calibrationConst"))
	  digiTree_.calibration[outCh]=opts.GetOpt<float>(channel+".calibration.calibrationConst");
	else
	  digiTree_.calibration[outCh]=1;
	
        //---WFs---
        if(fillWFtree)
        {
            auto analizedWF = WFs_[channel]->GetSamples();
            int nSamples = analizedWF->size();
            float tUnit = WFs_[channel]->GetTUnit();
            for(int jSample=0; jSample<analizedWF->size(); ++jSample)
            {
                outWFTree_.WF_ch[jSample+outCh*nSamples] = outCh;
                outWFTree_.WF_time[jSample+outCh*nSamples] = jSample*tUnit;
                outWFTree_.WF_val[jSample+outCh*nSamples] = analizedWF->at(jSample);
            }
        }
        //---increase output tree channel counter
        
	//for T1065Tree
	int ngroup_t   =  int(outCh/9);
	int nchannel_t =  outCh%9;
	
	//fill waveform data
	for(int iSample=0; iSample<1024; iSample++)        
	{
		t1065Tree_.b_c[ngroup_t][nchannel_t][iSample] = (short)(WFs_[channel]->GetiSample(iSample));
		t1065Tree_.raw[outCh][iSample] = (short)(WFs_[channel]->GetiSample(iSample));
		t1065Tree_.t0[iSample] = iSample;

	}


	t1065Tree_.time[ngroup_t][0] = 0.0;
	for( int i = 1; i < 1024; i++){
	  t1065Tree_.time[ngroup_t][i] = float(i);
	}

	//find minimum
	int index_min = FindMinAbsolute(1024, t1065Tree_.raw[outCh]); // return index of the minc

	//Make Pulse shape Graph
	TString pulseName = Form("pulse_event%d_ch%d", eventCount_, outCh);
	TGraphErrors* pulse = new TGraphErrors( GetTGraph( t1065Tree_.raw[outCh], t1065Tree_.time[ngroup_t] ) );
	
	//our baseline subtraction
	float baseline;
        if ( index_min < 105 ) { baseline = GetBaseline( pulse, 850, 1020, pulseName);}
        else { baseline = GetBaseline( pulse, 5 ,150, pulseName);}
        t1065Tree_.base[outCh] = baseline;	

	//Correct pulse shape for baseline offset
	for(int j = 0; j < 1024; j++) {
	  t1065Tree_.raw[outCh][j] = (short)((double)(t1065Tree_.raw[outCh][j]) + baseline);
	}

	// DRS-glitch finder: zero out bins which have large difference
	// with respect to neighbors in only one or two bins
	for(int j = 1; j < 1022; j++) {
	  short a0 = abs(t1065Tree_.raw[outCh][j-1]);
	  short a1 = abs(t1065Tree_.raw[outCh][j]);
	  short a2 = abs(t1065Tree_.raw[outCh][j+1]);
	  short a3 = abs(t1065Tree_.raw[outCh][j+2]);
	  
	  if ( ( a1>3*a0 && a2>3*a0 && a2>3*a3 && a1>30) )
	    {
	      t1065Tree_.raw[outCh][j] = 0;
	      t1065Tree_.raw[outCh][j+1] = 0;
	    }
	  
	  if ( ( a1>3*a0 && a1>3*a2 && a1>30) )
	    t1065Tree_.raw[outCh][j] = 0;
	}
	
	delete pulse;

	
	// Find Peak Location using the improved algorithm
	pulse = new TGraphErrors( GetTGraph( t1065Tree_.raw[outCh], t1065Tree_.time[ngroup_t] ) );
	index_min = FindRealMin (1024, t1065Tree_.raw[outCh]); // return index of the min
	//if ( index_min > 0 ) std::cout << "ch: " << totalIndex << std::endl;
	t1065Tree_.xmin[outCh] = index_min;
	
	
	//Apply Filter
	pulse = GetTGraphFilter( t1065Tree_.raw[outCh], t1065Tree_.time[ngroup_t], pulseName , false);

	
	//Compute Amplitude : use units V
	Double_t tmpAmp = 0.0;
	Double_t tmpMin = 0.0;
	pulse->GetPoint(index_min, tmpMin, tmpAmp);
	t1065Tree_.amp[outCh] = tmpAmp* (1.0 / 4096.0); 


	//Get Pulse Integral
	// if ( t1065Tree_.xmin[outCh] != 0 ) {
	  t1065Tree_.integral[outCh] = GetPulseIntegral( index_min , t1065Tree_.raw[outCh], "");
	  t1065Tree_.integralFull[outCh] = GetPulseIntegral( index_min , t1065Tree_.raw[outCh], "full");
	// } else {
	//   t1065Tree_.integral[outCh] = 0.0;
	//   t1065Tree_.integralFull[outCh] = 0.0;
	// }

	//Gauss Time-Stamping 
	Double_t min = 0.; Double_t low_edge =0.; Double_t high_edge =0.; Double_t y = 0.; 
	pulse->GetPoint(index_min, min, y);	
	pulse->GetPoint(index_min-3, low_edge, y); // get the time of the low edge of the fit range
	pulse->GetPoint(index_min+3, high_edge, y);  // get the time of the upper edge of the fit range	

	float timepeak = 0;
	float timecf0   = 0;
	float timecf15   = 0;
	float timecf30   = 0;
	float timecf45   = 0;
	float timecf60   = 0;
	if( drawDebugPulses) {
	  timepeak =  GausFit_MeanTime(pulse, low_edge, high_edge, pulseName); // get the time stamp
	  float fs[5];
	  if ( t1065Tree_.xmin[outCh] != 0.0 ) {
	    RisingEdgeFitTime( pulse, index_min, fs, eventCount_, "linearFit_" + pulseName, true);
	  } else {
	    for ( int kk = 0; kk < 5; kk++ ) fs[kk] = -999;
	  }
	  timecf0  = fs[0];
	  timecf15 = fs[1];
	  timecf30 = fs[2];
	  timecf45 = fs[3];
	  timecf60 = fs[4];	 
	} else {
	  timepeak =  GausFit_MeanTime(pulse, low_edge, high_edge); // get the time stamp
	  float fs[5];
	  if ( t1065Tree_.xmin[outCh] != 0.0 ) {
	    RisingEdgeFitTime( pulse, index_min, fs, eventCount_, "", false);
	  } else {
	    for ( int kk = 0; kk < 5; kk++ ) fs[kk] = -999;
	  }
	  timecf0  = fs[0];
	  timecf15 = fs[1];
	  timecf30 = fs[2];
	  timecf45 = fs[3];
	  timecf60 = fs[4];
	}
	t1065Tree_.gauspeak[outCh]   = timepeak;
	t1065Tree_.linearTime0[outCh] = timecf0;
	t1065Tree_.linearTime15[outCh] = timecf15;
	t1065Tree_.linearTime30[outCh] = timecf30;
	t1065Tree_.linearTime45[outCh] = timecf45;
	t1065Tree_.linearTime60[outCh] = timecf60;

	delete pulse;


	// t1065Tree_.amp[outCh] = interpolAmpMax.ampl;
	// t1065Tree_.integral[outCh] = WFs_[channel]->GetSignalIntegral(20,25);
	// t1065Tree_.integralFull[outCh] = WFs_[channel]->GetIntegral(5,1020);

        ++outCh;
    }
 
    //---fill the output trees 
    //---reco var
    digiTree_.Fill();
    t1065Tree_.Fill();
    //---WFs
    if(fillWFtree)
        outWFTree_.Fill();

    return true;
}
