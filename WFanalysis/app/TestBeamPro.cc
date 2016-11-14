//C++ INCLUDES
#include <iostream>
//ROOT INCLUDES
#include <TFile.h>
#include <TTree.h>
//LOCAL INCLUDES
#include "TimeReso.hh"
#include "digiSettings.hh"


int main( int argc, char* argv[])
{
	std::string inputfile = argv[1];

	if(inputfile == "")
	{
	  std::cerr << "[ERROR]: please provide an input file " << std::endl;
          return -1;
	}
	std::cout<<"using ntuple: "<<inputfile<<std::endl;

	//std::cout<<getTimeReso(inputfile, 16, 17)<<std::endl;
	return 0;
}
