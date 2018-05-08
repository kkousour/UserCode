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

#include "UserCode/TopAnalysis/plugins/TTVFlatTreeProducer.h"

using namespace std;
using namespace reco;

TTVFlatTreeProducer::TTVFlatTreeProducer(edm::ParameterSet const& cfg)
{
  jetsToken             = consumes<pat::JetCollection>(cfg.getParameter<edm::InputTag>("jets"));
  muonsToken            = consumes<pat::MuonCollection>(cfg.getParameter<edm::InputTag>("muons"));
  electronsToken        = consumes<pat::ElectronCollection>(cfg.getParameter<edm::InputTag>("electrons"));
  metToken              = consumes<pat::METCollection>(cfg.getParameter<edm::InputTag>("met"));
  rhoToken              = consumes<double>(cfg.getParameter<edm::InputTag>("rho"));
  recVtxsToken          = consumes<reco::VertexCollection>(cfg.getParameter<edm::InputTag>("vertices"));
  triggerResultsToken   = consumes<edm::TriggerResults>(cfg.getParameter<edm::InputTag>("triggerResults"));
  triggerPrescalesToken = consumes<pat::PackedTriggerPrescales>(cfg.getParameter<edm::InputTag>("triggerPrescales"));
  pupInfoToken          = consumes<edm::View<PileupSummaryInfo> >(edm::InputTag("slimmedAddPileupInfo"));
  genEvtInfoToken       = consumes<GenEventInfoProduct>(edm::InputTag("generator"));
  genParticlesToken     = consumes<edm::View<reco::GenParticle> >(edm::InputTag("prunedGenParticles"));
  lheEvtInfoToken       = consumes<LHEEventProduct>(edm::InputTag("externalLHEProducer"));
  runInfoToken          = consumes<LHERunInfoProduct>(edm::InputTag("externalLHEProducer"));
  kinfit_               = cfg.getParameter<std::string>("kinfit");
  vchi2Token            = consumes<edm::View<double> >(edm::InputTag(kinfit_,"Chi2"));
  vprobToken            = consumes<edm::View<double> >(edm::InputTag(kinfit_,"Prob"));
  vstatusToken          = consumes<edm::View<int> >(edm::InputTag(kinfit_,"Status"));
  partonsBToken         = consumes<edm::View<pat::Particle> >(edm::InputTag(kinfit_,"PartonsB"));
  partonsBbarToken      = consumes<edm::View<pat::Particle> >(edm::InputTag(kinfit_,"PartonsBBar"));
  partonsQToken         = consumes<edm::View<pat::Particle> >(edm::InputTag(kinfit_,"PartonsLightQ"));
  partonsQbarToken      = consumes<edm::View<pat::Particle> >(edm::InputTag(kinfit_,"PartonsLightQBar"));
  partonsPToken         = consumes<edm::View<pat::Particle> >(edm::InputTag(kinfit_,"PartonsLightP"));
  partonsPbarToken      = consumes<edm::View<pat::Particle> >(edm::InputTag(kinfit_,"PartonsLightPBar"));
  xmlFileTTW_           = cfg.getParameter<std::string>("xmlFileTTW");
  xmlFileTTZ_           = cfg.getParameter<std::string>("xmlFileTTZ");
  srcBtag_              = cfg.getParameter<std::string>("btagger");
  triggerNames_         = cfg.getParameter<std::vector<std::string> >("triggerNames");
  etaMax_               = cfg.getParameter<double>("etaMax");
  ptMin_                = cfg.getParameter<double>("ptMin");
  probMin_              = cfg.getParameter<double>("probMin");
  btagMin_              = cfg.getParameter<double>("btagMin");
  minMuPt_              = cfg.getParameter<double>("minMuPt");
  minElPt_              = cfg.getParameter<double>("minElPt");
  isMC_                 = cfg.getUntrackedParameter<bool>("isMC",false);
  saveWeights_          = cfg.getUntrackedParameter<bool>("saveWeights",false);
  debug_                = cfg.getUntrackedParameter<bool>("debug",false);
}
//////////////////////////////////////////////////////////////////////////////////////////
void TTVFlatTreeProducer::beginJob() 
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
  outTree_->Branch("status"               ,&status_            ,"status_/I");
  outTree_->Branch("prob"                 ,&prob_              ,"prob_/F");
  outTree_->Branch("chi2"                 ,&chi2_              ,"chi2_/F");
  outTree_->Branch("rho"                  ,&rho_               ,"rho_/F");
  outTree_->Branch("ht"                   ,&ht_                ,"ht_/F");
  outTree_->Branch("met"                  ,&met_               ,"met_/F");
  outTree_->Branch("metSig"               ,&metSig_            ,"metSig_/F");
  outTree_->Branch("mvaTTW"               ,&mvaTTW_            ,"mvaTTW_/F");
  outTree_->Branch("mvaTTZ"               ,&mvaTTZ_            ,"mvaTTZ_/F"); 
  outTree_->Branch("mW"                   ,&mW_                ,"mW_[2]/F");
  outTree_->Branch("mTop"                 ,&mTop_              ,"mTop_[2]/F");
  outTree_->Branch("ptTop"                ,&ptTop_             ,"ptTop_[2]/F");
  outTree_->Branch("yTop"                 ,&yTop_              ,"yTop_[2]/F");
  outTree_->Branch("etaTop"               ,&etaTop_            ,"etaTop_[2]/F");
  outTree_->Branch("phiTop"               ,&phiTop_            ,"phiTop_[2]/F");
  outTree_->Branch("mTTbar"               ,&mTTbar_            ,"mTTbar_/F");
  outTree_->Branch("yTTbar"               ,&yTTbar_            ,"yTTbar_/F");
  outTree_->Branch("ptTTbar"              ,&ptTTbar_           ,"ptTTbar_/F");
  outTree_->Branch("dRbbTop"              ,&dRbbTop_           ,"dRbbTop_/F");
  outTree_->Branch("dRlbMin"              ,&dRlbMin_           ,"dRlbMin_/F");
  outTree_->Branch("dRlbMax"              ,&dRlbMax_           ,"dRlbMax_/F");
  outTree_->Branch("dPhilbMin"            ,&dPhilbMin_         ,"dPhilbMin_/F");
  outTree_->Branch("dPhilbMax"            ,&dPhilbMax_         ,"dPhilbMax_/F");
  outTree_->Branch("dPhilTTbar"           ,&dPhilTTbar_        ,"dPhilbTTbar_/F");
  outTree_->Branch("llMass"               ,&llMass_            ,"llMass_/F");
  outTree_->Branch("llPt"                 ,&llPt_              ,"llPt_/F");
  outTree_->Branch("llRapidity"           ,&llRapidity_        ,"llRapidity_/F");
  outTree_->Branch("llPhi"                ,&llPhi_             ,"llPhi_/F");
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
  puMva_          = new std::vector<float>;
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
  outTree_->Branch("jetPuMva"             ,"vector<float>"     ,&puMva_);
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
    outTree_->Branch("lheOriginalXWGTUP"    ,&lheOriginalXWGTUP_ ,"lheOriginalXWGTUP_/F");
    if (saveWeights_) {
      scaleWeights_  = new std::vector<float>;
      pdfWeights_  = new std::vector<float>;
      outTree_->Branch("scaleWeights"         ,"vector<float>"     ,&scaleWeights_);
      outTree_->Branch("pdfWeights"           ,"vector<float>"     ,&pdfWeights_);
    } 
  }
  discrTTW_ = new TTVDiscriminatorMVA("UserCode/TopAnalysis/data/"+xmlFileTTW_,"selW");
  discrTTZ_  = new TTVDiscriminatorMVA("UserCode/TopAnalysis/data/"+xmlFileTTZ_,"selZ");
  cout<<"Begin job finished"<<endl;
}
//////////////////////////////////////////////////////////////////////////////////////////
void TTVFlatTreeProducer::endJob() 
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
  delete puMva_;
  delete discrTTW_;
  delete discrTTZ_;
  delete triggerBit_;
  delete triggerPre_;
  delete lId_;
  delete lIso_;
  delete lPt_;
  delete lEta_;
  delete lPhi_;
  delete lE_;
  if (isMC_) {
    if (saveWeights_) {
      delete scaleWeights_;
      delete pdfWeights_;
    } 
  }
}
//////////////////////////////////////////////////////////////////////////////////////////
void TTVFlatTreeProducer::beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup) 
{
  if (isMC_ && debug_) {
    iRun.getByToken(runInfoToken,runInfo);
    for(vector<LHERunInfoProduct::Header>::const_iterator it = runInfo->headers_begin();it != runInfo->headers_end(); it++) {
      cout<<it->tag()<<endl;
      vector<string> lines = it->lines();
      for(unsigned int iLine = 0; iLine < lines.size(); iLine++) {
        cout<< lines.at(iLine);
      }
    }
  }
}
//////////////////////////////////////////////////////////////////////////////////////////
void TTVFlatTreeProducer::endRun(edm::Run const& iRun, edm::EventSetup const& iSetup) 
{
}
//////////////////////////////////////////////////////////////////////////////////////////
bool TTVFlatTreeProducer::isGoodJet(const pat::Jet &jet)
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
bool TTVFlatTreeProducer::isGoodMuon(const pat::Muon &mu,const reco::Vertex &vtx,float rho)
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
float TTVFlatTreeProducer::MuonRelIso(const reco::Candidate *cand,float rho)
{
  pat::Muon mu = *((pat::Muon*)cand);
  reco::MuonPFIsolation pfIso = mu.pfIsolationR04();
  float relIso = (float)(pfIso.sumChargedHadronPt+max(0.0,pfIso.sumNeutralHadronEt+pfIso.sumPhotonEt-0.5*pfIso.sumPUPt))/mu.pt();
  return relIso;
}
//////////////////////////////////////////////////////////////////////////////////////////
bool TTVFlatTreeProducer::isGoodElectron(const pat::Electron &el,const reco::Vertex &vtx,float rho)
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
float TTVFlatTreeProducer::ElectronRelIso(const reco::Candidate *cand,float rho)
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
  relIsoWithEA = (float)(pfIso.sumChargedHadronPt+max(float(0.0),pfIso.sumNeutralHadronEt+pfIso.sumPhotonEt-rho*area))/el.pt();
  return relIsoWithEA;
}
//////////////////////////////////////////////////////////////////////////////////////////
void TTVFlatTreeProducer::analyze(edm::Event const& iEvent, edm::EventSetup const& iSetup) 
{
  initialize();

  iEvent.getByToken(jetsToken,jets);
  iEvent.getByToken(muonsToken,muons);
  iEvent.getByToken(electronsToken,electrons);
  iEvent.getByToken(metToken,met);
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
    if (nLeptons_ > 1) {
      TLorentzVector lP40(0,0,0,0),lP41(0,0,0,0);
      lP40.SetPtEtaPhiE((*lPt_)[0],(*lEta_)[0],(*lPhi_)[0],(*lE_)[0]);
      lP41.SetPtEtaPhiE((*lPt_)[1],(*lEta_)[1],(*lPhi_)[1],(*lE_)[1]);
      llMass_     = (lP40+lP41).M();
      llPt_       = (lP40+lP41).Pt();
      llPhi_      = (lP40+lP41).Phi();
      llRapidity_ = (lP40+lP41).Rapidity(); 
    }
  }// if vtx
  //----- PF jets ------------------------------
  vector<const reco::Candidate *> myJets;
  nJets_  = 0;
  nBJets_ = 0;
  ht_     = 0.0;
  
  vector<float> vqgl;
  vector<TLorentzVector> vP4;
 
  for(pat::JetCollection::const_iterator ijet =jets->begin();ijet != jets->end(); ++ijet) {  
    if (isGoodJet(*ijet)) {
      float btag= ijet->bDiscriminator(srcBtag_.c_str());
      bool isBtag = (btag >= btagMin_);
      bool isLeptonMatched = false;
      float DRmax = 0.4;
      for(auto & lep: myLeptons) if( deltaR(lep->eta(),lep->phi(),ijet->eta(),ijet->phi()) < DRmax ) isLeptonMatched = true;
      if (!isLeptonMatched) {
        myJets.push_back(&*ijet);
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
        puMva_         ->push_back(ijet->userFloat("pileupJetId:fullDiscriminant"));
        isBtag_        ->push_back(isBtag);
        ht_ += ijet->pt();
        nJets_++;
        vP4.push_back(TLorentzVector(ijet->px(),ijet->py(),ijet->pz(),ijet->energy()));
        if (isBtag) {
          nBJets_++;
        }
      }
    }
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

  if (kinfit_ != "") {
    iEvent.getByToken(vchi2Token,vchi2);
    iEvent.getByToken(vprobToken,vprob);
    iEvent.getByToken(vstatusToken,vstatus);
    iEvent.getByToken(partonsBToken,partonsB);
    iEvent.getByToken(partonsBbarToken,partonsBbar);
    iEvent.getByToken(partonsQToken,partonsQ);
    iEvent.getByToken(partonsQbarToken,partonsQbar);
    iEvent.getByToken(partonsPToken,partonsP);
    iEvent.getByToken(partonsPbarToken,partonsPbar); 

    //---- KinFit information -----------------------------
    status_   = (*vstatus)[0];  
    chi2_     = (*vchi2)[0];
    prob_     = (*vprob)[0];
    
    dRbbTop_     = deltaR((*partonsB)[0].p4(),(*partonsBbar)[0].p4());
    mW_[0]       = ((*partonsQ)[0].p4()+(*partonsQbar)[0].p4()).mass();
    mW_[1]       = ((*partonsP)[0].p4()+(*partonsPbar)[0].p4()).mass();
    mTop_[0]     = ((*partonsQ)[0].p4()+(*partonsQbar)[0].p4()+(*partonsB)[0].p4()).mass();
    mTop_[1]     = ((*partonsP)[0].p4()+(*partonsPbar)[0].p4()+(*partonsBbar)[0].p4()).mass();
    ptTop_[0]    = ((*partonsQ)[0].p4()+(*partonsQbar)[0].p4()+(*partonsB)[0].p4()).pt();
    ptTop_[1]    = ((*partonsP)[0].p4()+(*partonsPbar)[0].p4()+(*partonsBbar)[0].p4()).pt();
    yTop_[0]     = ((*partonsQ)[0].p4()+(*partonsQbar)[0].p4()+(*partonsB)[0].p4()).Rapidity();
    yTop_[1]     = ((*partonsP)[0].p4()+(*partonsPbar)[0].p4()+(*partonsBbar)[0].p4()).Rapidity();
    etaTop_[0]   = ((*partonsQ)[0].p4()+(*partonsQbar)[0].p4()+(*partonsB)[0].p4()).Eta();
    etaTop_[1]   = ((*partonsP)[0].p4()+(*partonsPbar)[0].p4()+(*partonsBbar)[0].p4()).Eta();
    phiTop_[0]   = ((*partonsQ)[0].p4()+(*partonsQbar)[0].p4()+(*partonsB)[0].p4()).Phi();
    phiTop_[1]   = ((*partonsP)[0].p4()+(*partonsPbar)[0].p4()+(*partonsBbar)[0].p4()).Phi();
    LorentzVector p4TTbar(0,0,0,0);
    p4TTbar      = (*partonsQ)[0].p4()+(*partonsQbar)[0].p4()+(*partonsB)[0].p4()+(*partonsP)[0].p4()+(*partonsPbar)[0].p4()+(*partonsBbar)[0].p4();
    mTTbar_      = p4TTbar.mass();
    yTTbar_      = p4TTbar.Rapidity();
    ptTTbar_     = p4TTbar.pt();

    if (nLeptons_ > 0) {
      float dRlB      = deltaR((*partonsB)[0].p4().Eta(),(*partonsB)[0].p4().Phi(),(*lEta_)[0],(*lPhi_)[0]);
      float dRlBbar   = deltaR((*partonsBbar)[0].p4().Eta(),(*partonsBbar)[0].p4().Phi(),(*lEta_)[0],(*lPhi_)[0]);
      float dPhilB    = fabs(deltaPhi((*partonsB)[0].p4().Phi(),(*lPhi_)[0]));
      float dPhilBbar = fabs(deltaPhi((*partonsBbar)[0].p4().Phi(),(*lPhi_)[0]));
      dRlbMin_        = TMath::Min(dRlB,dRlBbar); 
      dRlbMax_        = TMath::Max(dRlB,dRlBbar); 
      dPhilbMin_      = TMath::Min(dPhilB,dPhilBbar); 
      dPhilbMax_      = TMath::Max(dPhilB,dPhilBbar);
      dPhilTTbar_     = fabs(deltaPhi(p4TTbar.Phi(),(*lPhi_)[0]));
    }
  }// if kinfit

  //---------- mc -----------------------
  if (!iEvent.isRealData()) { 
    iEvent.getByToken(genEvtInfoToken,genEvtInfo);
    iEvent.getByToken(genParticlesToken,genParticles);
    iEvent.getByToken(pupInfoToken,pupInfo);

    genEvtWeight_ = genEvtInfo->weight();
  
    if (saveWeights_) {
      iEvent.getByToken(lheEvtInfoToken,lheEvtInfo);
      lheOriginalXWGTUP_ = lheEvtInfo->originalXWGTUP();
      for(unsigned i=0;i<lheEvtInfo->weights().size();i++) {
        string wtid(lheEvtInfo->weights()[i].id);
        float wgt(lheEvtInfo->weights()[i].wgt);
        if (wtid == "1002" || wtid == "2") scaleWeights_->push_back(wgt/lheOriginalXWGTUP_);
        if (wtid == "1003" || wtid == "3") scaleWeights_->push_back(wgt/lheOriginalXWGTUP_);
        if (wtid == "1004" || wtid == "4") scaleWeights_->push_back(wgt/lheOriginalXWGTUP_);
        if (wtid == "1005" || wtid == "5") scaleWeights_->push_back(wgt/lheOriginalXWGTUP_);
        if (wtid == "1007" || wtid == "7") scaleWeights_->push_back(wgt/lheOriginalXWGTUP_);
        if (wtid == "1009" || wtid == "9") scaleWeights_->push_back(wgt/lheOriginalXWGTUP_); 

        if ((stoi(wtid) > 2000 && stoi(wtid) <= 2102) || (stoi(wtid) > 10 && stoi(wtid) <= 110)) {
          pdfWeights_->push_back(wgt/lheOriginalXWGTUP_);
        }
      }  
    } 

    edm::View<PileupSummaryInfo>::const_iterator PUI;
    for(PUI = pupInfo->begin(); PUI != pupInfo->end(); ++PUI) {
      if (PUI->getBunchCrossing() == 0) {
        npu_ = PUI->getTrueNumInteractions();
      }
    }
  }//--- end of MC -------
  //cout<<passTrigger<<" "<<nJets_<<" "<<nBJets_<<" "<<ht_<<" "<<status_<<" "<<prob_<<endl;
  cutFlowHisto_->Fill("All",1);
  if (passTrigger || !passTrigger) {
    cutFlowHisto_->Fill("trigger",1);
    if (nLeptons_ >= 1) {
      cutFlowHisto_->Fill("nLeptons",1);
      if (nJets_ >= 6) {
        cutFlowHisto_->Fill("nJets",1);
        if (nBJets_ >= 1) {
          cutFlowHisto_->Fill("nBJets",1);
          if (status_ > -100) {
            cutFlowHisto_->Fill("kinFit",1);
            if (prob_ > probMin_) {
              cutFlowHisto_->Fill("probability",1);
              mvaTTW_ = discrTTW_->evalW(nJets_,nBJets_,(*lPt_)[0],(*lEta_)[0],met_,dRlbMin_,dRlbMax_,dRbbTop_,mTop_[0],chi2_,ht_); 
              if (nLeptons_ >= 2) {
                mvaTTZ_ = discrTTZ_->evalZ(nJets_,nBJets_,(*lPt_)[0],(*lPt_)[1],(*lEta_)[0],(*lEta_)[1],dRlbMin_,dRlbMax_,dRbbTop_,chi2_);
              }
              outTree_->Fill();
            }     
          }
        }
      }
    }  
  }
}
//////////////////////////////////////////////////////////////////////////////////////////
void TTVFlatTreeProducer::initialize()
{
  mvaTTW_         = -1;
  mvaTTZ_         = -1;
  status_         = -999;
  prob_           = -1;
  chi2_           = -1;
  dRbbTop_        = -1;
  dRlbMin_        = -1;
  dRlbMax_        = -1; 
  dPhilbMin_      = -1;
  dPhilbMax_      = -1;
  dPhilTTbar_     = -1;
  mW_[0]          = -1;
  mW_[1]          = -1;
  mTop_[0]        = -1;
  mTop_[1]        = -1;
  ptTop_[0]       = -1;
  ptTop_[1]       = -1;
  yTop_[0]        = -1;
  yTop_[1]        = -1;
  etaTop_[0]      = -1;
  etaTop_[1]      = -1;
  phiTop_[0]      = -1;
  phiTop_[1]      = -1;
  mTTbar_         = -1;
  yTTbar_         = -1;
  ptTTbar_        = -1;
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
  llMass_         = -999;
  llPt_           = -999;
  llPhi_          = -999;
  llRapidity_     = -999;
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
  puMva_          ->clear();
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
    lheOriginalXWGTUP_ = -999;
    if (saveWeights_) {
      scaleWeights_->clear();
      pdfWeights_->clear();
    } 
  }
}
//////////////////////////////////////////////////////////////////////////////////////////
TTVFlatTreeProducer::~TTVFlatTreeProducer() 
{
}

DEFINE_FWK_MODULE(TTVFlatTreeProducer);
















