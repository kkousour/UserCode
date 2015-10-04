#include "DrawVariable.C"

void DrawVariablesAll(TString CAT)
{
  DrawVariable("mva_"+CAT       ,false,4,-1,1,"BDT Output");
  DrawVariable("chi2_"+CAT      ,false,5,0,200,"#chi^{2}");
  DrawVariable("mTTbar_"+CAT    ,false,20,0,2000,"m_{t#bar{t}} (GeV)");
  DrawVariable("ptTTbar_"+CAT   ,false,10,0,600,"p_{T,t#bar{t}} (GeV)");
  DrawVariable("mTop[0]_"+CAT   ,false,4,100,400,"m_{t} (GeV)");
  DrawVariable("dRbbTop_"+CAT   ,false,2,0,6,"#DeltaR_{bb} (kin. fit)");
  DrawVariable("dRbbMin_"+CAT   ,false,1,0,6,"Minimum #DeltaR_{bb}");
  DrawVariable("mbbMin_"+CAT    ,false,1,0,400,"m_{bb} (min #DeltaR pair) (GeV)");
  DrawVariable("met_"+CAT       ,false,2,0,100,"MET(GeV)");
  DrawVariable("nJets_"+CAT     ,false,1,6,14,"Number of jets");
  DrawVariable("nBJets_"+CAT    ,false,1,2,10,"Number of b jets");
  DrawVariable("ht_"+CAT        ,false,2,400,2000,"H_{T} (GeV)");
  DrawVariable("sphericity_"+CAT,false,2,0,1,"Sphericity");
  DrawVariable("aplanarity_"+CAT,false,1,0,0.2,"Aplanarity");
  DrawVariable("FW0_"+CAT       ,false,1,0.2,0.45,"H0");
  DrawVariable("FW1_"+CAT       ,false,1,-0.2,0.15,"H1");
  DrawVariable("FW2_"+CAT       ,false,1,-0.05,0.4,"H2");
  DrawVariable("FW3_"+CAT       ,false,1,-0.2,0.2,"H3");  
  DrawVariable("jetPt0_"+CAT    ,false,2,40,800,"JetPt0 (GeV)");
  DrawVariable("jetPt1_"+CAT    ,false,2,40,500,"JetPt1 (GeV)");
  DrawVariable("jetPt2_"+CAT    ,false,2,40,300,"JetPt2 (GeV)");
  DrawVariable("jetPt3_"+CAT    ,false,1,40,200,"JetPt3 (GeV)");
  DrawVariable("jetPt4_"+CAT    ,false,1,40,160,"JetPt4 (GeV)");
  DrawVariable("jetPt5_"+CAT    ,false,1,40,120,"JetPt5 (GeV)");
}
