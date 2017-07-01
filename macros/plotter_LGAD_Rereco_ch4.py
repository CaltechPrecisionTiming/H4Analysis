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
tree=file.Get("pulse")

NEntries=tree.GetEntries()

print "\n"
print "plotting from file: "+options.inputDir+"/"+options.run+".root"
print "\n"
print "Num of events in tree pulse: "+str(NEntries)


myC = ROOT.TCanvas("myC","myC",10-50,50,800,700);

maxAmpRef=0
h_amp_Ref=ROOT.TH1F("h_amp_Ref","h_amp_Ref",100,0.04,1.0)
tree.Draw("amp["+options.channelRef+"]>>h_amp_Ref")
maxAmpRef=h_amp_Ref.GetBinCenter(h_amp_Ref.FindLastBinAbove(0))
maximumY=h_amp_Ref.GetMaximum()
high50_amp_Ref=h_amp_Ref.GetBinCenter(h_amp_Ref.FindLastBinAbove(int(0.5*maximumY)))
low50_amp_Ref=h_amp_Ref.GetBinCenter(h_amp_Ref.FindFirstBinAbove(int(0.5*maximumY)))
width_amp_Ref = high50_amp_Ref - low50_amp_Ref
max_amp_Ref = high50_amp_Ref+1.0*width_amp_Ref
min_amp_Ref = (low50_amp_Ref-1.0*width_amp_Ref) if (low50_amp_Ref-1.0*width_amp_Ref)>0.0 else 0.04 
print "amp_Ref range: "+str(min_amp_Ref) +" -> " + str(max_amp_Ref)
#h_amp_Ref.GetXaxis().SetRangeUser(min_amp_Ref,max_amp_Ref)
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
h_amp_Ref.Fit("gaus","","", low50_amp_Ref-0.2*width_amp_Ref, high50_amp_Ref+0.2*width_amp_Ref)
myC.Print(options.outputDir+"/"+options.run+"/Photek_Ch"+options.channelRef+"_Amp.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Photek_Ch"+options.channelRef+"_Amp.png")
myC.Print(options.outputDir+"/"+options.run+"/Photek_Ch"+options.channelRef+"_Amp.C")


cut="amp["+options.channelRef+"]> "+str(min_amp_Ref)+"&& amp["+options.channelRef+"]< "+str(max_amp_Ref) + "&& risetime["+options.channel+"] > 1.15 && risetime["+options.channel+"] < 1.5"
cutrisetime="amp["+options.channelRef+"]> "+str(min_amp_Ref)+"&& amp["+options.channelRef+"]< "+str(max_amp_Ref) 
print "\n cut -->  "+cut


maxAmp=0
h_amp=ROOT.TH1F("h_amp","h_amp",100,0.006,0.2)
h_amp.GetXaxis().SetTitle("Amplitude [V]")
h_amp.GetYaxis().SetTitle("Events")
h_amp.SetTitle("")
tree.Draw("amp["+options.channel+"]>>h_amp", "amp[5]>0.01")
maxAmp=h_amp.GetBinCenter(h_amp.FindLastBinAbove(0))
maximumY=h_amp.GetMaximum()
high50_amp=h_amp.GetBinCenter(h_amp.FindLastBinAbove(int(0.5*maximumY)))
low50_amp=h_amp.GetBinCenter(h_amp.FindFirstBinAbove(int(0.5*maximumY)))
width_amp = high50_amp - low50_amp
max_amp = high50_amp+1.0*width_amp
min_amp = (low50_amp-1.0*width_amp) if (low50_amp-1.0*width_amp)>0.0 else 0.006
print "amp range: "+str(min_amp) +" -> " + str(max_amp)
#h_amp.GetXaxis().SetRangeUser(min_amp,max_amp)
min_amp = 0.02
min_amp_fit = 0.02
max_amp = 0.5
max_amp_fit = 0.2
h_amp.GetXaxis().SetTitleOffset(0.9)
h_amp.GetXaxis().SetTitleSize(0.05)
h_amp.GetXaxis().SetLabelSize(0.042)
h_amp.GetXaxis().SetLabelOffset(0.0)
h_amp.GetYaxis().SetTitleOffset(1.7)
h_amp.GetYaxis().SetTitleSize(0.03)
h_amp.GetYaxis().SetLabelSize(0.032)
h_amp.GetYaxis().SetLabelOffset(0.0)
h_amp.Fit("gaus","","",low50_amp-0.2*width_amp, high50_amp+0.2*width_amp)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_Amp.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_Amp.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_Amp.C")



amp_inverseamp_h2=ROOT.TH2F("amp_inverseamp_h2","amp_inverseamp_h2",100,0.006,0.6,100,0.0,0.2)
tree.Draw("-1.0*amp_inverse["+options.channel+"]:amp["+options.channel+"]>>amp_inverseamp_h2", "amp[5]>0","COLZ")
amp_inverseamp_h2.GetXaxis().SetTitle("Amplitude [V]")
amp_inverseamp_h2.GetYaxis().SetTitle("Ringing Amplitude")
amp_inverseamp_h2.SetTitle("")
amp_inverseamp_h2.GetXaxis().SetTitleOffset(0.9)
amp_inverseamp_h2.GetXaxis().SetTitleSize(0.05)
amp_inverseamp_h2.GetXaxis().SetLabelSize(0.042)
amp_inverseamp_h2.GetXaxis().SetLabelOffset(0.0)
amp_inverseamp_h2.GetYaxis().SetTitleOffset(1.7)
amp_inverseamp_h2.GetYaxis().SetTitleSize(0.03)
amp_inverseamp_h2.GetYaxis().SetLabelSize(0.032)
amp_inverseamp_h2.GetYaxis().SetLabelOffset(0.0)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_amp_inverseamp_h2.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_amp_inverseamp_h2.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_amp_inverseamp_h2.C")


amp_inverseamp_h1=ROOT.TH1F("amp_inverseamp_h1","amp_inverseamp_h1",100,0,1.0)
tree.Draw("-1.0*amp_inverse["+options.channel+"]/amp["+options.channel+"]>>amp_inverseamp_h1", "amp[5]>0")
amp_inverseamp_h1.GetXaxis().SetTitle("Ringing Amplitude / Amplitude")
amp_inverseamp_h1.GetYaxis().SetTitle("Events")
amp_inverseamp_h1.SetTitle("")
amp_inverseamp_h1.GetXaxis().SetTitleOffset(0.9)
amp_inverseamp_h1.GetXaxis().SetTitleSize(0.05)
amp_inverseamp_h1.GetXaxis().SetLabelSize(0.042)
amp_inverseamp_h1.GetXaxis().SetLabelOffset(0.0)
amp_inverseamp_h1.GetYaxis().SetTitleOffset(1.7)
amp_inverseamp_h1.GetYaxis().SetTitleSize(0.03)
amp_inverseamp_h1.GetYaxis().SetLabelSize(0.032)
amp_inverseamp_h1.GetYaxis().SetLabelOffset(0.0)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_amp_inverseamp_h1.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_amp_inverseamp_h1.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_amp_inverseamp_h1.C")


maxAmp=0
h_amp_good=ROOT.TH1F("h_amp_good","h_amp_good",100,0.006,0.2)
h_amp_good.GetXaxis().SetTitle("Amplitude [V]")
h_amp_good.GetYaxis().SetTitle("Events")
h_amp_good.SetTitle("")
tree.Draw("amp["+options.channel+"]>>h_amp_good", "amp_inverse[5]/amp[5]>-0.7")
maxAmp=h_amp_good.GetBinCenter(h_amp_good.FindLastBinAbove(0))
maximumY=h_amp_good.GetMaximum()
high50_amp=h_amp_good.GetBinCenter(h_amp_good.FindLastBinAbove(int(0.5*maximumY)))
low50_amp=h_amp_good.GetBinCenter(h_amp_good.FindFirstBinAbove(int(0.5*maximumY)))
width_amp_good = high50_amp - low50_amp
print "amp range: "+str(min_amp) +" -> " + str(max_amp)
#h_amp_good.GetXaxis().SetRangeUser(min_amp,max_amp)
h_amp_good.GetXaxis().SetTitleOffset(0.9)
h_amp_good.GetXaxis().SetTitleSize(0.05)
h_amp_good.GetXaxis().SetLabelSize(0.042)
h_amp_good.GetXaxis().SetLabelOffset(0.0)
h_amp_good.GetYaxis().SetTitleOffset(1.7)
h_amp_good.GetYaxis().SetTitleSize(0.03)
h_amp_good.GetYaxis().SetLabelSize(0.032)
h_amp_good.GetYaxis().SetLabelOffset(0.0)
h_amp_good.Fit("gaus","","",low50_amp-0.2*width_amp_good, high50_amp+0.2*width_amp_good)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_Amp_good.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_Amp_good.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_Amp_good.C")



cut = cut + "&& amp["+options.channel+"]> "+str(min_amp)+"&& amp["+options.channel+"]< "+str(max_amp) + "&& amp_inverse["+options.channel+"]/amp["+options.channel+"]>-0.7"
cutrisetime = cutrisetime + "&& amp["+options.channel+"]> "+str(min_amp)+"&& amp["+options.channel+"]< "+str(max_amp) + "&& amp_inverse["+options.channel+"]/amp["+options.channel+"]> -0.7"
print "\n cut -->  "+cut

