#!usr/bin/python
import ROOT
from ROOT import TH1F, TCanvas, TFile, gROOT, TTree, TCut, TMath

gROOT.ForceStyle()		

FileName = ["ttHJetTobb_M125", "TT", "QCD_HT200to300", "QCD_HT300to500", "QCD_HT500to700", "QCD_HT700to1000", "QCD_HT1000to1500", "QCD_HT1500to2000", "QCD_HT2000toInf"]

ttH_xsec              = 5.09e-01 #pb #https://twiki.cern.ch/twiki/bin/view/LHCPhysics/CERNYellowReportPageAt1314TeV
Hbb_BR                = 5.77e-01 #pb #https://twiki.cern.ch/twiki/bin/view/LHCPhysics/CERNYellowReportPageBR3
ttbar_xsec            = 8.32e+02 #pb # https://twiki.cern.ch/twiki/bin/view/LHCPhysics/TtbarNNLO
QCD_HT200to300_xsec   = 1.74e+06
QCD_HT300to500_xsec   = 3.67e+05
QCD_HT500to700_xsec   = 2.94e+04 
QCD_HT700to1000_sec   = 6.52e+03
QCD_HT1000to1500_xsec = 1.06e+03
QCD_HT1500to2000_xsec = 1.22e+02
QCD_HT2000toInf_xsec  = 2.54e+01

XSEC = [ttH_xsec*Hbb_BR, ttbar_xsec, QCD_HT200to300_xsec, QCD_HT300to500_xsec, QCD_HT500to700_xsec, QCD_HT700to1000_sec, QCD_HT1000to1500_xsec, QCD_HT1500to2000_xsec, QCD_HT2000toInf_xsec]	

print FileName, XSEC

LUMI = 1		
NCAT = 2		
inf  = []		
tr   = []	
h = [[] for x in range(len(FileName))] 

COLOR = [ROOT.kRed, ROOT.kBlack, ROOT.kBlue, ROOT.kBlue-5, ROOT.kBlue-8, ROOT.kGreen, ROOT.kGreen-4, ROOT.kGray, ROOT.kRed+4]				
CUT = []		
CUT.append(TCut("ht>500 && jetPt[5]>40 && nBJets==2"))		
CUT.append(TCut("ht>500 && jetPt[5]>40 && nBJets>2"))		

can = TCanvas("Canvas0","Canvas0",600,600)
can.Divide(2,1)	

for i in xrange(len(FileName)):
  inf.append(TFile.Open("flatTree_" + FileName[i] + ".root"))
  print "flatTree_" + FileName[i] + ".root"
  tr.append(inf[-1].Get("hadtop/events"))
  hpu = inf[-1].Get("hadtop/pileup")
  
  for k in xrange(NCAT):
    can.cd(k + 1)
    print i, k
    h[i].append(TH1F("h" + str(i) + str(k), "h" + str(i) + str(k), 25,-1,1))	
    h[i][k].Sumw2()
    tr[i].Draw("mva>>"  +  "h" + str(i) + str(k), CUT[k])
    h[i][k].SetLineWidth(2)
    h[i][k].SetLineColor(COLOR[i])
    h[i][k].Scale(LUMI*XSEC[i]/hpu.GetEntries()) 		

outf = TFile("ttH-shapes.root","RECREATE")		
hQCD = []

for k in xrange(NCAT):
  can.cd(k + 1)

  hQCD.append(h[2][k].Clone("hQCD" + str(k)))
  hQCD[k].Add(h[3][k])
  hQCD[k].Add(h[4][k])
  h[0][k].SetFillColor(ROOT.kRed-10)
  h[1][k].SetFillColor(ROOT.kGreen-10)
  hQCD[k].SetFillColor(ROOT.kBlue-10)
  h[0][k].SetFillStyle(3001)	
  h[1][k].SetFillStyle(3001)
  print "QCD events:   ", hQCD[k].Integral()
  print "TTbar events: ", h[1][k].Integral()
  print "TTH events:   ", h[0][k].Integral()		

  max1 = TMath.Max(h[1][k].GetBinContent(h[1][k].GetMaximumBin()), hQCD[k].GetBinContent(hQCD[k].GetMaximumBin()))	
  max2 = TMath.Max(h[0][k].GetBinContent(h[0][k].GetMaximumBin()), max1)
  hQCD[k].SetMaximum(1.1*max2)
  hQCD[k].Draw("hist")
  h[0][k].Draw("same hist")
  h[1][k].Draw("same hist")

  outf.cd()
  hQCD[k].SetName("qcdCAT"      + str(k))
  hQCD[k].Write()

  h[0][k].SetName("signalCAT"   + str(k))
  h[0][k].Write()

  h[1][k].SetName("ttjetsCAT"   + str(k))
  h[1][k].Write()

  hQCD[k].SetName("data_obsCAT" + str(k)) 
  hQCD[k].Write()	

datacard = open("datacard_ttH.txt","w")	
datacard.write("imax "  + str(NCAT) + "\n")		
datacard.write("jmax *" + "\n")		
datacard.write("kmax *" + "\n")		
datacard.write(7*12*"-" + "\n")		
datacard.write("shapes *  * " + outf.GetName() + " $PROCESS$CHANNEL" + "\n")		
datacard.write(7*12*"-" + "\n")
datacard.write("bin         ")		

for k in xrange(NCAT):
  name = "CAT"+str(k) + " "	
  datacard.write(name)

datacard.write("\n")		
datacard.write("observation ")

for k in xrange(NCAT):
  datacard.write("-1 ")

datacard.write("\n")
datacard.write(7*12*"-" + "\n")
datacard.write('{:<12}'.format("bin"))

for k in xrange(NCAT):
  name = '{:>12} {:>12} {:>12}'.format("CAT" + str(k), "CAT" + str(k), "CAT" + str(k))
  datacard.write(name)

datacard.write("\n")
datacard.write('{:<12}'.format("process"))

for k in xrange(NCAT):
  datacard.write('{:>12} {:>12} {:>12}'.format("signal", "ttjets", "qcd"))

datacard.write("\n")
datacard.write('{:<12}'.format("process"))
 
for k in xrange(NCAT):
  datacard.write('{:>12} {:>12} {:>12}'.format("0", "1", "2"))

datacard.write("\n")
datacard.write('{:<12}'.format("rate"))

for k in xrange(NCAT):
  rate1 = h[0][k].Integral()
  rate2 = h[1][k].Integral()
  rate3 = hQCD[k].Integral()

  datacard.write('{:>12.3f} {:>12.3f} {:>12.3f}'.format(rate1, rate2, rate3))

datacard.write("\n")
datacard.close()

