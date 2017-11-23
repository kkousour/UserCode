#include <cmath>
#include "KKousour/TopAnalysis/plugins/BoostedDiscriminatorMVA.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"

BoostedDiscriminatorMVA::BoostedDiscriminatorMVA(std::string weights)
{
  weights_ = weights;
  reader_  = new TMVA::Reader("!Color:!Silent");

  reader_->AddVariable("jetTau3[0]",&var_[0]);
  reader_->AddVariable("jetTau3[1]",&var_[1]);
  reader_->AddVariable("jetTau2[0]",&var_[2]);
  reader_->AddVariable("jetTau2[1]",&var_[3]);
  reader_->AddVariable("jetTau1[0]",&var_[4]);
  reader_->AddVariable("jetTau1[1]",&var_[5]);

  edm::FileInPath f1(weights_);
  reader_->BookMVA("MVA",f1.fullPath());  
}
//-------------------------------------------------------------
float BoostedDiscriminatorMVA::eval(float tau30,float tau31,float tau20,float tau21,float tau10,float tau11)
{
  var_[0] = tau30;
  var_[1] = tau31;
  var_[2] = tau20;
  var_[3] = tau21;
  var_[4] = tau10;
  var_[5] = tau11;

  return reader_->EvaluateMVA("MVA");
}
//-------------------------------------------------------------
BoostedDiscriminatorMVA::~BoostedDiscriminatorMVA()
{
  delete reader_;
}
