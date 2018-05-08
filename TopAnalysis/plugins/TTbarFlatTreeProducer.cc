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

#include "UserCode/TopAnalysis/plugins/TTbarFlatTreeProducer.h"

using namespace std;
using namespace reco;

TTbarFlatTreeProducer::TTbarFlatTreeProducer(edm::ParameterSet const& cfg):
  jetsToken(consumes<pat::JetCollection>(cfg.getParameter<edm::InputTag>("jets"))),
  muonsToken(consumes<pat::MuonCollection>(cfg.getParameter<edm::InputTag>("muons"))),
  electronsToken(consumes<pat::ElectronCollection>(cfg.getParameter<edm::InputTag>("electrons"))),
  metToken(consumes<pat::METCollection>(cfg.getParameter<edm::InputTag>("met"))),
  qgtaggerToken(consumes<edm::ValueMap<float> >(cfg.getParameter<edm::InputTag>("qgtagger"))),
  rhoToken(consumes<double>(cfg.getParameter<edm::InputTag>("rho"))),
  recVtxsToken(consumes<reco::VertexCollection>(cfg.getParameter<edm::InputTag>("vertices"))),
  triggerResultsToken(consumes<edm::TriggerResults>(cfg.getParameter<edm::InputTag>("triggerResults"))),
  triggerPrescalesToken(consumes<pat::PackedTriggerPrescales>(cfg.getParameter<edm::InputTag>("triggerPrescales"))),
  pupInfoToken(consumes<edm::View<PileupSummaryInfo> >(edm::InputTag("slimmedAddPileupInfo"))),
  genEvtInfoToken(consumes<GenEventInfoProduct>(edm::InputTag("generator"))),
  genParticlesToken(consumes<edm::View<reco::GenParticle> >(edm::InputTag("prunedGenParticles"))),
  lheEvtInfoToken(consumes<LHEEventProduct>(edm::InputTag("externalLHEProducer"))),
  runInfoToken(consumes<LHERunInfoProduct>(edm::InputTag("externalLHEProducer"))),
  kinfit_(cfg.getParameter<std::string>("kinfit")),
  vchi2Token(consumes<edm::View<double> >(edm::InputTag(kinfit_,"Chi2"))),
  vprobToken(consumes<edm::View<double> >(edm::InputTag(kinfit_,"Prob"))),
  vstatusToken(consumes<edm::View<int> >(edm::InputTag(kinfit_,"Status"))),
  partonsBToken(consumes<edm::View<pat::Particle> >(edm::InputTag(kinfit_,"PartonsB"))),
  partonsBbarToken(consumes<edm::View<pat::Particle> >(edm::InputTag(kinfit_,"PartonsBBar"))), 
  partonsQToken(consumes<edm::View<pat::Particle> >(edm::InputTag(kinfit_,"PartonsLightQ"))),
  partonsQbarToken(consumes<edm::View<pat::Particle> >(edm::InputTag(kinfit_,"PartonsLightQBar"))), 
  partonsPToken(consumes<edm::View<pat::Particle> >(edm::InputTag(kinfit_,"PartonsLightP"))),
  partonsPbarToken(consumes<edm::View<pat::Particle> >(edm::InputTag(kinfit_,"PartonsLightPBar"))),  
  srcBtag_(cfg.getParameter<std::string>("btagger")),
  triggerNames_(cfg.getParameter<std::vector<std::string> >("triggerNames")),
  etaMax_(cfg.getParameter<double>("etaMax")),
  ptMin_(cfg.getParameter<double>("ptMin")),
  htMin_(cfg.getParameter<double>("htMin")), 
  probMin_(cfg.getParameter<double>("probMin")),
  btagMin_(cfg.getParameter<double>("btagMin")),
  nJetsMin_(cfg.getParameter<int>("nJetsMin")),
  nBJetsMin_(cfg.getParameter<int>("nBJetsMin")),
  isMC_(cfg.getUntrackedParameter<bool>("isMC",false)),
  saveWeights_(cfg.getUntrackedParameter<bool>("saveWeights",false)),
  debug_(cfg.getUntrackedParameter<bool>("debug",false)) 
{ 
}
//////////////////////////////////////////////////////////////////////////////////////////
void TTbarFlatTreeProducer::beginJob() 
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
  outTree_->Branch("pvRho"                ,&pvRho_             ,"pvRho_/F");
  outTree_->Branch("pvz"                  ,&pvz_               ,"pvz_/F");
  outTree_->Branch("pvchi2"               ,&pvchi2_            ,"pvchi2_/F");
  outTree_->Branch("pvndof"               ,&pvndof_            ,"pvndof_/F");
  outTree_->Branch("ht"                   ,&ht_                ,"ht_/F");
  outTree_->Branch("htBtag"               ,&htBtag_            ,"htBtag_/F");
  outTree_->Branch("met"                  ,&met_               ,"met_/F");
  outTree_->Branch("metSig"               ,&metSig_            ,"metSig_/F");
  outTree_->Branch("sphericity"           ,&sphericity_        ,"sphericity_/F");
  outTree_->Branch("aplanarity"           ,&aplanarity_        ,"aplanarity_/F");
  outTree_->Branch("foxWolfram"           ,&foxWolfram_        ,"foxWolfram_[4]/F");

  outTree_->Branch("qglAve"               ,&qglAve_            ,"qglAve_/F");
  outTree_->Branch("qglMin"               ,&qglMin_            ,"qglMin_/F");
  outTree_->Branch("qglMedian"            ,&qglMedian_         ,"qglMedian_/F");

  outTree_->Branch("idxQ"                 ,&idxQ_              ,"idxQ_/I");
  outTree_->Branch("idxQbar"              ,&idxQbar_           ,"idxQbar_/I");
  outTree_->Branch("idxB"                 ,&idxB_              ,"idxB_/I");
  outTree_->Branch("idxP"                 ,&idxP_              ,"idxP_/I");
  outTree_->Branch("idxPbar"              ,&idxPbar_           ,"idxPbar_/I");
  outTree_->Branch("idxBbar"              ,&idxBbar_           ,"idxBbar_/I");
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
  outTree_->Branch("mWReco"               ,&mWReco_            ,"mWReco_[2]/F");
  outTree_->Branch("mTopReco"             ,&mTopReco_          ,"mTopReco_[2]/F");
  outTree_->Branch("dRbbTopReco"          ,&dRbbTopReco_       ,"dRbbTopReco_/F");
  //------------------------------------------------------------------
  isBtag_         = new std::vector<bool>;
  flavor_         = new std::vector<int>;
  pt_             = new std::vector<float>;
  btag_           = new std::vector<float>;  
  qgl_            = new std::vector<float>;
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
  outTree_->Branch("jetQGL"               ,"vector<float>"     ,&qgl_);  
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
    outTree_->Branch("decay"                ,&decay_             ,"decay_/I");
    outTree_->Branch("npu"                  ,&npu_               ,"npu_/I");
    outTree_->Branch("genEvtWeight"         ,&genEvtWeight_      ,"genEvtWeight_/F");
    outTree_->Branch("lheOriginalXWGTUP"    ,&lheOriginalXWGTUP_ ,"lheOriginalXWGTUP_/F");
    if (saveWeights_) {
      scaleWeights_  = new std::vector<float>;
      pdfWeights_  = new std::vector<float>;
      outTree_->Branch("scaleWeights"         ,"vector<float>"     ,&scaleWeights_);
      outTree_->Branch("pdfWeights"           ,"vector<float>"     ,&pdfWeights_);
    } 
    outTree_->Branch("ptTopParton"          ,&ptTopParton_       ,"ptTopParton_[2]/F");
    outTree_->Branch("yTopParton"           ,&yTopParton_        ,"yTopParton_[2]/F");
    outTree_->Branch("mTTbarParton"         ,&mTTbarParton_      ,"mTTbarParton_/F");
    outTree_->Branch("yTTbarParton"         ,&yTTbarParton_      ,"yTTbarParton_/F");
    outTree_->Branch("ptTTbarParton"        ,&ptTTbarParton_     ,"ptTTbarParton_/F");
    partonId_       = new std::vector<int>;
    partonSt_       = new std::vector<int>;
    partonMatchIdx_ = new std::vector<int>;
    partonMatchDR_  = new std::vector<float>;
    partonPt_       = new std::vector<float>;
    partonEta_      = new std::vector<float>;
    partonPhi_      = new std::vector<float>;
    partonE_        = new std::vector<float>;
    outTree_->Branch("partonId"       ,"vector<int>"   ,&partonId_);
    outTree_->Branch("partonSt"       ,"vector<int>"   ,&partonSt_);
    outTree_->Branch("partonMatchIdx" ,"vector<int>"   ,&partonMatchIdx_);
    outTree_->Branch("partonMatchDR"  ,"vector<float>" ,&partonMatchDR_);
    outTree_->Branch("partonPt"       ,"vector<float>" ,&partonPt_);
    outTree_->Branch("partonEta"      ,"vector<float>" ,&partonEta_);
    outTree_->Branch("partonPhi"      ,"vector<float>" ,&partonPhi_);
    outTree_->Branch("partonE"        ,"vector<float>" ,&partonE_);
  }
  cout<<"Begin job finished"<<endl;
}
//////////////////////////////////////////////////////////////////////////////////////////
void TTbarFlatTreeProducer::endJob() 
{  
  delete isBtag_;
  delete flavor_;
  delete pt_;
  delete btag_;
  delete qgl_;
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
    delete partonSt_;
    delete partonId_;
    delete partonMatchIdx_;
    delete partonMatchDR_;
    delete partonPt_;
    delete partonEta_;
    delete partonPhi_;
    delete partonE_; 
  }
}
//////////////////////////////////////////////////////////////////////////////////////////
void TTbarFlatTreeProducer::beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup) 
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
void TTbarFlatTreeProducer::endRun(edm::Run const& iRun, edm::EventSetup const& iSetup) 
{
}
//////////////////////////////////////////////////////////////////////////////////////////
bool TTbarFlatTreeProducer::isGoodJet(const pat::Jet &jet)
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
bool TTbarFlatTreeProducer::isGoodMuon(const pat::Muon &mu,const reco::Vertex &vtx,float rho)
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
float TTbarFlatTreeProducer::MuonRelIso(const reco::Candidate *cand,float rho)
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
bool TTbarFlatTreeProducer::isGoodElectron(const pat::Electron &el,const reco::Vertex &vtx,float rho)
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
float TTbarFlatTreeProducer::ElectronRelIso(const reco::Candidate *cand,float rho)
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
void TTbarFlatTreeProducer::computeEventShapes(vector<const reco::Candidate *> myObj)
{
  float sumE(0.0),sumP2(0.0),sumPxx(0.0),sumPxy(0.0),sumPxz(0.0),sumPyy(0.0),sumPyz(0.0),sumPzz(0.0);
  vector<TLorentzVector> vP4;
  for(auto & obj: myObj) {
    vP4.push_back(TLorentzVector(obj->px(),obj->py(),obj->pz(),obj->energy())); 
    sumE   += obj->energy();
    sumP2  += obj->p()  * obj->p();
    sumPxx += obj->px() * obj->px();
    sumPxy += obj->px() * obj->py();
    sumPxz += obj->px() * obj->pz();
    sumPyy += obj->py() * obj->py();
    sumPyz += obj->py() * obj->pz();
    sumPzz += obj->pz() * obj->pz();
  } 
  if (sumP2 > 0) {
    //---- loop over jet pairs and compute Fox-Wolfram moments ----
    float sumPij[4] = {0.0,0.0,0.0,0.0};
    for(unsigned int k1=0;k1<vP4.size();k1++) {
      for(unsigned int k2=k1+1;k2<vP4.size();k2++) {
        float cosDPhi = cos(deltaPhi(vP4[k1].Phi(),vP4[k2].Phi()));
        float factor  = vP4[k1].P()*vP4[k2].P()/pow(sumE,2);
        for(int i=0;i<4;i++) {
          sumPij[i] += factor*ROOT::Math::legendre(i,cosDPhi);//legendre 0 = 1
        } 
      }
    }
    for(int i=0;i<4;i++) {
      foxWolfram_[i] = sumPij[i];
    }
    //---- compute sphericity -----------------------
    float Txx = sumPxx/sumP2;
    float Tyy = sumPyy/sumP2;
    float Tzz = sumPzz/sumP2;  
    float Txy = sumPxy/sumP2; 
    float Txz = sumPxz/sumP2; 
    float Tyz = sumPyz/sumP2;
    TMatrixDSym T(3);
    T(0,0) = Txx;
    T(0,1) = Txy;
    T(0,2) = Txz;
    T(1,0) = Txy;
    T(1,1) = Tyy;
    T(1,2) = Tyz;
    T(2,0) = Txz;
    T(2,1) = Tyz;
    T(2,2) = Tzz;
    TMatrixDSymEigen TEigen(T);
    TVectorD eigenValues(TEigen.GetEigenValues());
    sphericity_ = 1.5*(eigenValues(1)+eigenValues(2));
    aplanarity_ = 1.5*eigenValues(2); 
  }
}
//////////////////////////////////////////////////////////////////////////////////////////
int TTbarFlatTreeProducer::findMatch(LorentzVector p4,std::vector<TLorentzVector> vP4)
{
  float dRmin(1000.0);
  int match(-1);
  for(unsigned int k=0;k<vP4.size();k++) {
    float dR = deltaR(p4.Eta(),p4.Phi(),vP4[k].Eta(),vP4[k].Phi());
    if (dR < dRmin) {
      match = k;
      dRmin = dR; 
    } 
  }
  return match;
}
//////////////////////////////////////////////////////////////////////////////////////////
void TTbarFlatTreeProducer::analyze(edm::Event const& iEvent, edm::EventSetup const& iSetup) 
{
  initialize();

  iEvent.getByToken(jetsToken,jets);
  iEvent.getByToken(muonsToken,muons);
  iEvent.getByToken(electronsToken,electrons);
  iEvent.getByToken(metToken,met);
  iEvent.getByToken(qgtaggerToken,qgtagger);
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
    pvRho_  = (*recVtxs)[0].position().Rho();
    pvz_    = (*recVtxs)[0].z();
    pvndof_ = (*recVtxs)[0].ndof();
    pvchi2_ = (*recVtxs)[0].chi2();
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
  vector<const reco::Candidate *> myJets;
  nJets_  = 0;
  nBJets_ = 0;
  ht_     = 0.0;
  htBtag_ = 0.0;
    
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
        edm::RefToBase<pat::Jet> jetRef(edm::Ref<pat::JetCollection>(jets, ijet-jets->begin())); 
        qgl_           ->push_back((*qgtagger)[jetRef]);
        puMva_         ->push_back(ijet->userFloat("pileupJetId:fullDiscriminant"));
        isBtag_        ->push_back(isBtag);
        ht_ += ijet->pt();
        nJets_++;
        vP4.push_back(TLorentzVector(ijet->px(),ijet->py(),ijet->pz(),ijet->energy()));
        if (isBtag) {
          htBtag_ += ijet->pt();
          nBJets_++;
        }
        else {
          if ((*qgtagger)[jetRef] >= 0) {
            vqgl.push_back((*qgtagger)[jetRef]); 
          }
        } 
      }
    }
  }// jet loop
  //--- compute event-shape variables --------
  computeEventShapes(myJets); 
  //--- find the median of the qgl values
  if (vqgl.size() > 0) {
    qglMin_    = TMath::MinElement(vqgl.size(),&vqgl[0]);
    qglAve_    = TMath::Mean(vqgl.size(),&vqgl[0]);
    qglMedian_ = TMath::Median(vqgl.size(),&vqgl[0]);  
  }
          
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
    //---- find the jets that match the kinfit -----
    if (status_ > -1) {
      idxB_    = findMatch((*partonsB)[0].p4(),vP4);
      idxQ_    = findMatch((*partonsQ)[0].p4(),vP4);
      idxQbar_ = findMatch((*partonsQbar)[0].p4(),vP4);
      idxBbar_ = findMatch((*partonsBbar)[0].p4(),vP4);
      idxP_    = findMatch((*partonsP)[0].p4(),vP4);
      idxPbar_ = findMatch((*partonsPbar)[0].p4(),vP4);
      if ((idxB_+1)*(idxBbar_+1)*(idxQ_+1)*(idxQbar_+1)*(idxP_+1)*(idxPbar_+1) > 0) {
        dRbbTopReco_ = deltaR(vP4[idxB_].Eta(),vP4[idxB_].Phi(),vP4[idxBbar_].Eta(),vP4[idxBbar_].Phi());
        mTopReco_[0] = (vP4[idxQ_]+vP4[idxQbar_]+vP4[idxB_]).M();
        mTopReco_[1] = (vP4[idxP_]+vP4[idxPbar_]+vP4[idxBbar_]).M();
        mWReco_[0]   = (vP4[idxQ_]+vP4[idxQbar_]).M();
        mWReco_[1]   = (vP4[idxP_]+vP4[idxPbar_]).M(); 
      }              
    }
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
  }// if kinfit

  //---------- mc -----------------------
  if (!iEvent.isRealData()) { 
    iEvent.getByToken(genEvtInfoToken,genEvtInfo);
    iEvent.getByToken(lheEvtInfoToken,lheEvtInfo);
    iEvent.getByToken(genParticlesToken,genParticles);
    iEvent.getByToken(pupInfoToken,pupInfo);

    genEvtWeight_ = genEvtInfo->weight();
    lheOriginalXWGTUP_ = lheEvtInfo->originalXWGTUP();
  
    if (saveWeights_) {
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

    LorentzVector p4T(0,0,0,0),p4Tbar(0,0,0,0);
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
      if (fabs(p.pdgId()) == 6 && p.isLastCopy()) {
        if (p.pdgId() == 6) {
          p4T = p.p4();
        }
        if (p.pdgId() == -6) {
          p4Tbar = p.p4();
        }
        partonId_ ->push_back(p.pdgId());
        partonSt_ ->push_back(p.status()); 
        partonPt_ ->push_back(p.pt());
        partonEta_->push_back(p.eta());
        partonPhi_->push_back(p.phi());
        partonE_  ->push_back(p.energy());
        //----- match partons with jets ------------
        float dRmin(1000);  
        int imatch(0);
        for(int j=0;j<nJets_;j++) {
          float dR = deltaR(p.eta(),p.phi(),(*eta_)[j],(*phi_)[j]);
          if (dR < dRmin) {
            imatch = j;
            dRmin = dR;
          }
        }   
        partonMatchIdx_->push_back(imatch);
        partonMatchDR_->push_back(dRmin);
      }
    }// end of particle loop

    if (p4T.pt() > p4Tbar.pt()) {
      ptTopParton_[0] = p4T.pt();
      ptTopParton_[1] = p4Tbar.pt();
      yTopParton_[0]  = p4T.Rapidity();
      yTopParton_[1]  = p4Tbar.Rapidity();
    }
    else {
      ptTopParton_[1] = p4T.pt();
      ptTopParton_[0] = p4Tbar.pt();
      yTopParton_[1]  = p4T.Rapidity();
      yTopParton_[0]  = p4Tbar.Rapidity(); 
    } 
    mTTbarParton_   = (p4T+p4Tbar).mass();
    yTTbarParton_   = (p4T+p4Tbar).Rapidity();
    ptTTbarParton_  = (p4T+p4Tbar).pt();

    if (WPlusLep && WMinusLep)   decay_ = 2;
    if (WPlusLep && !WMinusLep)  decay_ = 1;
    if (!WPlusLep && WMinusLep)  decay_ = 1;
    if (!WPlusLep && !WMinusLep) decay_ = 0;
    
    edm::View<PileupSummaryInfo>::const_iterator PUI;
    for(PUI = pupInfo->begin(); PUI != pupInfo->end(); ++PUI) {
      if (PUI->getBunchCrossing() == 0) {
        npu_ = PUI->getTrueNumInteractions();
      }
    }
  }//--- end of MC -------
  //cout<<passTrigger<<" "<<nJets_<<" "<<nBJets_<<" "<<ht_<<" "<<status_<<" "<<prob_<<endl;
  cutFlowHisto_->Fill("All",1);
  if (passTrigger) {
    cutFlowHisto_->Fill("trigger",1);
    if (nJets_ >= nJetsMin_) {
      cutFlowHisto_->Fill("nJets",1);
      if (nBJets_ >= nBJetsMin_) {
        cutFlowHisto_->Fill("nBJets",1);
        if (ht_ > htMin_) {
          cutFlowHisto_->Fill("ht",1);
          if (status_ > -1) {
            cutFlowHisto_->Fill("kinFit",1);
            if (prob_ > probMin_) {
              cutFlowHisto_->Fill("probability",1);
              outTree_->Fill();
            }     
          }
        }
      }
    }  
  }
}
//////////////////////////////////////////////////////////////////////////////////////////
void TTbarFlatTreeProducer::initialize()
{
  qglAve_         = -1;
  qglMin_         = -1;
  qglMedian_      = -1;
  pvRho_          = -999;
  pvz_            = -999;
  pvndof_         = -999;
  pvchi2_         = -999;
  status_         = -999;
  prob_           = -1;
  chi2_           = -1;
  dRbbTop_        = -1;
  mW_[0]          = -1;
  mW_[1]          = -1;
  dRbbTopReco_    = -1;
  mWReco_[0]      = -1;
  mWReco_[1]      = -1;
  mTopReco_[0]    = -1;
  mTopReco_[1]    = -1;
  idxQ_           = -1;
  idxQbar_        = -1;
  idxB_           = -1;
  idxP_           = -1;
  idxPbar_        = -1;
  idxBbar_        = -1;
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
  htBtag_         = -1;
  sphericity_     = -1;
  aplanarity_     = -1;
  for(int i=0;i<4;i++) {
    foxWolfram_[i] = -999;
  }
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
  qgl_            ->clear();
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
    decay_ = -1;
    npu_ = -1;
    genEvtWeight_ = -999;
    lheOriginalXWGTUP_ = -999;
    if (saveWeights_) {
      scaleWeights_->clear();
      pdfWeights_->clear();
    }
    ptTopParton_[0] = -999;
    ptTopParton_[1] = -999;
    yTopParton_[0]  = -999;
    yTopParton_[1]  = -999;
    mTTbarParton_   = -999;
    yTTbarParton_   = -999;
    ptTTbarParton_  = -999;
    partonSt_->clear();
    partonId_->clear();
    partonMatchIdx_->clear();
    partonMatchDR_->clear();
    partonPt_->clear();
    partonEta_->clear();
    partonPhi_->clear();
    partonE_->clear(); 
  }
}
//////////////////////////////////////////////////////////////////////////////////////////
TTbarFlatTreeProducer::~TTbarFlatTreeProducer() 
{
}

DEFINE_FWK_MODULE(TTbarFlatTreeProducer);
















