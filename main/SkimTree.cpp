#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <assert.h>
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TKey.h"
#include "TDirectoryFile.h"

using namespace std;


//get list of files to open, add normalization branch to the tree in each file
int main(int argc, char* argv[]) {

    //parse input list to get names of ROOT files
    if(argc < 4){
        cerr << "usage SkimNtuple inputList.txt <outputDirectory> <outputfileLabel>" << endl;
        return -1;
    }
    string inputList(argv[1]);
    
    string outputDir = argv[2];
    string outputfileLabel = argv[3];

    ifstream filein(inputList.c_str());
    string curFilename;
    vector<string> inputLines;
    while(getline(filein, curFilename)){
        if(curFilename.at(0) != '#') inputLines.push_back(curFilename); //'#' denotes a comment
        else cout << "(Skipping commented line in input)" << endl;
    }

    //open each ROOT file and add the normalization branch
    for(auto& line : inputLines){
        //parse input -- input lines should be in the form datasetName fileName
        istringstream buf(line);
        istream_iterator<std::string> beg(buf), end;
        vector<std::string> inputs(beg, end);
        
        string fileName = inputs[0];

        //create output file
	string outputfilename = Form("%s/%s_%s.root", outputDir.c_str(), 
				     fileName.substr(fileName.find_last_of("/")+1, fileName.find_last_of(".")-fileName.find_last_of("/")-1).c_str(),
				     outputfileLabel.c_str());
	cout << "Output file: " << outputfilename << "\n";
        TFile *outputFile = new TFile(outputfilename.c_str(), "RECREATE");
	
        //loop over all TTrees in the file and add the weight branch to each of them
        TFile *inputFile = new TFile(fileName.c_str(), "READ");
        assert(inputFile);
        inputFile->cd();
 
	TTree *inputTree = (TTree*)inputFile->Get("t1065");
	cout << "Processing tree " << inputTree->GetName() << endl;

	//create new normalized tree
	outputFile->cd();
	TTree *outputTree = inputTree->CloneTree(0);  
	cout << "Events in the ntuple: " << inputTree->GetEntries() << endl;
	
	int EventsPassed = 0;
	
	float amp[36];
	float integral[36];
	bool isRinging[36];
	float x1;
	    
	inputTree->SetBranchAddress("amp",amp);	    
	inputTree->SetBranchAddress("integral",integral);
	inputTree->SetBranchAddress("x1",&x1);
	inputTree->SetBranchAddress("isRinging",&isRinging);
	    
	//store the weights
	for (int n=0;n<inputTree->GetEntries();n++) { 
	  if (n%100==0) cout << "Processed Event " << n << "\n";
	  inputTree->GetEntry(n);

	  bool passSkim = false;
	  passSkim = (  amp[0] > 0.05 && (amp[1] > 0.05 || amp[2] > 0.05 ||amp[3] > 0.05 ||amp[4] > 0.05 ||amp[5] > 0.05 ||amp[6] > 0.05 ) );
 
	  if (passSkim) {
	    EventsPassed++;
	    outputTree->Fill(); 
	  }
	}

	cout << "Skim Efficiency : " << EventsPassed << " / " << inputTree->GetEntries() 
	     << " = " << float(EventsPassed ) / float(inputTree->GetEntries()) 
	     << " \n";

	//save
	outputTree->Write();
	inputFile->cd();

        inputFile->Close();
        cout << "Closing output file." << endl;

        outputFile->Close();
        delete outputFile;
    }
}
