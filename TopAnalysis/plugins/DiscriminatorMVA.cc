#include <cmath>
#include "UserCode/TopAnalysis/plugins/DiscriminatorMVA.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"

DiscriminatorMVA::DiscriminatorMVA(std::string weights)
{
  weights_ = weights;
  reader_  = new TMVA::Reader("!Color:!Silent");

  reader_->AddVariable("ht"                   ,&var_[0]);
  reader_->AddVariable("jetPt[2]"             ,&var_[1]);
  reader_->AddVariable("jetPt[3]"             ,&var_[2]);
  reader_->AddVariable("jetPt[4]"             ,&var_[3]);
  reader_->AddVariable("jetPt[5]"             ,&var_[4]);
  reader_->AddVariable("qglMin"               ,&var_[5]);
  reader_->AddVariable("qglMedian"            ,&var_[6]);
  reader_->AddVariable("sphericity"           ,&var_[7]);
  reader_->AddVariable("aplanarity"           ,&var_[8]);
  reader_->AddVariable("foxWolfram[0]"        ,&var_[9]);
  reader_->AddVariable("foxWolfram[1]"        ,&var_[10]);
  reader_->AddVariable("foxWolfram[2]"        ,&var_[11]);
  reader_->AddVariable("foxWolfram[3]"        ,&var_[12]);
  reader_->AddVariable("mTop[0]"              ,&var_[13]);
  reader_->AddVariable("ptTTbar"              ,&var_[14]);
  reader_->AddVariable("mTTbar"               ,&var_[15]);
  reader_->AddVariable("chi2"                 ,&var_[16]); 
  reader_->AddVariable("centrality"           ,&var_[17]); 
  reader_->AddVariable("cosThetaStar1"        ,&var_[18]); 
  reader_->AddVariable("cosThetaStar2"        ,&var_[19]); 

  reader_->AddSpectator("nBJets"              ,&spc_[0]);

  edm::FileInPath f1(weights_);
  reader_->BookMVA("BDT",f1.fullPath());  
}
//-------------------------------------------------------------
float DiscriminatorMVA::eval(int nBJets,float ht,float jetPt2,float jetPt3,float jetPt4,float jetPt5,float qglMin,float qglMedian,float sphericity,float aplanarity,float H0,float H1,float H2,float H3,float mTop,float ptTTbar,float mTTbar,float chi2,float centrality,float cosThetaStar1,float cosThetaStar2)
{
  var_[0]  = ht;
  var_[1]  = jetPt2;
  var_[2]  = jetPt3;
  var_[3]  = jetPt4;
  var_[4]  = jetPt5;
  var_[5]  = qglMin;
  var_[6]  = qglMedian;
  var_[7] = sphericity;
  var_[8] = aplanarity;
  var_[9] = H0;
  var_[10] = H1;
  var_[11] = H2;
  var_[12] = H3;
  var_[13] = mTop;
  var_[14] = ptTTbar;
  var_[15] = mTTbar;
  var_[16] = chi2;
  var_[17] = centrality;
  var_[18] = cosThetaStar1;
  var_[19] = cosThetaStar2;
  
  spc_[0]  = nBJets;

  return reader_->EvaluateMVA("BDT");
}
//-------------------------------------------------------------
DiscriminatorMVA::~DiscriminatorMVA()
{
  delete reader_;
}
