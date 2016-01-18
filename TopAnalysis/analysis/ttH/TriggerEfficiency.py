#!usr/bin/python
import ROOT
from ROOT import TMath, TFile, TH1, TH1F, TCanvas, TEfficiency, TGraphAsymmErrors, TF1, TPaveText, TPaveStats, TLegend, TLine, gROOT, gPad, gStyle
from setTDRStyle import setTDRStyle
from array import array

gROOT.Reset()
setTDRStyle()
gROOT.SetStyle('tdrStyle')
gROOT.ForceStyle()

import optparse
usage = "usage: %prog [options]"
parser = optparse.OptionParser(usage)
parser.add_option("--var"  ,action="store",type="string",dest="var" ,default="jetPt[5]")
parser.add_option("--xmax" ,action="store",type="float", dest="xmax",default="100")
parser.add_option("--xmin" ,action="store",type="float", dest="xmin",default="30")
parser.add_option("--bins" ,action="store",type="int"  , dest="bins",default="14")

(options, args) = parser.parse_args()

var  = options.var
xmax = options.xmax
xmin = options.xmin
bins = options.bins 

can = TCanvas('Eff_'+var,'Eff_'+var,900,600)

#inf   = TFile.Open('root://eoscms//eos/cms/store/cmst3/user/kkousour/ttH/flat/flatTree_JetHT.root')
inf   = TFile.Open('/afs/cern.ch/work/k/kkousour/private/CMSSW_7_4_12/src/KKousour/TopAnalysis/prod/ttH/flatTree_JetHT.root')
tree  = inf.Get('hadtop/events')
hAll  = TH1F('hAll','hAll',bins,xmin,xmax)
hPass = TH1F('hPass','hPass',bins,xmin,xmax)
hAll.Sumw2()
hPass.Sumw2()
tree.Draw(var+'>>hAll','ht>450 && nBJets>1 && triggerBit[7]')
tree.Draw(var+'>>hPass','ht>450 && nBJets>1 && triggerBit[7] && (triggerBit[0] || triggerBit[2])')

print hAll.Integral() 
print hPass.Integral()

eff = TEfficiency('Efficiency','Efficiency',bins,xmin,xmax)
eff.SetPassedHistogram(hPass,"f")
eff.SetTotalHistogram(hAll,"f")

eff.SetMarkerColor(ROOT.kBlack)
eff.SetLineColor(ROOT.kBlack)
eff.SetMarkerStyle(20) 

x   = []
y   = []
exl = []
exh = []
eyl = []
eyh = []

for i in xrange(hAll.GetNbinsX()):
  x.append(hAll.GetBinCenter(i+1))
  exl.append(0) 
  exh.append(0) 
  y.append(eff.GetEfficiency(i+1))
  eyl.append(eff.GetEfficiencyErrorLow(i+1))
  eyh.append(eff.GetEfficiencyErrorUp(i+1))

vx   = array('d',x)
vy   = array('d',y)
vexl = array('d',exl)
vexh = array('d',exh)
veyl = array('d',eyl)
veyh = array('d',eyh)

gEff = TGraphAsymmErrors(hAll.GetNbinsX(),vx,vy,vexl,vexh,veyl,veyh)

gEff.SetMarkerColor(ROOT.kBlack)
gEff.SetLineColor(ROOT.kBlack)
gEff.SetMarkerStyle(20) 

fit = TF1('fit','([0]-[3])/pow(1+[1]*exp(-[2]*x),[4])+[3]+[5]*(1-pow(x,-[6]))',xmin,xmax)
fit.SetParameters(1,100,0.1,0,1,1,0.1)
fit.SetParNames('A','x0','#sigma')
fit.SetLineColor(ROOT.kBlue)

can.cd()
f = TF1('f','1',xmin,xmax)
f.SetLineColor(ROOT.kBlack)
f.SetLineStyle(3)
f.SetLineWidth(1)
f.GetXaxis().SetTitle('')
f.GetYaxis().SetTitle('Trigger Efficiency')
f.SetMinimum(0)
f.SetMaximum(1.15)
f.Draw()

#hAll.Draw("hist")
#hPass.Draw("sameE")

gEff.Draw("samePE")

gPad.Update()

#----- keep the GUI alive ------------
if __name__ == '__main__':
  rep = ''
  while not rep in ['q','Q']:
    rep = raw_input('enter "q" to quit: ')
    if 1 < len(rep):
      rep = rep[0]
