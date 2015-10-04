#include <cmath>
#include "KKousour/TopAnalysis/plugins/DiscriminatorMVA.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"

DiscriminatorMVA::DiscriminatorMVA(std::string weights)
{
  weights_ = weights;
  reader_  = new TMVA::Reader("!Color:!Silent");

  reader_->AddVariable("nJets"                ,&var_[0]);
  reader_->AddVariable("ht"                   ,&var_[1]);
  reader_->AddVariable("jetPt[0]"             ,&var_[2]);
  reader_->AddVariable("jetPt[1]"             ,&var_[3]);
  reader_->AddVariable("jetPt[2]"             ,&var_[4]);
  reader_->AddVariable("jetPt[3]"             ,&var_[5]);
  reader_->AddVariable("jetPt[4]"             ,&var_[6]);
  reader_->AddVariable("jetPt[5]"             ,&var_[7]);
  reader_->AddVariable("mbbMin"               ,&var_[8]);
  reader_->AddVariable("dRbbMin"              ,&var_[9]);
  //reader_->AddVariable("qglAve"               ,&var_[10]);
  //reader_->AddVariable("qglMin"               ,&var_[11]);
  //reader_->AddVariable("qglMedian"            ,&var_[12]);
  reader_->AddVariable("sphericity"           ,&var_[10]);
  reader_->AddVariable("aplanarity"           ,&var_[11]);
  reader_->AddVariable("foxWolfram[0]"        ,&var_[12]);
  reader_->AddVariable("foxWolfram[1]"        ,&var_[13]);
  reader_->AddVariable("foxWolfram[2]"        ,&var_[14]);
  reader_->AddVariable("foxWolfram[3]"        ,&var_[15]);
  reader_->AddVariable("mTop[0]"              ,&var_[16]);
  reader_->AddVariable("ptTTbar"              ,&var_[17]);
  reader_->AddVariable("mTTbar"               ,&var_[18]);
  reader_->AddVariable("dRbbTop"              ,&var_[19]);
  reader_->AddVariable("chi2"                 ,&var_[20]); 

  reader_->AddSpectator("status"              ,&spc_[0]);
  reader_->AddSpectator("nBJets"              ,&spc_[1]);

  edm::FileInPath f1(weights_);
  reader_->BookMVA("BDT",f1.fullPath());  
}
//-------------------------------------------------------------
float DiscriminatorMVA::eval(int status,int nBJets,int nJets,float ht,float jetPt0,float jetPt1,float jetPt2,float jetPt3,float jetPt4,float jetPt5,float mbbMin,float dRbbMin,float sphericity,float aplanarity,float H0,float H1,float H2,float H3,float mTop,float ptTTbar,float mTTbar,float dRbbTop,float chi2)
{
  var_[0]  = nJets;
  var_[1]  = ht;
  var_[2]  = jetPt0;
  var_[3]  = jetPt1;
  var_[4]  = jetPt2;
  var_[5]  = jetPt3;
  var_[6]  = jetPt4;
  var_[7]  = jetPt5;
  var_[8]  = mbbMin;
  var_[9]  = dRbbMin;
  //var_[10]  = qglAve;
  //var_[11]  = qglMin;
  //var_[12]  = qglMedian;
  var_[10] = sphericity;
  var_[11] = aplanarity;
  var_[12] = H0;
  var_[13] = H1;
  var_[14] = H2;
  var_[15] = H3;
  var_[16] = mTop;
  var_[17] = ptTTbar;
  var_[18] = mTTbar;
  var_[19] = dRbbTop;
  var_[20] = chi2;
  
  spc_[0]  = status;
  spc_[1]  = nBJets;

  return reader_->EvaluateMVA("BDT");
}
//-------------------------------------------------------------
DiscriminatorMVA::~DiscriminatorMVA()
{
  delete reader_;
}