h2_amp_X=ROOT.TH2F("h2_amp_X","h2_amp_X",60,-30,30,100,0,0.6)
h2_amp_X2=ROOT.TH2F("h2_amp_X2","h2_amp_X2",40,-20,20,100,0,0.6)
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
h1_amp_X.GetYaxis().SetTitleOffset(0.95)
h1_amp_X.GetYaxis().SetTitleSize(0.05)
h1_amp_X.GetYaxis().SetLabelSize(0.032)
h1_amp_X.GetYaxis().SetLabelOffset(0.0)
maximumX=h1_amp_X2.GetBinCenter(h1_amp_X2.GetMaximumBin())
min_TDCx = 0.0 #1.0
max_TDCx =6.0 #9.0
#h1_amp_X.GetXaxis().SetRangeUser(maximumX-20,maximumX+20)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_Amp_ProfileX.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_Amp_ProfileX.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_Amp_ProfileX.C")

h2_amp_Y=ROOT.TH2F("h2_amp_Y","h2_amp_Y",60,-30,30,100,0,0.6)
h2_amp_Y2=ROOT.TH2F("h2_amp_Y2","h2_amp_Y2",60,-20,20,100,0,0.6)
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
h1_amp_Y.GetYaxis().SetTitleOffset(0.95)
h1_amp_Y.GetYaxis().SetTitleSize(0.05)
h1_amp_Y.GetYaxis().SetLabelSize(0.032)
h1_amp_Y.GetYaxis().SetLabelOffset(0.0)
maximumY=h1_amp_Y2.GetBinCenter(h1_amp_Y2.GetMaximumBin())
min_TDCy = -6.0 #-5.0
max_TDCy = 0.0 #5.0
#h1_amp_Y.GetXaxis().SetRangeUser(maximumY-20,maximumY+20)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_Amp_ProfileY.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_Amp_ProfileY.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_Amp_ProfileY.C")


cut=cut+" && TDCx>"+str(min_TDCx)+" && TDCx<"+str(max_TDCx)+" && TDCy>"+str(min_TDCy)+" && TDCy<"+str(max_TDCy) 
cutrisetime=cutrisetime+" && TDCx>"+str(min_TDCx)+" && TDCx<"+str(max_TDCx)+" && TDCy>"+str(min_TDCy)+" && TDCy<"+str(max_TDCy) 

print "\n Final cut -->  "+cut

NEntries_cut=tree.GetEntries(cut)

print "\n Num of Events before and after cut: \n"
print str(NEntries)+"   ->   "+str(NEntries_cut)+" ("+str(NEntries_cut*1.0/(NEntries*1.0)*100.0)+"% )\n"



### rise time
maxRiseTime=0
h_risetime=ROOT.TH1F("h_risetime","h_risetime",100,0.5,2.0)
h_risetime.GetXaxis().SetTitle("risetime [ns]")
h_risetime.GetYaxis().SetTitle("Events")
h_risetime.SetTitle("")
tree.Draw("risetime["+options.channel+"]>>h_risetime", cutrisetime)
maxRiseTime=h_risetime.GetBinCenter(h_risetime.FindLastBinAbove(0))
maximumY=h_risetime.GetMaximum()
high50_risetime=h_risetime.GetBinCenter(h_risetime.FindLastBinAbove(int(0.5*maximumY)))
low50_risetime=h_risetime.GetBinCenter(h_risetime.FindFirstBinAbove(int(0.5*maximumY)))
width_risetime = high50_risetime - low50_risetime
max_risetime = high50_risetime+1.0*width_risetime
min_risetime = (low50_risetime-1.0*width_risetime) if (low50_risetime-1.0*width_risetime)>0.0 else 0.0
print "risetime range: "+str(min_risetime) +" -> " + str(max_risetime)
#h_risetime.GetXaxis().SetRangeUser(min_risetime,max_risetime)
h_risetime.GetXaxis().SetTitleSize(0.05)
h_risetime.GetXaxis().SetLabelSize(0.042)
h_risetime.GetXaxis().SetLabelOffset(0.0)
h_risetime.GetYaxis().SetTitleOffset(1.7)
h_risetime.GetYaxis().SetTitleSize(0.03)
h_risetime.GetYaxis().SetLabelSize(0.032)
h_risetime.GetYaxis().SetLabelOffset(0.0)
h_risetime.Fit("gaus","","",1.15,1.5)#,low50_risetime-0.2*width_risetime, high50_risetime+0.2*width_risetime)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_risetime.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_risetime.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_risetime.C")


h_risetime_vs_amp=ROOT.TH2F("h_risetime_vs_amp","h_risetime_vs_amp",100,0,0.5,100,0.0,2.0)
tree.Draw("risetime["+options.channel+"]:amp["+options.channel+"]>>h_risetime_vs_amp","","COLZ")
h_risetime_vs_amp.GetXaxis().SetTitle("Amplitude [V]")
h_risetime_vs_amp.GetYaxis().SetTitle("Risetime [ns]")
h_risetime_vs_amp.SetTitle("")
h_risetime_vs_amp.GetXaxis().SetTitleOffset(0.9)
h_risetime_vs_amp.GetXaxis().SetTitleSize(0.05)
h_risetime_vs_amp.GetXaxis().SetLabelSize(0.042)
h_risetime_vs_amp.GetXaxis().SetLabelOffset(0.0)
h_risetime_vs_amp.GetYaxis().SetTitleOffset(0.95)
h_risetime_vs_amp.GetYaxis().SetTitleSize(0.05)
h_risetime_vs_amp.GetYaxis().SetLabelSize(0.032)
h_risetime_vs_amp.GetYaxis().SetLabelOffset(0.0)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_risetime_vs_amp.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_risetime_vs_amp.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_risetime_vs_amp.C")

### charge

maxIntegral=0
h_int=ROOT.TH1F("h_int","h_int",100,0,9.0)
h_int.GetXaxis().SetTitle("Integraed Charge [pC]")
h_int.GetYaxis().SetTitle("Events")
h_int.SetTitle("")
tree.Draw("integral["+options.channel+"]>>h_int", cut)
maxIntegral=h_int.GetBinCenter(h_int.FindLastBinAbove(0))
maximumY=h_int.GetMaximum()
high50_int=h_int.GetBinCenter(h_int.FindLastBinAbove(int(0.5*maximumY)))
low50_int=h_int.GetBinCenter(h_int.FindFirstBinAbove(int(0.5*maximumY)))
width_int = high50_int - low50_int
max_int = high50_int+1.0*width_int
min_int = (low50_int-1.0*width_int) if (low50_int-1.0*width_int)>0.0 else 0.0
print "int range: "+str(min_int) +" -> " + str(max_int)
#h_int.GetXaxis().SetRangeUser(min_int,max_int)
h_int.GetXaxis().SetTitleSize(0.05)
h_int.GetXaxis().SetLabelSize(0.042)
h_int.GetXaxis().SetLabelOffset(0.0)
h_int.GetYaxis().SetTitleOffset(1.7)
h_int.GetYaxis().SetTitleSize(0.03)
h_int.GetYaxis().SetLabelSize(0.032)
h_int.GetYaxis().SetLabelOffset(0.0)
h_int.Fit("gaus","","",low50_int-0.2*width_int, high50_int+0.2*width_int)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_int.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_int.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_int.C")

maxIntegral=0
h_intfull=ROOT.TH1F("h_intfull","h_intfull",100,0,9.0)
h_intfull.GetXaxis().SetTitle("Integraed Charge [pC]")
h_intfull.GetYaxis().SetTitle("Events")
h_intfull.SetTitle("")
tree.Draw("intfull["+options.channel+"]>>h_intfull", cut)
maxIntegral=h_intfull.GetBinCenter(h_intfull.FindLastBinAbove(0))
maximumY=h_intfull.GetMaximum()
high50_intfull=h_intfull.GetBinCenter(h_intfull.FindLastBinAbove(int(0.5*maximumY)))
low50_intfull=h_intfull.GetBinCenter(h_intfull.FindFirstBinAbove(int(0.5*maximumY)))
width_intfull = high50_intfull - low50_intfull
max_intfull = high50_intfull+1.0*width_intfull
min_intfull = (low50_intfull-1.0*width_intfull) if (low50_intfull-1.0*width_intfull)>0.0 else 0.0
print "int range: "+str(min_intfull) +" -> " + str(max_intfull)
#h_intfull.GetXaxis().SetRangeUser(min_intfull,max_intfull)
h_intfull.GetXaxis().SetTitleSize(0.05)
h_intfull.GetXaxis().SetLabelSize(0.042)
h_intfull.GetXaxis().SetLabelOffset(0.0)
h_intfull.GetYaxis().SetTitleOffset(1.7)
h_intfull.GetYaxis().SetTitleSize(0.03)
h_intfull.GetYaxis().SetLabelSize(0.032)
h_intfull.GetYaxis().SetLabelOffset(0.0)
h_intfull.Fit("gaus","","",low50_intfull-0.2*width_intfull, high50_intfull+0.2*width_intfull)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_intfull.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_intfull.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_intfull.C")


### time resolution, raw
maxDeltaT30=0
minDeltaT30=999
h_deltaT30=ROOT.TH1F("h_deltaT30","h_deltaT30",15000,-50,50)
h_deltaT30.GetXaxis().SetTitle("#Delta t [ns]")
h_deltaT30.GetYaxis().SetTitle("Events")
h_deltaT30.SetTitle("")
tree.Draw("linearTime30["+options.channel+"]-gauspeak["+options.channelRef+"]>>h_deltaT30", cut)
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
h_deltaT30.Fit("gaus","","",low80DeltaT30-1.5*widthDeltaT30,high80DeltaT30+1.5*widthDeltaT30)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT30_raw.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT30_raw.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT30_raw.C")


