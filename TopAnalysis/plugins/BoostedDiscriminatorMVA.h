#ifndef BoostedDiscriminatorMVA_h
#define BoostedDiscriminatorMVA_h

#include "TMVA/Reader.h"

class BoostedDiscriminatorMVA
{
  public:
    BoostedDiscriminatorMVA(std::string weights);
    ~BoostedDiscriminatorMVA();
    float eval(float massSoftDrop,float massSub0,float massSub1,float tau32,float tau31);
    
  private:
    std::string weights_;
    TMVA::Reader *reader_;
    float var_[5];
};
#endif
