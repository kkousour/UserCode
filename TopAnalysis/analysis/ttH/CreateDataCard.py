#!usr/bin/python
import ROOT, os, re
from ROOT import TH1F, TCanvas, TFile, gROOT, TTree, TCut, TMath, RooRealVar, RooDataHist, RooArgList, RooArgSet, RooAddPdf, RooFit, RooGenericPdf, RooWorkspace, RooMsgService, RooHistPdf
import optparse

parser = optparse.OptionParser()
parser.add_option("--lumi",     action = "store", type = "float",  dest = "LUMI",     default = 1000.)
parser.add_option("--ncat",     action = "store", type = "string", dest = "NCAT",     default = "0,1")
parser.add_option("--merge_VV", action = "store", type = "int",    dest = "merge_VV", default = 1)
parser.add_option("--template", action = "store", type = "str",    dest = "TEMPLATE", default = 'MC') # if TEMPLATE option is MC then QCD MC samples are used to extract QCD background shape on BDT variable, otherwise the data with loose b-tagging will be used.
parser.add_option("--on_EOS", action = "store", type = "int",   dest = "on_EOS", default = False)

(options, args) = parser.parse_args()

LUMI     = options.LUMI
NCAT     = [int(i) for i in options.NCAT.split(',')]
TEMPLATE = options.TEMPLATE
on_EOS   = options.on_EOS
merge_VV = options.merge_VV

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
WWTo4Q                = 5.17e+01 #pb
ZZTo4Q                = 2.23e+01 #pb
WZ                    = 4.71e+01 #pb
TTWJetsToQQ           = 4.06e-01 #pb
TTZToQQ               = 5.30e-01 #pb
ZJetsToQQ             = 1.47e+01 #pb
WJetsToQQ             = 9.51e+01 #pb

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

CUT1   = [TCut("ht>450 && jetPt[5]>40 && nBJets==2 && nLeptons == 0 && mva > -0.8 && (triggerBit[0] || triggerBit[2])"), TCut("ht>450 && jetPt[5]>40 && nBJets>2 && nLeptons == 0 && mva > -0.8 && (triggerBit[0] || triggerBit[2])")]

CUT2   = [TCut("ht>450 && jetPt[5]>40 && nBJets==2 && nLeptons == 0 && mva > -0.8 && (triggerBit[1] || triggerBit[3])"), TCut("ht>450 && jetPt[5]>40 && nBJets>2 && nLeptons == 0 && mva > -0.8 && (triggerBit[1] || triggerBit[3])")]

can = TCanvas("Canvas0","Canvas0", 600, 600)
can.Divide(2,1)

for i in xrange(len(FileName)):
  if on_EOS:
    inf.append(TFile.Open("root://eoscms//eos/cms/store/cmst3/user/kkousour/ttH/flat/flatTree_" + FileName[i] + ".root"))
  else:
    inf.append(TFile.Open("flatTree_" + FileName[i] + ".root"))

  print "flatTree_" + FileName[i] + ".root\t", XSEC[i]
  
  if TEMPLATE == "MC":
    tr.append(inf[-1].Get("hadtop/events"))
    hpu = inf[-1].Get("hadtop/pileup")
  else:
    if i == 9: # gets the qcd shape from hadtopL directory
      tr.append(inf[-1].Get("hadtopL/events"))
      hpu = inf[-1].Get("hadtopL/pileup")
      h_event_norm = inf[-1].Get("hadtopL/TriggerPass")
      n_event_norm = h_event_norm.GetBinContent(1)
    else: # gets the other shapes from hadtop directory again
      tr.append(inf[-1].Get("hadtop/events"))
      hpu = inf[-1].Get("hadtop/pileup")

  for k in xrange(len(NCAT)):
    can.cd(k + 1)
    print i, k
    h[i].append(TH1F("h" + str(i) + str(NCAT[k]), "h" + str(i) + str(NCAT[k]), 25,-1,1))
    h[i][k].Sumw2()
    print CUT1[k], tr[i].GetName()
    
    if not (TEMPLATE == "DATA" and i == 9):
      tr[i].Draw("mva>>"  +  "h" + str(i) + str(NCAT[k]), CUT1[k])
      h[i][k].SetLineWidth(2)
      h[i][k].SetLineColor(i)
      h[i][k].Scale(LUMI*XSEC[i]/hpu.GetEntries())
      print hpu.GetEntries()
    else:
      tr[i].Draw("mva>>"  +  "h" + str(i) + str(NCAT[k]), CUT2[k])
      h[i][k].SetLineWidth(2)
      h[i][k].SetLineColor(i)    
      #number_of_gen  = 18459215.0 +  20086103.0 + 19478201.0 + 15011016.0 + 4717789.0 + 3404178.0 + 1865667.0
      #total_qcd_xsec = sum(XSEC[9:])
      #h[i][k].Scale(LUMI*total_qcd_xsec/n_event_norm)
      #print "n_event_norm", n_event_norm, LUMI*total_qcd_xsec, h[i][k].Integral()

