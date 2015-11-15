#!usr/bin/python
import ROOT, os, re
from ROOT import TH1F, TCanvas, TFile, gROOT, TTree, TCut, TMath, RooRealVar, RooDataHist, RooArgList, RooArgSet, RooAddPdf, RooFit, RooGenericPdf, RooWorkspace, RooMsgService, RooHistPdf
import optparse

parser = optparse.OptionParser()
parser.add_option("--lumi",     action = "store", type = "float", dest = "LUMI",     default = 1000.)
parser.add_option("--ncat",     action = "store", type = "string",   dest = "NCAT",     default = "0,1")
parser.add_option("--template", action = "store", type = "str",   dest = "TEMPLATE", default = 'MC') # if TEMPLATE option is MC then QCD MC samples are used to extract QCD background shape on BDT variable, otherwise the data with loose b-tagging will be used.
parser.add_option("--on_EOS", action = "store", type = "int",   dest = "on_EOS", default = False)

(options, args) = parser.parse_args()

LUMI     = options.LUMI
NCAT     = [int(i) for i in options.NCAT.split(',')]
TEMPLATE = options.TEMPLATE
on_EOS   = options.on_EOS

print 'lumi:', LUMI, ' ncat:', NCAT, ' template:', TEMPLATE, " on_EOS:", on_EOS

gROOT.ForceStyle()

if TEMPLATE == 'MC':
    FileName = ["ttHJetTobb_M125", "TT", "WWTo4Q", "ZZTo4Q", "WZ", "TTWJetsToQQ", "TTZToQQ", "ZJetsToQQ", "WJetsToQQ", "QCD_HT200to300", "QCD_HT300to500", "QCD_HT500to700", "QCD_HT700to1000", "QCD_HT1000to1500", "QCD_HT1500to2000", "QCD_HT2000toInf"]

else:
    FileName = ["ttHJetTobb_M125", "TT", "WWTo4Q", "ZZTo4Q", "WZ", "TTWJetsToQQ", "TTZToQQ", "ZJetsToQQ", "WJetsToQQ", "JetHT"]

ttH_xsec              = 5.09e-01 #pb #https://twiki.cern.ch/twiki/bin/view/LHCPhysics/CERNYellowReportPageAt1314TeV
Hbb_BR                = 5.77e-01 #pb #https://twiki.cern.ch/twiki/bin/view/LHCPhysics/CERNYellowReportPageBR3
ttbar_xsec            = 8.32e+02 #pb # https://twiki.cern.ch/twiki/bin/view/LHCPhysics/TtbarNNLO
QCD_HT200to300_xsec   = 1.74e+06 #pb
QCD_HT300to500_xsec   = 3.67e+05 #pb
QCD_HT500to700_xsec   = 2.94e+04 #pb
QCD_HT700to1000_sec   = 6.52e+03 #pb
QCD_HT1000to1500_xsec = 1.06e+03 #pb
QCD_HT1500to2000_xsec = 1.22e+02 #pb
QCD_HT2000toInf_xsec  = 2.54e+01 #pb
WWTo4Q                = 5.172+01 #pb
ZZTo4Q                = 2.229+01 #pb
WZ                    = 4.713+01 #pb
TTWJetsToQQ           = 4.062-01 #pb
TTZToQQ               = 5.297-01 #pb
ZJetsToQQ             = 1.468+01 #pb
WJetsToQQ             = 9.514+01 #pb

XSEC = [ttH_xsec*Hbb_BR, ttbar_xsec, WWTo4Q, ZZTo4Q, WZ, TTWJetsToQQ, TTZToQQ, ZJetsToQQ, WJetsToQQ, QCD_HT200to300_xsec, QCD_HT300to500_xsec, QCD_HT500to700_xsec, QCD_HT700to1000_sec, QCD_HT1000to1500_xsec, QCD_HT1500to2000_xsec, QCD_HT2000toInf_xsec]

print FileName, XSEC

inf   = []
tr    = []
h     = [[] for x in range(len(FileName))]
hData = []

if on_EOS:
    dataf = TFile.Open("root://eoscms//eos/cms/store/cmst3/user/kkousour/ttH/flat/flatTree_JetHT.root")
else:
  dataf = TFile.Open("flatTree_JetHT.root")