maxDeltaT15=0
minDeltaT15=999
h_deltaT15=ROOT.TH1F("h_deltaT15","h_deltaT15",5000,-50,50)
h_deltaT15.GetXaxis().SetTitle("#Delta t [ns]")
h_deltaT15.GetYaxis().SetTitle("Events")
h_deltaT15.SetTitle("")
tree.Draw("linearTime15["+options.channel+"]-linearTime15["+options.channelRef+"]>>h_deltaT15", cut)
maximumY=h_deltaT15.GetMaximum()
high80DeltaT15=h_deltaT15.GetBinCenter(h_deltaT15.FindLastBinAbove(int(0.8*maximumY)))
low80DeltaT15=h_deltaT15.GetBinCenter(h_deltaT15.FindFirstBinAbove(int(0.8*maximumY)))
widthDeltaT15 = high80DeltaT15 - low80DeltaT15
maxDeltaT15 = high80DeltaT15+8.0*widthDeltaT15
minDeltaT15 = low80DeltaT15-8.0*widthDeltaT15
h_deltaT15.GetXaxis().SetRangeUser(minDeltaT15,maxDeltaT15)
h_deltaT15.GetXaxis().SetTitleOffset(0.9)
h_deltaT15.GetXaxis().SetTitleSize(0.05)
h_deltaT15.GetXaxis().SetLabelSize(0.042)
h_deltaT15.GetXaxis().SetLabelOffset(0.0)
h_deltaT15.GetYaxis().SetTitleOffset(1.7)
h_deltaT15.GetYaxis().SetTitleSize(0.03)
h_deltaT15.GetYaxis().SetLabelSize(0.032)
h_deltaT15.GetYaxis().SetLabelOffset(0.0)
h_deltaT15.Fit("gaus","","",low80DeltaT15-1.5*widthDeltaT15,high80DeltaT15+1.5*widthDeltaT15)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT15_raw.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT15_raw.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT15_raw.C")

maxDeltaT45=0
minDeltaT45=999
h_deltaT45=ROOT.TH1F("h_deltaT45","h_deltaT45",5000,-50,50)
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
h_deltaT45.Fit("gaus","","",low80DeltaT45-1.5*widthDeltaT45,high80DeltaT45+1.5*widthDeltaT45)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT45_raw.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT45_raw.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT45_raw.C")



###time walk corrections
h2_deltaT30_amp=ROOT.TH2F("h2_deltaT30_amp","h2_deltaT30_amp",30,min_amp_fit,max_amp_fit,100,minDeltaT30,maxDeltaT30)
tree.Draw("linearTime30["+options.channel+"]-gauspeak["+options.channelRef+"]:amp["+options.channel+"]>>h2_deltaT30_amp",cut,"COLZ")
h2_deltaT30_amp.GetXaxis().SetTitle("Amplitude [V]")
h2_deltaT30_amp.GetYaxis().SetTitle("#Delta t [ns]")
h2_deltaT30_amp.SetTitle("")
h2_deltaT30_amp.GetXaxis().SetTitleOffset(0.9)
h2_deltaT30_amp.GetXaxis().SetTitleSize(0.05)
h2_deltaT30_amp.GetXaxis().SetLabelSize(0.042)
h2_deltaT30_amp.GetXaxis().SetLabelOffset(0.0)
h2_deltaT30_amp.GetYaxis().SetTitleOffset(0.95)
h2_deltaT30_amp.GetYaxis().SetTitleSize(0.05)
h2_deltaT30_amp.GetYaxis().SetLabelSize(0.032)
h2_deltaT30_amp.GetYaxis().SetLabelOffset(0.0)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT30_vs_amp.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT30_vs_amp.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT30_vs_amp.C")

h1_deltaT30_amp = ROOT.TH1F("h1_deltaT30_amp","h1_deltaT30_amp",30,min_amp_fit,max_amp_fit)
h1_deltaT30_amp = h2_deltaT30_amp.ProfileX()
h1_deltaT30_amp.Draw()
tf1_deltaT30_amp = ROOT.TF1("tf1_deltaT30_amp","[0]+x*[1]",min_amp,max_amp)
h1_deltaT30_amp.Fit("tf1_deltaT30_amp","","",min_amp_fit,max_amp_fit)
h1_deltaT30_amp.GetXaxis().SetTitle("Amplitude [V]")
h1_deltaT30_amp.GetYaxis().SetTitle("#Delta t [ns]")
h1_deltaT30_amp.SetTitle("")
h1_deltaT30_amp.GetXaxis().SetTitleOffset(0.9)
h1_deltaT30_amp.GetXaxis().SetTitleSize(0.05)
h1_deltaT30_amp.GetXaxis().SetLabelSize(0.042)
h1_deltaT30_amp.GetXaxis().SetLabelOffset(0.0)
h1_deltaT30_amp.GetYaxis().SetTitleOffset(0.95)
h1_deltaT30_amp.GetYaxis().SetTitleSize(0.05)
h1_deltaT30_amp.GetYaxis().SetLabelSize(0.032)
h1_deltaT30_amp.GetYaxis().SetLabelOffset(0.0)
h1_deltaT30_amp.GetYaxis().SetRangeUser(minDeltaT30,maxDeltaT30)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT30_vs_amp_ProfileX.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT30_vs_amp_ProfileX.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT30_vs_amp_ProfileX.C")

a_tf1_deltaT30_amp = tf1_deltaT30_amp.GetParameter(0)
b_tf1_deltaT30_amp = tf1_deltaT30_amp.GetParameter(1)

print "\n fitting result:  a = "+str(a_tf1_deltaT30_amp)+"  b = "+str(b_tf1_deltaT30_amp)

maxDeltaT30_WithTimeWalkCor=0
minDeltaT30_WithTimeWalkCor=999
h_deltaT30_WithTimeWalkCor=ROOT.TH1F("h_deltaT30_WithTimeWalkCor","h_deltaT30_WithTimeWalkCor",10000,-50,50)
h_deltaT30_WithTimeWalkCor.GetXaxis().SetTitle("#Delta t [ns]")
h_deltaT30_WithTimeWalkCor.GetYaxis().SetTitle("Events")
h_deltaT30_WithTimeWalkCor.SetTitle("")
tree.Draw("linearTime30["+options.channel+"]-gauspeak["+options.channelRef+"] - "+str(b_tf1_deltaT30_amp)+"*amp["+options.channel+"]>>h_deltaT30_WithTimeWalkCor", cut)
maximumY=h_deltaT30_WithTimeWalkCor.GetMaximum()
high80DeltaT30_WithTimeWalkCor=h_deltaT30_WithTimeWalkCor.GetBinCenter(h_deltaT30_WithTimeWalkCor.FindLastBinAbove(int(0.8*maximumY)))
low80DeltaT30_WithTimeWalkCor=h_deltaT30_WithTimeWalkCor.GetBinCenter(h_deltaT30_WithTimeWalkCor.FindFirstBinAbove(int(0.8*maximumY)))
widthDeltaT30_WithTimeWalkCor = high80DeltaT30_WithTimeWalkCor - low80DeltaT30_WithTimeWalkCor
maxDeltaT30_WithTimeWalkCor = high80DeltaT30_WithTimeWalkCor+8.0*widthDeltaT30_WithTimeWalkCor
minDeltaT30_WithTimeWalkCor = low80DeltaT30_WithTimeWalkCor-8.0*widthDeltaT30_WithTimeWalkCor
h_deltaT30_WithTimeWalkCor.GetXaxis().SetRangeUser(minDeltaT30_WithTimeWalkCor,maxDeltaT30_WithTimeWalkCor)
h_deltaT30_WithTimeWalkCor.GetXaxis().SetTitleOffset(0.9)
h_deltaT30_WithTimeWalkCor.GetXaxis().SetTitleSize(0.05)
h_deltaT30_WithTimeWalkCor.GetXaxis().SetLabelSize(0.042)
h_deltaT30_WithTimeWalkCor.GetXaxis().SetLabelOffset(0.0)
h_deltaT30_WithTimeWalkCor.GetYaxis().SetTitleOffset(1.7)
h_deltaT30_WithTimeWalkCor.GetYaxis().SetTitleSize(0.03)
h_deltaT30_WithTimeWalkCor.GetYaxis().SetLabelSize(0.032)
h_deltaT30_WithTimeWalkCor.GetYaxis().SetLabelOffset(0.0)
h_deltaT30_WithTimeWalkCor.Fit("gaus","","",low80DeltaT30_WithTimeWalkCor-1.5*widthDeltaT30_WithTimeWalkCor,high80DeltaT30_WithTimeWalkCor+1.5*widthDeltaT30_WithTimeWalkCor)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT30_WithTimeWalkCor.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT30_WithTimeWalkCor.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT30_WithTimeWalkCor.C")


