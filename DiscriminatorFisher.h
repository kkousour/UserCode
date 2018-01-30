#ifndef DiscriminatorFisher_h
#define DiscriminatorFisher_h

#include "TMVA/Reader.h"

class DiscriminatorFisher
{
  public:
    DiscriminatorFisher(std::string weights);
    ~DiscriminatorFisher();
    float eval(float JetTau1,float JetTau2,float JetTau3,int JetNBSub,int Pt_rank,float doubleB);
    
     
    
  private:
    
    std::string weights_;
    TMVA::Reader *reader_;float var_[21];
    float spc_[1];
};
#endif