CUT   = [TCut("ht>450 && jetPt[5]>40 && nBJets==2 && mva > -0.8"), TCut("ht>450 && jetPt[5]>40 && nBJets>2 && mva > -0.8")]

can = TCanvas("Canvas0","Canvas0", 600, 600)
can.Divide(2,1)

for i in xrange(len(FileName)):
  if on_EOS:
    inf.append(TFile.Open("root://eoscms//eos/cms/store/cmst3/user/kkousour/ttH/flat/flatTree_" + FileName[i] + ".root"))
  else:
    inf.append(TFile.Open("flatTree_" + FileName[i] + ".root"))

  print "flatTree_" + FileName[i] + ".root\t"
  
  if TEMPLATE == "MC":
    tr.append(inf[-1].Get("hadtop/events"))
    hpu = inf[-1].Get("hadtop/pileup")
  else:
    if i == 9: # gets the qcd shape from hadtopL directory
      tr.append(inf[-1].Get("hadtopL/events"))
      hpu = inf[-1].Get("hadtopL/pileup")
    else: # gets the other shapes from hadtop directory again
      tr.append(inf[-1].Get("hadtop/events"))
      hpu = inf[-1].Get("hadtop/pileup")

  for k in NCAT:
    can.cd(k + 1)
    print i, k
    h[i].append(TH1F("h" + str(i) + str(k), "h" + str(i) + str(k), 25,-1,1))
    h[i][-k].Sumw2()
    print CUT[k], tr[i].GetName()
    tr[i].Draw("mva>>"  +  "h" + str(i) + str(k), CUT[k])
    h[i][-k].SetLineWidth(2)
    h[i][-k].SetLineColor(i)
    if not (TEMPLATE == "DATA" and i == 9):
      h[i][-k].Scale(LUMI*XSEC[i]/hpu.GetEntries())
      print hpu.GetEntries()

hQCD = []

for k in NCAT:
  can.cd(k + 1)
  if TEMPLATE == "MC":
    hQCD.append(h[9][-k].Clone("hQCD" + str(k)))
    hQCDk.Add(h[10][-k])
    hQCDk.Add(h[11][-k])
    hQCDk.Add(h[12][-k])
    hQCDk.Add(h[13][-k])
    hQCDk.Add(h[14][-k])
    hQCDk.Add(h[15][-k])
  else:
    hQCD.append(h[9][-k].Clone("hQCD" + str(k)))

  h[0][-k].SetFillColor(ROOT.kRed-10)
  h[1][-k].SetFillColor(ROOT.kGreen-10)
  hQCD[-k].SetFillColor(ROOT.kBlue-10)
  h[0][-k].SetFillStyle(3001)
  h[1][-k].SetFillStyle(3001)

  print "QCD events:        ", hQCD[-k].Integral()
  print "TTbar events:      ", h[1][-k].Integral()
  print "WWTo4Q events:     ", h[2][-k].Integral()
  print "ZZTo4Q events:     ", h[3][-k].Integral()
  print "WZ events:         ", h[4][-k].Integral()
  print "TTWJetsToQQ events:", h[5][-k].Integral()
  print "TTZToQQ events:    ", h[6][-k].Integral()
  print "ZJetsToQQ events:  ", h[7][-k].Integral()
  print "WJetsToQQ events:  ", h[8][-k].Integral()
  print "TTH events:        ", h[0][-k].Integral()

  max1 = max(h[1][-k].GetBinContent(h[1][-k].GetMaximumBin()), hQCD[-k].GetBinContent(hQCD[-k].GetMaximumBin()))
  max2 = max(h[0][-k].GetBinContent(h[0][-k].GetMaximumBin()), max1)

  hQCD[-k].SetMaximum(1.1*max2)
  hQCD[-k].Draw("hist")
  h[0][-k].Draw("same hist")
  h[1][-k].Draw("same hist")

  hQCD[-k].SetName("qcdCAT"      + str(k))

  h[0][-k].SetName("signalCAT"   + str(k))

  h[1][-k].SetName("ttjetsCAT"   + str(k))

  dataf.cd()
  hData.append(TH1F("data_obsCAT" + str(k), "data_obsCAT" + str(k), 25, -1, 1))
  datatree = dataf.Get("hadtop/events")
  hpu = dataf.Get("hadtop/pileup")
  datatree.Draw("mva>>"  +  hData[-k].GetName(), CUT[k])

