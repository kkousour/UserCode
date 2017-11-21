#ifndef BoostedDiscriminatorMVA_h
#define BoostedDiscriminatorMVA_h

#include "TMVA/Reader.h"

class BoostedDiscriminatorMVA
{
  public:
    BoostedDiscriminatorMVA(std::string weights);
    ~BoostedDiscriminatorMVA();
    float eval(float tau30,float tau31,float tau20,float tau21,float tau10,float tau11);
    
  private:
    std::string weights_;
    TMVA::Reader *reader_;
    float var_[6];
};
#endif
