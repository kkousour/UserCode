#!usr/bin/python
import ROOT, os
from ROOT import TH1F, TCanvas, TFile, gROOT, TTree, TCut, TMath, RooRealVar, RooDataHist, RooArgList, RooArgSet, RooAddPdf, RooFit, RooGenericPdf, RooWorkspace, RooMsgService, RooHistPdf
import optparse

parser = optparse.OptionParser()
parser.add_option("--lumi",     action = "store", type = "float", dest = "LUMI",     default = 1000.)
parser.add_option("--ncat",     action = "store", type = "string",   dest = "NCAT",     default = "0,1")
parser.add_option("--template", action = "store", type = "str",   dest = "TEMPLATE", default = 'MC')

(options, args) = parser.parse_args()

LUMI     = options.LUMI
NCAT     = [int(i) for i in options.NCAT.split(',')]
TEMPLATE = options.TEMPLATE

print 'lumi:', LUMI, ' ncat:', NCAT, ' template:', TEMPLATE

gROOT.ForceStyle()

if TEMPLATE == 'MC':
    FileName = ["ttHJetTobb_M125", "TT", "QCD_HT200to300", "QCD_HT300to500", "QCD_HT500to700", "QCD_HT700to1000", "QCD_HT1000to1500", "QCD_HT1500to2000", "QCD_HT2000toInf"]

else:
    FileName = ["ttHJetTobb_M125", "TT", "JetHT"]

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

inf   = []
tr    = []
h     = [[] for x in range(len(FileName))]
hData = []

dataf = TFile.Open("flatTree_JetHT.root")  # data file

COLOR = [ROOT.kRed, ROOT.kBlack, ROOT.kBlue, ROOT.kBlue-5, ROOT.kBlue-8, ROOT.kGreen, ROOT.kGreen-4, ROOT.kGray, ROOT.kRed+4]
CUT   = [TCut("ht>500 && jetPt[5]>40 && nBJets==2"), TCut("ht>500 && jetPt[5]>40 && nBJets>2")]

can = TCanvas("Canvas0","Canvas0", 600, 600)
can.Divide(2,1)

for i in xrange(len(FileName)):
  inf.append(TFile.Open("flatTree_" + FileName[i] + ".root"))
  print "flatTree_" + FileName[i] + ".root\t"
  
  if TEMPLATE == "MC":
    tr.append(inf[-1].Get("hadtop/events"))
    hpu = inf[-1].Get("hadtop/pileup")
  else:
    tr.append(inf[-1].Get("hadtopL/events"))
    hpu = inf[-1].Get("hadtopL/pileup")

  for k in NCAT:
    can.cd(k + 1)
    print i, k
    h[i].append(TH1F("h" + str(i) + str(k), "h" + str(i) + str(k), 25,-1,1))
    h[i][k].Sumw2()
    tr[i].Draw("mva>>"  +  "h" + str(i) + str(k), CUT[k])
    h[i][k].SetLineWidth(2)
    h[i][k].SetLineColor(COLOR[i])
    h[i][k].Scale(LUMI*XSEC[i]/hpu.GetEntries())
    print hpu.GetEntries()

hQCD = []

for k in NCAT:
  can.cd(k + 1)
  if TEMPLATE == "MC":
    hQCD.append(h[2][k].Clone("hQCD" + str(k)))
    hQCD[k].Add(h[3][k])
    hQCD[k].Add(h[4][k])
    hQCD[k].Add(h[5][k])
    hQCD[k].Add(h[6][k])
    hQCD[k].Add(h[7][k])
    hQCD[k].Add(h[8][k])
  else:
    hQCD.append(h[2][k].Clone("hQCD" + str(k)))

  h[0][k].SetFillColor(ROOT.kRed-10)
  h[1][k].SetFillColor(ROOT.kGreen-10)
  hQCD[k].SetFillColor(ROOT.kBlue-10)
  h[0][k].SetFillStyle(3001)
  h[1][k].SetFillStyle(3001)

  print "QCD events:   ", hQCD[k].Integral()
  print "TTbar events: ", h[1][k].Integral()
  print "TTH events:   ", h[0][k].Integral()

  max1 = max(h[1][k].GetBinContent(h[1][k].GetMaximumBin()), hQCD[k].GetBinContent(hQCD[k].GetMaximumBin()))
  max2 = max(h[0][k].GetBinContent(h[0][k].GetMaximumBin()), max1)

  hQCD[k].SetMaximum(1.1*max2)
  hQCD[k].Draw("hist")
  h[0][k].Draw("same hist")
  h[1][k].Draw("same hist")

  hQCD[k].SetName("qcdCAT"      + str(k))

  h[0][k].SetName("signalCAT"   + str(k))

  h[1][k].SetName("ttjetsCAT"   + str(k))

  dataf.cd()
  hData.append(TH1F("data_obsCAT" + str(k), "data_obsCAT" + str(k), 25, -1, 1))
  datatree = dataf.Get("hadtop/events")
  hpu = dataf.Get("hadtop/pileup")
  datatree.Draw("mva>>"  +  hData[k].GetName(), CUT[k])

