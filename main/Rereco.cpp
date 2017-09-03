// C++ includes
#include <fstream>
#include <string>
#include <iostream>

// ROOT includes
#include <TROOT.h>
#include <TStyle.h>
#include <TFile.h>
#include <TTree.h>
#include <TGraphErrors.h>
#include <TCanvas.h>

//LOCAL INCLUDES
#include "interface/Aux.h"
#include "interface/Config.h"

using namespace std;

TStyle* style;

int graphic_init();

int index_CLOCK = 6;
int index_MCP0 = 7;
int index_MCP1 = 14;

int CLOCK_CYCLE = 100; 

std::string ParseCommandLine( int argc, char* argv[], std::string opt )
{
  for (int i = 1; i < argc; i++ )
    {
      std::string tmp( argv[i] );
      if ( tmp.find( opt ) != std::string::npos )
	{
	  if ( tmp.find( "=" )  != std::string::npos ) return tmp.substr( tmp.find_last_of("=") + 1 );
	  if ( tmp.find( "--" ) != std::string::npos ) return "yes";
	}
    }

  return "";
};


int main(int argc, char **argv) {
  gROOT->SetBatch();

  FILE* fp1;
  char stitle[200]; 
  int dummy;

  //**************************************
  // Parse command line arguments
  //**************************************

  int numRequiredArgs = 3;
  if (argc - 1 < numRequiredArgs) {
    std::cerr << "Usage: dat2root in_file.dat num_events" << std::endl;
    return -1;
  }
  std::cout << "\n=== Beginning program ===\n" << std::endl;

  std::string inputFilename = argv[1];
  std::string outputFilename = argv[2];
  std::cout << "Input file: " << inputFilename << std::endl;
  std::cout << "Output file: " << outputFilename << std::endl;

  // Check if has valid input file, otherwise exit with error
  ifstream ifile(inputFilename);
  if (!ifile) {
    printf("!USAGE! Input file does not exist. Please enter valid file name"); 
    exit(0);}

  // Check if input file is root type or dat type
  // datfile: true is dat, false is root

  std::string filetype = inputFilename.substr(inputFilename.find_last_of(".") + 1);
  if (filetype != "root") {
     printf("!USAGE! Invalid input file type");
    exit(0);
  }
  
  
  int nEvents = atoi(argv[3]);
  std::cout << "Will process " << nEvents << " events" << std::endl;

  // Board number is fixed at 1 for now because we only have one board
  std::string boardNumber = "1";
  std::cout << "Will use calibration files for board number " << boardNumber << "\n";

  bool saveRaw = false;
  std::string _saveRaw = ParseCommandLine( argc, argv, "--saveRaw" );
  if ( _saveRaw == "yes" ) {
    saveRaw = true;
    std::cout << "Will save raw pulses\n";
  }

  bool drawDebugPulses = false;
  std::string _drawDebugPulses = ParseCommandLine( argc, argv, "--debug" );
  if ( _drawDebugPulses == "yes" ) {
    drawDebugPulses = true;
    std::cout << "draw: " << drawDebugPulses << std::endl;
  }

  std::string configName = "cfg/RerecoConfig_BTL_06282017.config";
  //std::string configName = "alignmentTestConfig.config";
  std::string _configName = ParseCommandLine( argc, argv, "--config" );
  if ( _configName != "" ) {
    configName = _configName;
  }

  //	bool doFilter = false;
  //	std::string _doFilter = ParseCommandLine( argc, argv, "--doFilter" );
  //	if ( _doFilter == "yes" ) {
  //		saveRaw = true;
  //		doFilter = true;
  //		std::cout << "Will apply Weierstrass transform (gaussian filter) to input pulses\n";
  //	}


  std::cout << "\n=== Parsing configuration file " << configName << " ===\n" << std::endl;
  Config config(configName);
  if ( !config.hasChannels() || !config.isValid() ) {
    std::cerr << "\nFailed to load channel information from config " << configName << std::endl;
    return -1;
  }

  //// ONLY DO IF DAT FILETYPE

  double tcal_dV[4][1024];
  double tcal[4][1024];
  double off_mean[4][9][1024];


  //// DO FOR ALL FILE TYPES

  //**************************************
  // Define output
  //**************************************

  TFile* file = new TFile( outputFilename.c_str(), "RECREATE", "CAEN V1742");
  TTree* tree = new TTree("pulse", "Digitized waveforms");

  uint event;
  UShort_t tc[4]; // trigger counter bin
	
  // want groups so set everything to -1 for init
  float time[4][1024]; // calibrated time
	
  //memset(time, -1, sizeof(time[0][0]) * 4 * 1024);
	
  for (int i =0; i < 4; i++) {
    for (int j=0; j < 1024; j++) {
      //if (time[i][j] != -1)
      //std::cout << "NOT CORRECT" << std::endl;
      time[i][j] = -999.0;
    }
  }
  
  //std::cout << "FINISHED TESTING" << std::endl;


  short raw[36][1024]; // ADC counts
  short channel[36][1024]; // calibrated input (in V)
  double channelFilter[36][1024]; // calibrated input (in V)
  float xmin[36]; // location of peak
  float xminRestricted[36]; //location of peak restricted near to channel 0 reference
  float base[36]; // baseline voltage
  float amp[36]; // pulse amplitude
  float amp_inverse[36]; // pulse amplitude
  float ampRestricted[36]; // pulse amplitude within a restricted window near channel 0 reference
  float integral[36]; // integral in a window
  float integralFull[36]; // integral over all bins
  float gauspeak[36]; // time extracted with gaussian fit
  float MCPtoClockTime_gauspeak[2];
  float MCPtoClockTime_linearTime0[2];
  float MCPtoClockTime_linearTime15[2];
  float MCPtoClockTime_linearTime30[2];
  float MCPtoClockTime_linearTime45[2];
  float MCPtoClockTime_linearTime60[2];
  float sigmoidTime[36];//time extracted with sigmoid fit
  float linearTime0[36]; // constant fraction fit coordinates
  float linearTime15[36];
  float linearTime30[36];
  float linearTime45[36];
  float linearTime60[36];       
  float fallingTime[36]; // falling exponential timestamp
  float exactTimeCFD[36]; //algorithm from Nicola Minafra
	
  float risetime[36]; 
  float constantThresholdTime[36];
  bool _isRinging[36];

  float	hodoX[2]; 
  float	hodoY[2]; 
  int nFibresOnX[2];
  int nFibresOnY[2];
  float TDCx;
  float TDCy;
  float xSlope;
  float ySlope;
  float x1;
  float y1;
  float x2;
  float y2;

  //TOFPET variables
  bool   tofpet_isMatched;
  double tofpet_t_sipm[64];
  double tofpet_tot[64];
  double tofpet_energy[64];
  double tofpet_tqT[64];
  double tofpet_tqE[64];
  double tofpet_x[64];
  double tofpet_y[64];
  double tofpet_z[64];
  int tofpet_xi[64];
  int tofpet_yi[64];
  double tofpet_t_h4daq[64];
  double tofpet_t_tofpet[64];

  
  tree->Branch("event", &event, "event/I");
  tree->Branch("tc", tc, "tc[4]/s");
  tree->Branch("channel", channel, "channel[36][1024]/S");
  tree->Branch("channelFilter", channelFilter, "channelFilter[36][1024]/D");
  tree->Branch("time", time, "time[4][1024]/F");
  tree->Branch("xmin", xmin, "xmin[36]/F");
  tree->Branch("xminRestricted", xminRestricted, "xminRestricted[36]/F");
  tree->Branch("amp", amp, "amp[36]/F");
  tree->Branch("amp_inverse", amp_inverse, "amp_inverse[36]/F");
  tree->Branch("ampRestricted", ampRestricted, "ampRestricted[36]/F");
  tree->Branch("base", base, "base[36]/F");
  tree->Branch("integral", integral, "integral[36]/F");
  tree->Branch("intfull", integralFull, "intfull[36]/F");
  tree->Branch("gauspeak", gauspeak, "gauspeak[36]/F");
  tree->Branch("MCPtoClockTime_gauspeak", MCPtoClockTime_gauspeak, "MCPtoClockTime_gauspeak[2]/F");
  tree->Branch("MCPtoClockTime_linearTime0", MCPtoClockTime_linearTime0, "MCPtoClockTime_linearTime0[2]/F");
  tree->Branch("MCPtoClockTime_linearTime15", MCPtoClockTime_linearTime15, "MCPtoClockTime_linearTime15[2]/F");
  tree->Branch("MCPtoClockTime_linearTime30", MCPtoClockTime_linearTime30, "MCPtoClockTime_linearTime30[2]/F");
  tree->Branch("MCPtoClockTime_linearTime45", MCPtoClockTime_linearTime45, "MCPtoClockTime_linearTime45[2]/F");
  tree->Branch("MCPtoClockTime_linearTime60", MCPtoClockTime_linearTime60, "MCPtoClockTime_linearTime60[2]/F");
  tree->Branch("sigmoidTime", sigmoidTime, "sigmoidTime[36]/F");
  tree->Branch("linearTime0", linearTime0, "linearTime0[36]/F");
  tree->Branch("linearTime15", linearTime15, "linearTime15[36]/F");
  tree->Branch("linearTime30", linearTime30, "linearTime30[36]/F");
  tree->Branch("linearTime45", linearTime45, "linearTime45[36]/F");
  tree->Branch("linearTime60", linearTime60, "linearTime60[36]/F");
  tree->Branch("fallingTime", fallingTime, "fallingTime[36]/F");
  tree->Branch("exactTimeCFD", exactTimeCFD, "exactTimeCFD[36]/F");
  tree->Branch("risetime", risetime, "risetime[36]/F");
  tree->Branch("constantThresholdTime", constantThresholdTime, "constantThresholdTime[36]/F");
  tree->Branch("isRinging", _isRinging, "isRinging[36]/O");  
  tree->Branch("hodoX", hodoX, "hodoX[2]/F");
  tree->Branch("hodoY", hodoY, "hodoY[2]/F");
  tree->Branch("nFibresOnX", nFibresOnX, "nFibresOnX[2]/F");
  tree->Branch("nFibresOnY", nFibresOnY, "nFibresOnY[2]/F");

  tree->Branch("TDCx", &TDCx, "TDCx/F");
  tree->Branch("TDCy", &TDCy, "TDCy/F");
  tree->Branch("xSlope", &xSlope, "xSlope/F");
  tree->Branch("ySlope", &ySlope, "ySlope/F");
  tree->Branch("x1", &x1, "x1/F");
  tree->Branch("y1", &y1, "y1/F");
  tree->Branch("x2", &x2, "x2/F");
  tree->Branch("x2", &x2, "x2/F");
  tree->Branch("tofpet_isMatched", &tofpet_isMatched, "tofpet_isMatched/O");
  tree->Branch("tofpet_t_sipm", tofpet_t_sipm, "tofpet_t_sipm[64]/D");
  tree->Branch("tofpet_tot", tofpet_tot, "tofpet_tot[64]/D");
  tree->Branch("tofpet_energy", tofpet_energy, "tofpet_energy[64]/D");
  tree->Branch("tofpet_tqT", tofpet_tqT, "tofpet_tqT[64]/D");
  tree->Branch("tofpet_tqE", tofpet_tqE, "tofpet_tqE[64]/D");
  tree->Branch("tofpet_x", tofpet_x, "tofpet_x[64]/D");
  tree->Branch("tofpet_y", tofpet_y, "tofpet_y[64]/D");
  tree->Branch("tofpet_z", tofpet_z, "tofpet_z[64]/D");
  tree->Branch("tofpet_t_tofpet", tofpet_t_tofpet, "tofpet_t_tofpet[64]/D");
  tree->Branch("tofpet_t_h4daq", &tofpet_t_h4daq, "tofpet_t_h4daq/D");
  tree->Branch("tofpet_xi", tofpet_xi, "tofpet_xi[64]/I");
  tree->Branch("tofpet_yi", tofpet_yi, "tofpet_yi[64]/I");

  
  // temp variables for data input
  uint   event_header;
  uint   temp[3];
  ushort samples[9][1024];

  //*************************
  // Open Input File
  //*************************

  FILE *fpin = 0;
  TFile *rootInput = 0;
  TTree *rootInputTree = 0;
  TTree *rootInputTree_hodo = 0;
  TTree *rootInputTree_tofpet = 0;
  

  int nGoodEvents = 0;

  // Set according to if dat type or root type
  rootInput = new TFile( inputFilename.c_str() );
  rootInputTree = (TTree *)rootInput->Get("t1065");
  rootInputTree_hodo = (TTree *)rootInput->Get("hodo");
  rootInputTree_tofpet = (TTree *)rootInput->Get("tofp");
  
  rootInputTree->SetBranchAddress("event", &event);    
  rootInputTree->SetBranchAddress("time", time);    
  rootInputTree->SetBranchAddress("raw", channel);
  //rootInputTree->SetBranchAddress("base", base);
  rootInputTree_hodo->SetBranchAddress("X", hodoX);
  rootInputTree_hodo->SetBranchAddress("Y", hodoY);
  rootInputTree_hodo->SetBranchAddress("nFibresOnX", nFibresOnX);
  rootInputTree_hodo->SetBranchAddress("nFibresOnY", nFibresOnY);

  rootInputTree->SetBranchAddress("TDCx", &TDCx);
  rootInputTree->SetBranchAddress("TDCy", &TDCy);
  
  //copy over other stuff that wont get overwritten
  rootInputTree->SetBranchAddress("tc", tc);
  

  rootInputTree_tofpet->SetBranchAddress("isMatched", &tofpet_isMatched);
  rootInputTree_tofpet->SetBranchAddress("tot", tofpet_tot);
  rootInputTree_tofpet->SetBranchAddress("energy", tofpet_energy);
  rootInputTree_tofpet->SetBranchAddress("tqT", tofpet_tqT);
  rootInputTree_tofpet->SetBranchAddress("tqE", tofpet_tqE);
  rootInputTree_tofpet->SetBranchAddress("x", tofpet_x);
  rootInputTree_tofpet->SetBranchAddress("y", tofpet_y);
  rootInputTree_tofpet->SetBranchAddress("z", tofpet_z);
  rootInputTree_tofpet->SetBranchAddress("xi", tofpet_xi);
  rootInputTree_tofpet->SetBranchAddress("yi", tofpet_yi);
  rootInputTree_tofpet->SetBranchAddress("t_h4daq", &tofpet_t_h4daq);
  rootInputTree_tofpet->SetBranchAddress("t_tofpet", tofpet_t_tofpet);

  //// Only for dat data type:


  //*************************
  //Event Loop
  //*************************
  int maxEvent = min( int(rootInputTree->GetEntries()), nEvents);
  if (nEvents < 0) maxEvent = rootInputTree->GetEntries();
  std::cout << "\nMaxEvents = " << maxEvent << "\n";
  
  std::cout << "\n=== Processing input data ===\n" << std::endl;
  for( int iEvent = 0; iEvent < maxEvent; iEvent++){ 

    if ( iEvent % 100 == 0 ) {
      std::cout << "Event " << iEvent << " of " << maxEvent << std::endl; 
    }
    
    rootInputTree->GetEntry(iEvent);
    rootInputTree_hodo->GetEntry(iEvent);
    rootInputTree_tofpet->GetEntry(iEvent);

    // get number of active groups
    int activeGroupsN = 0;
    int realGroup[4] = {-1, -1, -1, -1};
    int totalIndex;
    for (int i = 0; i < 4; i++) {
      if (time[i][1] > 0) {
	realGroup[activeGroupsN] = i;
	activeGroupsN++; 
      }
    } 
    
    int index_MCP0_Min = 0;
    int index_MCP1_Min = 0;
    //************************************
    //Loop over active groups
    //************************************
    for ( int group = 0; group < activeGroupsN; group++ ) {
      ushort tcn; 

      
      //************************************
      // Loop over channels 0-8
      //************************************      
      
      for(int i = 0; i < 9; i++) {
	
	totalIndex = realGroup[group]*9 + i;
	bool isNINO = (totalIndex >= 0 && totalIndex <= 5);

	// Do not analyze disabled channels
	if ( !config.hasChannel(totalIndex) ) {
	  for ( int j = 0; j < 1024; j++ ) {
	    raw[totalIndex][j] = 0; 
	    channel[totalIndex][j] = 0;
	  }
	  xmin[totalIndex] = 0.;
	  xminRestricted[totalIndex] = 0.;
	  amp [totalIndex] = 0.;
	  amp_inverse [totalIndex] = 0.;
	  ampRestricted [totalIndex] = 0.;
	  base[totalIndex] = 0.;
	  integral[totalIndex] = 0.;
	  integralFull[totalIndex] = 0.;
	  gauspeak[totalIndex] = 0.;
	  sigmoidTime[totalIndex] = 0.;
	  linearTime0[totalIndex] = 0.;
	  linearTime15[totalIndex] = 0.;
	  linearTime30[totalIndex] = 0.;
	  linearTime45[totalIndex] = 0.;
	  linearTime60[totalIndex] = 0.;
	  risetime[totalIndex] = 0.;
	  constantThresholdTime[totalIndex] = 0.;
	  continue;
        }
          
	///////////////////////////////////////
	//////// Starting here, dat and root file types both use all the following code 
	////////////////////////////////////////
      
	// Make pulse shape graph
	TString pulseName = Form("pulse_event%d_group%d_ch%d", iEvent, realGroup[group], i);
	TGraphErrors* pulse = new TGraphErrors( GetTGraph( channel[totalIndex], time[realGroup[group]] ) );
      
      
	//Apply HighPass Filter (clipping circuit)
	//HighPassFilter( channel[totalIndex], channelFilter[totalIndex],  time[realGroup[group]], 1000., 0.01 );

      
	// DRS-glitch finder: zero out bins which have large difference
	// with respect to neighbors in only one or two bins
	for(int j = 0; j < 1024; j++) {
	  short a0 = abs(channel[totalIndex][j-1]);
	  short a1 = abs(channel[totalIndex][j]);
	  short a2 = abs(channel[totalIndex][j+1]);
	  short a3 = abs(channel[totalIndex][j+2]);
        
	  if (j == 56 && a1 == 0 && a2 == 0) {
	    //std::cout << "PRE: " << i << " " << a[i] << " " << xmin << " " << a[i+1] << std::endl;}
	    //std::cout << "POST a value: " << *channel[totalIndex] << std::endl; 
	    if (*channel[totalIndex] == 5.) {
	      channel[totalIndex][j] = 0; }}
        
	  if ( ( a1>3*a0 && a2>3*a0 && a2>3*a3 && a1>30) )
	    {
	      channel[totalIndex][j] = 0;
	      channel[totalIndex][j+1] = 0;
	    }

	  if ( ( a1>3*a0 && a1>3*a2 && a1>30) )
	    channel[totalIndex][j] = 0;
	}
      
            
      
	// Find the absolute minimum. This is only used as a rough determination 
	// to decide if we'll use the early time samples
	// or the late time samples to do the baseline fit
	//std::cout << "---event "  << event << "-------ch#: " << totalIndex << std::endl;
	int index_min = 0;
	int index_max = 0;
	if (isNINO) {
	  index_min = DigitalFindMin(1024, channel[totalIndex]);
	} else {
	  index_min = FindMinAbsolute(1024, channel[totalIndex]); 
	  if (totalIndex == index_MCP0) index_MCP0_Min = index_min;
	  if (totalIndex == index_MCP1) index_MCP1_Min = index_min;
	  index_max = FindInverseMaxAbsolute(1024, channel[totalIndex]); 
	}
	int index_min_restricted = index_min;
	if (totalIndex > 0) {
	  index_min_restricted = FindMinAbsolute(1024, channel[totalIndex], xmin[0] , xmin[0] + 40 );	
	}
	      
	// Recreate the pulse TGraph using baseline-subtracted channel data
	delete pulse;
	pulse = new TGraphErrors( GetTGraph( channel[totalIndex], time[realGroup[group]] ) );
	xmin[totalIndex] = index_min;
	xminRestricted[totalIndex] = index_min_restricted;
	
	//std::cout << "INDEX MIN: " << index_min << std::endl;

	//if (doFilter && totalIndex == 4) {
	//	pulse = WeierstrassTransform( channel[totalIndex], time[realGroup[group]], pulseName, 2.0, false);
	//}
	float filterWidth = config.getFilterWidth(totalIndex);
	if (filterWidth) {
	  pulse = WeierstrassTransform( channel[totalIndex], time[realGroup[group]], 
					pulseName, filterWidth, false );
	}
      
      
	//std::cout << "18" << std::endl;
      
	//Compute Amplitude : use units V
	Double_t tmpAmp = 0.0;
	Double_t tmpAmp_inverse = 0.0;
	Double_t tmpTime = 0.0;
	pulse->GetPoint(index_min, tmpTime, tmpAmp);
	pulse->GetPoint(index_max, tmpTime, tmpAmp_inverse);
	amp[totalIndex] = tmpAmp * (1.0 / 4096.0); 
	amp_inverse[totalIndex] = tmpAmp_inverse * (1.0 / 4096.0); 
	pulse->GetPoint(index_min_restricted, tmpTime, tmpAmp);
	ampRestricted[totalIndex] = tmpAmp * (1.0 / 4096.0); 

	// Get pulse integral
	if ( xmin[totalIndex] != 0 ) {
	  integral[totalIndex] = GetPulseIntegral( index_min , channel[totalIndex], "", 20, 40 );
	  integralFull[totalIndex] = GetPulseIntegral( index_min , channel[totalIndex], "full");
	}
	else {
	  integral[totalIndex] = 0.0;
	  integralFull[totalIndex] = 0.0;
	}
      
	//std::cout << "19" << std::endl;
      
	// Gaussian time stamp and constant-fraction fit
	Double_t min = 0.; Double_t low_edge = 0.; Double_t high_edge = 0.; Double_t y = 0.; 
	pulse->GetPoint(index_min, min, y);	
	pulse->GetPoint(index_min-4, low_edge, y); // time of the low edge of the fit range
	pulse->GetPoint(index_min+4, high_edge, y);  // time of the upper edge of the fit range	

	float timepeak   = 0;
	bool isTrigChannel = false;

	float fs[5]; // constant-fraction fit output
	float risetime_tmp;
	float fs_falling[6]; // falling exp timestapms
	float cft_low_range  = 0.30;
	float cft_high_range = 0.70;
	if ( !isTrigChannel ) {
	  if( drawDebugPulses ) {
	    if ( xmin[totalIndex] != 0.0 ) {
	      if (isNINO) {
		DigitalRisingEdgeFitTime( pulse, index_min, fs, risetime_tmp, event, "digitalFit_" + pulseName, true);
	      } else {
		timepeak =  GausFit_MeanTime(pulse, low_edge, high_edge, pulseName); 
		RisingEdgeFitTime( pulse, index_min, cft_low_range, cft_high_range, fs, risetime_tmp, event, "linearFit_" + pulseName, true );
		//TailFitTime( pulse, index_min, fs_falling, event, "expoFit_" + pulseName, true );
		//sigmoidTime[totalIndex] = SigmoidTimeFit( pulse, index_min, event, "linearFit_" + pulseName, true );
		//fullFitTime[totalIndex] = FullFitScint( pulse, index_min, event, "fullFit_" + pulseName, true );
	      }
	    }
	  }
	  else {
	    if ( xmin[totalIndex] != 0.0 ) {
	      if (isNINO) {
		DigitalRisingEdgeFitTime( pulse, index_min, fs, risetime_tmp, event, "digitalFit_" + pulseName, false);
	      } else {
		timepeak =  GausFit_MeanTime(pulse, low_edge, high_edge); 
		RisingEdgeFitTime( pulse, index_min, cft_low_range, cft_high_range, fs, risetime_tmp, event, "linearFit_" + pulseName, false );
		//TailFitTime( pulse, index_min, fs_falling, event, "expoFit_" + pulseName, false );
		//sigmoidTime[totalIndex] = SigmoidTimeFit( pulse, index_min, event, "linearFit_" + pulseName, false );
		//fullFitTime[totalIndex] = FullFitScint( pulse, index_min, event, "fullFit_" + pulseName, false );
	      }
	    }
	  }
	}

	else {
	  for ( int kk = 0; kk < 5; kk++ ) {
	    fs[kk] = -999;
	    fs_falling[kk] = -999;
	  }
	}

	_isRinging[totalIndex] = isRinging( index_min, channel[totalIndex] );
    	// if( isRinging( index_min, channel[totalIndex] ) )
	//   {
	//     std::cout << "event = " << event  << " channel = " << totalIndex << std::endl;
	//   };
	//std::cout << "20" << std::endl;
			
	// for output tree
	gauspeak[totalIndex] = timepeak;
	//risetime is already set implicitly above
	risetime[totalIndex] = risetime_tmp;
	linearTime0[totalIndex] = fs[0];
	linearTime15[totalIndex] = fs[1];
	linearTime30[totalIndex] = fs[2];
	linearTime45[totalIndex] = fs[3];
	linearTime60[totalIndex] = fs[4];
	fallingTime[totalIndex] = fs_falling[0];
	if (isNINO) {
	  constantThresholdTime[totalIndex] = ConstantThresholdTime( pulse, 300);
	} else {
	  constantThresholdTime[totalIndex] = ConstantThresholdTime( pulse, 50);
	}

	delete pulse;
      } //loop over channels per group    
    } //loop over active groups

    // reconstruct the clock time relative to MCPs

    TGraphErrors* pulse_CLOCK = new TGraphErrors( GetTGraph( channel[index_CLOCK], time[realGroup[0]] ) );	

    int index_max_MCP0 = 0; Double_t low_edge_MCP0 = 0.; Double_t high_edge_MCP0 = 0.; Double_t y_MCP0 = 0.;
    index_max_MCP0 = FindLeftMax(pulse_CLOCK, index_MCP0_Min, CLOCK_CYCLE);
    pulse_CLOCK->GetPoint(index_max_MCP0-4, low_edge_MCP0, y_MCP0); // time of the low edge of the fit range
    pulse_CLOCK->GetPoint(index_max_MCP0+4, high_edge_MCP0, y_MCP0);  // time of the upper edge of the fit range 
 
    int index_max_MCP1 = 0; Double_t low_edge_MCP1 = 0.; Double_t high_edge_MCP1 = 0.; Double_t y_MCP1 = 0.;
    index_max_MCP1 = FindLeftMax(pulse_CLOCK, index_MCP1_Min, CLOCK_CYCLE);
    pulse_CLOCK->GetPoint(index_max_MCP1-4, low_edge_MCP1, y_MCP1); // time of the low edge of the fit range
    pulse_CLOCK->GetPoint(index_max_MCP1+4, high_edge_MCP1, y_MCP1);  // time of the upper edge of the fit range 

    int index_min_MCP0 = 0;
    index_min_MCP0 = FindLeftMin(pulse_CLOCK, index_MCP0_Min, CLOCK_CYCLE);
 
    int index_min_MCP1 = 0; 
    index_min_MCP1 = FindLeftMin(pulse_CLOCK, index_MCP1_Min, CLOCK_CYCLE);
    
    TString pulseName = Form("pulse_event%d_group%d_ch%d", iEvent, 0, index_CLOCK);
    float fs[5];
    float risetime_tmp;
    float cft_low_range  = 0.10;
    float cft_high_range = 0.80;
    
    MCPtoClockTime_gauspeak[0] =  GausFit_MeanTime(pulse_CLOCK, low_edge_MCP0, high_edge_MCP0);
    MCPtoClockTime_gauspeak[1] =  GausFit_MeanTime(pulse_CLOCK, low_edge_MCP1, high_edge_MCP1);

    RisingEdgeFitTimeCLOCK( pulse_CLOCK, index_max_MCP0, index_min_MCP0, cft_low_range, cft_high_range, fs, risetime_tmp, event, "linearFit_" + pulseName, false, CLOCK_CYCLE );
    MCPtoClockTime_linearTime0[0] = fs[0];
    MCPtoClockTime_linearTime15[0] = fs[1];
    MCPtoClockTime_linearTime30[0] = fs[2];
    MCPtoClockTime_linearTime45[0] = fs[3];
    MCPtoClockTime_linearTime60[0] = fs[4];

    RisingEdgeFitTimeCLOCK( pulse_CLOCK, index_max_MCP1, index_min_MCP0, cft_low_range, cft_high_range, fs, risetime_tmp, event, "linearFit_" + pulseName, false, CLOCK_CYCLE );
    MCPtoClockTime_linearTime0[1] = fs[0];
    MCPtoClockTime_linearTime15[1] = fs[1];
    MCPtoClockTime_linearTime30[1] = fs[2];
    MCPtoClockTime_linearTime45[1] = fs[3];
    MCPtoClockTime_linearTime60[1] = fs[4];

//    if(MCPtoClockTime_gauspeak[0]<10) cout<<"clock reco for MCP 0 failed: "<<event<<endl;
//    if(MCPtoClockTime_gauspeak[1]<10) cout<<"clock reco for MCP 1 failed: "<<event<<endl;
  
    tree->Fill();
    nGoodEvents++;
  }

  //std::cout << "21" << std::endl;

  rootInput->Close();
  
  cout << "\nProcessed total of " << nGoodEvents << " events\n";

  file->Write();
  file->Close();

  return 0;
}



