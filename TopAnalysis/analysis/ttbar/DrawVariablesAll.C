#include "DrawVariable.C"

void DrawVariablesAll(bool PRINT)
{
  //------ Resolved ---------------------------
   
  DrawVariable("hadtop","mTop[0]"   ,false,5,100,400,"m_{t} (GeV)",PRINT);
  DrawVariable("hadtop","mWReco"    ,false,5,75,110,"m_{W} (GeV)",PRINT);
  DrawVariable("hadtop","dRbbTop"   ,false,2,0,6,"#DeltaR_{bb} (kin. fit)",PRINT);
  DrawVariable("hadtop","chi2"      ,false,1,0,10,"#chi^{2}",PRINT);
  
  
  
  
  //DrawVariable("hadtop","nJets"     ,false,1,6,12,"Number of jets",PRINT);
  //DrawVariable("hadtop","nBJets"    ,false,1,2,7,"Number of b jets",PRINT);
  
  //DrawVariable("hadtop","mTTbar"    ,false,2,200,400,"m_{t#bar{t}} (GeV)",PRINT);
  /*
  DrawVariable("hadtop","ptTTbar"   ,false,10,0,600,"p_{T,t#bar{t}} (GeV)",PRINT);
  DrawVariable("hadtop","met"       ,false,2,0,100,"MET(GeV)",PRINT);
  DrawVariable("hadtop","ht"        ,false,2,400,2000,"H_{T} (GeV)",PRINT);
  DrawVariable("hadtop","sphericity",false,2,0,1,"Sphericity",PRINT);
  DrawVariable("hadtop","aplanarity",false,1,0,0.2,"Aplanarity",PRINT);
  DrawVariable("hadtop","FW0"       ,false,1,0.2,0.45,"H0",PRINT);
  DrawVariable("hadtop","FW1"       ,false,1,-0.2,0.15,"H1",PRINT);
  DrawVariable("hadtop","FW2"       ,false,1,-0.05,0.4,"H2",PRINT);
  DrawVariable("hadtop","FW3"       ,false,1,-0.2,0.2,"H3",PRINT);  
  DrawVariable("hadtop","jetPt0"    ,false,2,40,800,"JetPt0 (GeV)",PRINT);
  DrawVariable("hadtop","jetPt1"    ,false,2,40,500,"JetPt1 (GeV)",PRINT);
  DrawVariable("hadtop","jetPt2"    ,false,2,40,300,"JetPt2 (GeV)",PRINT);
  DrawVariable("hadtop","jetPt3"    ,false,1,40,200,"JetPt3 (GeV)",PRINT);
  DrawVariable("hadtop","jetPt4"    ,false,1,40,160,"JetPt4 (GeV)",PRINT);
  DrawVariable("hadtop","jetPt5"    ,false,1,40,120,"JetPt5 (GeV)",PRINT); 
  */
  //------ Boosted ------------------------------
  ///*
  bool LOG = false;
  TString ss = "sig";
  /*
  DrawVariable("hadtopBoost","ht_"+ss                  ,true,5,400,3000,"H_{T} (GeV)",PRINT);
  DrawVariable("hadtopBoost","ht_Cut_"+ss              ,LOG,10,400,3000,"H_{T} (GeV)",PRINT);
  DrawVariable("hadtopBoost","nJets_"+ss               ,true,1,0,12,"Number of jets",PRINT);
  DrawVariable("hadtopBoost","nJets_Cut_"+ss           ,LOG,1,0,12,"Number of jets",PRINT);
  DrawVariable("hadtopBoost","mJJ_"+ss                 ,true,100,0,7000,"mJJ (GeV)",PRINT);
  DrawVariable("hadtopBoost","mJJ_Cut_"+ss             ,LOG,100,0,7000,"mJJ (GeV)",PRINT); 
  DrawVariable("hadtopBoost","ptJJ_"+ss                ,true,20,0,1000,"p_{T}JJ (GeV)",PRINT);
  DrawVariable("hadtopBoost","ptJJ_Cut_"+ss            ,LOG,50,0,1000,"p_{T}JJ (GeV)",PRINT);
  DrawVariable("hadtopBoost","yJJ_"+ss                 ,true,10,-3,3,"yJJ",PRINT);
  DrawVariable("hadtopBoost","yJJ_Cut_"+ss             ,LOG,20,-3,3,"yJJ",PRINT);
  DrawVariable("hadtopBoost","dPhiJJ_"+ss              ,true,5,0,3.142,"d#phiJJ",PRINT);
  DrawVariable("hadtopBoost","dPhiJJ_Cut_"+ss          ,LOG,5,0,3.142,"d#phiJJ",PRINT);
  
  DrawVariable("hadtopBoost","jetEta0_"+ss             ,true,2,-3,3,"Jet #eta",PRINT);
  DrawVariable("hadtopBoost","jetEta0_Cut_"+ss         ,LOG,2,-3,3,"Jet #eta",PRINT);
  
  DrawVariable("hadtopBoost","nBJets_"+ss              ,true,1,0,7,"Number of b jets",PRINT);
  DrawVariable("hadtopBoost","nBJets_Cut_"+ss          ,LOG,1,0,7,"Number of b jets",PRINT);
  
  DrawVariable("hadtopBoost","mva_"+ss                 ,true,4,-1,1,"mva",PRINT);
  DrawVariable("hadtopBoost","mva_Cut_"+ss             ,LOG,4,-1,1,"mva",PRINT);
  */
  DrawVariable("hadtopBoost","jetPt0_"+ss              ,true,10,500,1000,"Jet p_{T} (GeV)",PRINT);
  DrawVariable("hadtopBoost","jetPt0_Cut_"+ss          ,LOG,100,500,1000,"Jet p_{T} (GeV)",PRINT);
  //DrawVariable("hadtopBoost","jetMassSoftDrop0_"+ss    ,true,10,50,350,"Softdrop mass (GeV)",PRINT);
  //DrawVariable("hadtopBoost","jetMassSoftDrop0_Cut_"+ss,LOG,10,50,350,"Softdrop mass (GeV)",PRINT);
  //DrawVariable("hadtopBoost","jetTau320_"+ss           ,true,2,0,1,"#tau3/#tau2",PRINT);
  //DrawVariable("hadtopBoost","jetTau320_Cut_"+ss       ,LOG,2,0,1,"#tau3/#tau2",PRINT);
  //DrawVariable("hadtopBoost","jetTau310_"+ss           ,true,2,0,1,"#tau3/#tau1",PRINT);
  //DrawVariable("hadtopBoost","jetTau310_Cut_"+ss       ,LOG,2,0,1,"#tau3/#tau1",PRINT);
  /*
  DrawVariable("hadtopBoost","jetMassSub00_"+ss        ,true,5,0,200,"Leading subjet mass (GeV)",PRINT);
  DrawVariable("hadtopBoost","jetMassSub00_Cut_"+ss    ,LOG,10,0,200,"Leading subjet mass (GeV)",PRINT);
  DrawVariable("hadtopBoost","jetMassSub10_"+ss        ,true,2,0,100,"JetMassSub01 (GeV)",PRINT);
  DrawVariable("hadtopBoost","jetMassSub10_Cut_"+ss    ,LOG,4,0,100,"JetMassSub01 (GeV)",PRINT);
  */
}
