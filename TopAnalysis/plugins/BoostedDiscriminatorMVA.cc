#include <cmath>
#include "KKousour/TopAnalysis/plugins/BoostedDiscriminatorMVA.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"

BoostedDiscriminatorMVA::BoostedDiscriminatorMVA(std::string weights)
{
  weights_ = weights;
  reader_  = new TMVA::Reader("!Color:!Silent");

  reader_->AddVariable("jetMassSoftDrop[0]"   ,&var_[0]);
  reader_->AddVariable("jetMassSub0[0]"       ,&var_[1]);
  reader_->AddVariable("jetMassSub1[0]"       ,&var_[2]);
  reader_->AddVariable("jetTau3[0]/jetTau2[0]",&var_[3]);
  reader_->AddVariable("jetTau3[0]/jetTau1[0]",&var_[4]);

  edm::FileInPath f1(weights_);
  reader_->BookMVA("BDT",f1.fullPath());  
}
//-------------------------------------------------------------
float BoostedDiscriminatorMVA::eval(float massSoftDrop,float massSub0,float massSub1,float tau32,float tau31)
{
  var_[0] = massSoftDrop;
  var_[1] = massSub0;
  var_[2] = massSub1;
  var_[3] = tau32;
  var_[4] = tau31;

  return reader_->EvaluateMVA("BDT");
}
//-------------------------------------------------------------
BoostedDiscriminatorMVA::~BoostedDiscriminatorMVA()
{
  delete reader_;
}