hQCD = []
hVV  = []

for k in xrange(len(NCAT)):
  can.cd(k + 1)
  if TEMPLATE == "MC":
    hQCD.append(h[9][k].Clone("hQCD" + str(NCAT[k])))
    hQCD[k].Add(h[10][k])
    hQCD[k].Add(h[11][k])
    hQCD[k].Add(h[12][k])
    hQCD[k].Add(h[13][k])
    hQCD[k].Add(h[14][k])
    hQCD[k].Add(h[15][k])
  else:
    hQCD.append(h[9][k].Clone("hQCD" + str(NCAT[k])))


  print "QCD events:        ", hQCD[k].Integral()
  print "TTbar events:      ", h[1][k].Integral()
  print "WWTo4Q events:     ", h[2][k].Integral()
  print "ZZTo4Q events:     ", h[3][k].Integral()
  print "WZ events:         ", h[4][k].Integral()
  print "TTWJetsToQQ events:", h[5][k].Integral()
  print "TTZToQQ events:    ", h[6][k].Integral()
  print "ZJetsToQQ events:  ", h[7][k].Integral()
  print "WJetsToQQ events:  ", h[8][k].Integral()
  print "TTH events:        ", h[0][k].Integral()

  dataf.cd()
  hData.append(TH1F("data_obsCAT" + str(NCAT[k]), "data_obsCAT" + str(NCAT[k]), 25, -1, 1))
  datatree = dataf.Get("hadtop/events")
  hpu = dataf.Get("hadtop/pileup")
  datatree.Draw("mva>>"  +  hData[k].GetName(), CUT1[k])

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
RooZJetsToQQHist   = []
RooWJetsToQQHist   = []

ObsPDF             = []
SignalPDF          = []
QCDPDF             = []
TTJPDF             = []
WWTo4QPDF          = []
ZZTo4QPDF          = []
WZPDF              = []
TTWJetsToQQPDF     = []
TTZToQQPDF         = []
ZJetsToQQPDF       = []
WJetsToQQPDF       = []
VVPDF              = []

QCD_Norm           = []
TTJ_Norm           = []
WWTo4Q_Norm        = []
ZZTo4Q_Norm        = []
WZ_Norm            = []
TTZToQQ_Norm       = []
ZJetsToQQ_Norm     = []
WJetsToQQ_Norm     = []
VV_Norm            = []