workspace   = RooWorkspace("w","workspace")
RooSigHist         = []
MVA                = []
RooObsHist         = []
RooSigHist         = []
RooQCDHist         = []
RooTTJHist         = []
RooWWTo4QHist      = []
RooZZTo4QHist      = []
RooWZHist          = []
RooTTWJetsToQQHist = []
RooTTZToQQHist     = []
RooZJetsToQQHist  = []
RooWJetsToQQHist   = []

ObsPDF      = []
SignalPDF   = []
QCDPDF      = []
TTJPDF      = []
WWTo4QPDF      = []
ZZTo4QPDF      = []
WZPDF          = []
TTWJetsToQQPDF = []
TTZToQQPDF     = []
ZJetsToQQPDF  = []
WJetsToQQPDF   = []

QCD_Norm        = []
TTJ_Norm        = []
WWTo4Q_Norm     = []
ZZTo4Q_Norm     = []
WZ_Norm         = []
TTZToQQ_Norm    = []
ZJetsToQQ_Norm = []
WJetsToQQ_Norm  = []

for k in NCAT:

  MVA.append(RooRealVar("mva","mva", -1, 1))

  RooObsHist.append(RooDataHist(        "RooObsHist",         "RooObsHist",         RooArgList(MVA[-1]), hData[-k]))
  RooSigHist.append(RooDataHist(        "RooSigHist",         "RooSigHist",         RooArgList(MVA[-1]), h[0][-k]))
  RooQCDHist.append(RooDataHist(        "RooQCDHist",         "RooQCDHist",         RooArgList(MVA[-1]), hQCD[-k]))
  RooTTJHist.append(RooDataHist(        "RooTTJHist",         "RooTTJHist",         RooArgList(MVA[-1]), h[1][-k]))
  RooWWTo4QHist.append(RooDataHist(     "RooWWTo4QHist",      "RooWWTo4QHist",      RooArgList(MVA[-1]), h[2][-k]))
  RooZZTo4QHist.append(RooDataHist(     "RooZZTo4QHist",      "RooZZTo4QHist",      RooArgList(MVA[-1]), h[3][-k]))
  RooWZHist.append(RooDataHist(         "RooWZHist",          "RooWZHist",          RooArgList(MVA[-1]), h[4][-k]))
  RooTTWJetsToQQHist.append(RooDataHist("RooTTWJetsToQQHist", "RooTTWJetsToQQHist", RooArgList(MVA[-1]), h[5][-k]))
  RooTTZToQQHist.append(RooDataHist(    "RooTTZToQQHist",     "RooTTZToQQHist",     RooArgList(MVA[-1]), h[6][-k]))
  RooZJetsToQQHist.append(RooDataHist(  "RooZJetsToQQHist",   "RooZJetsToQQHist",   RooArgList(MVA[-1]), h[7][-k]))
  RooWJetsToQQHist.append(RooDataHist(  "RooWJetsToQQHist",   "RooWJetsToQQHist",   RooArgList(MVA[-1]), h[8][-k]))

  RooObsHist[-1].Print()

  SignalPDF.append(RooHistPdf(     "signalCAT"      + str(k), "signalCAT"      + str(k), RooArgSet(MVA[-1]), RooSigHist[-1]))
  QCDPDF.append(RooHistPdf(        "QCDCAT"         + str(k), "QCDCAT"         + str(k), RooArgSet(MVA[-1]), RooQCDHist[-1]))
  TTJPDF.append(RooHistPdf(        "ttjetsCAT"      + str(k), "ttjetsCAT"      + str(k), RooArgSet(MVA[-1]), RooTTJHist[-1]))
  WWTo4QPDF.append(RooHistPdf(     "WWTo4QCAT"      + str(k), "WWTo4QCAT"      + str(k), RooArgSet(MVA[-1]), RooWWTo4QHist[-1]))
  ZZTo4QPDF.append(RooHistPdf(     "ZZTo4QCAT"      + str(k), "ZZTo4QCAT"      + str(k), RooArgSet(MVA[-1]), RooZZTo4QHist[-1]))
  WZPDF.append(RooHistPdf(         "WZCAT"          + str(k), "WZCAT"          + str(k), RooArgSet(MVA[-1]), RooWZHist[-1]))
  TTWJetsToQQPDF.append(RooHistPdf("TTWJetsToQQCAT" + str(k), "TTWJetsToQQCAT" + str(k), RooArgSet(MVA[-1]), RooTTWJetsToQQHist[-1]))
  TTZToQQPDF.append(RooHistPdf(    "TTZToQQCAT"     + str(k), "TTZToQQCAT"     + str(k), RooArgSet(MVA[-1]), RooTTZToQQHist[-1]))
  ZJetsToQQPDF.append(RooHistPdf(  "ZJetsToQQCAT"   + str(k), "ZJetsToQQCAT"   + str(k), RooArgSet(MVA[-1]), RooZJetsToQQHist[-1]))
  WJetsToQQPDF.append(RooHistPdf(  "WJetsToQQCAT"   + str(k), "WJetsToQQCAT"   + str(k), RooArgSet(MVA[-1]), RooWJetsToQQHist[-1]))

  QCD_Norm.append(RooRealVar(       "QCD_Norm"       + str(k), "QCD_Norm"        + str(k), 0, -1e+04, 1e+04))
  TTJ_Norm.append(RooRealVar(       "TTJ_Norm"       + str(k), "TTJ_Norm"        + str(k), 0, -1e+04, 1e+04))
  WWTo4Q_Norm.append(RooRealVar(    "WWTo4Q_Norm"    + str(k), "WWTo4Q_Norm"     + str(k), 0, -1e+04, 1e+04))
  ZZTo4Q_Norm.append(RooRealVar(    "ZZTo4Q_Norm"    + str(k), "ZZTo4Q_Norm"     + str(k), 0, -1e+04, 1e+04))
  WZ_Norm.append(RooRealVar(        "WZ_Norm"        + str(k), "WZ_Norm"         + str(k), 0, -1e+04, 1e+04))
  TTZToQQ_Norm.append(RooRealVar(   "TTZToQQ_Norm"   + str(k), "TTZToQQ_Norm"    + str(k), 0, -1e+04, 1e+04))
  ZJetsToQQ_Norm.append(RooRealVar( "ZJetsToQQ_Norm" + str(k), "ZJetsToQQ_Norm"  + str(k), 0, -1e+04, 1e+04))
  WJetsToQQ_Norm.append(RooRealVar( "WJetsToQQ_Norm" + str(k), "WJetsToQQ_Norm"  + str(k), 0, -1e+04, 1e+04))

  getattr(workspace,'import')(SignalPDF[-1])
  getattr(workspace,'import')(QCDPDF[-1])
  getattr(workspace,'import')(TTJPDF[-1])
  getattr(workspace,'import')(WWTo4QPDF[-1])
  getattr(workspace,'import')(ZZTo4QPDF[-1])
  getattr(workspace,'import')(WZPDF[-1])
  getattr(workspace,'import')(TTWJetsToQQPDF[-1])
  getattr(workspace,'import')(TTZToQQPDF[-1])
  getattr(workspace,'import')(ZJetsToQQPDF[-1])
  getattr(workspace,'import')(WJetsToQQPDF[-1])  

  getattr(workspace,'import')(QCD_Norm[-1])
  getattr(workspace,'import')(TTJ_Norm[-1])
  getattr(workspace,'import')(WWTo4Q_Norm[-1])
  getattr(workspace,'import')(ZZTo4Q_Norm[-1])
  getattr(workspace,'import')(WZ_Norm[-1])
  getattr(workspace,'import')(TTZToQQ_Norm[-1])
  getattr(workspace,'import')(ZJetsToQQ_Norm[-1])
  getattr(workspace,'import')(WJetsToQQ_Norm[-1])

  getattr(workspace,'import')(RooObsHist[-1], RooFit.Rename("data_obsCAT" + str(k)))

