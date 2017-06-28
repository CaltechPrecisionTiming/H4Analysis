import ROOT
import array
import itertools
import os

ROOT.gROOT.SetBatch(True)

ROOT.gSystem.Load("lib/H4Dict.so")
ROOT.gSystem.Load("CfgManager/lib/CfgManagerDict.so")

ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptFit(111)

from optparse import OptionParser
parser = OptionParser()

parser.add_option("-r","--run")
parser.add_option("-o","--outputDir")
parser.add_option("-i","--inputDir")
parser.add_option("-c","--channel")
parser.add_option("-f","--channelRef")


(options,args)=parser.parse_args()


channel=int(options.channel)
channelRef=int(options.channelRef)


os.system("mkdir -p "+options.outputDir+"/"+options.run)
os.system("cp "+options.outputDir+"/index.* "+options.outputDir+"/"+options.run)


file=ROOT.TFile(options.inputDir+"/"+options.run+".root")
tree=file.Get("t1065")

NEntries=tree.GetEntries()

print "\n"
print "plotting from file: "+options.inputDir+"/"+options.run+".root"
print "\n"
print "Num of events in tree t1065: "+str(NEntries)


myC = ROOT.TCanvas("myC","myC",100,100,800,700);

maxAmpRef=0
h_amp_Ref=ROOT.TH1F("h_amp_Ref","h_amp_Ref",100,0,1.0)

tree.Draw("amp["+options.channelRef+"]>>h_amp_Ref")
maxAmpRef=h_amp_Ref.GetBinCenter(h_amp_Ref.FindLastBinAbove(0))
h_amp_Ref.GetXaxis().SetRangeUser(0.0,maxAmpRef*1.1)

h_amp_Ref.GetXaxis().SetTitle("Photek Amplitude [V]")
h_amp_Ref.GetYaxis().SetTitle("Events")
h_amp_Ref.SetTitle("")

h_amp_Ref.GetXaxis().SetTitleOffset(0.9)
h_amp_Ref.GetXaxis().SetTitleSize(0.05)
h_amp_Ref.GetXaxis().SetLabelSize(0.042)
h_amp_Ref.GetXaxis().SetLabelOffset(0.0)

h_amp_Ref.GetYaxis().SetTitleOffset(1.7)
h_amp_Ref.GetYaxis().SetTitleSize(0.03)
h_amp_Ref.GetYaxis().SetLabelSize(0.032)
h_amp_Ref.GetYaxis().SetLabelOffset(0.0)



print "\n maxAmpRef = "+str(maxAmpRef)

h_amp_Ref.Fit("gaus","","",0.2*maxAmpRef,0.8*maxAmpRef)

myC.Print(options.outputDir+"/"+options.run+"/Photek_Ch"+options.channelRef+"_Amp.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Photek_Ch"+options.channelRef+"_Amp.png")
myC.Print(options.outputDir+"/"+options.run+"/Photek_Ch"+options.channelRef+"_Amp.C")

cut="amp["+options.channelRef+"]> "+str(0.25*maxAmpRef)+"&& amp["+options.channelRef+"]< "+str(0.95*maxAmpRef)


h2_amp_X=ROOT.TH2F("h2_amp_X","h2_amp_X",70,-35,35,100,0,0.6)
h2_amp_X2=ROOT.TH2F("h2_amp_X2","h2_amp_X2",30,-15,15,100,0,0.6)
tree.Draw("amp["+options.channel+"]:TDCx>>h2_amp_X",cut,"COLZ")
tree.Draw("amp["+options.channel+"]:TDCx>>h2_amp_X2",cut,"COLZ")
h1_amp_X=h2_amp_X.ProfileX()
h1_amp_X2=h2_amp_X2.ProfileX()
h1_amp_X.Draw()
h1_amp_X.GetXaxis().SetTitle("Horizontal Beam Position [mm]")
h1_amp_X.GetYaxis().SetTitle("Sensor Response [V]")
h1_amp_X.SetTitle("")
h1_amp_X.GetXaxis().SetTitleOffset(0.9)
h1_amp_X.GetXaxis().SetTitleSize(0.05)
h1_amp_X.GetXaxis().SetLabelSize(0.042)
h1_amp_X.GetXaxis().SetLabelOffset(0.0)
h1_amp_X.GetYaxis().SetTitleOffset(0.9)
h1_amp_X.GetYaxis().SetTitleSize(0.05)
h1_amp_X.GetYaxis().SetLabelSize(0.032)
h1_amp_X.GetYaxis().SetLabelOffset(0.0)
maximumX=h1_amp_X2.GetBinCenter(h1_amp_X2.GetMaximumBin())
h1_amp_X.GetXaxis().SetRangeUser(maximumX-15,maximumX+15)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_ProfileX.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_ProfileX.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_ProfileX.C")