### impact point correction
h2_deltaT30_TDCx=ROOT.TH2F("h2_deltaT30_TDCx","h2_deltaT30_TDCx",30,min_TDCx,max_TDCx,100,minDeltaT30,maxDeltaT30)
tree.Draw("linearTime30["+options.channel+"]-gauspeak["+options.channelRef+"] - "+str(b_tf1_deltaT30_amp)+"    *amp["+options.channel+"]:TDCx>>h2_deltaT30_TDCx",cut,"COLZ")
h2_deltaT30_TDCx.GetXaxis().SetTitle("Horizontal Beam Position [mm]")
h2_deltaT30_TDCx.GetYaxis().SetTitle("#Delta t [ns]")
h2_deltaT30_TDCx.SetTitle("")
h2_deltaT30_TDCx.GetXaxis().SetTitleOffset(0.9)
h2_deltaT30_TDCx.GetXaxis().SetTitleSize(0.05)
h2_deltaT30_TDCx.GetXaxis().SetLabelSize(0.042)
h2_deltaT30_TDCx.GetXaxis().SetLabelOffset(0.0)
h2_deltaT30_TDCx.GetYaxis().SetTitleOffset(0.95)
h2_deltaT30_TDCx.GetYaxis().SetTitleSize(0.05)
h2_deltaT30_TDCx.GetYaxis().SetLabelSize(0.032)
h2_deltaT30_TDCx.GetYaxis().SetLabelOffset(0.0)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT30_vs_TDCx.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT30_vs_TDCx.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT30_vs_TDCx.C")

h1_deltaT30_TDCx = ROOT.TH1F("h1_deltaT30_TDCx","h1_deltaT30_TDCx",30,min_TDCx,max_TDCx)
h1_deltaT30_TDCx = h2_deltaT30_TDCx.ProfileX()
h1_deltaT30_TDCx.Draw()
tf1_deltaT30_TDCx = ROOT.TF1("tf1_deltaT30_TDCx","[0]+x*[1]",min_TDCx,max_TDCx)
h1_deltaT30_TDCx.Fit("tf1_deltaT30_TDCx","","",min_TDCx,max_TDCx)
h1_deltaT30_TDCx.GetXaxis().SetTitle("Horizontal Beam Position [mm]")
h1_deltaT30_TDCx.GetYaxis().SetTitle("#Delta t [ns]")
h1_deltaT30_TDCx.SetTitle("")
h1_deltaT30_TDCx.GetXaxis().SetTitleOffset(0.9)
h1_deltaT30_TDCx.GetXaxis().SetTitleSize(0.05)
h1_deltaT30_TDCx.GetXaxis().SetLabelSize(0.042)
h1_deltaT30_TDCx.GetXaxis().SetLabelOffset(0.0)
h1_deltaT30_TDCx.GetYaxis().SetTitleOffset(0.95)
h1_deltaT30_TDCx.GetYaxis().SetTitleSize(0.05)
h1_deltaT30_TDCx.GetYaxis().SetLabelSize(0.032)
h1_deltaT30_TDCx.GetYaxis().SetLabelOffset(0.0)
h1_deltaT30_TDCx.GetYaxis().SetRangeUser(minDeltaT30,maxDeltaT30)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT30_vs_TDCx_ProfileX.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT30_vs_TDCx_ProfileX.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT30_vs_TDCx_ProfileX.C")

a_tf1_deltaT30_TDCx = tf1_deltaT30_TDCx.GetParameter(0)
b_tf1_deltaT30_TDCx = tf1_deltaT30_TDCx.GetParameter(1)

print "\n fitting result:  a = "+str(a_tf1_deltaT30_TDCx)+"  b = "+str(b_tf1_deltaT30_TDCx)

h2_deltaT30_TDCy=ROOT.TH2F("h2_deltaT30_TDCy","h2_deltaT30_TDCy",30,min_TDCy,max_TDCy,100,minDeltaT30,maxDeltaT30)
tree.Draw("linearTime30["+options.channel+"]-gauspeak["+options.channelRef+"] - "+str(b_tf1_deltaT30_amp)+"    *amp["+options.channel+"] :TDCy>>h2_deltaT30_TDCy",cut,"COLZ")
h2_deltaT30_TDCy.GetXaxis().SetTitle("Vertical Beam Position [mm]")
h2_deltaT30_TDCy.GetYaxis().SetTitle("#Delta t [ns]")
h2_deltaT30_TDCy.SetTitle("")
h2_deltaT30_TDCy.GetXaxis().SetTitleOffset(0.9)
h2_deltaT30_TDCy.GetXaxis().SetTitleSize(0.05)
h2_deltaT30_TDCy.GetXaxis().SetLabelSize(0.042)
h2_deltaT30_TDCy.GetXaxis().SetLabelOffset(0.0)
h2_deltaT30_TDCy.GetYaxis().SetTitleOffset(0.95)
h2_deltaT30_TDCy.GetYaxis().SetTitleSize(0.05)
h2_deltaT30_TDCy.GetYaxis().SetLabelSize(0.032)
h2_deltaT30_TDCy.GetYaxis().SetLabelOffset(0.0)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT30_vs_TDCy.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT30_vs_TDCy.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT30_vs_TDCy.C")

h1_deltaT30_TDCy = ROOT.TH1F("h1_deltaT30_TDCy","h1_deltaT30_TDCy",30,min_TDCy,max_TDCy)
h1_deltaT30_TDCy = h2_deltaT30_TDCy.ProfileX()
h1_deltaT30_TDCy.Draw()
tf1_deltaT30_TDCy = ROOT.TF1("tf1_deltaT30_TDCy","[0]+x*[1]",min_TDCy,max_TDCy)
h1_deltaT30_TDCy.Fit("tf1_deltaT30_TDCy","","",min_TDCy,max_TDCy)
h1_deltaT30_TDCy.GetXaxis().SetTitle("Vertical Beam Position [mm]")
h1_deltaT30_TDCy.GetYaxis().SetTitle("#Delta t [ns]")
h1_deltaT30_TDCy.SetTitle("")
h1_deltaT30_TDCy.GetXaxis().SetTitleOffset(0.9)
h1_deltaT30_TDCy.GetXaxis().SetTitleSize(0.05)
h1_deltaT30_TDCy.GetXaxis().SetLabelSize(0.042)
h1_deltaT30_TDCy.GetXaxis().SetLabelOffset(0.0)
h1_deltaT30_TDCy.GetYaxis().SetTitleOffset(0.95)
h1_deltaT30_TDCy.GetYaxis().SetTitleSize(0.05)
h1_deltaT30_TDCy.GetYaxis().SetLabelSize(0.032)
h1_deltaT30_TDCy.GetYaxis().SetLabelOffset(0.0)
h1_deltaT30_TDCy.GetYaxis().SetRangeUser(minDeltaT30,maxDeltaT30)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT30_vs_TDCy_ProfileX.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT30_vs_TDCy_ProfileX.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT30_vs_TDCy_ProfileX.C")

a_tf1_deltaT30_TDCy = tf1_deltaT30_TDCy.GetParameter(0)
b_tf1_deltaT30_TDCy = tf1_deltaT30_TDCy.GetParameter(1)

print "\n fitting result:  a = "+str(a_tf1_deltaT30_TDCy)+"  b = "+str(b_tf1_deltaT30_TDCy)

maxDeltaT30_WithTimeWalkAndImpactPointCor=0
minDeltaT30_WithTimeWalkAndImpactPointCor=999
h_deltaT30_WithTimeWalkAndImpactPointCor=ROOT.TH1F("h_deltaT30_WithTimeWalkAndImpactPointCor","h_deltaT30_WithTimeWalkAndImpactPointCor",5000,-50,50)
h_deltaT30_WithTimeWalkAndImpactPointCor.GetXaxis().SetTitle("#Delta t [ns]")
h_deltaT30_WithTimeWalkAndImpactPointCor.GetYaxis().SetTitle("Events")
h_deltaT30_WithTimeWalkAndImpactPointCor.SetTitle("")
tree.Draw("linearTime30["+options.channel+"]-gauspeak["+options.channelRef+"] - "+str(b_tf1_deltaT30_amp)+"*amp["+options.channel+"] -"+ str(b_tf1_deltaT30_TDCx)+"*TDCx - "+ str(b_tf1_deltaT30_TDCy)+"*TDCy" +" >>h_deltaT30_WithTimeWalkAndImpactPointCor", cut)
maximumY=h_deltaT30_WithTimeWalkAndImpactPointCor.GetMaximum()
high80DeltaT30_WithTimeWalkAndImpactPointCor=h_deltaT30_WithTimeWalkAndImpactPointCor.GetBinCenter(h_deltaT30_WithTimeWalkAndImpactPointCor.FindLastBinAbove(int(0.8*maximumY)))
low80DeltaT30_WithTimeWalkAndImpactPointCor=h_deltaT30_WithTimeWalkAndImpactPointCor.GetBinCenter(h_deltaT30_WithTimeWalkAndImpactPointCor.FindFirstBinAbove(int(0.8*maximumY)))
widthDeltaT30_WithTimeWalkAndImpactPointCor = high80DeltaT30_WithTimeWalkAndImpactPointCor - low80DeltaT30_WithTimeWalkAndImpactPointCor
maxDeltaT30_WithTimeWalkAndImpactPointCor = high80DeltaT30_WithTimeWalkAndImpactPointCor+12.0*widthDeltaT30_WithTimeWalkAndImpactPointCor
minDeltaT30_WithTimeWalkAndImpactPointCor = low80DeltaT30_WithTimeWalkAndImpactPointCor-12.0*widthDeltaT30_WithTimeWalkAndImpactPointCor
h_deltaT30_WithTimeWalkAndImpactPointCor.GetXaxis().SetRangeUser(minDeltaT30_WithTimeWalkAndImpactPointCor,maxDeltaT30_WithTimeWalkAndImpactPointCor)
h_deltaT30_WithTimeWalkAndImpactPointCor.GetXaxis().SetTitleOffset(0.9)
h_deltaT30_WithTimeWalkAndImpactPointCor.GetXaxis().SetTitleSize(0.05)
h_deltaT30_WithTimeWalkAndImpactPointCor.GetXaxis().SetLabelSize(0.042)
h_deltaT30_WithTimeWalkAndImpactPointCor.GetXaxis().SetLabelOffset(0.0)
h_deltaT30_WithTimeWalkAndImpactPointCor.GetYaxis().SetTitleOffset(1.7)
h_deltaT30_WithTimeWalkAndImpactPointCor.GetYaxis().SetTitleSize(0.03)
h_deltaT30_WithTimeWalkAndImpactPointCor.GetYaxis().SetLabelSize(0.032)
h_deltaT30_WithTimeWalkAndImpactPointCor.GetYaxis().SetLabelOffset(0.0)
h_deltaT30_WithTimeWalkAndImpactPointCor.Fit("gaus","","",low80DeltaT30_WithTimeWalkAndImpactPointCor-1.5*widthDeltaT30_WithTimeWalkAndImpactPointCor,high80DeltaT30_WithTimeWalkAndImpactPointCor+1.5*widthDeltaT30_WithTimeWalkAndImpactPointCor)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT30_WithTimeWalkAndImpactPointCor.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT30_WithTimeWalkAndImpactPointCor.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT30_WithTimeWalkAndImpactPointCor.C")


