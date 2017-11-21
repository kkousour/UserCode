#ifndef JMEFlatTreeProducer_h
#define JMEFlatTreeProducer_h

#include "FWCore/Framework/interface/Event.h"
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
#include "DataFormats/PatCandidates/interface/PackedTriggerPrescales.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Particle.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/JetReco/interface/JetCollection.h"
#include "DataFormats/JetReco/interface/PileupJetIdentifier.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "TTree.h"
#include "TH1F.h"
#include "TLorentzVector.h"

class JMEFlatTreeProducer : public edm::EDAnalyzer 
{
  public:
    explicit JMEFlatTreeProducer(edm::ParameterSet const& cfg);
    virtual void beginJob();
    virtual void analyze(edm::Event const& iEvent, edm::EventSetup const& iSetup);
    virtual void endJob();
    virtual ~JMEFlatTreeProducer();

  private:  
    virtual bool isGoodMuon(const pat::Muon &mu,const reco::Vertex &vtx);
    virtual bool isGoodElectron(const pat::Electron &el,const reco::Vertex &vtx,float rho);
    float MuonRelIso(const reco::Candidate *cand);
    float ElectronRelIso(const reco::Candidate *cand,float rho);
    float LeptonRelIso(const reco::Candidate *cand,float rho){return cand->isElectron() ? ElectronRelIso(cand,rho) : MuonRelIso(cand);}
    void initialize();
    //---- configurable parameters --------  
    edm::EDGetTokenT<pat::JetCollection> jetsToken;
    edm::EDGetTokenT<pat::MuonCollection> muonsToken;
    edm::EDGetTokenT<pat::ElectronCollection> electronsToken;
    edm::EDGetTokenT<pat::METCollection> metToken;
    edm::EDGetTokenT<double> rhoToken;
    edm::EDGetTokenT<reco::VertexCollection> recVtxsToken;
    edm::EDGetTokenT<edm::ValueMap<StoredPileupJetIdentifier> > pileupJetIdToken;
    edm::EDGetTokenT<edm::ValueMap<int> > pileupJetIdFlagToken;
    edm::EDGetTokenT<edm::ValueMap<float> > pileupJetIdDiscriminantToken;
    edm::EDGetTokenT<edm::TriggerResults> triggerResultsToken;
    edm::EDGetTokenT<pat::PackedTriggerPrescales> triggerPrescalesToken;
    edm::EDGetTokenT<edm::View<PileupSummaryInfo> > pupInfoToken;
    edm::EDGetTokenT<GenEventInfoProduct> genEvtInfoToken;
    edm::EDGetTokenT<reco::GenJetCollection> genJetsToken;
    std::string srcBtag_;
    std::vector<std::string> triggerNames_;
    double minLepPt_,maxLepEta_,minLLMass_,maxLLMass_;
    bool isMC_;
    //---------------------------------------------------
    edm::Service<TFileService> fs_;
    TTree *outTree_; 
    //---- TRIGGER -------------------------  
    TH1F *triggerPassHisto_,*triggerNamesHisto_,*cutFlowHisto_;
    std::vector<bool> *triggerBit_;
    std::vector<int>  *triggerPre_;
    //---- output TREE variables ------
    bool  isZEvent_;
    int   run_,evt_,nVtx_,lumi_,nLeptons_,nJets_;
    float rho_,met_,pvz_,sumet_,llMass_,llPt_,llPhi_,llRapidity_,dPhiZJ_;
    std::vector<bool>  *puIdLoose_,*puIdMedium_,*puIdTight_;
    std::vector<int>   *flavorParton_,*flavorHadron_,*npr_,*chm_,*nhm_,*phm_,*elm_,*mum_;
    std::vector<float> *pt_,*eta_,*phi_,*mass_,*energy_,*chf_,*nhf_,*nemf_,*cemf_,*phf_,*elf_,*muf_,*btag_,*pumva_;
    std::vector<float> *hHFf_,*eHFf_;
    std::vector<int>   *hHFm_,*eHFm_;
    std::vector<float> *genDR_,*genPt_;
    std::vector<float> *dR2Mean_,*majW_,*minW_,*frac01_,*frac02_,*frac03_,*frac04_,*ptD_,*beta_,*betaStar_,*pull_,*jetR_,*jetRchg_;
    std::vector<int>   *nParticles_,*nCharged_,*nNeutral_;
    //---- lepton variables -----------
    std::vector<int>   *lId_;
    std::vector<float> *lPt_,*lEta_,*lPhi_,*lE_,*lIso_;
    int npu_;
    float genEvtWeight_;

    edm::Handle<pat::JetCollection> jets;
    edm::Handle<pat::MuonCollection> muons;
    edm::Handle<pat::ElectronCollection> electrons;
    edm::Handle<reco::GenJetCollection> genjets;
    edm::Handle<pat::METCollection>  met;
    edm::Handle<double> rho;
    edm::Handle<reco::VertexCollection> recVtxs;
    edm::Handle<edm::ValueMap<StoredPileupJetIdentifier> > pileupJetId; 
    edm::Handle<edm::ValueMap<int> > pileupJetIdFlag;
    edm::Handle<edm::ValueMap<float> > pileupJetIdDiscriminant;
    edm::Handle<edm::TriggerResults> triggerResults;
    edm::Handle<pat::PackedTriggerPrescales> triggerPrescales;
    edm::Handle<edm::View<PileupSummaryInfo> > pupInfo;
    edm::Handle<GenEventInfoProduct> genEvtInfo;
};





#endif
