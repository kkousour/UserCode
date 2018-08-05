#ifndef TTVFlatTreeProducer_h
#define TTVFlatTreeProducer_h

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h" 
#include "FWCore/Common/interface/TriggerNames.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "HLTrigger/HLTcore/interface/TriggerExpressionData.h"
#include "HLTrigger/HLTcore/interface/TriggerExpressionEvaluator.h"
#include "HLTrigger/HLTcore/interface/TriggerExpressionParser.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/Candidate/interface/Particle.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Particle.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/PackedTriggerPrescales.h"
#include "DataFormats/JetReco/interface/JetCollection.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "UserCode/TopAnalysis/plugins/TTVDiscriminatorMVA.h"
#include "TTree.h"
#include "TH1F.h"
#include "TLorentzVector.h"

class TTVFlatTreeProducer : public edm::EDAnalyzer 
{
  public:
    typedef reco::Particle::LorentzVector LorentzVector;
    explicit TTVFlatTreeProducer(edm::ParameterSet const& cfg);
    virtual void beginJob();
    virtual void beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup);
    virtual void endRun(edm::Run const& iRun, edm::EventSetup const& iSetup);
    virtual void analyze(edm::Event const& iEvent, edm::EventSetup const& iSetup);
    virtual void endJob();
    virtual ~TTVFlatTreeProducer();

  private:  
    virtual bool isGoodMuon(const pat::Muon &mu,const reco::Vertex &vtx,float rho);
    virtual bool isGoodElectron(const pat::Electron &el,const reco::Vertex &vtx,float rho);
    virtual bool isGoodJet(const pat::Jet &jet);
    float MuonRelIso(const reco::Candidate *cand,float rho);
    float ElectronRelIso(const reco::Candidate *cand,float rho);
    float LeptonRelIso(const reco::Candidate *cand,float rho){return cand->isElectron() ? ElectronRelIso(cand,rho) : MuonRelIso(cand,rho);}
    void initialize();
    //---- configurable parameters -------- 
    edm::EDGetTokenT<pat::JetCollection> jetsToken;
    edm::EDGetTokenT<pat::MuonCollection> muonsToken;
    edm::EDGetTokenT<pat::ElectronCollection> electronsToken;
    edm::EDGetTokenT<pat::METCollection>  metToken;
    edm::EDGetTokenT<double> rhoToken;
    edm::EDGetTokenT<reco::VertexCollection> recVtxsToken;
    edm::EDGetTokenT<edm::TriggerResults> triggerResultsToken;
    edm::EDGetTokenT<pat::PackedTriggerPrescales> triggerPrescalesToken;
    edm::EDGetTokenT<edm::View<PileupSummaryInfo> > pupInfoToken;
    edm::EDGetTokenT<GenEventInfoProduct> genEvtInfoToken;
    edm::EDGetTokenT<edm::View<reco::GenParticle> > genParticlesToken;
    edm::EDGetTokenT<LHEEventProduct> lheEvtInfoToken;
    edm::EDGetTokenT<LHERunInfoProduct> runInfoToken;
    std::string kinfit_; 
    edm::EDGetTokenT<edm::View<double> > vchi2Token;
    edm::EDGetTokenT<edm::View<double> > vprobToken;
    edm::EDGetTokenT<edm::View<int> > vstatusToken;
    edm::EDGetTokenT<edm::View<pat::Particle> > partonsBToken;
    edm::EDGetTokenT<edm::View<pat::Particle> > partonsBbarToken;
    edm::EDGetTokenT<edm::View<pat::Particle> > partonsQToken;
    edm::EDGetTokenT<edm::View<pat::Particle> > partonsQbarToken;
    edm::EDGetTokenT<edm::View<pat::Particle> > partonsPToken;
    edm::EDGetTokenT<edm::View<pat::Particle> > partonsPbarToken;
   
    std::string srcBtag_,xmlFileTTW_,xmlFileTTZ_;
    std::vector<std::string> triggerNames_;
    double etaMax_,ptMin_,probMin_,btagMin_,minMuPt_,minElPt_;
    bool   isMC_,saveWeights_,debug_;
    //------------------------------------------------------------------------    
    edm::Service<TFileService> fs_;
    TTree *outTree_; 
    TH1F *cutFlowHisto_;
    //---- TRIGGER -------------------------
    TH1F *triggerPassHisto_,*triggerNamesHisto_;
    //---- output TREE variables ------
    //---- global event variables -----
    int   run_,evt_,nVtx_,lumi_,nJets_,nBJets_,nLeptons_,status_;
    float rho_,met_,metSig_,ht_,prob_,chi2_,mvaTTW_,mvaTTZ_;
    std::vector<bool> *triggerBit_;
    std::vector<int>  *triggerPre_;
    //---- top variables --------------
    float mTop_[2],mW_[2],ptTop_[2],yTop_[2],phiTop_[2],etaTop_[2],dRbbTop_,mTTbar_,yTTbar_,ptTTbar_;
    //---- jet variables --------------
    std::vector<bool>  *isBtag_;
    std::vector<int>   *flavor_;
    std::vector<float> *pt_,*eta_,*phi_,*mass_,*energy_,*chf_,*nhf_,*phf_,*elf_,*muf_,*btag_,*puMva_;
    //---- lepton variables -----------
    std::vector<int>   *lId_;
    std::vector<float> *lPt_,*lEta_,*lPhi_,*lE_,*lIso_;
    float llMass_,llPt_,llRapidity_,llPhi_;
    float dRlbMin_,dRlbMax_,dPhilbMin_,dPhilbMax_,dPhilTTbar_;
    //---- MVA discriminators ----------
    TTVDiscriminatorMVA  *discrTTW_,*discrTTZ_;
    //---- MC variables ---------------
    int npu_;
    float genEvtWeight_,lheOriginalXWGTUP_;
    std::vector<float> *scaleWeights_;
    std::vector<float> *pdfWeights_;

    edm::Handle<pat::JetCollection> jets;
    edm::Handle<pat::MuonCollection> muons;
    edm::Handle<pat::ElectronCollection> electrons;
    edm::Handle<pat::METCollection>  met;
    edm::Handle<double> rho;
    edm::Handle<reco::VertexCollection> recVtxs;
    edm::Handle<edm::TriggerResults> triggerResults;
    edm::Handle<pat::PackedTriggerPrescales> triggerPrescales;
    edm::Handle<edm::View<PileupSummaryInfo> > pupInfo;
    edm::Handle<edm::View<reco::GenParticle> > genParticles;
    edm::Handle<GenEventInfoProduct> genEvtInfo;
    edm::Handle<LHEEventProduct> lheEvtInfo;
    edm::Handle<LHERunInfoProduct> runInfo;
    edm::Handle<edm::View<double> > vchi2;
    edm::Handle<edm::View<double> > vprob;
    edm::Handle<edm::View<int> > vstatus;
    edm::Handle<edm::View<pat::Particle> > partonsB;
    edm::Handle<edm::View<pat::Particle> > partonsBbar;
    edm::Handle<edm::View<pat::Particle> > partonsQ;
    edm::Handle<edm::View<pat::Particle> > partonsQbar;
    edm::Handle<edm::View<pat::Particle> > partonsP;
    edm::Handle<edm::View<pat::Particle> > partonsPbar;
};





#endif