### time walk corrections
h2_deltaT15_amp=ROOT.TH2F("h2_deltaT15_amp","h2_deltaT15_amp",30,min_amp_fit,max_amp_fit,100,minDeltaT15,maxDeltaT15)
tree.Draw("linearTime15["+options.channel+"]-linearTime15["+options.channelRef+"]:amp["+options.channel+"]>>h2_deltaT15_amp",cut,"COLZ")
h2_deltaT15_amp.GetXaxis().SetTitle("Amplitude [V]")
h2_deltaT15_amp.GetYaxis().SetTitle("#Delta t [ns]")
h2_deltaT15_amp.SetTitle("")
h2_deltaT15_amp.GetXaxis().SetTitleOffset(0.9)
h2_deltaT15_amp.GetXaxis().SetTitleSize(0.05)
h2_deltaT15_amp.GetXaxis().SetLabelSize(0.042)
h2_deltaT15_amp.GetXaxis().SetLabelOffset(0.0)
h2_deltaT15_amp.GetYaxis().SetTitleOffset(0.95)
h2_deltaT15_amp.GetYaxis().SetTitleSize(0.05)
h2_deltaT15_amp.GetYaxis().SetLabelSize(0.032)
h2_deltaT15_amp.GetYaxis().SetLabelOffset(0.0)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT15_vs_amp.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT15_vs_amp.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT15_vs_amp.C")

h1_deltaT15_amp = ROOT.TH1F("h1_deltaT15_amp","h1_deltaT15_amp",30,min_amp_fit,max_amp_fit)
h1_deltaT15_amp = h2_deltaT15_amp.ProfileX()
h1_deltaT15_amp.Draw()
tf1_deltaT15_amp = ROOT.TF1("tf1_deltaT15_amp","[0]+x*[1]",min_amp_fit,max_amp_fit)
h1_deltaT15_amp.Fit("tf1_deltaT15_amp","","",min_amp_fit,max_amp_fit)
h1_deltaT15_amp.GetXaxis().SetTitle("Amplitude [V]")
h1_deltaT15_amp.GetYaxis().SetTitle("#Delta t [ns]")
h1_deltaT15_amp.SetTitle("")
h1_deltaT15_amp.GetXaxis().SetTitleOffset(0.9)
h1_deltaT15_amp.GetXaxis().SetTitleSize(0.05)
h1_deltaT15_amp.GetXaxis().SetLabelSize(0.042)
h1_deltaT15_amp.GetXaxis().SetLabelOffset(0.0)
h1_deltaT15_amp.GetYaxis().SetTitleOffset(0.95)
h1_deltaT15_amp.GetYaxis().SetTitleSize(0.05)
h1_deltaT15_amp.GetYaxis().SetLabelSize(0.032)
h1_deltaT15_amp.GetYaxis().SetLabelOffset(0.0)
h1_deltaT15_amp.GetYaxis().SetRangeUser(minDeltaT15,maxDeltaT15)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT15_vs_amp_ProfileX.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT15_vs_amp_ProfileX.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT15_vs_amp_ProfileX.C")

a_tf1_deltaT15_amp = tf1_deltaT15_amp.GetParameter(0)
b_tf1_deltaT15_amp = tf1_deltaT15_amp.GetParameter(1)

print "\n fitting result:  a = "+str(a_tf1_deltaT15_amp)+"  b = "+str(b_tf1_deltaT15_amp)

maxDeltaT15_WithTimeWalkCor=0
minDeltaT15_WithTimeWalkCor=999
h_deltaT15_WithTimeWalkCor=ROOT.TH1F("h_deltaT15_WithTimeWalkCor","h_deltaT15_WithTimeWalkCor",5000,-50,50)
h_deltaT15_WithTimeWalkCor.GetXaxis().SetTitle("#Delta t [ns]")
h_deltaT15_WithTimeWalkCor.GetYaxis().SetTitle("Events")
h_deltaT15_WithTimeWalkCor.SetTitle("")
tree.Draw("linearTime15["+options.channel+"]-linearTime15["+options.channelRef+"] - "+str(b_tf1_deltaT15_amp)+"*amp["+options.channel+"]>>h_deltaT15_WithTimeWalkCor", cut)
maximumY=h_deltaT15_WithTimeWalkCor.GetMaximum()
high80DeltaT15_WithTimeWalkCor=h_deltaT15_WithTimeWalkCor.GetBinCenter(h_deltaT15_WithTimeWalkCor.FindLastBinAbove(int(0.8*maximumY)))
low80DeltaT15_WithTimeWalkCor=h_deltaT15_WithTimeWalkCor.GetBinCenter(h_deltaT15_WithTimeWalkCor.FindFirstBinAbove(int(0.8*maximumY)))
widthDeltaT15_WithTimeWalkCor = high80DeltaT15_WithTimeWalkCor - low80DeltaT15_WithTimeWalkCor
maxDeltaT15_WithTimeWalkCor = high80DeltaT15_WithTimeWalkCor+8.0*widthDeltaT15_WithTimeWalkCor
minDeltaT15_WithTimeWalkCor = low80DeltaT15_WithTimeWalkCor-8.0*widthDeltaT15_WithTimeWalkCor
h_deltaT15_WithTimeWalkCor.GetXaxis().SetRangeUser(minDeltaT15_WithTimeWalkCor,maxDeltaT15_WithTimeWalkCor)
h_deltaT15_WithTimeWalkCor.GetXaxis().SetTitleOffset(0.9)
h_deltaT15_WithTimeWalkCor.GetXaxis().SetTitleSize(0.05)
h_deltaT15_WithTimeWalkCor.GetXaxis().SetLabelSize(0.042)
h_deltaT15_WithTimeWalkCor.GetXaxis().SetLabelOffset(0.0)
h_deltaT15_WithTimeWalkCor.GetYaxis().SetTitleOffset(1.7)
h_deltaT15_WithTimeWalkCor.GetYaxis().SetTitleSize(0.03)
h_deltaT15_WithTimeWalkCor.GetYaxis().SetLabelSize(0.032)
h_deltaT15_WithTimeWalkCor.GetYaxis().SetLabelOffset(0.0)
h_deltaT15_WithTimeWalkCor.Fit("gaus","","",low80DeltaT15_WithTimeWalkCor-1.5*widthDeltaT15_WithTimeWalkCor,high80DeltaT15_WithTimeWalkCor+1.5*widthDeltaT15_WithTimeWalkCor)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT15_WithTimeWalkCor.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT15_WithTimeWalkCor.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT15_WithTimeWalkCor.C")


### impact point correction
h2_deltaT15_TDCx=ROOT.TH2F("h2_deltaT15_TDCx","h2_deltaT15_TDCx",30,min_TDCx,max_TDCx,100,minDeltaT15,maxDeltaT15)
tree.Draw("linearTime15["+options.channel+"]-linearTime15["+options.channelRef+"] - "+str(b_tf1_deltaT15_amp)+"    *amp["+options.channel+"]:TDCx>>h2_deltaT15_TDCx",cut,"COLZ")
h2_deltaT15_TDCx.GetXaxis().SetTitle("Horizontal Beam Position [mm]")
h2_deltaT15_TDCx.GetYaxis().SetTitle("#Delta t [ns]")
h2_deltaT15_TDCx.SetTitle("")
h2_deltaT15_TDCx.GetXaxis().SetTitleOffset(0.9)
h2_deltaT15_TDCx.GetXaxis().SetTitleSize(0.05)
h2_deltaT15_TDCx.GetXaxis().SetLabelSize(0.042)
h2_deltaT15_TDCx.GetXaxis().SetLabelOffset(0.0)
h2_deltaT15_TDCx.GetYaxis().SetTitleOffset(0.95)
h2_deltaT15_TDCx.GetYaxis().SetTitleSize(0.05)
h2_deltaT15_TDCx.GetYaxis().SetLabelSize(0.032)
h2_deltaT15_TDCx.GetYaxis().SetLabelOffset(0.0)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT15_vs_TDCx.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT15_vs_TDCx.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT15_vs_TDCx.C")

