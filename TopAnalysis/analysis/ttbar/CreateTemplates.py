#!usr/bin/python

import ROOT
from ROOT import TFile, TH1F, TF1, TCanvas, TPad, TString
from ROOT import gROOT, gPad 
from ROOT import RooRealVar, RooDataHist, RooPlot, RooArgList, RooArgSet, RooAddPdf, RooFit, RooWorkspace, RooMsgService, RooGaussian, RooFitResult, RooFormulaVar
from setTDRStyle import setTDRStyle

import optparse
usage = "usage: %prog [options]"
parser = optparse.OptionParser(usage)
parser.add_option("--xmin"  ,action="store",type="float" ,dest="xmin"  ,default=50)
parser.add_option("--xmax"  ,action="store",type="float" ,dest="xmax"  ,default=350)
parser.add_option("--rebin" ,action="store",type="int"   ,dest="rebin" ,default=5)
parser.add_option("--lumi"  ,action="store",type="float" ,dest="lumi"  ,default=1000)

(options, args) = parser.parse_args()

xmin   = options.xmin
xmax   = options.xmax
rebin  = options.rebin
lumi   = options.lumi

gROOT.Reset()
setTDRStyle()
gROOT.ForceStyle()
gROOT.SetStyle('tdrStyle')

RooMsgService.instance().setSilentMode(ROOT.kTRUE)
RooMsgService.instance().setStreamStatus(0,ROOT.kFALSE)
RooMsgService.instance().setStreamStatus(1,ROOT.kFALSE)

filename = [
   'Histo_TT.root',
   'Histo_JetHT.root'
]
norm        = []
nevents     = []
histo       = []
roohisto    = []
histoCtl    = []
roohistoCtl = []
# define observable
x = RooRealVar('mTop','mTop',xmin,xmax)

for ff in filename:
  inf = TFile.Open(ff)
  h = inf.Get('hadtopBoost/h_jetMassSoftDrop0_Cut_sig')
  norm.append((inf.Get('hadtopBoost/TriggerPass')).GetBinContent(1))
  nevents.append(h.Integral())
  print h.GetName()
  h.Rebin(rebin)  
  hCtl = inf.Get('hadtopBoost/h_jetMassSoftDrop0_Cut_ctl')  
  print hCtl.GetName()
  hCtl.Rebin(rebin)
  histo.append(h)
  histoCtl.append(hCtl)
  rooh = RooDataHist('roohist','roohist',RooArgList(x),h)
  roohisto.append(rooh)
  roohCtl = RooDataHist('roohistCtl','roohistCtl',RooArgList(x),hCtl)
  roohistoCtl.append(roohCtl)

ttbar_xsection = 832
signal_yield = nevents[0]*ttbar_xsection*lumi/norm[0]
YieldTT = RooRealVar("YieldTT","YieldTT",signal_yield)
print "Signal yield = "+str(signal_yield)

# define parameters for signal fit

kJES = RooRealVar("kJES","kJES",1,0.9,1.1)
kJER = RooRealVar("kJER","kJER",1,0.8,1.2)
kJES.setConstant(ROOT.kTRUE)
kJER.setConstant(ROOT.kTRUE)

m1 = RooRealVar('ttbar_mean1','ttbar_mean1',172,150,180)
s1 = RooRealVar('ttbar_sigma1','ttbar_sigma1',20,0,50)

m1Shift = RooFormulaVar('ttbar_shifted_mean1',"@0*@1",RooArgList(m1,kJES))
s1Shift = RooFormulaVar('ttbar_shifted_sigma1',"@0*@1",RooArgList(s1,kJER)); 

sig1 = RooGaussian('ttbar_gaus1','ttbar_gaus1',x,m1Shift,s1Shift)         

m2 = RooRealVar('ttbar_mean2' ,'ttbar_mean2',140,130,300)
s2 = RooRealVar('ttbar_sigma2','ttbar_sigma2',50,10,200)
sig2 = RooGaussian('ttbar_gaus2','ttbar_gaus2',x,m2,s2) 

