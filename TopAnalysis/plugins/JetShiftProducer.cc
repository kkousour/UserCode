// system include files
#include <memory>
#include <iostream>
#include <string>
#include <vector>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/Service.h" 
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"

#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "JetMETCorrections/Objects/interface/JetCorrectionsRecord.h"
#include "JetMETCorrections/Objects/interface/JetCorrector.h"
#include "JetMETCorrections/Modules/interface/JetResolution.h"
#include "CommonTools/Utils/interface/PtComparator.h"

class JetShiftProducer : public edm::EDProducer {
   public:
      explicit JetShiftProducer(const edm::ParameterSet&);
      virtual ~JetShiftProducer();

   private:
      virtual void beginJob() ;
      virtual void produce(edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;
      
      edm::EDGetTokenT<pat::JetCollection> jetsToken; 
      edm::EDGetTokenT<double> rhoToken; 
      std::string payload_;
      std::string resSFFile_; 
      double shiftJES_,shiftJER_;
      bool doSmear_,doShift_,debug_,isJESset_;
      edm::Handle<pat::JetCollection> jets;
      edm::Handle<double> rho;
      JME::JetResolutionScaleFactor res_sf;
      JetCorrectorParameters par_;
      JetCorrectionUncertainty *jecUnc_; 
};

JetShiftProducer::JetShiftProducer(const edm::ParameterSet& iConfig)
{
  jetsToken  = consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("jets"));
  rhoToken   = consumes<double>(iConfig.getParameter<edm::InputTag>("rho"));
  payload_   = iConfig.getUntrackedParameter<std::string>("payload","");
  shiftJES_  = iConfig.getUntrackedParameter<double>("shiftJES",1.0);
  shiftJER_  = iConfig.getUntrackedParameter<double>("shiftJER",0.0);
  doSmear_   = iConfig.getUntrackedParameter<bool>("doSmear",false);
  doShift_   = iConfig.getUntrackedParameter<bool>("doShift",false);
  debug_     = iConfig.getUntrackedParameter<bool>("debug",false);
  if (doSmear_) {
    resSFFile_ = iConfig.getUntrackedParameter<std::string>("resSFFile","");
  }
  produces<pat::JetCollection>();
}

JetShiftProducer::~JetShiftProducer()
{
}

// ------------ method called to produce the data  ------------
void JetShiftProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  iEvent.getByToken(rhoToken,rho);
  iEvent.getByToken(jetsToken,jets);
  pat::JetCollection pat_jets = *jets;  

  std::unique_ptr<pat::JetCollection> result(new pat::JetCollection); //Shifted jets
  const int size = pat_jets.size();
  result->reserve(size);

  //-------------- Set the JEC uncertainty object ----------------
  edm::ESHandle<JetCorrectorParametersCollection> JetCorParColl;
  iSetup.get<JetCorrectionsRecord>().get(payload_,JetCorParColl); 
  if (!isJESset_ && doShift_) {
    par_ = (*JetCorParColl)["Uncertainty"]; 
    jecUnc_ = new JetCorrectionUncertainty(par_);
    isJESset_ = true;
  }
  //-------------- Set the JER scale factor object ----------------
  /*
  res_sf = JME::JetResolutionScaleFactor::get(iSetup,payload_+"_pt"); 
  */
  if (doSmear_) {
    res_sf = JME::JetResolutionScaleFactor(edm::FileInPath("UserCode/TopAnalysis/data/"+resSFFile_).fullPath());
  }
  
  for(pat::JetCollection::const_iterator ijet = pat_jets.begin(); ijet != pat_jets.end(); ++ijet) {
    pat::Jet shiftedJet = *ijet; // copy original jet
    float aJES = 1.0;
    if (doShift_) {
      jecUnc_->setJetEta(ijet->eta());
      jecUnc_->setJetPt(ijet->pt());
      aJES = 1+shiftJES_*jecUnc_->getUncertainty(true);
    }
    float aJER = 1.0;
    float sigma_sf = 1.0;
    float ptGen = ijet->pt();  
    if (doSmear_) {
      if (shiftJER_ > 0) { 
        sigma_sf = res_sf.getScaleFactor({{JME::Binning::JetEta,ijet->eta()}},Variation::UP);  
      }
      else if (shiftJER_ < 0) { 
        sigma_sf = res_sf.getScaleFactor({{JME::Binning::JetEta,ijet->eta()}},Variation::DOWN);  
      }
      else {
        sigma_sf = res_sf.getScaleFactor({{JME::Binning::JetEta,ijet->eta()}});
      }
      if (ijet->genJet()) {
        ptGen = ijet->genJet()->pt();
      }
      aJER = TMath::Max(0.0,ptGen+sigma_sf*(ijet->pt()-ptGen))/ijet->pt();
    }
    double factor = aJES*aJER;
    shiftedJet.scaleEnergy(factor);
    if (shiftedJet.isPFJet()) {
      pat::PFSpecific specificPF = shiftedJet.pfSpecific();
      specificPF.mChargedHadronEnergy *= factor;
      specificPF.mNeutralHadronEnergy *= factor;
      specificPF.mPhotonEnergy *= factor;
      specificPF.mElectronEnergy *= factor;
      specificPF.mMuonEnergy *= factor;
      specificPF.mHFHadronEnergy *= factor;
      specificPF.mHFEMEnergy *= factor;
      specificPF.mChargedEmEnergy *= factor;
      specificPF.mChargedMuEnergy *= factor;
      specificPF.mNeutralEmEnergy *= factor;
      shiftedJet.setPFSpecific(specificPF);
    }
    
    if (debug_) {
      std::cout<<"original pt = "<<ijet->pt()<<", shifted pt = "<<shiftedJet.pt()<<", eta = "<<shiftedJet.eta()<<", aJES = "<<aJES<<", aJER = "<<aJER<<", sigma_sf = "<<sigma_sf<<std::endl;
    }
    result->push_back(shiftedJet); 
  }
  NumericSafeGreaterByPt<pat::Jet> compJets;
  std::sort(result->begin(),result->end(),compJets);
  iEvent.put(std::move(result));

  return;
}

// ------------ method called once each job just before starting event loop  ------------
void JetShiftProducer::beginJob()
{
  isJESset_ = false;
}

// ------------ method called once each job just after ending the event loop  ------------
void JetShiftProducer::endJob() 
{
  if (isJESset_) {
    delete jecUnc_;
  }
}
//define this as a plug-in
DEFINE_FWK_MODULE(JetShiftProducer);