for k in xrange(len(NCAT)):

  MVA.append(RooRealVar("mva","mva", -1, 1))

  RooObsHist.append(         RooDataHist("RooObsHist",         "RooObsHist",         RooArgList(MVA[-1]), hData[k]))
  RooSigHist.append(         RooDataHist("RooSigHist",         "RooSigHist",         RooArgList(MVA[-1]), h[0][k]))
  RooQCDHist.append(         RooDataHist("RooQCDHist",         "RooQCDHist",         RooArgList(MVA[-1]), hQCD[k]))
  RooTTJHist.append(         RooDataHist("RooTTJHist",         "RooTTJHist",         RooArgList(MVA[-1]), h[1][k]))
  RooWWTo4QHist.append(      RooDataHist("RooWWTo4QHist",      "RooWWTo4QHist",      RooArgList(MVA[-1]), h[2][k]))
  RooZZTo4QHist.append(      RooDataHist("RooZZTo4QHist",      "RooZZTo4QHist",      RooArgList(MVA[-1]), h[3][k]))
  RooWZHist.append(          RooDataHist("RooWZHist",          "RooWZHist",          RooArgList(MVA[-1]), h[4][k]))
  RooTTWJetsToQQHist.append( RooDataHist("RooTTWJetsToQQHist", "RooTTWJetsToQQHist", RooArgList(MVA[-1]), h[5][k]))
  RooTTZToQQHist.append(     RooDataHist("RooTTZToQQHist",     "RooTTZToQQHist",     RooArgList(MVA[-1]), h[6][k]))
  RooZJetsToQQHist.append(   RooDataHist("RooZJetsToQQHist",   "RooZJetsToQQHist",   RooArgList(MVA[-1]), h[7][k]))
  RooWJetsToQQHist.append(   RooDataHist("RooWJetsToQQHist",   "RooWJetsToQQHist",   RooArgList(MVA[-1]), h[8][k]))

  RooObsHist[-1].Print()

  SignalPDF.append(      RooHistPdf("signalCAT"      + str(NCAT[k]), "signalCAT"      + str(NCAT[k]), RooArgSet(MVA[-1]), RooSigHist[-1]))
  QCDPDF.append(         RooHistPdf("QCDCAT"         + str(NCAT[k]), "QCDCAT"         + str(NCAT[k]), RooArgSet(MVA[-1]), RooQCDHist[-1]))
  TTJPDF.append(         RooHistPdf("ttjetsCAT"      + str(NCAT[k]), "ttjetsCAT"      + str(NCAT[k]), RooArgSet(MVA[-1]), RooTTJHist[-1]))
  WWTo4QPDF.append(      RooHistPdf("WWTo4QCAT"      + str(NCAT[k]), "WWTo4QCAT"      + str(NCAT[k]), RooArgSet(MVA[-1]), RooWWTo4QHist[-1]))
  ZZTo4QPDF.append(      RooHistPdf("ZZTo4QCAT"      + str(NCAT[k]), "ZZTo4QCAT"      + str(NCAT[k]), RooArgSet(MVA[-1]), RooZZTo4QHist[-1]))
  WZPDF.append(          RooHistPdf("WZCAT"          + str(NCAT[k]), "WZCAT"          + str(NCAT[k]), RooArgSet(MVA[-1]), RooWZHist[-1]))
  TTWJetsToQQPDF.append( RooHistPdf("TTWJetsToQQCAT" + str(NCAT[k]), "TTWJetsToQQCAT" + str(NCAT[k]), RooArgSet(MVA[-1]), RooTTWJetsToQQHist[-1]))
  TTZToQQPDF.append(     RooHistPdf("TTZToQQCAT"     + str(NCAT[k]), "TTZToQQCAT"     + str(NCAT[k]), RooArgSet(MVA[-1]), RooTTZToQQHist[-1]))
  ZJetsToQQPDF.append(   RooHistPdf("ZJetsToQQCAT"   + str(NCAT[k]), "ZJetsToQQCAT"   + str(NCAT[k]), RooArgSet(MVA[-1]), RooZJetsToQQHist[-1]))
  WJetsToQQPDF.append(   RooHistPdf("WJetsToQQCAT"   + str(NCAT[k]), "WJetsToQQCAT"   + str(NCAT[k]), RooArgSet(MVA[-1]), RooWJetsToQQHist[-1]))

  if merge_VV == 1:
    VVPDF.append(RooAddPdf("VVCAT"   + str(NCAT[k]), "VVCAT"   + str(NCAT[k]), RooArgList(WZPDF[-1], ZZTo4QPDF[-1], WWTo4QPDF[-1])))


  QCD_Norm.append(       RooRealVar("QCD_Norm_CAT"       + str(NCAT[k]), "QCD_Norm_CAT"       + str(NCAT[k]), 0, -1e+04, 1e+04))
  TTJ_Norm.append(       RooRealVar("TTJ_Norm_CAT"       + str(NCAT[k]), "TTJ_Norm_CAT"       + str(NCAT[k]), 0, -1e+04, 1e+04))
  WWTo4Q_Norm.append(    RooRealVar("WWTo4Q_Norm_CAT"    + str(NCAT[k]), "WWTo4Q_Norm_CAT"    + str(NCAT[k]), 0, -1e+04, 1e+04))
  ZZTo4Q_Norm.append(    RooRealVar("ZZTo4Q_Norm_CAT"    + str(NCAT[k]), "ZZTo4Q_Norm_CAT"    + str(NCAT[k]), 0, -1e+04, 1e+04))
  WZ_Norm.append(        RooRealVar("WZ_Norm_CAT"        + str(NCAT[k]), "WZ_Norm_CAT"        + str(NCAT[k]), 0, -1e+04, 1e+04))
  TTZToQQ_Norm.append(   RooRealVar("TTZToQQ_Norm_CAT"   + str(NCAT[k]), "TTZToQQ_Norm_CAT"   + str(NCAT[k]), 0, -1e+04, 1e+04))
  ZJetsToQQ_Norm.append( RooRealVar("ZJetsToQQ_Norm_CAT" + str(NCAT[k]), "ZJetsToQQ_Norm_CAT" + str(NCAT[k]), 0, -1e+04, 1e+04))
  WJetsToQQ_Norm.append( RooRealVar("WJetsToQQ_Norm_CAT" + str(NCAT[k]), "WJetsToQQ_Norm_CAT" + str(NCAT[k]), 0, -1e+04, 1e+04))
  VV_Norm.append(        RooRealVar("VV_Norm_CAT"        + str(NCAT[k]), "VV_Norm_CAT"        + str(NCAT[k]), 0, -1e+04, 1e+04))

  getattr(workspace,'import')(SignalPDF[-1])
  getattr(workspace,'import')(QCDPDF[-1])
  getattr(workspace,'import')(TTJPDF[-1])

  if merge_VV == 1:
    getattr(workspace,'import')(VVPDF[-1])
  else:
    getattr(workspace,'import')(WWTo4QPDF[-1])
    getattr(workspace,'import')(ZZTo4QPDF[-1])
    getattr(workspace,'import')(WZPDF[-1])
    

  getattr(workspace,'import')(TTWJetsToQQPDF[-1])
  getattr(workspace,'import')(TTZToQQPDF[-1])
  getattr(workspace,'import')(ZJetsToQQPDF[-1])
  getattr(workspace,'import')(WJetsToQQPDF[-1])  

  getattr(workspace,'import')(QCD_Norm[-1])
  getattr(workspace,'import')(TTJ_Norm[-1])

  if merge_VV == 1:
    getattr(workspace,'import')(VV_Norm[-1])
  else:
    getattr(workspace,'import')(WWTo4Q_Norm[-1])
    getattr(workspace,'import')(ZZTo4Q_Norm[-1])
    getattr(workspace,'import')(WZ_Norm[-1])

  getattr(workspace,'import')(TTZToQQ_Norm[-1])
  getattr(workspace,'import')(ZJetsToQQ_Norm[-1])
  getattr(workspace,'import')(WJetsToQQ_Norm[-1])

  getattr(workspace,'import')(RooObsHist[-1], RooFit.Rename("data_obsCAT" + str(NCAT[k])))

