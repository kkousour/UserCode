#include <cmath>
#include "UserCode/TopAnalysis/plugins/DiscriminatorMVA2.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"

DiscriminatorMVA2::DiscriminatorMVA2(std::string weights)
{
  weights_ = weights;
  reader_  = new TMVA::Reader("!Color:!Silent");

  reader_->AddVariable("yTop[0]"   ,&var_[0]);
  reader_->AddVariable("yTop[1]"   ,&var_[1]);
  reader_->AddVariable("ptTop[0]"  ,&var_[2]);
  reader_->AddVariable("ptTop[1]"  ,&var_[3]);
  reader_->AddVariable("yTTbar"    ,&var_[4]);
  reader_->AddVariable("mbbMin"    ,&var_[5]);
  reader_->AddVariable("dRbbAve"   ,&var_[6]);
  reader_->AddVariable("dRbbTop"   ,&var_[7]);
  reader_->AddVariable("EtStar1"   ,&var_[8]);
  reader_->AddVariable("EtStar2"   ,&var_[9]);
  reader_->AddVariable("jetEta[0]" ,&var_[10]);
  reader_->AddVariable("jetEta[1]" ,&var_[11]);
  reader_->AddVariable("jetEta[2]" ,&var_[12]);
  reader_->AddVariable("jetEta[3]" ,&var_[13]);
  reader_->AddVariable("jetEta[4]" ,&var_[14]);
  reader_->AddVariable("jetEta[5]" ,&var_[15]);  

  reader_->AddSpectator("nBJets"   ,&spc_[0]);

  edm::FileInPath f1(weights_);
  reader_->BookMVA("BDT",f1.fullPath());  
}
//-------------------------------------------------------------
float DiscriminatorMVA2::eval(int nBJets,float yTop0,float yTop1,float ptTop0,float ptTop1,float yTTbar,float mbbMin,float dRbbAve,float dRbbTop,float EtStar1,float EtStar2,float jetEta0,float jetEta1,float jetEta2,float jetEta3,float jetEta4,float jetEta5)
{
  var_[0]  = yTop0;
  var_[1]  = yTop1;
  var_[2]  = ptTop0;
  var_[3]  = ptTop1;
  var_[4]  = yTTbar;
  var_[5]  = mbbMin;
  var_[6]  = dRbbAve;
  var_[7]  = dRbbTop;
  var_[8]  = EtStar1;
  var_[9]  = EtStar2;
  var_[10] = jetEta0;
  var_[11] = jetEta1;
  var_[12] = jetEta2;
  var_[13] = jetEta3;
  var_[14] = jetEta4;
  var_[15] = jetEta5;
 
  spc_[0]  = nBJets;
  
  return reader_->EvaluateMVA("BDT");
}
//-------------------------------------------------------------
DiscriminatorMVA2::~DiscriminatorMVA2()
{
  delete reader_;
}