int graphic_init(){

  style = new TStyle("style", "style");

  style->SetLabelFont(132,"X");
  style->SetLabelFont(132,"Y");
  style->SetTitleFont(132,"X");
  style->SetTitleFont(132,"Y");
  style->SetTitleFont(132,"");
  style->SetTitleFontSize( 0.07);
  style->SetStatFont(132);
  style->GetAttDate()->SetTextFont(132);
  style->SetStatW(0.20);
  style->SetStatH(0.23);
  style->SetFuncColor(2);
  style->SetFuncWidth(2);
  style->SetLineWidth(2);
  style->SetOptFile(0);
  style->SetOptTitle(1);
  style->SetFrameBorderMode(0);
  style->SetCanvasBorderMode(0);
  style->SetPadBorderMode(0);
  style->SetTitleStyle(4000);
  style->SetPadColor(0);
  style->SetCanvasColor(0);
  style->SetTitleFillColor(0);
  style->SetTitleBorderSize(0);
  style->SetStatColor(0);
  style->SetStatBorderSize(1);
  style->SetOptStat("emri");
  style->SetOptFit(1);
  style->SetTitleOffset( 1.0,"Y");
  style->SetMarkerStyle(20);
  style->SetMarkerSize( 0.3);
  style->SetMarkerColor(4);

  style->cd();

  return 0;
}

