void drawSpectra(const std::string& inFileName)
{
    gStyle -> SetOptStat(0);
    
    TFile* inFile = TFile::Open(inFileName.c_str(),"READ");
    TTree* t = (TTree*)( inFile->Get("data") );
    
    int chIds[]  = { 31, 23, 19, 26, 14, 7,  9, 32, 40, 63, 56, 34, 33, 43, 41 };
    int colors[] = { 54, 57, 60, 64, 67, 8, 80, 91, 93, 97,  2, 18, 16, 13,  1 };
    std::map<int,TH1F*> histos;
    
    TLegend* legend = new TLegend(0.70,0.40,0.99,0.99);
    legend -> SetFillColor(kWhite);
    legend -> SetFillStyle(0);
    
    float max = -999.;
    for(int ii = 0; ii < 16; ++ii)
    {
        histos[ii] = new TH1F(Form("histo_%d",chIds[ii]),"",128,0,512);
        histos[ii] -> SetLineWidth(3);
        histos[ii] -> SetLineColor(colors[ii]);
        
        if( ii == 0)
            t -> Draw(Form("energy >>histo_%d",chIds[ii]),Form("channelID==%d",chIds[ii]),"");
        else
            t -> Draw(Form("energy >>histo_%d",chIds[ii]),Form("channelID==%d",chIds[ii]),"");
        
        if( histos[ii]->GetMaximum() > max ) max = histos[ii]->GetMaximum();
        
        legend -> AddEntry(histos[ii],Form("chId %d",chIds[ii]),"L");
    }
    
    for(int ii = 0; ii < 16; ++ii)
    {
        if( ii == 0 )
        {
            histos[ii] -> GetYaxis() -> SetRangeUser(0.,1.1*max);
            histos[ii] -> Draw();
        }
        else
        {
            histos[ii] -> Draw("same");
        }
    }
    
    legend -> Draw("same");
}
