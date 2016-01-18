#include <string>
#include <cmath>
#include <functional>
#include <vector>
#include <cassert>
#include "Math/SpecFuncMathMore.h"
#include "TMath.h"
#include "TVectorD.h"
#include "TMatrixDSym.h"
#include "TMatrixDSymEigen.h"

#include "KKousour/TopAnalysis/plugins/BoostedTTbarFlatTreeProducer.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/PatCandidates/interface/Particle.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/PackedTriggerPrescales.h"
#include "DataFormats/JetReco/interface/JetCollection.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

using namespace std;
using namespace reco;

BoostedTTbarFlatTreeProducer::BoostedTTbarFlatTreeProducer(edm::ParameterSet const& cfg) 
{
  srcJets_            = cfg.getParameter<edm::InputTag>                      ("jets");
  srcMuons_           = cfg.getParameter<edm::InputTag>                      ("muons");
  srcElectrons_       = cfg.getParameter<edm::InputTag>                      ("electrons");
  srcMET_             = cfg.getParameter<edm::InputTag>                      ("met");
  srcRho_             = cfg.getParameter<edm::InputTag>                      ("rho"); 
  srcVtx_             = cfg.getParameter<edm::InputTag>                      ("vertices");
  srcBtag_            = cfg.getParameter<std::string>                        ("btagger");
  xmlFile_            = cfg.getParameter<std::string>                        ("xmlFile");
  etaMax_             = cfg.getParameter<double>                             ("etaMax");
  ptMin_              = cfg.getParameter<double>                             ("ptMin");
  ptMinLeading_       = cfg.getParameter<double>                             ("ptMinLeading");
  massMin_            = cfg.getParameter<double>                             ("massMin");
  btagMinThreshold_   = cfg.getParameter<double>                             ("btagMinThreshold");
  btagMaxThreshold_   = cfg.getParameter<double>                             ("btagMaxThreshold");
  srcPU_              = cfg.getUntrackedParameter<std::string>               ("pu","");
  srcGenParticles_    = cfg.getUntrackedParameter<edm::InputTag>             ("genparticles",edm::InputTag("")); 
  triggerNames_       = cfg.getParameter<std::vector<std::string> >          ("triggerNames");
  triggerResults_     = cfg.getParameter<edm::InputTag>                      ("triggerResults");
  triggerPrescales_   = cfg.getParameter<edm::InputTag>                      ("triggerPrescales");
}
//////////////////////////////////////////////////////////////////////////////////////////
void BoostedTTbarFlatTreeProducer::beginJob() 
{
  //--- book the trigger histograms ---------
  triggerNamesHisto_ = fs_->make<TH1F>("TriggerNames","TriggerNames",1,0,1);
  triggerNamesHisto_->SetBit(TH1::kCanRebin);
  for(unsigned i=0;i<triggerNames_.size();i++) {
    triggerNamesHisto_->Fill(triggerNames_[i].c_str(),1);
  }
  triggerPassHisto_ = fs_->make<TH1F>("TriggerPass","TriggerPass",1,0,1);
  triggerPassHisto_->SetBit(TH1::kCanRebin);

  cutFlowHisto_ = fs_->make<TH1F>("CutFlow","CutFlow",1,0,1);
  cutFlowHisto_->SetBit(TH1::kCanRebin);
  
  //--- book the pileup histogram -----------
  puHisto_ = fs_->make<TH1F>("pileup","pileup",40,0,40);
  //--- book the tree -----------------------
  outTree_ = fs_->make<TTree>("events","events");
  outTree_->Branch("runNo"                ,&run_               ,"run_/I");
  outTree_->Branch("evtNo"                ,&evt_               ,"evt_/I");
  outTree_->Branch("lumi"                 ,&lumi_              ,"lumi_/I");
  outTree_->Branch("nvtx"                 ,&nVtx_              ,"nVtx_/I");
  outTree_->Branch("nJets"                ,&nJets_             ,"nJets_/I");
  outTree_->Branch("nLeptons"             ,&nLeptons_          ,"nLeptons_/I");
  outTree_->Branch("nBJets"               ,&nBJets_            ,"nBJets_/I");
  outTree_->Branch("rho"                  ,&rho_               ,"rho_/F");
  outTree_->Branch("ht"                   ,&ht_                ,"ht_/F");
  outTree_->Branch("mva"                  ,&mva_               ,"mva_/F");
  outTree_->Branch("met"                  ,&met_               ,"met_/F");
  outTree_->Branch("metSig"               ,&metSig_            ,"metSig_/F");
  outTree_->Branch("mJJ"                  ,&mJJ_               ,"mJJ_/F");
  outTree_->Branch("yJJ"                  ,&yJJ_               ,"yJJ_/F");
  outTree_->Branch("ptJJ"                 ,&ptJJ_              ,"ptJJ_/F");
  outTree_->Branch("dRJJ"                 ,&dRJJ_              ,"dRJJ_/F");
  outTree_->Branch("dPhiJJ"               ,&dPhiJJ_            ,"dPhiJJ_/F");
  //------------------------------------------------------------------
  isBtag_         = new std::vector<bool>;
  flavor_         = new std::vector<int>;
  nSubJets_       = new std::vector<int>;
  nBSubJets_      = new std::vector<int>;
  pt_             = new std::vector<float>;
  btag_           = new std::vector<float>;  
  eta_            = new std::vector<float>;
  phi_            = new std::vector<float>;
  mass_           = new std::vector<float>;
  massSoftDrop_   = new std::vector<float>;
  energy_         = new std::vector<float>;
  chf_            = new std::vector<float>;
  nhf_            = new std::vector<float>;
  phf_            = new std::vector<float>;
  muf_            = new std::vector<float>;
  elf_            = new std::vector<float>;
  tau1_           = new std::vector<float>;
  tau2_           = new std::vector<float>;
  tau3_           = new std::vector<float>;
  btagSub0_       = new std::vector<float>;
  btagSub1_       = new std::vector<float>;
  massSub0_       = new std::vector<float>;
  massSub1_       = new std::vector<float>;
  lId_            = new std::vector<int>;
  lPt_            = new std::vector<float>;
  lEta_           = new std::vector<float>;
  lPhi_           = new std::vector<float>;
  lE_             = new std::vector<float>;
  lIso_           = new std::vector<float>;
  outTree_->Branch("jetIsBtag"            ,"vector<bool>"      ,&isBtag_); 
  outTree_->Branch("jetFlavor"            ,"vector<int>"       ,&flavor_);
  outTree_->Branch("jetNSub"              ,"vector<int>"       ,&nSubJets_);
  outTree_->Branch("jetNBSub"             ,"vector<int>"       ,&nBSubJets_);
  outTree_->Branch("jetPt"                ,"vector<float>"     ,&pt_);
  outTree_->Branch("jetBtag"              ,"vector<float>"     ,&btag_);  
  outTree_->Branch("jetEta"               ,"vector<float>"     ,&eta_);
  outTree_->Branch("jetPhi"               ,"vector<float>"     ,&phi_);
  outTree_->Branch("jetMass"              ,"vector<float>"     ,&mass_);
  outTree_->Branch("jetMassSoftDrop"      ,"vector<float>"     ,&massSoftDrop_);
  outTree_->Branch("jetEnergy"            ,"vector<float>"     ,&energy_);
  outTree_->Branch("jetChf"               ,"vector<float>"     ,&chf_);
  outTree_->Branch("jetNhf"               ,"vector<float>"     ,&nhf_);
  outTree_->Branch("jetPhf"               ,"vector<float>"     ,&phf_);
  outTree_->Branch("jetMuf"               ,"vector<float>"     ,&muf_);
  outTree_->Branch("jetElf"               ,"vector<float>"     ,&elf_);
  outTree_->Branch("jetTau1"              ,"vector<float>"     ,&tau1_);
  outTree_->Branch("jetTau2"              ,"vector<float>"     ,&tau2_);
  outTree_->Branch("jetTau3"              ,"vector<float>"     ,&tau3_);
  outTree_->Branch("jetBtagSub0"          ,"vector<float>"     ,&btagSub0_);
  outTree_->Branch("jetBtagSub1"          ,"vector<float>"     ,&btagSub1_);
  outTree_->Branch("jetMassSub0"          ,"vector<float>"     ,&massSub0_);
  outTree_->Branch("jetMassSub1"          ,"vector<float>"     ,&massSub1_);
  outTree_->Branch("lepId"                ,"vector<int>"       ,&lId_);
  outTree_->Branch("lepPt"                ,"vector<float>"     ,&lPt_);
  outTree_->Branch("lepEta"               ,"vector<float>"     ,&lEta_);
  outTree_->Branch("lepPhi"               ,"vector<float>"     ,&lPhi_);
  outTree_->Branch("lepEnergy"            ,"vector<float>"     ,&lE_);
  outTree_->Branch("lepIso"               ,"vector<float>"     ,&lIso_);
  //------------------------------------------------------------------
  triggerBit_ = new std::vector<bool>;
  triggerPre_ = new std::vector<int>;
  outTree_->Branch("triggerBit"           ,"vector<bool>"      ,&triggerBit_);
  outTree_->Branch("triggerPre"           ,"vector<int>"       ,&triggerPre_);
  //------------------------------------------------------------------
  discr_ = new BoostedDiscriminatorMVA("KKousour/TopAnalysis/data/"+xmlFile_);
  //------------------- MC ---------------------------------
  outTree_->Branch("decay"                ,&decay_             ,"decay_/I");
  outTree_->Branch("npu"                  ,&npu_               ,"npu_/I");
  cout<<"Begin job finished"<<endl;
}
//////////////////////////////////////////////////////////////////////////////////////////
void BoostedTTbarFlatTreeProducer::endJob() 
{  
  delete isBtag_;
  delete flavor_;
  delete nSubJets_;
  delete nBSubJets_;
  delete pt_;
  delete btag_;
  delete eta_;
  delete phi_;
  delete mass_;
  delete massSoftDrop_;
  delete energy_;
  delete chf_;
  delete nhf_;
  delete phf_;
  delete muf_;
  delete elf_;
  delete tau1_;
  delete tau2_;
  delete tau3_;
  delete btagSub0_;
  delete btagSub1_;
  delete massSub0_;
  delete massSub1_;
  delete triggerBit_;
  delete triggerPre_;
  delete lId_;
  delete lIso_;
  delete lPt_;
  delete lEta_;
  delete lPhi_;
  delete lE_;
  delete discr_;
}
//////////////////////////////////////////////////////////////////////////////////////////
bool BoostedTTbarFlatTreeProducer::isGoodJet(const pat::Jet &jet)
{
  bool res  = true; // by default is good, unless fails a cut bellow
  float chf = jet.chargedHadronEnergyFraction();
  float nhf = jet.neutralHadronEnergyFraction();
  float phf = jet.photonEnergyFraction();
  float muf = jet.muonEnergyFraction();
  float elf = jet.electronEnergyFraction();
  int chm   = jet.chargedHadronMultiplicity();
  int npr   = jet.neutralMultiplicity()+jet.chargedMultiplicity();
  float eta = fabs(jet.eta());
  float pt  = jet.pt();
  bool idL  = (npr>1 && phf<0.99 && nhf<0.99);
  //bool idM = (idL && ((eta<=2.4 && nhf<0.9 && phf<0.9 && elf<0.99 && muf<0.99 && chf>0 && chm>0) || eta>2.4));
  bool idT = (idL && ((eta<=2.4 && nhf<0.9 && phf<0.9 && elf<0.9 && muf<0.9 && chf>0 && chm>0) || eta>2.4));
  if (!idT) res = false;
  if (pt < ptMin_) res = false;
  if (eta > etaMax_) res = false;
  return res;
}
//////////////////////////////////////////////////////////////////////////////////////////
bool BoostedTTbarFlatTreeProducer::isGoodMuon(const pat::Muon &mu,const reco::Vertex &vtx,float rho)
{
  bool res = true; // by default is good, unless fails a cut bellow
  if(mu.pt() < 10) res = false;
  if(fabs(mu.eta()) > 2.4) res = false;
  if(!mu.isTightMuon(vtx)) res = false;
  // --- isolation --- those not used are commented out
  if(res && LeptonRelIso((reco::Candidate*)&mu,rho) > 0.15) res = false;
  return res;
}
//////////////////////////////////////////////////////////////////////////////////////////
float BoostedTTbarFlatTreeProducer::MuonRelIso(const reco::Candidate *cand,float rho)
{
  pat::Muon mu = *((pat::Muon*)cand);
  float relIsoWithEA = 0.001;
  const int nEtaBins = 5;
  const float etaBinLimits[nEtaBins+1] = {0.0, 0.8, 1.3, 2.0, 2.2, 2.5};
  const float effectiveAreaValues[nEtaBins] = {0.0913, 0.0765, 0.0546, 0.0728, 0.1177};
  reco::MuonPFIsolation pfIso = mu.pfIsolationR03();
  // Find eta bin first. If eta>2.5, the last eta bin is used.
  int etaBin = 0;
  while(etaBin < nEtaBins-1 && abs(mu.eta()) > etaBinLimits[etaBin+1]) ++etaBin;
  float area = effectiveAreaValues[etaBin];
  relIsoWithEA = (float)(pfIso.sumChargedHadronPt+max(float(0.0),pfIso.sumNeutralHadronEt+pfIso.sumPhotonEt-rho*area))/mu.pt();
  return relIsoWithEA;
}
//////////////////////////////////////////////////////////////////////////////////////////
bool BoostedTTbarFlatTreeProducer::isGoodElectron(const pat::Electron &el,const reco::Vertex &vtx,float rho)
{
  bool res = true; // by default is good, unless fails a cut bellow
  bool isEBEEGap = fabs(el.superCluster()->eta()) > 1.4442 && fabs(el.superCluster()->eta()) < 1.5660 ? 1 : 0;
  if(el.pt() < 10) res = false;
  if(fabs(el.eta()) > 2.4 && res == true) res = false;
  if(isEBEEGap && res==true) res = false;
  bool isEB = fabs(el.superCluster()->eta()) < 1.4442 ? 1 : 0;
  bool isEE = fabs(el.superCluster()->eta()) > 1.5660 ? 1 : 0;
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
    if(isEB) {
      if(res && full5x5_sigmaIetaIeta > 0.010557) res = false;
      if(res && fabs(dEtaIn) > 0.012442) res = false;
      if(res && fabs(dPhiIn) > 0.072624) res = false;
      if(res && HoE > 0.121476) res = false;
      if(res && ooEmooP > 0.221803) res = false;
      if(res && fabs(d0) > 0.022664) res = false;
      if(res && fabs(dz) > 0.173670) res = false;
      if(res && expectedMissingInnerHits >= 2 ) res = false;
      if(res && passConversionVeto == false ) res = false;
    }
    if(isEE) {
      if(res && full5x5_sigmaIetaIeta > 0.032602) res = false;
      if(res && fabs(dEtaIn) > 0.010654) res = false;
      if(res && fabs(dPhiIn) > 0.145129) res = false;
      if(res && HoE > 0.131862) res = false;
      if(res && ooEmooP > 0.142283) res = false;
      if(res && fabs(d0) > 0.097358) res = false;
      if(res && fabs(dz) > 0.198444) res = false;
      if(res && expectedMissingInnerHits >= 2 ) res = false;
      if(res && passConversionVeto == false ) res = false;
    }
  }
  if(res && LeptonRelIso((reco::Candidate*)&el,rho) > 0.15) res = false;
  return res;
}
//////////////////////////////////////////////////////////////////////////////////////////
float BoostedTTbarFlatTreeProducer::ElectronRelIso(const reco::Candidate *cand,float rho)
{
  pat::Electron el = *((pat::Electron*)cand); 
  float relIsoWithEA = 0;
  const int nEtaBins = 5;
  const float etaBinLimits[nEtaBins+1] = {0.0, 0.8, 1.3, 2.0, 2.2, 2.5};
  const float effectiveAreaValues[nEtaBins] = {0.1013, 0.0988, 0.0572, 0.0842, 0.1530};
  reco::GsfElectron::PflowIsolationVariables pfIso = el.pfIsolationVariables();
  float etaSC = el.superCluster()->eta();
  // Find eta bin first. If eta>2.5, the last eta bin is used.
  int etaBin = 0;
  while(etaBin < nEtaBins-1 && abs(etaSC) > etaBinLimits[etaBin+1]) ++etaBin;
  float area = effectiveAreaValues[etaBin];
  relIsoWithEA = (float)(pfIso.sumChargedHadronPt+max(float(0.0),pfIso.sumNeutralHadronEt+pfIso.sumPhotonEt-rho*area))/el.pt();
  return relIsoWithEA;
}
//////////////////////////////////////////////////////////////////////////////////////////
void BoostedTTbarFlatTreeProducer::analyze(edm::Event const& iEvent, edm::EventSetup const& iSetup) 
{
  initialize();

  edm::Handle<pat::JetCollection> jets;
  iEvent.getByLabel(srcJets_,jets);

  edm::Handle<pat::MuonCollection> muons;
  iEvent.getByLabel(srcMuons_,muons);

  edm::Handle<pat::ElectronCollection> electrons;
  iEvent.getByLabel(srcElectrons_,electrons);

  edm::Handle<pat::METCollection>  met;
  iEvent.getByLabel(srcMET_,met);

  edm::Handle<double> rho;
  iEvent.getByLabel(srcRho_,rho);

  edm::Handle<reco::VertexCollection> recVtxs;
  iEvent.getByLabel(srcVtx_,recVtxs);  

  //---------- mc -----------------------
  edm::Handle<std::vector<PileupSummaryInfo> > pupInfo;
  edm::Handle<GenParticleCollection> genParticles;
  
  if (!iEvent.isRealData()) { 
    iEvent.getByLabel(edm::InputTag(srcGenParticles_),genParticles);
    bool WPlusLep(false),WMinusLep(false);
    for(unsigned ip = 0; ip < genParticles->size(); ++ ip) {
      const GenParticle &p = (*genParticles)[ip];
      if (p.pdgId() == 24) {
        for(unsigned k = 0; k < p.numberOfDaughters(); k++) {
          int daughterID = p.daughter(k)->pdgId();
          if (daughterID == -11 || daughterID == -13 || daughterID == -15) {
            WPlusLep = true;
          }
        }
      }
      if (p.pdgId() == -24) {
        for(unsigned k = 0; k < p.numberOfDaughters(); k++) {
          int daughterID = p.daughter(k)->pdgId();
          if (daughterID == 11 || daughterID == 13 || daughterID == 15) {
            WMinusLep = true;
          }
        }
      }
    }// end of particle loop
    if (WPlusLep && WMinusLep)   decay_ = 0;
    if (WPlusLep && !WMinusLep)  decay_ = 1;
    if (!WPlusLep && WMinusLep)  decay_ = 1;
    if (!WPlusLep && !WMinusLep) decay_ = 2;
    
    iEvent.getByLabel(srcPU_,pupInfo);
    std::vector<PileupSummaryInfo>::const_iterator PUI;
    for(PUI = pupInfo->begin(); PUI != pupInfo->end(); ++PUI) {
      if (PUI->getBunchCrossing() == 0) {
        npu_ = PUI->getTrueNumInteractions();
      }
    }
    puHisto_->Fill(npu_);   
  }
  //-------------- Trigger Info -----------------------------------
  triggerPassHisto_->Fill("totalEvents",1);

  edm::Handle<edm::TriggerResults> triggerResults;
  iEvent.getByLabel(triggerResults_,triggerResults);  

  edm::Handle<pat::PackedTriggerPrescales> triggerPrescales;
  iEvent.getByLabel(triggerPrescales_,triggerPrescales); 

  const edm::TriggerNames &names = iEvent.triggerNames(*triggerResults);  
  for(unsigned int k=0;k<triggerNames_.size();k++) {
    bool bit(false);
    int pre(1);
    for(unsigned int itrig=0;itrig<triggerResults->size();itrig++) {
      string trigger_name = string(names.triggerName(itrig));
      //--- erase the last character, i.e. the version number----
      trigger_name.pop_back();
      if (trigger_name == triggerNames_[k]) {
        bit = triggerResults->accept(itrig); 
        pre = triggerPrescales->getPrescaleForIndex(itrig);
        if (bit) {
          triggerPassHisto_->Fill(triggerNames_[k].c_str(),1);
        } 
      }
    }
    triggerBit_->push_back(bit); 
    triggerPre_->push_back(pre);   
  }   
  vector<const reco::Candidate *> myLeptons;
  //----- at least one good vertex -----------
  bool cut_vtx = (recVtxs->size() > 0);
  if (cut_vtx) {
    //----- loop over leptons --------------------
    for (const pat::Muon &mu : *muons) {
      if (isGoodMuon(mu,(*recVtxs)[0],*rho)) myLeptons.push_back(&mu);
    }
    for (const pat::Electron &el : *electrons) {
      if (isGoodElectron(el,(*recVtxs)[0],*rho)) myLeptons.push_back(&el);
    }
    std::sort(myLeptons.begin(),myLeptons.end(),[](const reco::Candidate *a,const reco::Candidate *b){return a->pt() > b->pt();});
    nLeptons_ = (int)myLeptons.size();
    for(int ii = 0 ; ii < nLeptons_; ii++) {
      lPt_->push_back(myLeptons[ii]->pt());
      lEta_->push_back(myLeptons[ii]->eta());
      lPhi_->push_back(myLeptons[ii]->phi());
      lE_->push_back(myLeptons[ii]->energy());
      lId_->push_back(myLeptons[ii]->pdgId());
      lIso_->push_back(LeptonRelIso(myLeptons[ii],*rho));
    }
  }// if vtx
  //----- PF jets ------------------------------
  nJets_  = 0;
  nBJets_ = 0;
  ht_     = 0.0;
  vector<LorentzVector> vP4; 
  for(pat::JetCollection::const_iterator ijet =jets->begin();ijet != jets->end(); ++ijet) {   
    if (isGoodJet(*ijet)) {
      float btag= ijet->bDiscriminator(srcBtag_.c_str());
      bool isBtag = (btag >= btagMinThreshold_ && btag < btagMaxThreshold_);
      bool isLeptonMatched = false;
      float DRmax = 0.4;
      for(auto & lep: myLeptons) if( deltaR(lep->eta(),lep->phi(),ijet->eta(),ijet->phi()) < DRmax ) isLeptonMatched = true;
      if (!isLeptonMatched) {
        flavor_        ->push_back(ijet->partonFlavour());
        chf_           ->push_back(ijet->chargedHadronEnergyFraction());
        nhf_           ->push_back(ijet->neutralHadronEnergyFraction());
        phf_           ->push_back(ijet->photonEnergyFraction());
        elf_           ->push_back(ijet->electronEnergyFraction());
        muf_           ->push_back(ijet->muonEnergyFraction());
        pt_            ->push_back(ijet->pt());
        phi_           ->push_back(ijet->phi());
        eta_           ->push_back(ijet->eta());
        mass_          ->push_back(ijet->mass());
        massSoftDrop_  ->push_back(ijet->userFloat("ak8PFJetsCHSSoftDropMass"));
        energy_        ->push_back(ijet->energy());
        btag_          ->push_back(btag);
        isBtag_        ->push_back(isBtag);
        tau1_          ->push_back(ijet->userFloat("NjettinessAK8:tau1"));
        tau2_          ->push_back(ijet->userFloat("NjettinessAK8:tau2"));
        tau3_          ->push_back(ijet->userFloat("NjettinessAK8:tau3"));
        vP4.push_back(ijet->p4());
        ht_ += ijet->pt();
        nJets_++;
        if (isBtag) {
          nBJets_++;
        } 
        //---- subjets --------------------
        int nSub((ijet->subjets("SoftDrop")).size());
        int nBSub(0);
        if (nSub > 0) {
          btagSub0_->push_back((ijet->subjets("SoftDrop"))[0]->bDiscriminator(srcBtag_.c_str()));
          massSub0_->push_back((ijet->subjets("SoftDrop"))[0]->mass());
          if ((ijet->subjets("SoftDrop"))[0]->bDiscriminator(srcBtag_.c_str()) >= btagMinThreshold_) {
            nBSub++;
          }
          if (nSub > 1) {
            btagSub1_->push_back((ijet->subjets("SoftDrop"))[1]->bDiscriminator(srcBtag_.c_str()));
            massSub1_->push_back((ijet->subjets("SoftDrop"))[1]->mass());
            if ((ijet->subjets("SoftDrop"))[1]->bDiscriminator(srcBtag_.c_str()) >= btagMinThreshold_) {
              nBSub++;
            }
          } 
        }
        nSubJets_->push_back(nSub);
        nBSubJets_->push_back(nBSub);  
      }// if not matched with leptons
    }// if good jet
  }// jet loop       
  rho_    = *rho;
  met_    = (*met)[0].et();
  if ((*met)[0].sumEt() > 0) {
    metSig_ = (*met)[0].et()/(*met)[0].sumEt();
  }
  nVtx_   = recVtxs->size();
  run_    = iEvent.id().run();
  evt_    = iEvent.id().event();
  lumi_   = iEvent.id().luminosityBlock();

  cutFlowHisto_->Fill("All",1);
  if (nJets_ > 1) {
    cutFlowHisto_->Fill("nJets",1);
    if ((*pt_)[0] > ptMinLeading_) {
      cutFlowHisto_->Fill("ptMinLeading",1);
      dPhiJJ_ = fabs(deltaPhi(vP4[0].phi(),vP4[1].phi())); 
      dRJJ_   = deltaR(vP4[0],vP4[1]);
      mJJ_    = (vP4[0]+vP4[1]).mass();
      yJJ_    = (vP4[0]+vP4[1]).Rapidity();
      ptJJ_   = (vP4[0]+vP4[1]).pt();
      if (nBJets_ > -1) {
        cutFlowHisto_->Fill("nBJets",1); 
        if ((*massSoftDrop_)[0] > massMin_) {
          cutFlowHisto_->Fill("massSoftDrop0",1);
          if ((*nSubJets_)[0] > 1) {
            cutFlowHisto_->Fill("nSubJets0",1);
            mva_ = discr_->eval((*massSoftDrop_)[0],(*massSub0_)[0],(*massSub1_)[0],(*tau3_)[0]/(*tau2_)[0],(*tau3_)[0]/(*tau1_)[0]);
            outTree_->Fill();  
          }
        }
      }
    }
  }
}
//////////////////////////////////////////////////////////////////////////////////////////
void BoostedTTbarFlatTreeProducer::initialize()
{
  dPhiJJ_         = -1;
  dRJJ_           = -1;
  mJJ_            = -1;
  yJJ_            = -1;
  ptJJ_           = -1;
  run_            = -1;
  evt_            = -1;
  lumi_           = -1;
  nVtx_           = -1;
  nJets_          = -1;
  nLeptons_       = -1;
  nBJets_         = -1;
  rho_            = -1;
  met_            = -1;
  metSig_         = -1;
  ht_             = -1;
  mva_            = -999;
  flavor_         ->clear();
  nSubJets_       ->clear();
  nBSubJets_      ->clear();
  pt_             ->clear();
  eta_            ->clear();
  phi_            ->clear();
  mass_           ->clear();
  massSoftDrop_   ->clear();
  energy_         ->clear();
  chf_            ->clear();
  nhf_            ->clear();
  phf_            ->clear();
  elf_            ->clear();
  muf_            ->clear();
  tau1_           ->clear();
  tau2_           ->clear();
  tau3_           ->clear();
  btagSub0_       ->clear();
  btagSub1_       ->clear();
  massSub0_       ->clear();
  massSub1_       ->clear();
  btag_           ->clear();
  isBtag_         ->clear();
  triggerBit_     ->clear();
  triggerPre_     ->clear();
  lId_            ->clear();
  lIso_           ->clear();
  lPt_            ->clear();
  lEta_           ->clear();
  lPhi_           ->clear();
  lE_             ->clear();
  //----- MC -------
  decay_ = -1;
  npu_ = -1;
}
//////////////////////////////////////////////////////////////////////////////////////////
BoostedTTbarFlatTreeProducer::~BoostedTTbarFlatTreeProducer() 
{
}

DEFINE_FWK_MODULE(BoostedTTbarFlatTreeProducer);
