workspace.Print()
workspace.writeToFile("ttH-shapes_" + TEMPLATE + "_NCAT" + str(NCAT).replace("[","_").replace("]","").replace(",", "_").replace(" ", "") + ".root")

format_width = 25
col_format = "{:<" + str(format_width) + "}"
if merge_VV == 1:
    bkg_list = ["ttjets", "VV", "TTWJetsToQQ", "TTZToQQ", "ZJetsToQQ", "WJetsToQQ", "QCD"] 
else:
    bkg_list = ["ttjets", "WWTo4Q", "ZZTo4Q", "WZ", "TTWJetsToQQ", "TTZToQQ", "ZJetsToQQ", "WJetsToQQ", "QCD"] 

datacard = open("datacard_ttH_QCDfrom" + TEMPLATE + "_NCAT" + str(NCAT).replace("[","_").replace("]","").replace(",", "_").replace(" ", "") + ".txt","w")
datacard.write("imax "  + str(len(NCAT)) + "\n")
datacard.write("jmax *" + "\n")
datacard.write("kmax *" + "\n")
datacard.write(((len(bkg_list)+1)*len(NCAT)+1)*format_width*"-" + "\n")
datacard.write("shapes *  * " + "ttH-shapes_" + TEMPLATE + "_NCAT" + str(NCAT).replace("[","_").replace("]","").replace(",", "_").replace(" ", "") + ".root" + " w:$PROCESS$CHANNEL" + "\n")
datacard.write(((len(bkg_list)+1)*len(NCAT)+1)*format_width*"-" + "\n")
datacard.write(col_format.format("bin"))

for k in xrange(len(NCAT)):
  name = "CAT"+str(NCAT[k])
  datacard.write(col_format.format(name))

datacard.write("\n")
datacard.write(col_format.format("observation "))

for k in xrange(len(NCAT)):
  nevents_observed = hData[k].Integral()
  print "Number of events observed: ", nevents_observed
  datacard.write(col_format.format(str(nevents_observed)))

datacard.write("\n")
datacard.write(((len(bkg_list)+1)*len(NCAT)+1)*format_width*"-" + "\n")
datacard.write(col_format.format("bin"))

for k in xrange(len(NCAT)):
  specific_format = col_format.replace("<",">")
  for j in xrange(len(bkg_list)+1):
    datacard.write(specific_format.format("CAT" + str(NCAT[k])))
datacard.write("\n")

col_format = col_format.replace(">","<")
datacard.write(col_format.format("process"))
col_format = col_format.replace("<",">")

