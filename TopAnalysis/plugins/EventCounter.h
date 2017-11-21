#ifndef EventCounter_h
#define EventCounter_h

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/Candidate/interface/Particle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "TH1F.h"

class EventCounter : public edm::EDAnalyzer 
{
  public:
    typedef reco::Particle::LorentzVector LorentzVector;
    explicit EventCounter(edm::ParameterSet const& cfg);
    virtual void beginJob();
    virtual void analyze(edm::Event const& iEvent, edm::EventSetup const& iSetup);
    virtual void endJob();
    virtual ~EventCounter();

  private:  
    edm::Service<TFileService> fs_;
    TH1F *puHisto_,*evtWtHisto_,*decayHisto_;
    TH1F *h_ptTopParton_[2],*h_yTopParton_[2],*h_mTTbarParton_,*h_yTTbarParton_,*h_ptTTbarParton_;
    TH1F *h_ptTopPartonFiducial_[2],*h_yTopPartonFiducial_[2],*h_mTTbarPartonFiducial_,*h_yTTbarPartonFiducial_,*h_ptTTbarPartonFiducial_;
 
    double etaTopMax_,ptTopMin_;

    edm::Handle<edm::View<PileupSummaryInfo> > pupInfo;
    edm::Handle<GenEventInfoProduct> genEvtInfo;
    edm::Handle<edm::View<reco::GenParticle> > genParticles;

    edm::EDGetTokenT<edm::View<PileupSummaryInfo> > pupInfoToken;
    edm::EDGetTokenT<GenEventInfoProduct> genEvtInfoToken;
    edm::EDGetTokenT<edm::View<reco::GenParticle> > genParticlesToken;
};

#endif