workspace.Print()
workspace.writeToFile("ttH-shapes_" + TEMPLATE + "_NCAT" + str(NCAT).replace("[","_").replace("]","").replace(",", "_").replace(" ", "") + ".root")

datacard = open("datacard_ttH_QCDfrom" + TEMPLATE + "_NCAT" + str(NCAT).replace("[","_").replace("]","").replace(",", "_").replace(" ", "") + ".txt","w")
datacard.write("imax "  + str(len(NCAT)) + "\n")
datacard.write("jmax *" + "\n")
datacard.write("kmax *" + "\n")
datacard.write((10*len(NCAT)+1)*18*"-" + "\n")
datacard.write("shapes *  * " + "ttH-shapes_" + TEMPLATE + "_NCAT" + str(NCAT).replace("[","_").replace("]","").replace(",", "_").replace(" ", "") + ".root" + " w:$PROCESS$CHANNEL" + "\n")
datacard.write((10*len(NCAT)+1)*18*"-" + "\n")
datacard.write("{:<18}".format("bin"))

cat_name = 0
for k in NCAT:
  name = "CAT"+str(k)
  datacard.write("{:>18}".format(name))

datacard.write("\n")
datacard.write("{:<18}".format("observation "))

for k in NCAT:
  nevents_observed = hData[-k].Integral()
  print "Number of events observed: ", nevents_observed
  datacard.write("{:>18}".format(str(nevents_observed)))