h2_amp_Y=ROOT.TH2F("h2_amp_Y","h2_amp_Y",70,-35,35,100,0,0.6)
h2_amp_Y2=ROOT.TH2F("h2_amp_Y2","h2_amp_Y2",30,-15,15,100,0,0.6)
tree.Draw("amp["+options.channel+"]:TDCy>>h2_amp_Y",cut,"COLZ")
tree.Draw("amp["+options.channel+"]:TDCy>>h2_amp_Y2",cut,"COLZ")
h1_amp_Y=h2_amp_Y.ProfileX()
h1_amp_Y2=h2_amp_Y2.ProfileX()
h1_amp_Y.Draw()
h1_amp_Y.GetXaxis().SetTitle("Vertical Beam Position [mm]")
h1_amp_Y.GetYaxis().SetTitle("Sensor Response [V]")
h1_amp_Y.SetTitle("")
h1_amp_Y.GetXaxis().SetTitleOffset(0.9)
h1_amp_Y.GetXaxis().SetTitleSize(0.05)
h1_amp_Y.GetXaxis().SetLabelSize(0.042)
h1_amp_Y.GetXaxis().SetLabelOffset(0.0)
h1_amp_Y.GetYaxis().SetTitleOffset(0.9)
h1_amp_Y.GetYaxis().SetTitleSize(0.05)
h1_amp_Y.GetYaxis().SetLabelSize(0.032)
h1_amp_Y.GetYaxis().SetLabelOffset(0.0)
maximumY=h1_amp_Y2.GetBinCenter(h1_amp_Y2.GetMaximumBin())
h1_amp_Y.GetXaxis().SetRangeUser(maximumY-15,maximumY+15)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_ProfileY.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_ProfileY.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_ProfileY.C")


cut=cut+" && TDCx>"+str(maximumX-7.5)+" && TDCx<"+str(maximumX+7.5)+" && TDCy>"+str(maximumY-7.5)+" && TDCy<"+str(maximumY+7.5)


print "\n cut -->  "+cut


maxAmp=0
h_amp=ROOT.TH1F("h_amp","h_amp",200,0,1.0)
h_amp.GetXaxis().SetTitle("Amplitude [V]")
h_amp.GetYaxis().SetTitle("Events")
h_amp.SetTitle("")

tree.Draw("amp["+options.channel+"]>>h_amp", cut)
maxAmp=h_amp.GetBinCenter(h_amp.FindLastBinAbove(0))
h_amp.GetXaxis().SetRangeUser(0.0,maxAmp*1.1)

h_amp.GetXaxis().SetTitleOffset(0.9)
h_amp.GetXaxis().SetTitleSize(0.05)
h_amp.GetXaxis().SetLabelSize(0.042)
h_amp.GetXaxis().SetLabelOffset(0.0)

h_amp.GetYaxis().SetTitleOffset(1.7)
h_amp.GetYaxis().SetTitleSize(0.03)
h_amp.GetYaxis().SetLabelSize(0.032)
h_amp.GetYaxis().SetLabelOffset(0.0)



print "\n maxAmp = "+str(maxAmp)

h_amp.Fit("gaus","","",0.02*maxAmp,0.25*maxAmp)

myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_Amp.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_Amp.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_Amp.C")

maxRiseTime=0
h_risetime=ROOT.TH1F("h_risetime","h_risetime",100,0,3.0)
h_risetime.GetXaxis().SetTitle("risetime [ns]")
h_risetime.GetYaxis().SetTitle("Events")
h_risetime.SetTitle("")

tree.Draw("risetime["+options.channel+"]>>h_risetime", cut)
maxRiseTime=h_risetime.GetBinCenter(h_risetime.FindLastBinAbove(0))
#h_risetime.GetXaxis().SetRangeUser(0.0,maxRiseTime*1.1)

h_risetime.GetXaxis().SetTitleOffset(0.9)
h_risetime.GetXaxis().SetTitleSize(0.05)
h_risetime.GetXaxis().SetLabelSize(0.042)
h_risetime.GetXaxis().SetLabelOffset(0.0)

h_risetime.GetYaxis().SetTitleOffset(1.7)
h_risetime.GetYaxis().SetTitleSize(0.03)
h_risetime.GetYaxis().SetLabelSize(0.032)
h_risetime.GetYaxis().SetLabelOffset(0.0)


print "\n maxRiseTime = "+str(maxRiseTime)

h_risetime.Fit("gaus","","",0.1*maxRiseTime,0.35*maxRiseTime)

myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_risetime.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_risetime.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_risetime.C")

maxDeltaT30=0
minDeltaT30=999

h_deltaT30=ROOT.TH1F("h_deltaT30","h_deltaT30",5000,0,100)
h_deltaT30.GetXaxis().SetTitle("#Delta t [ns]")
h_deltaT30.GetYaxis().SetTitle("Events")
h_deltaT30.SetTitle("")

tree.Draw("linearTime30["+options.channel+"]-linearTime30["+options.channelRef+"]>>h_deltaT30", cut)

maximumY=h_deltaT30.GetMaximum()

high80DeltaT30=h_deltaT30.GetBinCenter(h_deltaT30.FindLastBinAbove(int(0.8*maximumY)))
low80DeltaT30=h_deltaT30.GetBinCenter(h_deltaT30.FindFirstBinAbove(int(0.8*maximumY)))

