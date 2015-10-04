#include "TMVA/Factory.h"
#include "TMVA/MethodCategory.h"
#include "TFile.h"
#include "TTree.h"
#include "TCut.h" 
#include "TH1F.h"
using namespace TMVA;
using namespace TMath;

void trainMVACat();

void trainMVACat()
{
  char name[1000];
  float XSEC[6] = {3.67e+5,2.94e+4,6.524e+03,1.064e+03,121.5,2.542e+01};
  float NORM[6];
  TCut preselectionCut = "ht>400 && jetPt[5]>40 && (triggerBit[0] || triggerBit[2]) && nBJets>1 && nLeptons==0";
  TFile *bkgSrc[6];
  bkgSrc[0] = TFile::Open("flatTree_QCD_HT300to500.root");
  bkgSrc[1] = TFile::Open("flatTree_QCD_HT500to700.root");
  bkgSrc[2] = TFile::Open("flatTree_QCD_HT700to1000.root");
  bkgSrc[3] = TFile::Open("flatTree_QCD_HT1000to1500.root");
  bkgSrc[4] = TFile::Open("flatTree_QCD_HT1500to2000.root");
  bkgSrc[5] = TFile::Open("flatTree_QCD_HT2000toInf.root");

  TFile *sigSrc = TFile::Open("flatTree_ttHJetTobb_M125.root");
  //TFile *sigSrc = TFile::Open("flatTree_TT.root");
  TTree *sigTree = (TTree*)sigSrc->Get("hadtop/events"); 
  TTree *bkgTree[6];
  
  
  TFile *outf = new TFile("mva_Cat_QCD.root","RECREATE");
  TMVA::Factory* factory = new TMVA::Factory("factory_mva_Cat_QCD_",outf,"!V:!Silent:Color:DrawProgressBar:Transformations=I;G:AnalysisType=Classification");
  factory->AddSignalTree(sigTree);

  for(int k=0;k<6;k++) {
    NORM[k] = ((TH1F*)bkgSrc[k]->Get("hadtop/pileup"))->GetEntries();
    bkgTree[k] = (TTree*)bkgSrc[k]->Get("hadtop/events");
    factory->AddBackgroundTree(bkgTree[k],XSEC[k]/NORM[k]);
  }
  
  //int N_SIG(sigTree->GetEntries(preselectionCut));
  
  //int N_BKG0(bkgTree[0]->GetEntries(preselectionCut));
  //int N_BKG1(bkgTree[1]->GetEntries(preselectionCut));
  //int N_BKG2(bkgTree[2]->GetEntries(preselectionCut));
  //int N_BKG3(bkgTree[3]->GetEntries(preselectionCut));

  //float N_BKG_EFF = N_BKG0*XSEC[0]/NORM[0]+N_BKG1*XSEC[1]/NORM[1]+N_BKG2*XSEC[2]/NORM[2]+N_BKG3*XSEC[3]/NORM[3];
  
  //int N = TMath::Min((float)N_SIG,N_BKG_EFF);

  //cout<<N_SIG<<" "<<N_BKG_EFF<<endl;
  
  const int NVAR = 21;
  TString VAR[NVAR] = {
    "nJets",
    //"nBJets",
    "ht",
    "jetPt[0]","jetPt[1]","jetPt[2]","jetPt[3]","jetPt[4]","jetPt[5]",
    "mbbMin","dRbbMin",
    //"dRbbAve","mbbAve",
    //"btagAve","btagMax","btagMin",
    //"qglAve","qglMin","qglMedian",
    "sphericity","aplanarity","foxWolfram[0]","foxWolfram[1]","foxWolfram[2]","foxWolfram[3]",
    "mTop[0]","ptTTbar","mTTbar","dRbbTop","chi2"
  };
  char TYPE[NVAR] = {
    'I',
    //'I',
    'F',
    'F','F','F','F','F','F', 
    'F','F',
    //'F','F',
    //'F','F','F',
    //'F','F','F',
    'F','F','F','F','F','F', 
    'F','F','F','F','F'
  };

  for(int i=0;i<NVAR;i++) {
    factory->AddVariable(VAR[i],TYPE[i]);
  }

  factory->AddSpectator("status",'I');
  factory->AddSpectator("nBJets",'I');

  sprintf(name,"nTrain_Signal=%d:nTrain_Background=%d:nTest_Signal=%d:nTest_Background=%d",-1,-1,-1,-1);
  factory->PrepareTrainingAndTestTree(preselectionCut,name);

  TMVA::IMethod* BDT_Category = factory->BookMethod( TMVA::Types::kCategory,"BDT_Category");
  TMVA::MethodCategory* mcategory_BDT = dynamic_cast<TMVA::MethodCategory*>(BDT_Category); 

  mcategory_BDT->AddMethod("status == 0 && nBJets == 2",
                      "nJets:ht:jetPt[0]:jetPt[1]:jetPt[2]:jetPt[3]:jetPt[4]:jetPt[5]:mbbMin:dRbbMin:sphericity:aplanarity:foxWolfram[0]:foxWolfram[1]:foxWolfram[2]:foxWolfram[3]:mTop[0]:ptTTbar:mTTbar:dRbbTop:chi2:",
                      TMVA::Types::kBDT,
                      "BDT_Cat1",
                      "NTrees=2000:BoostType=Grad:Shrinkage=0.1");

  mcategory_BDT->AddMethod("status == 0 && nBJets > 2",
                      "nJets:ht:jetPt[0]:jetPt[1]:jetPt[2]:jetPt[3]:jetPt[4]:jetPt[5]:mbbMin:dRbbMin:sphericity:aplanarity:foxWolfram[0]:foxWolfram[1]:foxWolfram[2]:foxWolfram[3]:mTop[0]:ptTTbar:mTTbar:dRbbTop:chi2:",
                      TMVA::Types::kBDT,
                      "BDT_Cat2",
                      "NTrees=2000:BoostType=Grad:Shrinkage=0.1");

  mcategory_BDT->AddMethod("status < 0 && nBJets == 2",
                      "nJets:ht:jetPt[0]:jetPt[1]:jetPt[2]:jetPt[3]:jetPt[4]:jetPt[5]:mbbMin:dRbbMin:sphericity:aplanarity:foxWolfram[0]:foxWolfram[1]:foxWolfram[2]:foxWolfram[3]:",
                      TMVA::Types::kBDT,
                      "BDT_Cat3",
                      "NTrees=2000:BoostType=Grad:Shrinkage=0.1");

  mcategory_BDT->AddMethod("status < 0 && nBJets > 2",
                      "nJets:ht:jetPt[0]:jetPt[1]:jetPt[2]:jetPt[3]:jetPt[4]:jetPt[5]:mbbMin:dRbbMin:sphericity:aplanarity:foxWolfram[0]:foxWolfram[1]:foxWolfram[2]:foxWolfram[3]:",
                      TMVA::Types::kBDT,
                      "BDT_Cat4",
                      "NTrees=2000:BoostType=Grad:Shrinkage=0.1");

  TMVA::IMethod* Fisher_Category = factory->BookMethod( TMVA::Types::kCategory,"Fisher_Category");
  TMVA::MethodCategory* mcategory_Fisher = dynamic_cast<TMVA::MethodCategory*>(Fisher_Category);
  
  mcategory_Fisher->AddMethod("status == 0 && nBJets == 2",
                      "nJets:ht:jetPt[0]:jetPt[1]:jetPt[2]:jetPt[3]:jetPt[4]:jetPt[5]:mbbMin:dRbbMin:sphericity:aplanarity:foxWolfram[0]:foxWolfram[1]:foxWolfram[2]:foxWolfram[3]:mTop[0]:ptTTbar:mTTbar:dRbbTop:chi2:",
                      TMVA::Types::kFisher,
                      "Fisher_Cat1","H:!V:Fisher");

  mcategory_Fisher->AddMethod("status == 0 && nBJets > 2",
                      "nJets:ht:jetPt[0]:jetPt[1]:jetPt[2]:jetPt[3]:jetPt[4]:jetPt[5]:mbbMin:dRbbMin:sphericity:aplanarity:foxWolfram[0]:foxWolfram[1]:foxWolfram[2]:foxWolfram[3]:mTop[0]:ptTTbar:mTTbar:dRbbTop:chi2:",
                      TMVA::Types::kFisher,
                      "Fisher_Cat2","H:!V:Fisher");

  mcategory_Fisher->AddMethod("status < 0 && nBJets == 2",
                      "nJets:ht:jetPt[0]:jetPt[1]:jetPt[2]:jetPt[3]:jetPt[4]:jetPt[5]:mbbMin:dRbbMin:sphericity:aplanarity:foxWolfram[0]:foxWolfram[1]:foxWolfram[2]:foxWolfram[3]:",
                      TMVA::Types::kFisher,
                      "Fisher_Cat3","H:!V:Fisher");

  mcategory_Fisher->AddMethod("status < 0 && nBJets > 2",
                      "nJets:ht:jetPt[0]:jetPt[1]:jetPt[2]:jetPt[3]:jetPt[4]:jetPt[5]:mbbMin:dRbbMin:sphericity:aplanarity:foxWolfram[0]:foxWolfram[1]:foxWolfram[2]:foxWolfram[3]:",
                      TMVA::Types::kFisher,
                      "Fisher_Cat4","H:!V:Fisher");

  // specify the training methods
  //factory->BookMethod(TMVA::Types::kFisher,"Fisher");
  //factory->BookMethod(TMVA::Types::kBDT,"BDT_GRAD_2000","NTrees=2000:BoostType=Grad:Shrinkage=0.1");
  
  factory->TrainAllMethods();
  factory->TestAllMethods();
  factory->EvaluateAllMethods(); 
  outf->Close();
}