datacard.write("\n")
datacard.write((10*len(NCAT)+1)*18*"-" + "\n")
datacard.write("{:<18}".format("bin"))

for k in NCAT:
  name = "{:>18}{:>18}{:>18}{:>18}{:>18}{:>18}{:>18}{:>18}{:>18}{:>18}".format("CAT" + str(k), "CAT" + str(k), "CAT" + str(k), "CAT" + str(k), "CAT" + str(k), "CAT" + str(k), "CAT" + str(k), "CAT" + str(k), "CAT" + str(k), "CAT" + str(k))
  datacard.write(name)
datacard.write("\n")
datacard.write("{:<18}".format("process"))

for k in NCAT:
  datacard.write("{:>18}{:>18}{:>18}{:>18}{:>18}{:>18}{:>18}{:>18}{:>18}{:>18}".format("signal", "ttjets", "WWTo4Q", "ZZTo4Q", "WZ", "TTWJetsToQQ", "TTZToQQ", "ZJetsToQQ", "WJetsToQQ", "QCD"))
datacard.write("\n")
datacard.write("{:<18}".format("process"))

for k in NCAT:
  datacard.write("{:>18}{:>18}{:>18}{:>18}{:>18}{:>18}{:>18}{:>18}{:>18}{:>18}".format("0", "1", "2", "3", "4", "5", "6", "7", "8", "9"))
datacard.write("\n")

datacard.write("{:<18}".format("rate"))

for k in NCAT:
  rate1 = h[0][-k].Integral()
  rate2 = h[1][-k].Integral()
  rate3 = h[2][-k].Integral()
  rate4 = h[3][-k].Integral()
  rate5 = h[4][-k].Integral()
  rate6 = h[5][-k].Integral()
  rate7 = h[6][-k].Integral()
  rate8 = h[7][-k].Integral()
  rate9 = h[8][-k].Integral()
  rate10 = hQCD[-k].Integral()

  datacard.write("{:>18.3f}{:>18.3f}{:>18.3f}{:>18.3f}{:>18.3f}{:>18.3f}{:>18.3f}{:>18.3f}{:>18.3f}{:>18.3f}".format(rate1, rate2, rate3, rate4, rate5, rate6, rate7, rate8, rate9, rate10))

datacard.write("\n")

datacard.write((10*len(NCAT)+1)*18*"-" + "\n")

for i in FileName[1:]:
  if TEMPLATE == "DATA":
    datacard.write("{:<14}{:<4}".format(i, "lnU"))
    for j  in NCAT:
      for k in xrange(len(FileName)):
        if k!=FileName.index(i):
          datacard.write("{:>18}".format("-"))
        else:
          datacard.write("{:>18}".format("1.5"))
  else:
    if i < 9:
      datacard.write("{:<14}{:<4}".format(i, "lnU"))
      for j  in NCAT:
        for k in xrange(len(FileName)-7):
          if k!=FileName.index(i):
            datacard.write("{:>18}".format("-"))
          else:
            datacard.write("{:>18}".format("1.5"))
    else:
      datacard.write("{:<14}{:<4}".format("QCD", "lnU"))
      for j  in NCAT:
        for k in xrange(len(FileName)-7):
          if k!=FileName.index(i):
            datacard.write("{:>18}".format("-"))
          else:
            datacard.write("{:>18}".format("1.5"))
  datacard.write("\n")


datacard.close()