for k in xrange(len(NCAT)):
    datacard.write(specific_format.format("signal"))
    for j in xrange(len(bkg_list)):
        datacard.write(col_format.format(bkg_list[j]))
datacard.write("\n")

col_format = col_format.replace(">","<")
datacard.write(col_format.format("process"))
col_format = col_format.replace("<",">")

for k in xrange(len(NCAT)):
  specific_format = col_format
  datacard.write(specific_format.format("0"))
  for j in xrange(len(bkg_list)):
    datacard.write(specific_format.format(str(bkg_list.index(bkg_list[j])+1)))
datacard.write("\n")


col_format = col_format.replace(">","<")
datacard.write(col_format.format("rate"))


for k in xrange(len(NCAT)):
    rate = []
    rate.append(h[0][k].Integral())
    rate.append(h[1][k].Integral())
    if merge_VV == 1:
        rate.append(h[2][k].Integral() + h[3][k].Integral() + h[4][k].Integral())
    else:
        rate.append(h[2][k].Integral())
        rate.append(h[3][k].Integral())
        rate.append(h[4][k].Integral())
    rate.append(h[5][k].Integral())
    rate.append(h[6][k].Integral())
    rate.append(h[7][k].Integral())
    rate.append(h[8][k].Integral())
    rate.append(hQCD[k].Integral())
    specific_format = "{:>"+str(format_width)+".3f}"

    for j in xrange(len(bkg_list)+1):
        datacard.write(specific_format.format(rate[j]))

datacard.write("\n")

bkg_unc  = str(1.2)
qcd_unc  = str(1.5)
lumi_unc = str(1.1)
acc_jes  = str(1.1)

datacard.write(((len(bkg_list)+1)*len(NCAT)+1)*format_width*"-" + "\n")
col_format = col_format.replace("<",">")
specific_format = "{:<21}{:>4}"
datacard.write(specific_format.format("lumi","lnN"))
datacard.write(col_format.format(lumi_unc))
for j in xrange(len(bkg_list)):
    if bkg_list[j] != "QCD":
        datacard.write(col_format.format(lumi_unc))
    else:
        datacard.write(col_format.format("-"))

if len(NCAT) > 1:
    datacard.write(col_format.format(lumi_unc))
    for j in xrange(len(bkg_list)):
        if bkg_list[j] != "QCD":
            datacard.write(col_format.format(lumi_unc))
        else:
            datacard.write(col_format.format("-"))
datacard.write("\n")

specific_format = "{:<21}{:>4}"
datacard.write(specific_format.format("acc_jes","lnN"))
datacard.write(col_format.format(acc_jes))
for j in xrange(len(bkg_list)):
    if bkg_list[j] != "QCD":
        datacard.write(col_format.format(acc_jes))
    else:
        datacard.write(col_format.format("-"))
if len(NCAT) > 1:
    datacard.write(col_format.format(acc_jes))
    for j in xrange(len(bkg_list)):
        if bkg_list[j] != "QCD":
            datacard.write(col_format.format(acc_jes))
        else:
            datacard.write(col_format.format("-"))
datacard.write("\n")
datacard.write(((len(bkg_list)+1)*len(NCAT)+1)*format_width*"-" + "\n")

def write_parameter_info(name, category, type, i_par, n_par, uncertainty, width):
  col_format = "{:<" + str(width-4) + "}{:>" + str(4) + "}"
  print col_format
  datacard.write(col_format.format(name + "_Norm_" + category, type))
  col_format = "{:>" + str(width) + "}"
  datacard.write(col_format.format("-"))
  for i in xrange((len(NCAT))*n_par-1):
    if len(NCAT)>1:
      if category == "CAT0":
        if i==i_par:
          datacard.write(col_format.format(str(uncertainty)))
        else:
          datacard.write(col_format.format("-"))
      elif category == "CAT1":
          if i==i_par + n_par:
            datacard.write(col_format.format(str(uncertainty)))
          else:
            datacard.write(col_format.format("-"))
    else:
      if i==i_par:
        datacard.write(col_format.format(str(uncertainty)))
      else:
        datacard.write(col_format.format("-"))
  datacard.write("\n")


for i in NCAT:
  for j in bkg_list:
    if j != "QCD":
      write_parameter_info(j, "CAT"+str(i), "lnN", bkg_list.index(j), len(bkg_list)+1, 1.1, 25)
    else:
      write_parameter_info(j, "CAT"+str(i), "lnU", bkg_list.index(j), len(bkg_list)+1, 1.2, 25)

datacard.close()