h1_deltaT15_TDCx = ROOT.TH1F("h1_deltaT15_TDCx","h1_deltaT15_TDCx",30,min_TDCx,max_TDCx)
h1_deltaT15_TDCx = h2_deltaT15_TDCx.ProfileX()
h1_deltaT15_TDCx.Draw()
tf1_deltaT15_TDCx = ROOT.TF1("tf1_deltaT15_TDCx","[0]+x*[1]",min_TDCx,max_TDCx)
h1_deltaT15_TDCx.Fit("tf1_deltaT15_TDCx","","",min_TDCx,max_TDCx)
h1_deltaT15_TDCx.GetXaxis().SetTitle("Horizontal Beam Position [mm]")
h1_deltaT15_TDCx.GetYaxis().SetTitle("#Delta t [ns]")
h1_deltaT15_TDCx.SetTitle("")
h1_deltaT15_TDCx.GetXaxis().SetTitleOffset(0.9)
h1_deltaT15_TDCx.GetXaxis().SetTitleSize(0.05)
h1_deltaT15_TDCx.GetXaxis().SetLabelSize(0.042)
h1_deltaT15_TDCx.GetXaxis().SetLabelOffset(0.0)
h1_deltaT15_TDCx.GetYaxis().SetTitleOffset(0.95)
h1_deltaT15_TDCx.GetYaxis().SetTitleSize(0.05)
h1_deltaT15_TDCx.GetYaxis().SetLabelSize(0.032)
h1_deltaT15_TDCx.GetYaxis().SetLabelOffset(0.0)
h1_deltaT15_TDCx.GetYaxis().SetRangeUser(minDeltaT15,maxDeltaT15)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT15_vs_TDCx_ProfileX.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT15_vs_TDCx_ProfileX.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT15_vs_TDCx_ProfileX.C")

a_tf1_deltaT15_TDCx = tf1_deltaT15_TDCx.GetParameter(0)
b_tf1_deltaT15_TDCx = tf1_deltaT15_TDCx.GetParameter(1)

print "\n fitting result:  a = "+str(a_tf1_deltaT15_TDCx)+"  b = "+str(b_tf1_deltaT15_TDCx)

h2_deltaT15_TDCy=ROOT.TH2F("h2_deltaT15_TDCy","h2_deltaT15_TDCy",30,min_TDCy,max_TDCy,100,minDeltaT15,maxDeltaT15)
tree.Draw("linearTime15["+options.channel+"]-linearTime15["+options.channelRef+"] - "+str(b_tf1_deltaT15_amp)+"    *amp["+options.channel+"] :TDCy>>h2_deltaT15_TDCy",cut,"COLZ")
h2_deltaT15_TDCy.GetXaxis().SetTitle("Vertical Beam Position [mm]")
h2_deltaT15_TDCy.GetYaxis().SetTitle("#Delta t [ns]")
h2_deltaT15_TDCy.SetTitle("")
h2_deltaT15_TDCy.GetXaxis().SetTitleOffset(0.9)
h2_deltaT15_TDCy.GetXaxis().SetTitleSize(0.05)
h2_deltaT15_TDCy.GetXaxis().SetLabelSize(0.042)
h2_deltaT15_TDCy.GetXaxis().SetLabelOffset(0.0)
h2_deltaT15_TDCy.GetYaxis().SetTitleOffset(0.95)
h2_deltaT15_TDCy.GetYaxis().SetTitleSize(0.05)
h2_deltaT15_TDCy.GetYaxis().SetLabelSize(0.032)
h2_deltaT15_TDCy.GetYaxis().SetLabelOffset(0.0)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT15_vs_TDCy.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT15_vs_TDCy.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT15_vs_TDCy.C")

h1_deltaT15_TDCy = ROOT.TH1F("h1_deltaT15_TDCy","h1_deltaT15_TDCy",30,min_TDCy,max_TDCy)
h1_deltaT15_TDCy = h2_deltaT15_TDCy.ProfileX()
h1_deltaT15_TDCy.Draw()
tf1_deltaT15_TDCy = ROOT.TF1("tf1_deltaT15_TDCy","[0]+x*[1]",min_TDCy,max_TDCy)
h1_deltaT15_TDCy.Fit("tf1_deltaT15_TDCy","","",min_TDCy,max_TDCy)
h1_deltaT15_TDCy.GetXaxis().SetTitle("Vertical Beam Position [mm]")
h1_deltaT15_TDCy.GetYaxis().SetTitle("#Delta t [ns]")
h1_deltaT15_TDCy.SetTitle("")
h1_deltaT15_TDCy.GetXaxis().SetTitleOffset(0.9)
h1_deltaT15_TDCy.GetXaxis().SetTitleSize(0.05)
h1_deltaT15_TDCy.GetXaxis().SetLabelSize(0.042)
h1_deltaT15_TDCy.GetXaxis().SetLabelOffset(0.0)
h1_deltaT15_TDCy.GetYaxis().SetTitleOffset(0.95)
h1_deltaT15_TDCy.GetYaxis().SetTitleSize(0.05)
h1_deltaT15_TDCy.GetYaxis().SetLabelSize(0.032)
h1_deltaT15_TDCy.GetYaxis().SetLabelOffset(0.0)
h1_deltaT15_TDCy.GetYaxis().SetRangeUser(minDeltaT15,maxDeltaT15)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT15_vs_TDCy_ProfileX.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT15_vs_TDCy_ProfileX.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT15_vs_TDCy_ProfileX.C")

a_tf1_deltaT15_TDCy = tf1_deltaT15_TDCy.GetParameter(0)
b_tf1_deltaT15_TDCy = tf1_deltaT15_TDCy.GetParameter(1)

print "\n fitting result:  a = "+str(a_tf1_deltaT15_TDCy)+"  b = "+str(b_tf1_deltaT15_TDCy)

maxDeltaT15_WithTimeWalkAndImpactPointCor=0
minDeltaT15_WithTimeWalkAndImpactPointCor=999
h_deltaT15_WithTimeWalkAndImpactPointCor=ROOT.TH1F("h_deltaT15_WithTimeWalkAndImpactPointCor","h_deltaT15_WithTimeWalkAndImpactPointCor",10000,-50,50)
h_deltaT15_WithTimeWalkAndImpactPointCor.GetXaxis().SetTitle("#Delta t [ns]")
h_deltaT15_WithTimeWalkAndImpactPointCor.GetYaxis().SetTitle("Events")
h_deltaT15_WithTimeWalkAndImpactPointCor.SetTitle("")
tree.Draw("linearTime15["+options.channel+"]-linearTime15["+options.channelRef+"] - "+str(b_tf1_deltaT15_amp)+"*amp["+options.channel+"] -"+ str(b_tf1_deltaT15_TDCx)+"*TDCx - "+ str(b_tf1_deltaT15_TDCy)+"*TDCy" +" >>h_deltaT15_WithTimeWalkAndImpactPointCor", cut)
maximumY=h_deltaT15_WithTimeWalkAndImpactPointCor.GetMaximum()
high80DeltaT15_WithTimeWalkAndImpactPointCor=h_deltaT15_WithTimeWalkAndImpactPointCor.GetBinCenter(h_deltaT15_WithTimeWalkAndImpactPointCor.FindLastBinAbove(int(0.8*maximumY)))
low80DeltaT15_WithTimeWalkAndImpactPointCor=h_deltaT15_WithTimeWalkAndImpactPointCor.GetBinCenter(h_deltaT15_WithTimeWalkAndImpactPointCor.FindFirstBinAbove(int(0.8*maximumY)))
widthDeltaT15_WithTimeWalkAndImpactPointCor = high80DeltaT15_WithTimeWalkAndImpactPointCor - low80DeltaT15_WithTimeWalkAndImpactPointCor
maxDeltaT15_WithTimeWalkAndImpactPointCor = high80DeltaT15_WithTimeWalkAndImpactPointCor+10.0*widthDeltaT15_WithTimeWalkAndImpactPointCor
minDeltaT15_WithTimeWalkAndImpactPointCor = low80DeltaT15_WithTimeWalkAndImpactPointCor-10.0*widthDeltaT15_WithTimeWalkAndImpactPointCor
h_deltaT15_WithTimeWalkAndImpactPointCor.GetXaxis().SetRangeUser(minDeltaT15_WithTimeWalkAndImpactPointCor,maxDeltaT15_WithTimeWalkAndImpactPointCor)
h_deltaT15_WithTimeWalkAndImpactPointCor.GetXaxis().SetTitleOffset(0.9)
h_deltaT15_WithTimeWalkAndImpactPointCor.GetXaxis().SetTitleSize(0.05)
h_deltaT15_WithTimeWalkAndImpactPointCor.GetXaxis().SetLabelSize(0.042)
h_deltaT15_WithTimeWalkAndImpactPointCor.GetXaxis().SetLabelOffset(0.0)
h_deltaT15_WithTimeWalkAndImpactPointCor.GetYaxis().SetTitleOffset(1.7)
h_deltaT15_WithTimeWalkAndImpactPointCor.GetYaxis().SetTitleSize(0.03)
h_deltaT15_WithTimeWalkAndImpactPointCor.GetYaxis().SetLabelSize(0.032)
h_deltaT15_WithTimeWalkAndImpactPointCor.GetYaxis().SetLabelOffset(0.0)
h_deltaT15_WithTimeWalkAndImpactPointCor.Fit("gaus","","",low80DeltaT15_WithTimeWalkAndImpactPointCor-2.0*widthDeltaT15_WithTimeWalkAndImpactPointCor,high80DeltaT15_WithTimeWalkAndImpactPointCor+2.0*widthDeltaT15_WithTimeWalkAndImpactPointCor)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT15_WithTimeWalkAndImpactPointCor.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT15_WithTimeWalkAndImpactPointCor.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT15_WithTimeWalkAndImpactPointCor.C")

