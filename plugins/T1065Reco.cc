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
    //std::cout << i << " " << a[i] << std::endl;
    if (xmin > a[i])  
      {
	//std::cout << "lower\n";
	xmin = a[i];
	loc = i;
      }
  }
  //std::cout << "loc0: " << loc << std::endl;
  return loc;
}

int T1065Reco::DigitalFindMin( int n, short *a) {
  
  if (n <= 0 || !a) return -1;
  float xmin = a[5];
  int loc = 0;
  int rise = 0;
  //for  (int i = 5; i < n-10; i++) {
  for  (int i = 0; i < n-10; i++) {
    //std::cout << i << " " << a[i] << std::endl;
    if (xmin > a[i])  
      {
	if (a[i]<-100 && rise==0) {
	  rise=i;
	  //std::cout << "rise = " << rise << std::endl;
	}
	
	//std::cout << "lower, " << i << ", " << a[i] << "; " << loc << ", " << xmin << std::endl;
	if (i-loc>20 && i-rise>50 && rise!=0) break;

	xmin = a[i];
	loc = i;
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
  
  //find noise level from first 100 bins
  for ( int i = 5; i < 100; i++)
    {
      if( abs(a[i]) > noise ) 
	{
	  noise = abs(a[i]);
	}
    }
  //std::cout << "noise: " << noise << std::endl;
  
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
      std::cout << "new peak @ " << loc_new << ", ymin: " << xmin_new << std::endl;
      if ( xmin_new > -2*noise || xmin_new > -40 ) loc_new = 0;
      xmin_new = a[5];
      loc = loc_new;
    }

  std::cout << "LOC2: " << loc << std::endl;                                                                                                                               
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
float T1065Reco::GausFit_MeanTime(TGraphErrors* pulse, const float index_first, const float index_last) {
  TF1* fpeak = new TF1("fpeak","gaus", index_first, index_last);
  pulse->Fit("fpeak","Q","", index_first, index_last);
  
  float timepeak = fpeak->GetParameter(1);
  delete fpeak;
  
  return timepeak;
};

float T1065Reco::GausFit_MeanTime(TGraphErrors* pulse, const float index_first, const float index_last, TString fname) {
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

float T1065Reco::RisingEdgeFitTime(TGraphErrors * pulse, const float index_min, const float constantFraction, TString fname, bool makePlot ) {
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


void T1065Reco::RisingEdgeFitTime(TGraphErrors * pulse, const float index_min, const float fitLowEdge, const float fitHighEdge, float* tstamp, float &risetime, int event, TString fname, bool makePlot ) {
  double x_low, x_high, y, dummy;
  double ymax;
  pulse->GetPoint(index_min, x_low, ymax);

  double x_tmp, y_tmp;
  for (int i=0; i<1024; i++) {
    pulse->GetPoint(i, x_tmp, y_tmp);
  }

  for ( int i = 1; i < 600; i++ )
    {
      pulse->GetPoint(index_min-i, x_low, y);
      if ( y < fitLowEdge*ymax ) break;
    }
  for ( int i = 1; i < 600; i++ )
    {
      pulse->GetPoint(index_min-i, x_high, y);
      if ( y < fitHighEdge*ymax ) break;
    }
  pulse->GetPoint(index_min, dummy, y);

  TF1* flinear = new TF1("flinear","[0]*x+[1]", x_low, x_high );
  float max = -9999;
  double* yy = pulse->GetY();
  
  for ( int i = 0; i < 1024; i++ )
    {
      if ( yy[i] > max ) max = yy[i];
    }

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
      pulse->GetXaxis()->SetLimits(x_low-100, x_high+100);
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
  if (slope != 0) {
    risetime = (0.90*y-b)/slope - (0.10*y-b)/slope;
  }
 
  delete flinear;
};

void T1065Reco::DigitalRisingEdgeFitTime(TGraphErrors * pulse, const float index_min, float* tstamp, float &risetime, int event, TString fname, bool makePlot ) {
  double x_low, x_high, y, dummy;
  double ymax;
  pulse->GetPoint(index_min, x_low, ymax);

  double x_tmp, y_tmp;
  for (int i=0; i<1024; i++) {
    pulse->GetPoint(i, x_tmp, y_tmp);
  }

  for ( int i = 1; i < 600; i++ )
    {
      pulse->GetPoint(index_min-i, x_low, y);

      if ( y < 0.05*ymax ) break;
    }

  for ( int i = 1; i < 600; i++ )
    {
      pulse->GetPoint(index_min-i, x_high, y);
      if ( y < 0.95*ymax ) break;
    }
  pulse->GetPoint(index_min, dummy, y);

  TF1* flinear = new TF1("flinear","[0]*x+[1]", x_low, x_high );
  float max = -9999;
  double* yy = pulse->GetY();
  
  for ( int i = 0; i < 1024; i++ )
    {
      if ( yy[i] > max ) max = yy[i];
    }

  pulse->Fit("flinear","Q","", x_low, x_high );
  double slope = flinear->GetParameter(0);
  double b     = flinear->GetParameter(1);
  
  if ( makePlot )
    {
      std::cout << "make plot" << std::endl;
      TCanvas* c = new TCanvas("canvas","canvas",800,400) ;
      pulse->GetXaxis()->SetLimits(x_low-100, x_high+100);
      pulse->SetMarkerSize(0.3);
      pulse->SetMarkerStyle(20);
      pulse->Draw("AP");
      c->SaveAs(fname+"LinearFit.pdf");
      //delete c;
    }
  if (slope<=0) {

    tstamp[0]=1000;
    tstamp[1]=1000;
    tstamp[2]=1000;
    tstamp[3]=1000;
    tstamp[4]=1000;
    risetime =1000;
    //std::cout << "!!!!!! problem: " << slope << ", " << risetime << std::endl;
  }
  else {
    tstamp[0] = (0.0*y-b)/slope;
    tstamp[1] = (0.15*y-b)/slope;
    tstamp[2] = (0.30*y-b)/slope;
    tstamp[3] = (0.45*y-b)/slope;
    tstamp[4] = (0.60*y-b)/slope;
    //if (slope != 0) {
    risetime = (0.90*y-b)/slope - (0.10*y-b)/slope;
    //}
  }
 
  delete flinear;
};

double T1065Reco::GetGaussTime( TGraphErrors* pulse ) {
  return 0;
};


float T1065Reco::GetBaseline(TGraphErrors * pulse, int i_low, int i_high) {
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
  //std::cout << a << std::endl;
  delete flinear;
  
  return a;
}

float T1065Reco::DigitalGetBaseline(TGraphErrors * pulse, int i_low, int i_high) {
  double x_low, x_high, y_low, y_high, dummy;
  pulse->GetPoint(i_low, x_low, y_low);
  pulse->GetPoint(i_high, x_high, y_high);

  

  if (abs(y_high-y_low)>30) {
    pulse->GetPoint(0, x_low, y_low);
    pulse->GetPoint(floor(i_high/10), x_high, y_high);
  }
  
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
  //std::cout << a << std::endl;
  delete flinear;
  
  return a;
}

float T1065Reco::GetBaseline( int peak, short *a , int nbinsExcludedLeftOfPeak , int nbinsExcludedRightOfPeak) {

  float tmpsum = 0;
  float tmpcount = 0;
  //std::cout << "GGG\n";
  //cout << peak << "\n";
  //if (peak + nbinsExcludedRightOfPeak > 950 || peak-nbinsExcludedLeftOfPeak <= 50) {
  //  cout << "Warning: Peak = " << peak << " is too close to left or right boundary \n";
  //  return 0;
  //}

  if (peak+nbinsExcludedRightOfPeak < 1024) {
    for  (int i = peak + nbinsExcludedRightOfPeak; i < 1000; i++) {
      //std::cout << i << " : " << a[i] << "\n";
      tmpsum += a[i];
      tmpcount += 1.0;
    }
  }
  if (peak-nbinsExcludedLeftOfPeak >= 0) {
    for  (int i = 5; i < peak-nbinsExcludedLeftOfPeak; i++) {
      //std::cout << i << " : " << a[i] << "\n";
      tmpsum += a[i];
      tmpcount += 1.0;
    }
  }

  //std::cout << tmpsum / tmpcount << "\n";

  if (tmpcount == 0) return 0;
  else return tmpsum / tmpcount;
}


float T1065Reco::GetPulseIntegral(int peak, short *a, std::string option) {
  float integral = 0.;

  if (option == "full") {
    for (int i=5; i < 1010; i++) {
      integral += a[i] * 0.2 * 1e-9 * (1.0/4096.0) * (1.0/50.0) * 1e12; //in units of pC, for 50Ohm termination
      //cout << "int: " << i << " " << a[i] << " --> " << integral << "\n";
    }
    //cout << "final int = " << -1.0*integral << "\n";
  }
  else {
    for (int i=peak-20; i < peak+25; i++) {
      integral += a[i] * 0.2 * 1e-9 * (1.0/4096.0) * (1.0/50.0) * 1e12; //in units of pC, for 50Ohm termination
    }
  }

  return -1.0 * integral;
}

TGraphErrors* T1065Reco::GetTGraphFilter( short* channel, float* time, TString pulseName, bool makePlot ) {
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


float ConstantThresholdTime(TGraphErrors* pulse, const float threshold)
{
  double* yy = pulse->GetY();
  double* xx = pulse->GetX();
  int indexCrossThreshold = 0;
  for ( int i = 0; i < 1024; i++ )
    {
      if (yy[i] > threshold) {
	indexCrossThreshold = i;
	break;
      }
    }

  double y2 = yy[indexCrossThreshold];
  double x2 = xx[indexCrossThreshold];
  double y1 = y2;
  double x1 = x2; 
  if (indexCrossThreshold>0) {
    y1 = yy[indexCrossThreshold-1];
    x1 = xx[indexCrossThreshold-1];
  }
  double xThreshold = (threshold - y1) * (x2-x1)/(y2-y1) + x1;  

  return xThreshold;
};

float SigmoidTimeFit(TGraphErrors * pulse, const float index_min, int event, TString fname, bool makePlot )
{
  double x_low, x_high, y, dummy;
  double ymax;
  pulse->GetPoint(index_min, x_low, ymax);
  
  pulse->GetPoint(index_min-150, x_low, y);
  
  for ( int i = 1; i < 200; i++ )
    {
      pulse->GetPoint(index_min-i, x_high, y);
      if ( y < 0.6*ymax ) break;
    }
  //pulse->GetPoint(index_min-8, x_low, y);
  //pulse->GetPoint(index_min-3, x_high, y);


  //pulse->GetPoint(index_min-12, x_low, y);
  //pulse->GetPoint(index_min-7, x_high, y);
  pulse->GetPoint(index_min, dummy, y);
  
  TF1* fsigmoid = new TF1("fsigmoid","[0]/(1.0+exp(-(x-[1])/[2]))",x_low-50,x_high+50);
  fsigmoid->SetParameter(0,1000);
  fsigmoid->SetParLimits(0,0,10000);
  fsigmoid->SetParameter(1,50);
  fsigmoid->SetParameter(2,2);
  
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
  pulse->Fit("fsigmoid","Q","", x_low, x_high );
  double maxAmp   = fsigmoid->GetParameter(0);
  double midpoint = fsigmoid->GetParameter(1);
  double width    = fsigmoid->GetParameter(2);
  
  if ( makePlot )
    {
      TCanvas* c = new TCanvas("canvas","canvas",800,400) ;
      pulse->GetXaxis()->SetLimits(x_low-50, x_high+50);
      pulse->SetMarkerSize(0.3);
      pulse->SetMarkerStyle(20);
      pulse->Draw("AP");
      fsigmoid->SetLineColor(kBlue);
      fsigmoid->Draw("same");
      c->SaveAs(fname+"Sigmoid.pdf");
      delete c;
    }
  

  return midpoint-width*log(maxAmp/0.1 -1);
  
  delete fsigmoid;
};

float FullFitScint( TGraphErrors * pulse, const float index_min, int event, TString fname, bool makePlot) 
{

  double x_max;
  double ymax;
  pulse->GetPoint(index_min, x_max, ymax);
  TF1* fullFit = new TF1("fullFit","[0]*([1]/2.0)*exp([1]/2.*(2*[2]+[1]*[3]*[3]-2*x))*ROOT::Math::erfc(([2]+[1]*[3]*[3]-x)/(TMath::Sqrt(2.)*[3]))",x_max-50,x_max+50);
  fullFit->SetParameter(0,ymax/0.1);
  fullFit->SetParameter(1,0.5);
  fullFit->SetParameter(2,135);
  fullFit->SetParameter(3,10);
  
  float max = -9999;
  double* yy = pulse->GetY();
  
  for ( int i = 0; i < 1024; i++ )
    {
      if ( yy[i] > max ) max = yy[i];
    }
 
  pulse->Fit("fullFit","Q","", x_max-50, x_max+50 );
  double maxAmp   = fullFit->GetParameter(0);
  double lambda   = fullFit->GetParameter(1);
  double mu       = fullFit->GetParameter(2);
  double sigma    = fullFit->GetParameter(3);

  mu = -999;
  for( int i = 0; i < 1000; i++ )
    {
      if ( fullFit->Eval(30.+0.001*i) > 0.05*ymax )
	{
	  mu = 30.+0.001*i;
	  break;
	}
    }

  TLine* line  = new TLine( mu, 0, mu, 1000);
  if ( makePlot )
    {
      TCanvas* c = new TCanvas("canvas","canvas",800,400) ;
      pulse->GetXaxis()->SetLimits(0, 200);
      pulse->SetMarkerSize(0.3);
      pulse->SetMarkerStyle(20);
      pulse->Draw("AP");
      fullFit->SetLineColor(kBlue-3);
      fullFit->Draw("same");
      line->Draw("same");
      line->SetLineColor(kRed);
      line->SetLineWidth(2);
      line->SetLineStyle(2);
      c->SaveAs(fname+"fullFit.pdf");
      delete c;
    }

  delete fullFit;
  return mu;
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

bool T1065Reco::ProcessEvent(const H4Tree& event, map<string, PluginBase*>& plugins, CfgManager& opts) {
	
  //---setup options
  bool drawDebugPulses = false;
  bool doTimeRecoFits = true;

  //---setup output event 
  int outCh=0;

  eventCount_++;
  t1065Tree_.event = eventCount_;
  //std::cout << eventCount_ << std::endl;
  if (eventCount_ % 100 == 0) cerr << "Processing Event " << eventCount_ << "\n";

  //if (t1065Tree_.event != 39) return true;

  //---load WFs from source instance shared data
  for(auto& channel : channelsNames_)
    {
      auto shared_data = plugins[srcInstance_]->GetSharedData(srcInstance_+"_"+channel, "", false);
      if(shared_data.size() != 0)
	WFs_[channel] = (WFClass*)shared_data.at(0).obj;
    }
    
  //---compute reco variables
  for(auto& channel : channelsNames_) {

    //cout << "Channel " << outCh << " : " << channel << "\n";
    //---skip dead channels
    if(WFs_.find(channel) == WFs_.end())
      {
	++outCh;
	continue; 
      }                    	
       
    //for T1065Tree
    int ngroup_t   =  int(outCh/6);
    int nchannel_t =  outCh%6;
	
    //fill waveform data
    short rawInverted[1024];
	
    int NSample_t = WFs_[channel]->GetNSample();
    //NSample_t = 1024;//
    //cout << "NSample: " << NSample_t << "\n";
    for(int iSample=0; iSample<1024; iSample++) {
      //t1065Tree_.b_c[ngroup_t][nchannel_t][iSample] = (short)(WFs_[channel]->GetiSample(iSample));
      if(iSample<NSample_t)
	{
	  t1065Tree_.raw[outCh][iSample] = (short)(-1*WFs_[channel]->GetiSample(iSample));
	  rawInverted[iSample] = (short)(-1*t1065Tree_.raw[outCh][iSample]);
	  //cout << "Debug: " << iSample << " : " << WFs_[channel]->GetiSample(iSample) << "\n";
	}
      else
	{
	  t1065Tree_.raw[outCh][iSample] = 0;
	  rawInverted[iSample] = 0;
	}
      t1065Tree_.t0[iSample] = iSample;	 
	
    }

    t1065Tree_.time[ngroup_t][0] = 0.0;
    for( int i = 1; i < 1024; i++){
      t1065Tree_.time[ngroup_t][i] = float(i) * 0.200;
    }

    //Make Pulse shape Graph
    TString pulseName = Form("pulse_event%d_ch%d", eventCount_, outCh);
    TGraphErrors* pulse = new TGraphErrors( GetTGraph( t1065Tree_.raw[outCh], t1065Tree_.time[ngroup_t] ) );
    //TGraphErrors* pulse = new TGraphErrors( GetTGraph( t1065Tree_.raw[outCh], index ) );

    /*TCanvas* c = new TCanvas("canvas","canvas",800,400) ;
    //pulse->GetXaxis()->SetLimits(x_low-50, x_high+50);
    pulse->SetMarkerSize(0.3);
    pulse->SetMarkerStyle(20);
    pulse->Draw("AP");
    c->SaveAs(pulseName+"_sanity.pdf");
    delete c;*/

    //our baseline subtraction
    float baseline;
    if (channel.substr(0,4) == "NINO") {
      baseline = DigitalGetBaseline(pulse, 0, 40);
    }
    else {
      baseline = GetBaseline(pulse, 5, 45);
    }
    t1065Tree_.base[outCh] = baseline;	

    //Correct pulse shape for baseline offset
    for(int j = 0; j < 1024; j++) {
      t1065Tree_.raw[outCh][j] = (short)((double)(t1065Tree_.raw[outCh][j]) + baseline);
    }

    //find minimum
    int index_min = 0;
    if (channel.substr(0,4) == "NINO") {
      //std::cout << channel << std::endl;
      index_min = DigitalFindMin(1024, t1065Tree_.raw[outCh]);
      //std::cout << index_min << ", " << t1065Tree_.time[ngroup_t][index_min] << std::endl;
    }
    else {
      index_min = FindMinAbsolute(1024, t1065Tree_.raw[outCh]); // return index of the minc
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
    t1065Tree_.xmin[outCh] = index_min;	
	
    //Apply Filter
    pulse = GetTGraphFilter( t1065Tree_.raw[outCh], t1065Tree_.time[ngroup_t], pulseName , false);
	
    //Compute Amplitude : use units V
    Double_t tmpAmp = 0.0;
    Double_t tmpMin = 0.0;
    if (index_min > 0) {
      pulse->GetPoint(index_min, tmpMin, tmpAmp);
      //cout << "amp: " << tmpAmp << " " << t1065Tree_.raw[outCh][index_min] << "\n";
      t1065Tree_.amp[outCh] = tmpAmp * (1.0 / 4096.0); 
    } else {
      t1065Tree_.amp[outCh] = 0;
    }

    // Get Pulse Integral
    t1065Tree_.integral[outCh] = GetPulseIntegral( index_min , t1065Tree_.raw[outCh], "");
    t1065Tree_.integralFull[outCh] = GetPulseIntegral( index_min , t1065Tree_.raw[outCh], "full");


    //Gauss Time-Stamping 
    Double_t min = 0.; Double_t low_edge =0.; Double_t high_edge =0.; Double_t y = 0.; 
    pulse->GetPoint(index_min, min, y);	
    pulse->GetPoint(index_min-4, low_edge, y); // get the time of the low edge of the fit range
    pulse->GetPoint(index_min+4, high_edge, y);  // get the time of the upper edge of the fit range	


    if (doTimeRecoFits) {
      float risetime = 0;
      float timepeak = 0;
      float timecf0   = 0; 
      float timecf15   = 0;
      float timecf30   = 0;
      float timecf45   = 0;
      float timecf60   = 0;
      float timesigmoid = 0;
      float timefullfit = 0;
      float timeconsthresh = 0;
      float cft_low_range  = 0.15;
      float cft_high_range = 0.70;
      if( drawDebugPulses) {
	timepeak =  GausFit_MeanTime(pulse, low_edge, high_edge, pulseName); // get the time stamp
	float fs[5];
	if ( t1065Tree_.xmin[outCh] != 0.0 ) {
	  if (channel.substr(0,4) == "NINO") {
	    DigitalRisingEdgeFitTime( pulse, index_min, fs, risetime, eventCount_, "digitalFit_" + pulseName, true);
	    //if (risetime==1000) std::cout << eventCount_ << ", " << outCh << " has a problem with the rising edge" << std::endl;
	  }
	  else {
	    RisingEdgeFitTime( pulse, index_min, cft_low_range, cft_high_range, fs, risetime, eventCount_, "linearFit_" + pulseName, true);
	  }
	} else {
	  for ( int kk = 0; kk < 5; kk++ ) fs[kk] = -999;
	}
	timecf0  = fs[0];
	timecf15 = fs[1];
	timecf30 = fs[2];
	timecf45 = fs[3];
	timecf60 = fs[4];
	//timesigmoid = SigmoidTimeFit( pulse, index_min, event, "SigmoidFit_" + pulseName, true );
	//timefullfit = FullFitScint( pulse, index_min, event, "fullFit_" + pulseName, true );
      } else {
	timepeak =  GausFit_MeanTime(pulse, low_edge, high_edge); // get the time stamp
	float fs[5];
	if ( t1065Tree_.xmin[outCh] != 0.0 ) {
	  if (channel.substr(0,4) == "NINO") {
	    //std::cout << eventCount_ << ", "<< index_min << ", " << t1065Tree_.time[ngroup_t][index_min] << std::endl;
	    DigitalRisingEdgeFitTime( pulse, index_min, fs, risetime, eventCount_, "digitalFit_" + pulseName, false);
	    //if (risetime==1000) std::cout << eventCount_ << ", " << outCh << " has a problem with the rising edge" << std::endl;
	  }
	  else {
	    RisingEdgeFitTime( pulse, index_min,  cft_low_range, cft_high_range, fs, risetime, eventCount_, "linearFit_" + pulseName, false);
	  }
	} else {
	  for ( int kk = 0; kk < 5; kk++ ) fs[kk] = -999;
	}
	timecf0  = fs[0];
	timecf15 = fs[1];
	timecf30 = fs[2];
	timecf45 = fs[3];
	timecf60 = fs[4];
	//timesigmoid = SigmoidTimeFit( pulse, index_min, event, "SigmoidFit_" + pulseName, false );
	//timefullfit = FullFitScint( pulse, index_min, event, "fullFit_" + pulseName, false );
      }
      t1065Tree_.gauspeak[outCh]   = timepeak;
      t1065Tree_.linearTime0[outCh] = timecf0;
      t1065Tree_.linearTime15[outCh] = timecf15;
      t1065Tree_.linearTime30[outCh] = timecf30;
      t1065Tree_.linearTime45[outCh] = timecf45;
      t1065Tree_.linearTime60[outCh] = timecf60;
      t1065Tree_.risetime[outCh]   = risetime;

      //t1065Tree_.sigmoidTime[outCh] = timesigmoid;
      //t1065Tree_.fullFitTime[outCh] = timefullfit;
      //t1065Tree_.constantThresholdTime[outCh] = ConstantThresholdTime( pulse, 50);
      
  }
	
    delete pulse;

    ++outCh;
  }

     //WireChamber reco
    int chXl_ = opts.GetOpt<int>(instanceName_+".chXleft");
    int chXr_ = opts.GetOpt<int>(instanceName_+".chXright");
    int chYu_ = opts.GetOpt<int>(instanceName_+".chYup");
    int chYd_ = opts.GetOpt<int>(instanceName_+".chYdown");

    vector<float> timeL, timeR, timeU, timeD;
    for(int iCh=0; iCh<event.nTdcChannels; ++iCh)
      {
        if(event.tdcChannel[iCh]==chXl_)
	  timeL.push_back(event.tdcData[iCh]);
        if(event.tdcChannel[iCh]==chXr_)
	  timeR.push_back(event.tdcData[iCh]);
        if(event.tdcChannel[iCh]==chYu_)
	  timeU.push_back(event.tdcData[iCh]);
        if(event.tdcChannel[iCh]==chYd_)
	  timeD.push_back(event.tdcData[iCh]);
      }

    if(timeR.size()!=0 && timeL.size()!=0)
      t1065Tree_.TDCx = (*min_element(timeR.begin(), timeR.begin()+timeR.size()) -
			 *min_element(timeL.begin(), timeL.begin()+timeL.size()))*0.005;
    else
      t1065Tree_.TDCx = -1000;
    if(timeU.size()!=0 && timeD.size()!=0)
      t1065Tree_.TDCy = (*min_element(timeU.begin(), timeU.begin()+timeU.size()) -
			 *min_element(timeD.begin(), timeD.begin()+timeD.size()))*0.005;
    else
      t1065Tree_.TDCy = -1000;

  //---fill the output trees 
  //---reco var
  digiTree_.Fill();
  t1065Tree_.Fill();

  return true;
}