workspace   = RooWorkspace("w","workspace")
RooSigHist  = []
MVA         = []
RooObsHist  = []
RooSigHist  = []
RooQCDHist  = []
RooTTJHist  = []
ObsPDF      = []
SignalPDF   = []
QCDPDF      = []
TTJPDF      = []
QCD_Norm = []
TTJ_Norm = []

for k in NCAT:

  MVA.append(RooRealVar("mva","mva", -1, 1))

  RooObsHist.append(RooDataHist("RooObsHist", "RooObsHist", RooArgList(MVA[-1]), hData[k]))
  RooSigHist.append(RooDataHist("RooSigHist", "RooSigHist", RooArgList(MVA[-1]), h[0][k]))
  RooQCDHist.append(RooDataHist("RooQCDHist", "RooQCDHist", RooArgList(MVA[-1]), hQCD[k]))
  RooTTJHist.append(RooDataHist("RooTTJHist", "RooTTJHist", RooArgList(MVA[-1]), h[1][k]))
  RooObsHist[-1].Print()

  SignalPDF.append(RooHistPdf( "signalCAT"   + str(k), "signalCAT"   + str(k), RooArgSet(MVA[-1]), RooSigHist[-1]))
  QCDPDF.append(RooHistPdf(    "qcdCAT"      + str(k), "qcdCAT"      + str(k), RooArgSet(MVA[-1]), RooQCDHist[-1]))
  TTJPDF.append(RooHistPdf(    "ttjetsCAT"   + str(k), "ttjetsCAT"   + str(k), RooArgSet(MVA[-1]), RooTTJHist[-1]))
  QCD_Norm.append(RooRealVar(  "QCD_Norm"    + str(k),"QCD_Norm"     + str(k), 0, -1e+04, 1e+04))
  TTJ_Norm.append(RooRealVar(  "TTJ_Norm"    + str(k),"TTJ_Norm"     + str(k), 0, -1e+04, 1e+04))

  getattr(workspace,'import')(SignalPDF[-1])
  getattr(workspace,'import')(QCDPDF[-1])
  getattr(workspace,'import')(TTJPDF[-1])
  getattr(workspace,'import')(QCD_Norm[-1])
  getattr(workspace,'import')(TTJ_Norm[-1])
  getattr(workspace,'import')(RooObsHist[-1], RooFit.Rename("data_obsCAT" + str(k)))

workspace.Print()
workspace.writeToFile("ttH-shapes.root")

datacard = open("datacard_ttH_QCDfrom" + TEMPLATE + "_NCAT_" + NCAT + ".txt","w")
datacard.write("imax "  + str(NCAT) + "\n")
datacard.write("jmax *" + "\n")
datacard.write("kmax *" + "\n")
datacard.write(7*12*"-" + "\n")
datacard.write("shapes *  * " + outf.GetName() + " w:$PROCESS$CHANNEL" + "\n")
datacard.write(7*12*"-" + "\n")
datacard.write("{:<12}".format("bin"))

cat_name = 0
for k in NCAT:
  name = "CAT"+str(k) + " "
  datacard.write("{:<12}".format(name))

datacard.write("\n")
datacard.write("{:<12}".format("observation "))

for k in NCAT:
  nevents_observed = hData[k].Integral()
  print "Number of events observed: ", nevents_observed
  datacard.write("{:<12}".format(str(nevents_observed)))

datacard.write("\n")
datacard.write(7*12*"-" + "\n")
datacard.write("{:<12}".format("bin"))

for k in NCAT:
  name = "{:>12} {:>12} {:>12}".format("CAT" + str(k), "CAT" + str(k), "CAT" + str(k))
  datacard.write(name)

datacard.write("\n")
datacard.write("{:<12}".format("process"))

for k in NCAT:
  datacard.write("{:>12} {:>12} {:>12}".format("signal", "ttjets", "qcd"))

datacard.write("\n")
datacard.write("{:<12}".format("process"))

for k in NCAT:
  datacard.write("{:>12} {:>12} {:>12}".format("0", "1", "2"))

datacard.write("\n")
datacard.write("{:<12}".format("rate"))

for k in NCAT:
  rate1 = h[0][k].Integral()
  rate2 = h[1][k].Integral()
  rate3 = hQCD[k].Integral()

  datacard.write("{:>12.3f} {:>12.3f} {:>12.3f}".format(rate1, rate2, rate3))

datacard.write("\n")

datacard.write(7*12*"-" + "\n")

datacard.write("{:<8} {:<4}".format("QCD_Norm", "lnU"))
for k in NCAT:
  datacard.write("{:>12} {:>12} {:>12}".format("-", "-", "1.5"))

datacard.write("\n")

datacard.write("{:<8} {:<4}".format("TTJ_Norm", "lnU"))
for k in NCAT:
  datacard.write("{:>12} {:>12} {:>12}".format("-", "1.5", "-"))

datacard.close()