### time walk corrections
h2_deltaT45_amp=ROOT.TH2F("h2_deltaT45_amp","h2_deltaT45_amp",30,min_amp_fit,max_amp_fit,100,minDeltaT45,maxDeltaT45)
tree.Draw("linearTime45["+options.channel+"]-linearTime45["+options.channelRef+"]:amp["+options.channel+"]>>h2_deltaT45_amp",cut,"COLZ")
h2_deltaT45_amp.GetXaxis().SetTitle("Amplitude [V]")
h2_deltaT45_amp.GetYaxis().SetTitle("#Delta t [ns]")
h2_deltaT45_amp.SetTitle("")
h2_deltaT45_amp.GetXaxis().SetTitleOffset(0.9)
h2_deltaT45_amp.GetXaxis().SetTitleSize(0.05)
h2_deltaT45_amp.GetXaxis().SetLabelSize(0.042)
h2_deltaT45_amp.GetXaxis().SetLabelOffset(0.0)
h2_deltaT45_amp.GetYaxis().SetTitleOffset(0.95)
h2_deltaT45_amp.GetYaxis().SetTitleSize(0.05)
h2_deltaT45_amp.GetYaxis().SetLabelSize(0.032)
h2_deltaT45_amp.GetYaxis().SetLabelOffset(0.0)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT45_vs_amp.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT45_vs_amp.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT45_vs_amp.C")

h1_deltaT45_amp = ROOT.TH1F("h1_deltaT45_amp","h1_deltaT45_amp",30,min_amp_fit,max_amp_fit)
h1_deltaT45_amp = h2_deltaT45_amp.ProfileX()
h1_deltaT45_amp.Draw()
tf1_deltaT45_amp = ROOT.TF1("tf1_deltaT45_amp","[0]+x*[1]",min_amp_fit,max_amp_fit)
h1_deltaT45_amp.Fit("tf1_deltaT45_amp","","",min_amp_fit,max_amp_fit)
h1_deltaT45_amp.GetXaxis().SetTitle("Amplitude [V]")
h1_deltaT45_amp.GetYaxis().SetTitle("#Delta t [ns]")
h1_deltaT45_amp.SetTitle("")
h1_deltaT45_amp.GetXaxis().SetTitleOffset(0.9)
h1_deltaT45_amp.GetXaxis().SetTitleSize(0.05)
h1_deltaT45_amp.GetXaxis().SetLabelSize(0.042)
h1_deltaT45_amp.GetXaxis().SetLabelOffset(0.0)
h1_deltaT45_amp.GetYaxis().SetTitleOffset(0.95)
h1_deltaT45_amp.GetYaxis().SetTitleSize(0.05)
h1_deltaT45_amp.GetYaxis().SetLabelSize(0.032)
h1_deltaT45_amp.GetYaxis().SetLabelOffset(0.0)
h1_deltaT45_amp.GetYaxis().SetRangeUser(minDeltaT45,maxDeltaT45)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT45_vs_amp_ProfileX.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT45_vs_amp_ProfileX.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT45_vs_amp_ProfileX.C")

a_tf1_deltaT45_amp = tf1_deltaT45_amp.GetParameter(0)
b_tf1_deltaT45_amp = tf1_deltaT45_amp.GetParameter(1)

print "\n fitting result:  a = "+str(a_tf1_deltaT45_amp)+"  b = "+str(b_tf1_deltaT45_amp)

maxDeltaT45_WithTimeWalkCor=0
minDeltaT45_WithTimeWalkCor=999
h_deltaT45_WithTimeWalkCor=ROOT.TH1F("h_deltaT45_WithTimeWalkCor","h_deltaT45_WithTimeWalkCor",5000,-50,50)
h_deltaT45_WithTimeWalkCor.GetXaxis().SetTitle("#Delta t [ns]")
h_deltaT45_WithTimeWalkCor.GetYaxis().SetTitle("Events")
h_deltaT45_WithTimeWalkCor.SetTitle("")
tree.Draw("linearTime45["+options.channel+"]-linearTime45["+options.channelRef+"] - "+str(b_tf1_deltaT45_amp)+"*amp["+options.channel+"]>>h_deltaT45_WithTimeWalkCor", cut)
maximumY=h_deltaT45_WithTimeWalkCor.GetMaximum()
high80DeltaT45_WithTimeWalkCor=h_deltaT45_WithTimeWalkCor.GetBinCenter(h_deltaT45_WithTimeWalkCor.FindLastBinAbove(int(0.8*maximumY)))
low80DeltaT45_WithTimeWalkCor=h_deltaT45_WithTimeWalkCor.GetBinCenter(h_deltaT45_WithTimeWalkCor.FindFirstBinAbove(int(0.8*maximumY)))
widthDeltaT45_WithTimeWalkCor = high80DeltaT45_WithTimeWalkCor - low80DeltaT45_WithTimeWalkCor
maxDeltaT45_WithTimeWalkCor = high80DeltaT45_WithTimeWalkCor+8.0*widthDeltaT45_WithTimeWalkCor
minDeltaT45_WithTimeWalkCor = low80DeltaT45_WithTimeWalkCor-8.0*widthDeltaT45_WithTimeWalkCor
h_deltaT45_WithTimeWalkCor.GetXaxis().SetRangeUser(minDeltaT45_WithTimeWalkCor,maxDeltaT45_WithTimeWalkCor)
h_deltaT45_WithTimeWalkCor.GetXaxis().SetTitleOffset(0.9)
h_deltaT45_WithTimeWalkCor.GetXaxis().SetTitleSize(0.05)
h_deltaT45_WithTimeWalkCor.GetXaxis().SetLabelSize(0.042)
h_deltaT45_WithTimeWalkCor.GetXaxis().SetLabelOffset(0.0)
h_deltaT45_WithTimeWalkCor.GetYaxis().SetTitleOffset(1.7)
h_deltaT45_WithTimeWalkCor.GetYaxis().SetTitleSize(0.03)
h_deltaT45_WithTimeWalkCor.GetYaxis().SetLabelSize(0.032)
h_deltaT45_WithTimeWalkCor.GetYaxis().SetLabelOffset(0.0)
h_deltaT45_WithTimeWalkCor.Fit("gaus","","",low80DeltaT45_WithTimeWalkCor-1.5*widthDeltaT45_WithTimeWalkCor,high80DeltaT45_WithTimeWalkCor+1.5*widthDeltaT45_WithTimeWalkCor)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT45_WithTimeWalkCor.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT45_WithTimeWalkCor.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT45_WithTimeWalkCor.C")


### impact point correction
h2_deltaT45_TDCx=ROOT.TH2F("h2_deltaT45_TDCx","h2_deltaT45_TDCx",30,min_TDCx,max_TDCx,100,minDeltaT45,maxDeltaT45)
tree.Draw("linearTime45["+options.channel+"]-linearTime45["+options.channelRef+"] - "+str(b_tf1_deltaT45_amp)+"    *amp["+options.channel+"]:TDCx>>h2_deltaT45_TDCx",cut,"COLZ")
h2_deltaT45_TDCx.GetXaxis().SetTitle("Horizontal Beam Position [mm]")
h2_deltaT45_TDCx.GetYaxis().SetTitle("#Delta t [ns]")
h2_deltaT45_TDCx.SetTitle("")
h2_deltaT45_TDCx.GetXaxis().SetTitleOffset(0.9)
h2_deltaT45_TDCx.GetXaxis().SetTitleSize(0.05)
h2_deltaT45_TDCx.GetXaxis().SetLabelSize(0.042)
h2_deltaT45_TDCx.GetXaxis().SetLabelOffset(0.0)
h2_deltaT45_TDCx.GetYaxis().SetTitleOffset(0.95)
h2_deltaT45_TDCx.GetYaxis().SetTitleSize(0.05)
h2_deltaT45_TDCx.GetYaxis().SetLabelSize(0.032)
h2_deltaT45_TDCx.GetYaxis().SetLabelOffset(0.0)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT45_vs_TDCx.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT45_vs_TDCx.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT45_vs_TDCx.C")

