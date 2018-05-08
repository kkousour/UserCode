// system include files
#include <memory>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <functional>
#include <cassert>
#include "Math/SpecFuncMathMore.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/Service.h" 
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"
#include "DataFormats/Candidate/interface/Particle.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "CommonTools/Utils/interface/PtComparator.h"

class JetCleanedProducer : public edm::EDProducer {
   public:
      explicit JetCleanedProducer(const edm::ParameterSet&);
      virtual ~JetCleanedProducer();

   private:
      virtual void beginJob() ;
      virtual void produce(edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;
      virtual bool isGoodMuon(const pat::Muon &mu,const reco::Vertex &vtx,float rho);
      virtual bool isGoodElectron(const pat::Electron &el,const reco::Vertex &vtx,float rho);
      float MuonRelIso(const reco::Candidate *cand,float rho);
      float ElectronRelIso(const reco::Candidate *cand,float rho);
      float LeptonRelIso(const reco::Candidate *cand,float rho){return cand->isElectron() ? ElectronRelIso(cand,rho) : MuonRelIso(cand,rho);}
      
      double minMuPt_,minElPt_;

      edm::EDGetTokenT<pat::JetCollection> jetsToken; 
      edm::EDGetTokenT<pat::MuonCollection> muonsToken;
      edm::EDGetTokenT<pat::ElectronCollection> electronsToken;
      edm::EDGetTokenT<reco::VertexCollection> recVtxsToken;
      edm::EDGetTokenT<double> rhoToken; 
     
      edm::Handle<pat::JetCollection> jets;
      edm::Handle<pat::MuonCollection> muons;
      edm::Handle<pat::ElectronCollection> electrons;
      edm::Handle<reco::VertexCollection> recVtxs;
      edm::Handle<double> rho; 
};
//////////////////////////////////////////////////////////////////////////////////////////
JetCleanedProducer::JetCleanedProducer(const edm::ParameterSet& iConfig)
{
  jetsToken      = consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("jets"));
  muonsToken     = consumes<pat::MuonCollection>(iConfig.getParameter<edm::InputTag>("muons"));
  electronsToken = consumes<pat::ElectronCollection>(iConfig.getParameter<edm::InputTag>("electrons"));
  recVtxsToken   = consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertices"));
  rhoToken       = consumes<double>(iConfig.getParameter<edm::InputTag>("rho"));
  minMuPt_       = iConfig.getParameter<double>("minMuPt");
  minElPt_       = iConfig.getParameter<double>("minElPt");
  
  produces<pat::JetCollection>();
}

