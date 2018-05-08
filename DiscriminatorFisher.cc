#include <cmath>
#include "UserCode/TopAnalysis/plugins/DiscriminatorFisher.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"


DiscriminatorFisher::DiscriminatorFisher(std::string weights)
{
weights_ = weights;
reader_  = new TMVA::Reader("!Color:!Silent");

reader_->AddVariable("JetTau1",  & var_[0]);
reader_->AddVariable("JetTau2",  & var_[1]);
reader_->AddVariable("JetTau3",  & var_[2]);
reader_->AddVariable("Pt_rank",  & var_[3]);
reader_->AddVariable("JetNBSub", & var_[4]);
reader_->AddVariable("doubleB",  & var_[5]);
				   
edm::FileInPath f1(weights_);

reader_->BookMVA("Fisher",f1.fullPath());
}
//-------------------------------------------------------------
float DiscriminatorFisher::eval(float JetTau1,float JetTau2,float JetTau3,int JetNBSub,int Pt_rank, float doubleB)
{

var_[0]  = JetTau1 ;
var_[1]  = JetTau2 ;
var_[2]  = JetTau3 ;
var_[3]  = (float)Pt_rank ;
var_[4]  = (float)JetNBSub;
var_[5]  = doubleB;

return reader_->EvaluateMVA("Fisher");


}



DiscriminatorFisher::~DiscriminatorFisher()
{
  delete reader_;
}