h1_deltaT45_TDCx = ROOT.TH1F("h1_deltaT45_TDCx","h1_deltaT45_TDCx",30,min_TDCx,max_TDCx)
h1_deltaT45_TDCx = h2_deltaT45_TDCx.ProfileX()
h1_deltaT45_TDCx.Draw()
tf1_deltaT45_TDCx = ROOT.TF1("tf1_deltaT45_TDCx","[0]+x*[1]",min_TDCx,max_TDCx)
h1_deltaT45_TDCx.Fit("tf1_deltaT45_TDCx","","",min_TDCx,max_TDCx)
h1_deltaT45_TDCx.GetXaxis().SetTitle("Horizontal Beam Position [mm]")
h1_deltaT45_TDCx.GetYaxis().SetTitle("#Delta t [ns]")
h1_deltaT45_TDCx.SetTitle("")
h1_deltaT45_TDCx.GetXaxis().SetTitleOffset(0.9)
h1_deltaT45_TDCx.GetXaxis().SetTitleSize(0.05)
h1_deltaT45_TDCx.GetXaxis().SetLabelSize(0.042)
h1_deltaT45_TDCx.GetXaxis().SetLabelOffset(0.0)
h1_deltaT45_TDCx.GetYaxis().SetTitleOffset(0.95)
h1_deltaT45_TDCx.GetYaxis().SetTitleSize(0.05)
h1_deltaT45_TDCx.GetYaxis().SetLabelSize(0.032)
h1_deltaT45_TDCx.GetYaxis().SetLabelOffset(0.0)
h1_deltaT45_TDCx.GetYaxis().SetRangeUser(minDeltaT45,maxDeltaT45)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT45_vs_TDCx_ProfileX.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT45_vs_TDCx_ProfileX.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT45_vs_TDCx_ProfileX.C")

a_tf1_deltaT45_TDCx = tf1_deltaT45_TDCx.GetParameter(0)
b_tf1_deltaT45_TDCx = tf1_deltaT45_TDCx.GetParameter(1)

print "\n fitting result:  a = "+str(a_tf1_deltaT45_TDCx)+"  b = "+str(b_tf1_deltaT45_TDCx)

h2_deltaT45_TDCy=ROOT.TH2F("h2_deltaT45_TDCy","h2_deltaT45_TDCy",30,min_TDCy,max_TDCy,100,minDeltaT45,maxDeltaT45)
tree.Draw("linearTime45["+options.channel+"]-linearTime45["+options.channelRef+"] - "+str(b_tf1_deltaT45_amp)+"    *amp["+options.channel+"] :TDCy>>h2_deltaT45_TDCy",cut,"COLZ")
h2_deltaT45_TDCy.GetXaxis().SetTitle("Vertical Beam Position [mm]")
h2_deltaT45_TDCy.GetYaxis().SetTitle("#Delta t [ns]")
h2_deltaT45_TDCy.SetTitle("")
h2_deltaT45_TDCy.GetXaxis().SetTitleOffset(0.9)
h2_deltaT45_TDCy.GetXaxis().SetTitleSize(0.05)
h2_deltaT45_TDCy.GetXaxis().SetLabelSize(0.042)
h2_deltaT45_TDCy.GetXaxis().SetLabelOffset(0.0)
h2_deltaT45_TDCy.GetYaxis().SetTitleOffset(0.95)
h2_deltaT45_TDCy.GetYaxis().SetTitleSize(0.05)
h2_deltaT45_TDCy.GetYaxis().SetLabelSize(0.032)
h2_deltaT45_TDCy.GetYaxis().SetLabelOffset(0.0)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT45_vs_TDCy.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT45_vs_TDCy.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT45_vs_TDCy.C")

h1_deltaT45_TDCy = ROOT.TH1F("h1_deltaT45_TDCy","h1_deltaT45_TDCy",30,min_TDCy,max_TDCy)
h1_deltaT45_TDCy = h2_deltaT45_TDCy.ProfileX()
h1_deltaT45_TDCy.Draw()
tf1_deltaT45_TDCy = ROOT.TF1("tf1_deltaT45_TDCy","[0]+x*[1]",min_TDCy,max_TDCy)
h1_deltaT45_TDCy.Fit("tf1_deltaT45_TDCy","","",min_TDCy,max_TDCy)
h1_deltaT45_TDCy.GetXaxis().SetTitle("Vertical Beam Position [mm]")
h1_deltaT45_TDCy.GetYaxis().SetTitle("#Delta t [ns]")
h1_deltaT45_TDCy.SetTitle("")
h1_deltaT45_TDCy.GetXaxis().SetTitleOffset(0.9)
h1_deltaT45_TDCy.GetXaxis().SetTitleSize(0.05)
h1_deltaT45_TDCy.GetXaxis().SetLabelSize(0.042)
h1_deltaT45_TDCy.GetXaxis().SetLabelOffset(0.0)
h1_deltaT45_TDCy.GetYaxis().SetTitleOffset(0.95)
h1_deltaT45_TDCy.GetYaxis().SetTitleSize(0.05)
h1_deltaT45_TDCy.GetYaxis().SetLabelSize(0.032)
h1_deltaT45_TDCy.GetYaxis().SetLabelOffset(0.0)
h1_deltaT45_TDCy.GetYaxis().SetRangeUser(minDeltaT45,maxDeltaT45)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT45_vs_TDCy_ProfileX.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT45_vs_TDCy_ProfileX.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT45_vs_TDCy_ProfileX.C")

a_tf1_deltaT45_TDCy = tf1_deltaT45_TDCy.GetParameter(0)
b_tf1_deltaT45_TDCy = tf1_deltaT45_TDCy.GetParameter(1)

print "\n fitting result:  a = "+str(a_tf1_deltaT45_TDCy)+"  b = "+str(b_tf1_deltaT45_TDCy)

maxDeltaT45_WithTimeWalkAndImpactPointCor=0
minDeltaT45_WithTimeWalkAndImpactPointCor=999
h_deltaT45_WithTimeWalkAndImpactPointCor=ROOT.TH1F("h_deltaT45_WithTimeWalkAndImpactPointCor","h_deltaT45_WithTimeWalkAndImpactPointCor",10000,-50,50)
h_deltaT45_WithTimeWalkAndImpactPointCor.GetXaxis().SetTitle("#Delta t [ns]")
h_deltaT45_WithTimeWalkAndImpactPointCor.GetYaxis().SetTitle("Events")
h_deltaT45_WithTimeWalkAndImpactPointCor.SetTitle("")
tree.Draw("linearTime45["+options.channel+"]-linearTime45["+options.channelRef+"] - "+str(b_tf1_deltaT45_amp)+"*amp["+options.channel+"] -"+ str(b_tf1_deltaT45_TDCx)+"*TDCx - "+ str(b_tf1_deltaT45_TDCy)+"*TDCy" +" >>h_deltaT45_WithTimeWalkAndImpactPointCor", cut)
maximumY=h_deltaT45_WithTimeWalkAndImpactPointCor.GetMaximum()
high80DeltaT45_WithTimeWalkAndImpactPointCor=h_deltaT45_WithTimeWalkAndImpactPointCor.GetBinCenter(h_deltaT45_WithTimeWalkAndImpactPointCor.FindLastBinAbove(int(0.8*maximumY)))
low80DeltaT45_WithTimeWalkAndImpactPointCor=h_deltaT45_WithTimeWalkAndImpactPointCor.GetBinCenter(h_deltaT45_WithTimeWalkAndImpactPointCor.FindFirstBinAbove(int(0.8*maximumY)))
widthDeltaT45_WithTimeWalkAndImpactPointCor = high80DeltaT45_WithTimeWalkAndImpactPointCor - low80DeltaT45_WithTimeWalkAndImpactPointCor
maxDeltaT45_WithTimeWalkAndImpactPointCor = high80DeltaT45_WithTimeWalkAndImpactPointCor+10.0*widthDeltaT45_WithTimeWalkAndImpactPointCor
minDeltaT45_WithTimeWalkAndImpactPointCor = low80DeltaT45_WithTimeWalkAndImpactPointCor-10.0*widthDeltaT45_WithTimeWalkAndImpactPointCor
h_deltaT45_WithTimeWalkAndImpactPointCor.GetXaxis().SetRangeUser(minDeltaT45_WithTimeWalkAndImpactPointCor,maxDeltaT45_WithTimeWalkAndImpactPointCor)
h_deltaT45_WithTimeWalkAndImpactPointCor.GetXaxis().SetTitleOffset(0.9)
h_deltaT45_WithTimeWalkAndImpactPointCor.GetXaxis().SetTitleSize(0.05)
h_deltaT45_WithTimeWalkAndImpactPointCor.GetXaxis().SetLabelSize(0.042)
h_deltaT45_WithTimeWalkAndImpactPointCor.GetXaxis().SetLabelOffset(0.0)
h_deltaT45_WithTimeWalkAndImpactPointCor.GetYaxis().SetTitleOffset(1.7)
h_deltaT45_WithTimeWalkAndImpactPointCor.GetYaxis().SetTitleSize(0.03)
h_deltaT45_WithTimeWalkAndImpactPointCor.GetYaxis().SetLabelSize(0.032)
h_deltaT45_WithTimeWalkAndImpactPointCor.GetYaxis().SetLabelOffset(0.0)
h_deltaT45_WithTimeWalkAndImpactPointCor.Fit("gaus","","",low80DeltaT45_WithTimeWalkAndImpactPointCor-2.0*widthDeltaT45_WithTimeWalkAndImpactPointCor,high80DeltaT45_WithTimeWalkAndImpactPointCor+2.0*widthDeltaT45_WithTimeWalkAndImpactPointCor)
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT45_WithTimeWalkAndImpactPointCor.pdf")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT45_WithTimeWalkAndImpactPointCor.png")
myC.Print(options.outputDir+"/"+options.run+"/Ch"+options.channel+"_deltaT45_WithTimeWalkAndImpactPointCor.C")

