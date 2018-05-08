#include <cmath>
#include "UserCode/TopAnalysis/plugins/TTVDiscriminatorMVA.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"

TTVDiscriminatorMVA::TTVDiscriminatorMVA(std::string weights,std::string selection)
{
  weights_ = weights;
  reader_  = new TMVA::Reader("!Color:!Silent");

  if (selection == "selZ") {
    reader_->AddVariable("lepPt[0]"      ,&var_[0]);
    reader_->AddVariable("lepPt[1]"      ,&var_[1]);
    reader_->AddVariable("abs(lepEta[0])",&var_[2]);
    reader_->AddVariable("abs(lepEta[1])",&var_[3]);
    reader_->AddVariable("dRlbMin"       ,&var_[4]);
    reader_->AddVariable("dRlbMax"       ,&var_[5]);
    reader_->AddVariable("dRbbTop"       ,&var_[6]);
    reader_->AddVariable("chi2"          ,&var_[7]);
    reader_->AddVariable("nJets"         ,&var_[8]);
    reader_->AddVariable("nBJets"        ,&var_[9]);
  }
  else if (selection == "selW") {
    reader_->AddVariable("lepPt[0]"      ,&var_[0]);
    reader_->AddVariable("abs(lepEta[0])",&var_[1]);
    reader_->AddVariable("met"           ,&var_[2]);
    reader_->AddVariable("dRlbMin"       ,&var_[3]);
    reader_->AddVariable("dRlbMax"       ,&var_[4]);
    reader_->AddVariable("dRbbTop"       ,&var_[5]);
    reader_->AddVariable("mTop[0]"       ,&var_[6]);
    reader_->AddVariable("chi2"          ,&var_[7]);
    reader_->AddVariable("ht"            ,&var_[8]);
    reader_->AddVariable("nJets"         ,&var_[9]);
    reader_->AddVariable("nBJets"        ,&var_[10]);
  }
  else {
    std::cout<<"******** wrong selection flag **************"<<std::endl;
  }

  edm::FileInPath f1(weights_);
  reader_->BookMVA("BDT",f1.fullPath());  
}
//-------------------------------------------------------------
float TTVDiscriminatorMVA::evalZ(int nJets,int nBJets,float lepPt0,float lepPt1,float lepEta0,float lepEta1,float dRlbMin,float dRlbMax,float dRbbTop,float chi2)
{
  var_[0]  = lepPt0;
  var_[1]  = lepPt1;
  var_[2]  = fabs(lepEta0);
  var_[3]  = fabs(lepEta1);
  var_[4]  = dRlbMin;
  var_[5]  = dRlbMax;
  var_[6]  = dRbbTop;
  var_[7]  = chi2;
  var_[8]  = nJets;
  var_[9]  = nBJets;
 
  return reader_->EvaluateMVA("BDT");
}
float TTVDiscriminatorMVA::evalW(int nJets,int nBJets,float lepPt0,float lepEta0,float met,float dRlbMin,float dRlbMax,float dRbbTop,float mTop,float chi2,float ht)
{
  var_[0]  = lepPt0;
  var_[1]  = fabs(lepEta0);
  var_[2]  = met;
  var_[3]  = dRlbMin;
  var_[4]  = dRlbMax;
  var_[5]  = dRbbTop;
  var_[6]  = mTop;
  var_[7]  = chi2;
  var_[8]  = ht;
  var_[9]  = nJets;
  var_[10] = nBJets;

  return reader_->EvaluateMVA("BDT");
}
//-------------------------------------------------------------
TTVDiscriminatorMVA::~TTVDiscriminatorMVA()
{
  delete reader_;
}
