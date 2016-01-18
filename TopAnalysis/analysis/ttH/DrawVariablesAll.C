#include "DrawVariable.C"

void DrawVariablesAll(TString CAT,bool PRINT)
{
  DrawVariable("nvtx_"+CAT      ,false,1,0,30,"Number of vertices",PRINT);
  DrawVariable("mva_"+CAT       ,false,4,-1,1,"BDT Output",PRINT);
  
  DrawVariable("chi2_"+CAT      ,false,5,0,200,"#chi^{2}",PRINT);
  DrawVariable("mTTbar_"+CAT    ,false,20,0,2000,"m_{t#bar{t}} (GeV)",PRINT);
  DrawVariable("ptTTbar_"+CAT   ,false,10,0,600,"p_{T,t#bar{t}} (GeV)",PRINT);
  DrawVariable("mTop[0]_"+CAT   ,false,4,100,400,"m_{t} (GeV)",PRINT);
  DrawVariable("dRbbTop_"+CAT   ,false,2,0,6,"#DeltaR_{bb} (kin. fit)",PRINT);
  DrawVariable("dRbbMin_"+CAT   ,false,1,0,6,"Minimum #DeltaR_{bb}",PRINT);
  DrawVariable("mbbMin_"+CAT    ,false,1,0,400,"m_{bb} (min #DeltaR pair) (GeV)",PRINT);
  DrawVariable("met_"+CAT       ,false,2,0,100,"MET(GeV)",PRINT);
  DrawVariable("nJets_"+CAT     ,false,1,6,14,"Number of jets",PRINT);
  DrawVariable("nBJets_"+CAT    ,false,1,2,10,"Number of b jets",PRINT);
  DrawVariable("ht_"+CAT        ,false,2,400,2000,"H_{T} (GeV)",PRINT);
  DrawVariable("sphericity_"+CAT,false,2,0,1,"Sphericity",PRINT);
  DrawVariable("aplanarity_"+CAT,false,1,0,0.2,"Aplanarity",PRINT);
  DrawVariable("FW0_"+CAT       ,false,1,0.2,0.45,"H0",PRINT);
  DrawVariable("FW1_"+CAT       ,false,1,-0.2,0.15,"H1",PRINT);
  DrawVariable("FW2_"+CAT       ,false,1,-0.05,0.4,"H2",PRINT);
  DrawVariable("FW3_"+CAT       ,false,1,-0.2,0.2,"H3",PRINT);  
  DrawVariable("jetPt0_"+CAT    ,false,2,40,800,"JetPt0 (GeV)",PRINT);
  DrawVariable("jetPt1_"+CAT    ,false,2,40,500,"JetPt1 (GeV)",PRINT);
  DrawVariable("jetPt2_"+CAT    ,false,2,40,300,"JetPt2 (GeV)",PRINT);
  DrawVariable("jetPt3_"+CAT    ,false,1,40,200,"JetPt3 (GeV)",PRINT);
  DrawVariable("jetPt4_"+CAT    ,false,1,40,160,"JetPt4 (GeV)",PRINT);
  DrawVariable("jetPt5_"+CAT    ,false,1,40,120,"JetPt5 (GeV)",PRINT);
  /*
  DrawVariable("jetQGL0_"+CAT   ,false,2,0,1,"JetQGL0",PRINT);
  DrawVariable("jetQGL1_"+CAT   ,false,2,0,1,"JetQGL1",PRINT);
  DrawVariable("jetQGL2_"+CAT   ,false,2,0,1,"JetQGL2",PRINT);
  DrawVariable("jetQGL3_"+CAT   ,false,2,0,1,"JetQGL3",PRINT);
  DrawVariable("jetQGL4_"+CAT   ,false,2,0,1,"JetQGL4",PRINT);
  DrawVariable("jetQGL5_"+CAT   ,false,2,0,1,"JetQGL5",PRINT); 
  */
}
