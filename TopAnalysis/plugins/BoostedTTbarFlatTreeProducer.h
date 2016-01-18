#ifndef BoostedTTbarFlatTreeProducer_h
#define BoostedTTbarFlatTreeProducer_h

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "HLTrigger/HLTcore/interface/TriggerExpressionData.h"
#include "HLTrigger/HLTcore/interface/TriggerExpressionEvaluator.h"
#include "HLTrigger/HLTcore/interface/TriggerExpressionParser.h"
#include "DataFormats/Candidate/interface/Particle.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "KKousour/TopAnalysis/plugins/BoostedDiscriminatorMVA.h"
#include "TTree.h"
#include "TH1F.h"
#include "TLorentzVector.h"

class BoostedTTbarFlatTreeProducer : public edm::EDAnalyzer 
{
  public:
    typedef reco::Particle::LorentzVector LorentzVector;
    explicit BoostedTTbarFlatTreeProducer(edm::ParameterSet const& cfg);
    virtual void beginJob();
    virtual void analyze(edm::Event const& iEvent, edm::EventSetup const& iSetup);
    virtual void endJob();
    virtual ~BoostedTTbarFlatTreeProducer();

  private:  
    virtual bool isGoodMuon(const pat::Muon &mu,const reco::Vertex &vtx,float rho);
    virtual bool isGoodElectron(const pat::Electron &el,const reco::Vertex &vtx,float rho);
    virtual bool isGoodJet(const pat::Jet &jet);
    float MuonRelIso(const reco::Candidate *cand,float rho);
    float ElectronRelIso(const reco::Candidate *cand,float rho);
    float LeptonRelIso(const reco::Candidate *cand,float rho){return cand->isElectron() ? ElectronRelIso(cand,rho) : MuonRelIso(cand,rho);}
    void initialize();
    //---- configurable parameters --------   
    edm::InputTag srcJets_,srcMET_,srcMuons_,srcElectrons_,srcRho_,srcVtx_,triggerResults_,triggerPrescales_,srcGenParticles_;
    std::string srcBtag_,srcPU_,xmlFile_;
    double massMin_;
    double ptMin_;
    double ptMinLeading_;
    double etaMax_;
    double btagMinThreshold_,btagMaxThreshold_;
    edm::Service<TFileService> fs_;
    TTree *outTree_; 
    TH1F *puHisto_,*cutFlowHisto_;
    //---- TRIGGER -------------------------
    std::vector<std::string> triggerNames_;
    TH1F *triggerPassHisto_,*triggerNamesHisto_;
    //---- output TREE variables ------
    //---- global event variables -----
    int   run_,evt_,nVtx_,lumi_,nJets_,nBJets_,nLeptons_;
    float rho_,met_,metSig_,ht_,mva_;
    std::vector<bool> *triggerBit_;
    std::vector<int>  *triggerPre_;
    //---- top variables --------------
    float dRJJ_,dPhiJJ_,mJJ_,yJJ_,ptJJ_;
    //---- jet variables --------------
    std::vector<bool>  *isBtag_;
    std::vector<int>   *flavor_,*nSubJets_,*nBSubJets_;
    std::vector<float> *pt_,*eta_,*phi_,*mass_,*massSoftDrop_,*energy_,*chf_,*nhf_,*phf_,*elf_,*muf_,*btag_,*tau1_,*tau2_,*tau3_;
    std::vector<float> *btagSub0_,*btagSub1_,*massSub0_,*massSub1_;
    //---- lepton variables -----------
    std::vector<int>   *lId_;
    std::vector<float> *lPt_,*lEta_,*lPhi_,*lE_,*lIso_;
    //---- MVA discriminator ----------
    BoostedDiscriminatorMVA *discr_;
    //---- MC variables ---------------
    int npu_,decay_;
};





#endif
