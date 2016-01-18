#ifndef ApplyJSON_h
#define ApplyJSON_h

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

class ApplyJSON : public edm::EDFilter 
{
  public:
    ApplyJSON(edm::ParameterSet const& cfg);
    virtual bool filter(edm::Event & iEvent, edm::EventSetup const& iSetup);
    virtual ~ApplyJSON();

  private:  
    void parse();
    std::string jsonFile_;
    std::vector<std::pair<int,std::vector<std::pair<int,int> > > > vrun_;
};





#endif