JetCleanedProducer::~JetCleanedProducer()
{
}
//////////////////////////////////////////////////////////////////////////////////////////
bool JetCleanedProducer::isGoodMuon(const pat::Muon &mu,const reco::Vertex &vtx,float rho)
{
  bool res = true; // by default is good, unless fails a cut bellow
  if(mu.pt() < minMuPt_) res = false;
  if(fabs(mu.eta()) > 2.4) res = false;
  if(!mu.isTightMuon(vtx)) res = false;
  // --- isolation --- those not used are commented out
  if(res && LeptonRelIso((reco::Candidate*)&mu,rho) > 0.15) res = false;
  return res;
}
//////////////////////////////////////////////////////////////////////////////////////////
float JetCleanedProducer::MuonRelIso(const reco::Candidate *cand,float rho)
{
  pat::Muon mu = *((pat::Muon*)cand);
  reco::MuonPFIsolation pfIso = mu.pfIsolationR04();
  float relIso = (float)(pfIso.sumChargedHadronPt+std::max(0.0,pfIso.sumNeutralHadronEt+pfIso.sumPhotonEt-0.5*pfIso.sumPUPt))/mu.pt();
  return relIso;
}
//////////////////////////////////////////////////////////////////////////////////////////
bool JetCleanedProducer::isGoodElectron(const pat::Electron &el,const reco::Vertex &vtx,float rho)
{
  bool res = true; // by default is good, unless fails a cut bellow
  bool isEBEEGap = fabs(el.superCluster()->eta()) > 1.4442 && fabs(el.superCluster()->eta()) < 1.5660 ? 1 : 0;
  if(el.pt() < minElPt_) res = false;
  if(fabs(el.eta()) > 2.4 && res == true) res = false;
  if(isEBEEGap && res==true) res = false;
  bool isEB = fabs(el.superCluster()->eta()) < 1.479 ? 1 : 0;
  bool isEE = fabs(el.superCluster()->eta()) > 1.479 ? 1 : 0;
  if(res) {
    float trackMomentumAtVtx = (float)sqrt(el.trackMomentumAtVtx().mag2());
    float ecalEnergy = (float)el.ecalEnergy();
    float full5x5_sigmaIetaIeta = (float)el.full5x5_sigmaIetaIeta();
    float dEtaIn = (float)el.deltaEtaSuperClusterTrackAtVtx();
    float dPhiIn = (float)el.deltaPhiSuperClusterTrackAtVtx();
    float HoE = (float)el.hadronicOverEm();
    float ooEmooP = (float)fabs(1/ecalEnergy - 1/trackMomentumAtVtx);
    float d0 = (float)el.gsfTrack()->dxy(vtx.position());
    float dz = (float)el.gsfTrack()->dz(vtx.position());
    int expectedMissingInnerHits = el.gsfTrack()->hitPattern().numberOfHits(reco::HitPattern::MISSING_INNER_HITS);
    bool passConversionVeto = el.passConversionVeto();
    if(isEB) {// medium working point
      if(res && full5x5_sigmaIetaIeta > 0.0101) res = false;
      if(res && fabs(dEtaIn) > 0.0103) res = false;
      if(res && fabs(dPhiIn) > 0.0336) res = false;
      if(res && HoE > 0.0876) res = false;
      if(res && ooEmooP > 0.0174) res = false;
      if(res && fabs(d0) > 0.0118) res = false;
      if(res && fabs(dz) > 0.373) res = false;
      if(res && expectedMissingInnerHits >= 2 ) res = false;
      if(res && passConversionVeto == false ) res = false;
      if(res && LeptonRelIso((reco::Candidate*)&el,rho) > 0.0766) res = false;
    }
    if(isEE) {// medium working point
      if(res && full5x5_sigmaIetaIeta > 0.0283) res = false;
      if(res && fabs(dEtaIn) > 0.00733) res = false;
      if(res && fabs(dPhiIn) > 0.114) res = false;
      if(res && HoE > 0.0678) res = false;
      if(res && ooEmooP > 0.0898) res = false;
      if(res && fabs(d0) > 0.0739) res = false;
      if(res && fabs(dz) > 0.602) res = false;
      if(res && expectedMissingInnerHits > 1 ) res = false;
      if(res && passConversionVeto == false ) res = false;
      if(res && LeptonRelIso((reco::Candidate*)&el,rho) > 0.0678) res = false;
    }
  }
  return res;
}
//////////////////////////////////////////////////////////////////////////////////////////
float JetCleanedProducer::ElectronRelIso(const reco::Candidate *cand,float rho)
{
  pat::Electron el = *((pat::Electron*)cand); 
  float relIsoWithEA = 0;
  const int nEtaBins = 7;
  const float etaBinLimits[nEtaBins+1] = {0.0, 1.0, 1.479, 2.0, 2.2, 2.3, 2.4, 2.5};
  const float effectiveAreaValues[nEtaBins] = {0.1752, 0.1862, 0.1411, 0.1534, 0.1903, 0.2243, 0.2687};
  reco::GsfElectron::PflowIsolationVariables pfIso = el.pfIsolationVariables();
  float etaSC = el.superCluster()->eta();
  // Find eta bin first. If eta>2.5, the last eta bin is used.
  int etaBin = 0;
  while(etaBin < nEtaBins-1 && abs(etaSC) > etaBinLimits[etaBin+1]) ++etaBin;
  float area = effectiveAreaValues[etaBin];
  relIsoWithEA = (float)(pfIso.sumChargedHadronPt+std::max(float(0.0),pfIso.sumNeutralHadronEt+pfIso.sumPhotonEt-rho*area))/el.pt();
  return relIsoWithEA;
}
// ------------ method called to produce the data  ------------
void JetCleanedProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  iEvent.getByToken(rhoToken,rho);
  iEvent.getByToken(jetsToken,jets);
  iEvent.getByToken(muonsToken,muons);
  iEvent.getByToken(electronsToken,electrons);
  iEvent.getByToken(recVtxsToken,recVtxs);

  std::unique_ptr<pat::JetCollection> result(new pat::JetCollection); //Cleaned jets
  const int size = jets->size();
  result->reserve(size);
  
  std::vector<const reco::Candidate *> myLeptons;
  
  if (recVtxs->size() > 0) {
    //----- loop over leptons --------------------
    for (const pat::Muon &mu : *muons) {
      if (isGoodMuon(mu,(*recVtxs)[0],*rho)) myLeptons.push_back(&mu);
    }
    for (const pat::Electron &el : *electrons) {
      if (isGoodElectron(el,(*recVtxs)[0],*rho)) myLeptons.push_back(&el);
    }
    std::sort(myLeptons.begin(),myLeptons.end(),[](const reco::Candidate *a,const reco::Candidate *b){return a->pt() > b->pt();});
  }// if vtx

  for(pat::JetCollection::const_iterator ijet = jets->begin(); ijet != jets->end(); ++ijet) {
    pat::Jet cleanedJet = *ijet; // copy original jet
    
    bool isLeptonMatched = false;
    float DRmax = 0.4;
    for(auto & lep: myLeptons) if( deltaR(lep->eta(),lep->phi(),ijet->eta(),ijet->phi()) < DRmax ) isLeptonMatched = true;
      if (!isLeptonMatched) {
        result->push_back(cleanedJet); 
      }	
  }
  
  //std::cout<<"Initial jets: "<<jets->size()<<", Leptons: "<<myLeptons.size()<<", final jets: "<<result->size()<<std::endl;
  
  NumericSafeGreaterByPt<pat::Jet> compJets;
  std::sort(result->begin(),result->end(),compJets);
  iEvent.put(std::move(result));

  return;
}

// ------------ method called once each job just before starting event loop  ------------
void JetCleanedProducer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void JetCleanedProducer::endJob() 
{
}
//define this as a plug-in
DEFINE_FWK_MODULE(JetCleanedProducer);