widthDeltaT30 = high80DeltaT30 - low80DeltaT30
maxDeltaT30 = high80DeltaT30+8.0*widthDeltaT30
minDeltaT30 = low80DeltaT30-8.0*widthDeltaT30

h_deltaT30.GetXaxis().SetRangeUser(minDeltaT30,maxDeltaT30)


h_deltaT30.GetXaxis().SetTitleOffset(0.9)
h_deltaT30.GetXaxis().SetTitleSize(0.05)
h_deltaT30.GetXaxis().SetLabelSize(0.042)
h_deltaT30.GetXaxis().SetLabelOffset(0.0)

h_deltaT30.GetYaxis().SetTitleOffset(1.7)
h_deltaT30.GetYaxis().SetTitleSize(0.03)
h_deltaT30.GetYaxis().SetLabelSize(0.032)
h_deltaT30.GetYaxis().SetLabelOffset(0.0)



h_deltaT30.Fit("gaus","","",low80DeltaT30-2.0*widthDeltaT30,high80DeltaT30+2.0*widthDeltaT30)

myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT30.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT30.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT30.C")

maxDeltaT45=0
minDeltaT45=999

h_deltaT45=ROOT.TH1F("h_deltaT45","h_deltaT45",5000,0,100)
h_deltaT45.GetXaxis().SetTitle("#Delta t [ns]")
h_deltaT45.GetYaxis().SetTitle("Events")
h_deltaT45.SetTitle("")

tree.Draw("linearTime45["+options.channel+"]-linearTime45["+options.channelRef+"]>>h_deltaT45", cut)

maximumY=h_deltaT45.GetMaximum()

high80DeltaT45=h_deltaT45.GetBinCenter(h_deltaT45.FindLastBinAbove(int(0.8*maximumY)))
low80DeltaT45=h_deltaT45.GetBinCenter(h_deltaT45.FindFirstBinAbove(int(0.8*maximumY)))

widthDeltaT45 = high80DeltaT45 - low80DeltaT45
maxDeltaT45 = high80DeltaT45+8.0*widthDeltaT45
minDeltaT45 = low80DeltaT45-8.0*widthDeltaT45

h_deltaT45.GetXaxis().SetRangeUser(minDeltaT45,maxDeltaT45)

h_deltaT45.GetXaxis().SetTitleOffset(0.9)
h_deltaT45.GetXaxis().SetTitleSize(0.05)
h_deltaT45.GetXaxis().SetLabelSize(0.042)
h_deltaT45.GetXaxis().SetLabelOffset(0.0)

h_deltaT45.GetYaxis().SetTitleOffset(1.7)
h_deltaT45.GetYaxis().SetTitleSize(0.03)
h_deltaT45.GetYaxis().SetLabelSize(0.032)
h_deltaT45.GetYaxis().SetLabelOffset(0.0)



h_deltaT45.Fit("gaus","","",low80DeltaT45-2.0*widthDeltaT45,high80DeltaT45+2.0*widthDeltaT45)

myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT45.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT45.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT45.C")



maxDeltaT60=0
minDeltaT60=999

h_deltaT60=ROOT.TH1F("h_deltaT60","h_deltaT60",5000,0,100)
h_deltaT60.GetXaxis().SetTitle("#Delta t [ns]")
h_deltaT60.GetYaxis().SetTitle("Events")
h_deltaT60.SetTitle("")

tree.Draw("linearTime60["+options.channel+"]-linearTime60["+options.channelRef+"]>>h_deltaT60", cut)

maximumY=h_deltaT60.GetMaximum()

high80DeltaT60=h_deltaT60.GetBinCenter(h_deltaT60.FindLastBinAbove(int(0.8*maximumY)))
low80DeltaT60=h_deltaT60.GetBinCenter(h_deltaT60.FindFirstBinAbove(int(0.8*maximumY)))

widthDeltaT60 = high80DeltaT60 - low80DeltaT60
maxDeltaT60 = high80DeltaT60+8.0*widthDeltaT60
minDeltaT60 = low80DeltaT60-8.0*widthDeltaT60

h_deltaT60.GetXaxis().SetRangeUser(minDeltaT60,maxDeltaT60)

h_deltaT60.GetXaxis().SetTitleOffset(0.9)
h_deltaT60.GetXaxis().SetTitleSize(0.05)
h_deltaT60.GetXaxis().SetLabelSize(0.042)
h_deltaT60.GetXaxis().SetLabelOffset(0.0)

h_deltaT60.GetYaxis().SetTitleOffset(1.7)
h_deltaT60.GetYaxis().SetTitleSize(0.03)
h_deltaT60.GetYaxis().SetLabelSize(0.032)
h_deltaT60.GetYaxis().SetLabelOffset(0.0)


h_deltaT60.Fit("gaus","","",low80DeltaT60-2.0*widthDeltaT60,high80DeltaT60+2.0*widthDeltaT60)

myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT60.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT60.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT60.C")

