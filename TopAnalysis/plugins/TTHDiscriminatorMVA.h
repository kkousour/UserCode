#ifndef TTHDiscriminatorMVA_h
#define TTHDiscriminatorMVA_h

#include "TMVA/Reader.h"

class TTHDiscriminatorMVA
{
  public:
    TTHDiscriminatorMVA(std::string weights, std::string CAT, std::string TYPE_BKG);
    ~TTHDiscriminatorMVA();
   
    
    float eval(int nJets,float ht,float jetPt5,float mbbMin,float dRbbMin,float qglMedian,float cosThetaStar1,float cosThetaStar2,float sphericity,float aplanarity,float centrality,float H0,float H1,float H2,float H3,float mTop,float ptTTbar,float mTTbar,float dRbbTop,float chi2);
    
    
    
  private:
    std::string weights_;
    TMVA::Reader *reader_;
    std::string CAT_;
    std::string TYPE_BKG_;
    float var_[20];
};
#endif