fsig = RooRealVar('ttbar_f1','ttbar_f1',0.5,0,1)

signal = RooAddPdf('ttbar_pdf','ttbar_pdf',RooArgList(sig1,sig2),RooArgList(fsig))

# -----------------------------------------
# fit signal
canS = TCanvas('Template_TT','Template_TT',900,600)
#gPad.SetLogy() 
res = signal.fitTo(roohisto[0],RooFit.Save())
res.Print()
frame = x.frame()
roohisto[0].plotOn(frame)
signal.plotOn(frame)
signal.plotOn(frame,RooFit.Components('ttbar_gaus1'),RooFit.LineColor(ROOT.kRed),RooFit.LineWidth(2),RooFit.LineStyle(ROOT.kDashed))
signal.plotOn(frame,RooFit.Components('ttbar_gaus2'),RooFit.LineColor(ROOT.kGreen+1),RooFit.LineWidth(2),RooFit.LineStyle(ROOT.kDashed))
#signal.plotOn(frame,RooFit.Components('sig3'),RooFit.LineColor(ROOT.kMagenta),RooFit.LineWidth(2),RooFit.LineStyle(ROOT.kDashed))
frame.GetXaxis().SetTitle('m_{t} (GeV)')
frame.Draw()
gPad.Update()
canS.Print(canS.GetName()+".pdf") 

parsSig = signal.getParameters(roohisto[0])
parsSig.setAttribAll('Constant', True)

# -----------------------------------------
# fit qcd

m1QCD = RooRealVar('qcd_mean1' ,'qcd_mean1',200,130,350)
s1QCD = RooRealVar('qcd_sigma1','qcd_sigma1',50,20,200)
qcd1  = RooGaussian('qcd_gaus1','qcd_gaus1',x,m1QCD,s1QCD)

m2QCD = RooRealVar('qcd_mean2' ,'qcd_mean2',140,130,300)
s2QCD = RooRealVar('qcd_sigma2','qcd_sigma2',50,10,200)
qcd2  = RooGaussian('qcd_gaus2'  ,'qcd_gaus2',x,m2QCD,s2QCD)

fqcd = RooRealVar('qcd_f1','qcd_f1',0.5,0,1)

qcd = RooAddPdf('qcd_pdf','qcd_pdf',qcd1,qcd2,fqcd)


canB = TCanvas('Template_QCD','Template_QCD',900,600)
#gPad.SetLogy() 
res = qcd.fitTo(roohistoCtl[1],RooFit.Save())
res.Print()
frame1 = x.frame()
roohistoCtl[1].plotOn(frame1)
qcd.plotOn(frame1)
qcd.plotOn(frame1,RooFit.Components('qcd_gaus1'),RooFit.LineColor(ROOT.kRed),RooFit.LineWidth(2),RooFit.LineStyle(2))
qcd.plotOn(frame1,RooFit.Components('qcd_gaus2'),RooFit.LineColor(ROOT.kGreen+1),RooFit.LineWidth(2),RooFit.LineStyle(2))
frame1.GetXaxis().SetTitle('m_{t} (GeV)')
frame1.Draw()
gPad.Update()
canB.Print(canB.GetName()+".pdf")

parsQCD = qcd.getParameters(roohistoCtl[1])
parsQCD.setAttribAll('Constant', True)
# -----------------------------------------


w = RooWorkspace('w','workspace')
getattr(w,'import')(signal,ROOT.RooCmdArg())
getattr(w,'import')(qcd,ROOT.RooCmdArg())
getattr(w,'import')(x,ROOT.RooCmdArg())
getattr(w,'import')(YieldTT,ROOT.RooCmdArg())
w.Print()
w.writeToFile('templates_boosted_workspace.root')
                            
#----- keep the GUI alive ------------
if __name__ == '__main__':
  rep = ''
  while not rep in ['q','Q']:
    rep = raw_input('enter "q" to quit: ')
    if 1 < len(rep):
      rep = rep[0]
