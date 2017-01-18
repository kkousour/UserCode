#include <cmath>
#include "KKousour/TopAnalysis/plugins/TTHDiscriminatorMVA.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"

TTHDiscriminatorMVA::TTHDiscriminatorMVA(std::string weights, std::string CAT, std::string TYPE_BKG)
{
  weights_ = weights;
  CAT_     = CAT;
  TYPE_BKG_ = TYPE_BKG;
  
  reader_  = new TMVA::Reader("!Color:!Silent");

  
  if(TYPE_BKG_ == "TTbar" && CAT_ == "CAT0"){

reader_->AddVariable("nJets"                ,&var_[0]);
reader_->AddVariable("ht"                   ,&var_[1]);
reader_->AddVariable("jetPt[5]"             ,&var_[2]);
reader_->AddVariable("mbbMin"               ,&var_[3]);
reader_->AddVariable("dRbbMin"              ,&var_[4]);
reader_->AddVariable("qglMedian"            ,&var_[5]);
reader_->AddVariable("aplanarity"           ,&var_[6]);
reader_->AddVariable("centrality"           ,&var_[7]); 
reader_->AddVariable("foxWolfram[0]"	    ,&var_[8]);
reader_->AddVariable("foxWolfram[1]"	    ,&var_[9]);
reader_->AddVariable("mTop[0]"              ,&var_[10]);
reader_->AddVariable("ptTTbar"              ,&var_[11]);
reader_->AddVariable("mTTbar"               ,&var_[12]);
reader_->AddVariable("chi2"                 ,&var_[13]);
}						   


if(TYPE_BKG_ == "TTbar" && CAT_ == "CAT1"){

reader_->AddVariable("nJets"                ,&var_[0]);
reader_->AddVariable("ht"                   ,&var_[1]);
reader_->AddVariable("jetPt[5]"             ,&var_[2]);
reader_->AddVariable("mbbMin"               ,&var_[3]);
reader_->AddVariable("dRbbMin"              ,&var_[4]);
reader_->AddVariable("abs(cosThetaStar1)"   ,&var_[5]); 
reader_->AddVariable("centrality"           ,&var_[6]); 
reader_->AddVariable("foxWolfram[0]"        ,&var_[7]);
reader_->AddVariable("foxWolfram[1]"        ,&var_[8]);
reader_->AddVariable("foxWolfram[2]"        ,&var_[9]);
reader_->AddVariable("ptTTbar"              ,&var_[10]);
reader_->AddVariable("mTTbar"               ,&var_[11]);
reader_->AddVariable("dRbbTop"              ,&var_[12]);  


}


if(TYPE_BKG_ == "QCD" && CAT_ == "CAT0"){

reader_->AddVariable("jetPt[5]"             ,&var_[0]);
reader_->AddVariable("mbbMin"               ,&var_[1]);
reader_->AddVariable("dRbbMin"              ,&var_[2]);
reader_->AddVariable("qglMedian"            ,&var_[3]);
reader_->AddVariable("sphericity"           ,&var_[4]);  
reader_->AddVariable("aplanarity"           ,&var_[5]);
reader_->AddVariable("centrality"           ,&var_[6]); 
reader_->AddVariable("foxWolfram[0]"        ,&var_[7]);
reader_->AddVariable("foxWolfram[1]"        ,&var_[8]);
reader_->AddVariable("mTop[0]"              ,&var_[9]);
reader_->AddVariable("ptTTbar"              ,&var_[10]);
reader_->AddVariable("mTTbar"               ,&var_[11]);     
reader_->AddVariable("chi2"                 ,&var_[12]); 

}

if(TYPE_BKG_ == "QCD" && CAT_ == "CAT1"){

reader_->AddVariable("nJets"                ,&var_[0]);
reader_->AddVariable("jetPt[5]"             ,&var_[1]);
reader_->AddVariable("mbbMin"               ,&var_[2]);
reader_->AddVariable("dRbbMin"              ,&var_[3]);
reader_->AddVariable("qglMedian"            ,&var_[4]);
reader_->AddVariable("aplanarity"           ,&var_[5]);
reader_->AddVariable("centrality"	    ,&var_[6]); 
reader_->AddVariable("foxWolfram[0]"	    ,&var_[7]);
reader_->AddVariable("foxWolfram[1]"	    ,&var_[8]);
reader_->AddVariable("mTop[0]"              ,&var_[9]); 
reader_->AddVariable("chi2"                 ,&var_[10]); 

}

  edm::FileInPath f1(weights_);
  reader_->BookMVA("BDT",f1.fullPath());  
}
  
  
//-------------------------------------------------------------
float TTHDiscriminatorMVA::eval(int nJets,float ht,float jetPt5,float mbbMin,float dRbbMin,float qglMedian,float cosThetaStar1,float cosThetaStar2,float sphericity,float aplanarity,float centrality,float H0,float H1,float H2,float H3,float mTop,float ptTTbar,float mTTbar,float dRbbTop,float chi2)
{

if(TYPE_BKG_ == "TTbar" && CAT_ == "CAT0"){


var_[0]    =  nJets;	      
var_[1]    =  ht;	      
var_[2]    =  jetPt5;       
var_[3]    =  mbbMin;	      
var_[4]    =  dRbbMin;        
var_[5]    =  qglMedian;      
var_[6]    =  aplanarity;     
var_[7]    =  centrality;     
var_[8]    =  H0;
var_[9]    =  H1;
var_[10]   =  mTop;       
var_[11]   =  ptTTbar;        
var_[12]   =  mTTbar;	      
var_[13]   =  chi2;	      



}




if(TYPE_BKG_ == "TTbar" && CAT_ == "CAT1"){


var_[0]    =  nJets;		    
var_[1]    =  ht;		    
var_[2]    =  jetPt5; 	    
var_[3]    =  mbbMin;		    
var_[4]    =  dRbbMin;  	    
var_[5]    =  abs(cosThetaStar1);   
var_[6]    =  centrality;	    
var_[7]    =  H0;     
var_[8]    =  H1;
var_[9]    =  H2;
var_[10]   =  ptTTbar;  	 
var_[11]   =  mTTbar;		    
var_[12]   =  dRbbTop;  	    
    

}


if(TYPE_BKG_ == "QCD" && CAT_ == "CAT0"){

var_[0]    =  jetPt5;     
var_[1]    =  mbbMin;	    
var_[2]    =  dRbbMin;      
var_[3]    =  qglMedian;    
var_[4]    =  sphericity;   
var_[5]    =  aplanarity;   
var_[6]    =  centrality;   
var_[7]    =  H0;
var_[8]    =  H1;
var_[9]    =  mTop;      
var_[10]   =  ptTTbar;     
var_[11]   =  mTTbar;	    
var_[12]   =  chi2;	    




}



if(TYPE_BKG_ == "QCD" && CAT_ == "CAT1"){

var_[0]    = nJets;	    
var_[1]    = jetPt5;      
var_[2]    = mbbMin;	    
var_[3]    = dRbbMin;	    
var_[4]    = qglMedian;     
var_[5]    = aplanarity;    
var_[6]    = centrality;
var_[7]    = H0;
var_[8]    = H1;
var_[9]    = mTop;	    
var_[10]   = chi2;	 

}
  
  return reader_->EvaluateMVA("BDT");
}
//-------------------------------------------------------------
TTHDiscriminatorMVA::~TTHDiscriminatorMVA()
{
  delete reader_;
}
