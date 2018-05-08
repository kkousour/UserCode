#include <string>
#include <cmath>
#include <functional>
#include <vector>
#include <cassert>
#include "Math/SpecFuncMathMore.h"
#include "TMath.h"
#include "UserCode/TopAnalysis/plugins/VVFlatTreeProducer.h"

using namespace std;
using namespace reco;

VVFlatTreeProducer::VVFlatTreeProducer(edm::ParameterSet const& cfg)
{ 
  jetsToken             = consumes<pat::JetCollection>(cfg.getParameter<edm::InputTag>("jets"));
  muonsToken            = consumes<pat::MuonCollection>(cfg.getParameter<edm::InputTag>("muons"));
  electronsToken        = consumes<pat::ElectronCollection>(cfg.getParameter<edm::InputTag>("electrons"));
  metToken              = consumes<pat::METCollection>(cfg.getParameter<edm::InputTag>("met"));
  candsToken            = consumes<pat::PackedCandidateCollection>(cfg.getParameter<edm::InputTag>("candidates"));
  rhoToken              = consumes<double>(cfg.getParameter<edm::InputTag>("rho"));
  recVtxsToken          = consumes<reco::VertexCollection>(cfg.getParameter<edm::InputTag>("vertices"));
  triggerResultsToken   = consumes<edm::TriggerResults>(cfg.getParameter<edm::InputTag>("triggerResults"));
  triggerPrescalesToken = consumes<pat::PackedTriggerPrescales>(cfg.getParameter<edm::InputTag>("triggerPrescales"));
  pupInfoToken          = consumes<edm::View<PileupSummaryInfo> >(edm::InputTag("slimmedAddPileupInfo"));
  genEvtInfoToken       = consumes<GenEventInfoProduct>(edm::InputTag("generator"));
  srcBtag_              = cfg.getParameter<std::string>("btagger");
  triggerNames_         = cfg.getParameter<std::vector<std::string> >("triggerNames");
  etaMax_               = cfg.getParameter<double>("etaMax");
  ptMin_                = cfg.getParameter<double>("ptMin");
  btagMin_              = cfg.getParameter<double>("btagMin");
  minMuPt_              = cfg.getParameter<double>("minMuPt");
  minElPt_              = cfg.getParameter<double>("minElPt");
  isMC_                 = cfg.getUntrackedParameter<bool>("isMC",false);
}
//////////////////////////////////////////////////////////////////////////////////////////
void VVFlatTreeProducer::beginJob() 
{
  //--- book the trigger histograms ---------
  triggerNamesHisto_ = fs_->make<TH1F>("TriggerNames","TriggerNames",1,0,1);
  triggerNamesHisto_->SetCanExtend(TH1::kAllAxes);
  for(unsigned i=0;i<triggerNames_.size();i++) {
    triggerNamesHisto_->Fill(triggerNames_[i].c_str(),1);
  }
  triggerPassHisto_ = fs_->make<TH1F>("TriggerPass","TriggerPass",1,0,1);
  triggerPassHisto_->SetCanExtend(TH1::kAllAxes);

  cutFlowHisto_ = fs_->make<TH1F>("CutFlow","CutFlow",1,0,1);
  cutFlowHisto_->SetCanExtend(TH1::kAllAxes);
 
  //--- book the tree -----------------------
  outTree_ = fs_->make<TTree>("events","events");
  outTree_->Branch("runNo"                ,&run_               ,"run_/I");
  outTree_->Branch("evtNo"                ,&evt_               ,"evt_/I");
  outTree_->Branch("lumi"                 ,&lumi_              ,"lumi_/I");
  outTree_->Branch("nvtx"                 ,&nVtx_              ,"nVtx_/I");
  outTree_->Branch("nJets"                ,&nJets_             ,"nJets_/I");
  outTree_->Branch("nLeptons"             ,&nLeptons_          ,"nLeptons_/I");
  outTree_->Branch("nBJets"               ,&nBJets_            ,"nBJets_/I");
  outTree_->Branch("pvRho"                ,&pvRho_             ,"pvRho_/F");
  outTree_->Branch("pvz"                  ,&pvz_               ,"pvz_/F");
  outTree_->Branch("pvchi2"               ,&pvchi2_            ,"pvchi2_/F");
  outTree_->Branch("pvndof"               ,&pvndof_            ,"pvndof_/F");
  outTree_->Branch("rho"                  ,&rho_               ,"rho_/F");
  outTree_->Branch("ht"                   ,&ht_                ,"ht_/F");
  outTree_->Branch("met"                  ,&met_               ,"met_/F");
  outTree_->Branch("metSig"               ,&metSig_            ,"metSig_/F");
  outTree_->Branch("massLL"               ,&massLL_            ,"massLL_/F");
  outTree_->Branch("ptLL"                 ,&ptLL_              ,"ptLL_/F");
  outTree_->Branch("dPhiLL"               ,&dPhiLL_            ,"dPhiLL_/F");
  //------------------------------------------------------------------
  isBtag_         = new std::vector<bool>;
  flavor_         = new std::vector<int>;
  pt_             = new std::vector<float>;
  btag_           = new std::vector<float>;  
  eta_            = new std::vector<float>;
  phi_            = new std::vector<float>;
  mass_           = new std::vector<float>;
  energy_         = new std::vector<float>;
  chf_            = new std::vector<float>;
  nhf_            = new std::vector<float>;
  phf_            = new std::vector<float>;
  muf_            = new std::vector<float>;
  elf_            = new std::vector<float>;
  lId_            = new std::vector<int>;
  lPt_            = new std::vector<float>;
  lEta_           = new std::vector<float>;
  lPhi_           = new std::vector<float>;
  lE_             = new std::vector<float>;
  lIso_           = new std::vector<float>;
  outTree_->Branch("jetIsBtag"            ,"vector<bool>"      ,&isBtag_); 
  outTree_->Branch("jetFlavor"            ,"vector<int>"       ,&flavor_);
  outTree_->Branch("jetPt"                ,"vector<float>"     ,&pt_);
  outTree_->Branch("jetBtag"              ,"vector<float>"     ,&btag_);  
  outTree_->Branch("jetEta"               ,"vector<float>"     ,&eta_);
  outTree_->Branch("jetPhi"               ,"vector<float>"     ,&phi_);
  outTree_->Branch("jetMass"              ,"vector<float>"     ,&mass_);
  outTree_->Branch("jetEnergy"            ,"vector<float>"     ,&energy_);
  outTree_->Branch("jetChf"               ,"vector<float>"     ,&chf_);
  outTree_->Branch("jetNhf"               ,"vector<float>"     ,&nhf_);
  outTree_->Branch("jetPhf"               ,"vector<float>"     ,&phf_);
  outTree_->Branch("jetMuf"               ,"vector<float>"     ,&muf_);
  outTree_->Branch("jetElf"               ,"vector<float>"     ,&elf_);
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
  //------------------- MC ---------------------------------
  if (isMC_) {
    outTree_->Branch("npu"                  ,&npu_               ,"npu_/I");
    outTree_->Branch("genEvtWeight"         ,&genEvtWeight_      ,"genEvtWeight_/F");
  }
  cout<<"Begin job finished"<<endl;
}
//////////////////////////////////////////////////////////////////////////////////////////
void VVFlatTreeProducer::endJob() 
{  
  delete isBtag_;
  delete flavor_;
  delete pt_;
  delete btag_;
  delete eta_;
  delete phi_;
  delete mass_;
  delete energy_;
  delete chf_;
  delete nhf_;
  delete phf_;
  delete muf_;
  delete elf_;
  delete lId_;
  delete lIso_;
  delete lPt_;
  delete lEta_;
  delete lPhi_;
  delete lE_;
}
//////////////////////////////////////////////////////////////////////////////////////////
bool VVFlatTreeProducer::isGoodJet(const pat::Jet &jet)
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
float VVFlatTreeProducer::getPFMiniIsolation(edm::Handle<pat::PackedCandidateCollection> pfcands,const reco::Candidate *cand)
{
  if (cand->pt() < 5.0) return 99999;
  float deadcone_ch(0.0),deadcone_nh(0.0),deadcone_ph(0.0),deadcone_pu(0.0);
  
  if (cand->isElectron()) {
    if (fabs(cand->eta()) > 1.479) {
      deadcone_ch = 0.015;
      deadcone_nh = 0.0;
      deadcone_ph = 0.08;
      deadcone_pu = 0.015;
    }
  }
  if (cand->isMuon()) {
    deadcone_ch = 0.0001 ;
    deadcone_nh = 0.01;
    deadcone_ph = 0.01;
    deadcone_pu = 0.01;
  }

  float r_iso(0.2);

  if (cand->pt() > 50 && cand->pt() < 200) {
    r_iso = 10./cand->pt();
  }
  if (cand->pt() >= 200) {
    r_iso = 0.05;
  }

  float iso_ch(0.0),iso_nh(0.0),iso_ph(0.0),iso_pu(0.0);

  for(const pat::PackedCandidate &pfc : *pfcands) {
    if (abs(pfc.pdgId()) < 7) continue;
    float dr = reco::deltaR(pfc,*cand);
    if (dr > r_iso) continue;
    //--- NEUTRALS ----------
    if (pfc.charge() == 0) {
      if (pfc.pt() > 0.5) {
        //--- PHOTONS ------
        if (abs(pfc.pdgId()) == 22) {
          if (dr < deadcone_ph) continue;
          iso_ph += pfc.pt();
        }
        //--- HADRONS ------
        if (abs(pfc.pdgId()) == 130) {
          if (dr < deadcone_nh) continue;
          iso_nh += pfc.pt();
        }  
      } 
    }
    //--- CHARGED FROM PV -----
    else if (pfc.fromPV() > 1) {
      if (fabs(pfc.pdgId()) == 211) {
        if (dr < deadcone_ch) continue;
        iso_ch += pfc.pt();
      }
    }
    //--- CHARGED FROM PU -----
    else {
      if (pfc.pt() > 0.5) {
        if (dr < deadcone_pu) continue;
        iso_pu += pfc.pt();
      }
    }
  }
  float iso = iso_ch + std::max(0.0,iso_ph + iso_nh - 0.5*iso_pu);
  return iso;
}
//////////////////////////////////////////////////////////////////////////////////////////
bool VVFlatTreeProducer::isGoodMuon(const pat::Muon &mu,edm::Handle<pat::PackedCandidateCollection> pfcands)
{
  bool res = true; // by default is good, unless fails a cut bellow
  if(mu.pt() < minMuPt_) res = false;
  if(fabs(mu.eta()) > 2.4) res = false;
  if(!mu.isMediumMuon()) res = false;
  // --- isolation ---
  if(res && getPFMiniIsolation(pfcands,(reco::Candidate*)&mu)/mu.pt() > 0.1) res = false;
  return res;
}
//////////////////////////////////////////////////////////////////////////////////////////
bool VVFlatTreeProducer::isGoodElectron(const pat::Electron &el,const reco::Vertex &vtx,edm::Handle<pat::PackedCandidateCollection> pfcands)
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
    if(isEB) {// tight working point
      if(res && full5x5_sigmaIetaIeta > 0.0101) res = false;
      if(res && fabs(dEtaIn) > 0.00926) res = false;
      if(res && fabs(dPhiIn) > 0.0336) res = false;
      if(res && HoE > 0.0597) res = false;
      if(res && ooEmooP > 0.012) res = false;
      if(res && fabs(d0) > 0.0111) res = false;
      if(res && fabs(dz) > 0.0466) res = false;
      if(res && expectedMissingInnerHits >= 2 ) res = false;
      if(res && passConversionVeto == false ) res = false;
    }
    if(isEE) {// tight working point
      if(res && full5x5_sigmaIetaIeta > 0.0279) res = false;
      if(res && fabs(dEtaIn) > 0.00724) res = false;
      if(res && fabs(dPhiIn) > 0.0918) res = false;
      if(res && HoE > 0.0615) res = false;
      if(res && ooEmooP > 0.00999) res = false;
      if(res && fabs(d0) > 0.0351) res = false;
      if(res && fabs(dz) > 0.417) res = false;
      if(res && expectedMissingInnerHits > 1 ) res = false;
      if(res && passConversionVeto == false ) res = false;
    }
  }
  if(res && getPFMiniIsolation(pfcands,(reco::Candidate*)&el)/el.pt() > 0.1) res = false;
  return res;
}
//////////////////////////////////////////////////////////////////////////////////////////
void VVFlatTreeProducer::analyze(edm::Event const& iEvent, edm::EventSetup const& iSetup) 
{
  initialize();

  iEvent.getByToken(jetsToken,jets);
  iEvent.getByToken(muonsToken,muons);
  iEvent.getByToken(electronsToken,electrons);
  iEvent.getByToken(metToken,met);
  iEvent.getByToken(candsToken,cands);
  iEvent.getByToken(rhoToken,rho);
  iEvent.getByToken(recVtxsToken,recVtxs);  
  iEvent.getByToken(triggerResultsToken,triggerResults);  
  iEvent.getByToken(triggerPrescalesToken,triggerPrescales); 

  //-------------- Trigger Info -----------------------------------
  triggerPassHisto_->Fill("totalEvents",1);
  const edm::TriggerNames &names = iEvent.triggerNames(*triggerResults);  
  bool passTrigger(false);
  for(unsigned int k=0;k<triggerNames_.size();k++) {
    bool bit(false);
    int pre(1);
    for(unsigned int itrig=0;itrig<triggerResults->size();itrig++) {
      string trigger_name = string(names.triggerName(itrig));
      //--- erase the last character, i.e. the version number----
      
      std::size_t last_index = trigger_name.find_last_not_of("0123456789");
      //increment by +1 since the index we have is for the non-numeric character
      //and erase everything starting starting from that index
      trigger_name.erase(last_index+1, trigger_name.length()-1);
      
      if (trigger_name == triggerNames_[k]) {
        bit = triggerResults->accept(itrig); 
        pre = triggerPrescales->getPrescaleForIndex(itrig);
        if (bit) {
          triggerPassHisto_->Fill(triggerNames_[k].c_str(),1);
        } 
      }
    }
    //--- if at least one monitored trigger has fired passTrigger becomes true
    passTrigger += bit;
    triggerBit_->push_back(bit); 
    triggerPre_->push_back(pre);   
  }   
  vector<const reco::Candidate *> myLeptons;
  //----- at least one good vertex -----------
  bool cut_vtx = (recVtxs->size() > 0);
  if (cut_vtx) {
    pvRho_    = (*recVtxs)[0].position().Rho();
    pvz_    = (*recVtxs)[0].z();
    pvndof_ = (*recVtxs)[0].ndof();
    pvchi2_ = (*recVtxs)[0].chi2();
    //----- loop over leptons --------------------
    for (const pat::Muon &mu : *muons) {
      if (isGoodMuon(mu,cands)) myLeptons.push_back(&mu);
    }
    for (const pat::Electron &el : *electrons) {
      if (isGoodElectron(el,(*recVtxs)[0],cands)) myLeptons.push_back(&el);
    }
    std::sort(myLeptons.begin(),myLeptons.end(),[](const reco::Candidate *a,const reco::Candidate *b){return a->pt() > b->pt();});
    nLeptons_ = (int)myLeptons.size();
    for(int ii = 0 ; ii < nLeptons_; ii++) {
      lPt_->push_back(myLeptons[ii]->pt());
      lEta_->push_back(myLeptons[ii]->eta());
      lPhi_->push_back(myLeptons[ii]->phi());
      lE_->push_back(myLeptons[ii]->energy());
      lId_->push_back(myLeptons[ii]->pdgId());
      lIso_->push_back(getPFMiniIsolation(cands,myLeptons[ii])/myLeptons[ii]->pt());
    }
  }// if vtx
  //----- PF jets ------------------------------
  nJets_  = 0;
  nBJets_ = 0;
  ht_     = 0.0;
  for(pat::JetCollection::const_iterator ijet =jets->begin();ijet != jets->end(); ++ijet) {  
    if (isGoodJet(*ijet)) {
      float btag= ijet->bDiscriminator(srcBtag_.c_str());
      bool isBtag = (btag >= btagMin_);
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
        energy_        ->push_back(ijet->energy());
        btag_          ->push_back(btag);
        isBtag_        ->push_back(isBtag);
        ht_ += ijet->pt();
        nJets_++;
        if (isBtag) {
          nBJets_++;
        } 
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
  if (nLeptons_ > 1) {
    TLorentzVector lP40(0,0,0,0),lP41(0,0,0,0);
    lP40.SetPtEtaPhiE((*lPt_)[0],(*lEta_)[0],(*lPhi_)[0],(*lE_)[0]);
    lP41.SetPtEtaPhiE((*lPt_)[1],(*lEta_)[1],(*lPhi_)[1],(*lE_)[1]);
    massLL_ = (lP40+lP41).M();
    ptLL_   = (lP40+lP41).Pt();
    dPhiLL_ = fabs(deltaPhi((*lPhi_)[0],(*lPhi_)[1]));
  }

  //---------- mc -----------------------

  if (!iEvent.isRealData()) { 
    iEvent.getByToken(genEvtInfoToken,genEvtInfo);
    iEvent.getByToken(pupInfoToken,pupInfo);

    genEvtWeight_ = genEvtInfo->weight();
    edm::View<PileupSummaryInfo>::const_iterator PUI;
    for(PUI = pupInfo->begin(); PUI != pupInfo->end(); ++PUI) {
      if (PUI->getBunchCrossing() == 0) {
        npu_ = PUI->getTrueNumInteractions();
      }
    }
  }//--- end of MC -------

  cutFlowHisto_->Fill("All",1);
  if (passTrigger || !passTrigger) {
    cutFlowHisto_->Fill("trigger",1);
    if (nLeptons_ > 1) {
      cutFlowHisto_->Fill("nLeptons",1);
      outTree_->Fill();  
    }
  }
}
//////////////////////////////////////////////////////////////////////////////////////////
void VVFlatTreeProducer::initialize()
{
  dPhiLL_         = -1;
  massLL_         = -1;
  ptLL_           = -1;
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
  pvRho_          = -999;
  pvz_            = -999;
  pvndof_         = -999;
  pvchi2_         = -999;
  flavor_         ->clear();
  pt_             ->clear();
  eta_            ->clear();
  phi_            ->clear();
  mass_           ->clear();
  energy_         ->clear();
  chf_            ->clear();
  nhf_            ->clear();
  phf_            ->clear();
  elf_            ->clear();
  muf_            ->clear();
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
  if (isMC_) {
    npu_ = -1;
    genEvtWeight_ = -999;
  }
}
//////////////////////////////////////////////////////////////////////////////////////////
VVFlatTreeProducer::~VVFlatTreeProducer() 
{
}

DEFINE_FWK_MODULE(VVFlatTreeProducer);
















