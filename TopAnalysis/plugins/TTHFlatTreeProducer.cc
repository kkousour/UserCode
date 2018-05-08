#include <iostream>
#include <sstream>
#include <istream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cmath>
#include <functional>
#include <vector>
#include <cassert>
#include "Math/SpecFuncMathMore.h"
#include "Math/Vector3D.h"
#include "Math/Vector4D.h"
#include "Math/VectorUtil.h"
#include "TFile.h"
#include "TBranch.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TMath.h"
#include "TVector3.h"
#include "TVectorD.h"
#include "TLorentzVector.h"
#include "TMatrixDSym.h"
#include "TMatrixDSymEigen.h"

#include "UserCode/TopAnalysis/plugins/TTHFlatTreeProducer.h"

using namespace std;
using namespace reco;

TTHFlatTreeProducer::TTHFlatTreeProducer(edm::ParameterSet const& cfg) 
{
  jetsToken             = consumes<pat::JetCollection>(cfg.getParameter<edm::InputTag>("jets"));
  muonsToken            = consumes<pat::MuonCollection>(cfg.getParameter<edm::InputTag>("muons"));
  electronsToken        = consumes<pat::ElectronCollection>(cfg.getParameter<edm::InputTag>("electrons"));
  metToken              = consumes<pat::METCollection>(cfg.getParameter<edm::InputTag>("met"));
  qgtaggerToken         = consumes<edm::ValueMap<float> >(cfg.getParameter<edm::InputTag>("qgtagger"));
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
  srcBtag_              = cfg.getParameter<std::string>                        ("btagger");
  
  
  xmlFileQCD_CAT0_      = cfg.getParameter<std::string>                        ("xmlFileQCD_CAT0");
  xmlFileQCD_CAT1_      = cfg.getParameter<std::string>                        ("xmlFileQCD_CAT1");
  xmlFileTTbar_CAT0_	= cfg.getParameter<std::string>			       ("xmlFileTTbar_CAT0");
  xmlFileTTbar_CAT1_	= cfg.getParameter<std::string>			       ("xmlFileTTbar_CAT1");
  
  nJetsMin_             = cfg.getParameter<int>                                ("nJetsMin");
  nBJetsMin_            = cfg.getParameter<int>                                ("nBJetsMin");
  etaMax_               = cfg.getParameter<double>                             ("etaMax");
  ptMin_                = cfg.getParameter<double>                             ("ptMin");
  htMin_                = cfg.getParameter<double>                             ("htMin");
  minMuPt_              = cfg.getParameter<double>                             ("minMuPt");
  minElPt_              = cfg.getParameter<double>                             ("minElPt");
  btagMinThreshold_     = cfg.getParameter<double>                             ("btagMinThreshold");
  btagMaxThreshold_     = cfg.getParameter<double>                             ("btagMaxThreshold"); 
  triggerNames_         = cfg.getParameter<std::vector<std::string> >          ("triggerNames");
  isMC_                 = cfg.getUntrackedParameter<bool>                      ("isMC",false);
  saveWeights_          = cfg.getUntrackedParameter<bool>                      ("saveWeights",false);
}
//////////////////////////////////////////////////////////////////////////////////////////
void TTHFlatTreeProducer::beginJob() 
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
  outTree_->Branch("pvRho"                ,&pvRho_             ,"pvRho_/F");
  outTree_->Branch("pvz"                  ,&pvz_               ,"pvz_/F");
  outTree_->Branch("pvchi2"               ,&pvchi2_            ,"pvchi2_/F");
  outTree_->Branch("pvndof"               ,&pvndof_            ,"pvndof_/F");
  outTree_->Branch("nJets"                ,&nJets_             ,"nJets_/I");
  outTree_->Branch("nLeptons"             ,&nLeptons_          ,"nLeptons_/I");
  outTree_->Branch("nBJets"               ,&nBJets_            ,"nBJets_/I");
  outTree_->Branch("status"               ,&status_            ,"status_/I");
  outTree_->Branch("prob"                 ,&prob_              ,"prob_/F");
  outTree_->Branch("chi2"                 ,&chi2_              ,"chi2_/F");
  outTree_->Branch("mvaQCD"               ,&mvaQCD_            ,"mvaQCD_/F");
  outTree_->Branch("mvaTTbar"             ,&mvaTTbar_          ,"mvaTTbar_/F");
  outTree_->Branch("rho"                  ,&rho_               ,"rho_/F");
  outTree_->Branch("ht"                   ,&ht_                ,"ht_/F");
  outTree_->Branch("htBtag"               ,&htBtag_            ,"htBtag_/F");
  outTree_->Branch("met"                  ,&met_               ,"met_/F");
  outTree_->Branch("metPhi"               ,&metPhi_            ,"metPhi_/F");
  outTree_->Branch("metSig"               ,&metSig_            ,"metSig_/F");
  outTree_->Branch("sphericity"           ,&sphericity_        ,"sphericity_/F");
  outTree_->Branch("aplanarity"           ,&aplanarity_        ,"aplanarity_/F");
  outTree_->Branch("foxWolfram"           ,&foxWolfram_        ,"foxWolfram_[4]/F");
  outTree_->Branch("hcMoments"            ,&hcMoments_         ,"hcMoments_[5]/F");
  outTree_->Branch("centrality"           ,&centrality_        ,"centrality_/F");
  outTree_->Branch("cosThetaStar1"        ,&cosThetaStar1_     ,"cosThetaStar1_/F");
  outTree_->Branch("cosThetaStar2"        ,&cosThetaStar2_     ,"cosThetaStar2_/F");
  outTree_->Branch("EtStar1"              ,&EtStar1_           ,"EtStar1_/F");
  outTree_->Branch("EtStar2"              ,&EtStar2_           ,"EtStar2_/F");

  outTree_->Branch("mbbAve"               ,&mbbAve_            ,"mbbAve_/F");
  outTree_->Branch("mbbMin"               ,&mbbMin_            ,"mbbMin_/F");
  outTree_->Branch("dRbbAve"              ,&dRbbAve_           ,"dRbbAve_/F");
  outTree_->Branch("dRbbMin"              ,&dRbbMin_           ,"dRbbMin_/F");
  outTree_->Branch("btagAve"              ,&btagAve_           ,"btagAve_/F");
  outTree_->Branch("btagMax"              ,&btagMax_           ,"btagMax_/F");
  outTree_->Branch("btagMin"              ,&btagMin_           ,"btagMin_/F");
  outTree_->Branch("qglAve"               ,&qglAve_            ,"qglAve_/F");
  outTree_->Branch("qglMin"               ,&qglMin_            ,"qglMin_/F");
  outTree_->Branch("qglMedian"            ,&qglMedian_         ,"qglMedian_/F");

  outTree_->Branch("mW"                   ,&mW_                ,"mW_[2]/F");
  outTree_->Branch("mTop"                 ,&mTop_              ,"mTop_[2]/F");
  outTree_->Branch("ptTop"                ,&ptTop_             ,"ptTop_[2]/F");
  outTree_->Branch("yTop"                 ,&yTop_              ,"yTop_[2]/F");
  outTree_->Branch("mTTbar"               ,&mTTbar_            ,"mTTbar_/F");
  outTree_->Branch("yTTbar"               ,&yTTbar_            ,"yTTbar_/F");
  outTree_->Branch("ptTTbar"              ,&ptTTbar_           ,"ptTTbar_/F");
  outTree_->Branch("dRbbTop"              ,&dRbbTop_           ,"dRbbTop_/F");
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
  
  discrQCD_CAT0_   = new TTHDiscriminatorMVA("UserCode/TopAnalysis/data/"+xmlFileQCD_CAT0_, "CAT0",  "QCD");
  discrQCD_CAT1_   = new TTHDiscriminatorMVA("UserCode/TopAnalysis/data/"+xmlFileQCD_CAT1_, "CAT1",  "QCD");
  discrTTbar_CAT0_ = new TTHDiscriminatorMVA("UserCode/TopAnalysis/data/"+xmlFileTTbar_CAT0_,"CAT0", "TTbar");
  discrTTbar_CAT1_ = new TTHDiscriminatorMVA("UserCode/TopAnalysis/data/"+xmlFileTTbar_CAT1_,"CAT1", "TTbar");
  
  triggerBit_  = new std::vector<bool>;
  triggerPre_  = new std::vector<int>;
  outTree_->Branch("triggerBit"           ,"vector<bool>"      ,&triggerBit_);
  outTree_->Branch("triggerPre"           ,"vector<int>"       ,&triggerPre_);
  //------------------- MC ---------------------------------
  if (isMC_) {
    outTree_->Branch("decay"         ,&decay_        ,"decay_/I");
    outTree_->Branch("HToBB"         ,&HToBB_        ,"HToBB_/O");
    outTree_->Branch("npu"           ,&npu_          ,"npu_/I");
    outTree_->Branch("genEvtWeight"         ,&genEvtWeight_      ,"genEvtWeight_/F");
    outTree_->Branch("lheOriginalXWGTUP"    ,&lheOriginalXWGTUP_ ,"lheOriginalXWGTUP_/F");
    if (saveWeights_) {
      scaleWeights_  = new std::vector<float>;
      pdfWeights_  = new std::vector<float>;
      outTree_->Branch("scaleWeights"         ,"vector<float>"     ,&scaleWeights_);
      outTree_->Branch("pdfWeights"           ,"vector<float>"     ,&pdfWeights_);
    }
  }
  cout<<"Begin job finished"<<endl;
}
//////////////////////////////////////////////////////////////////////////////////////////
void TTHFlatTreeProducer::endJob() 
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
  delete discrQCD_CAT0_;  
  delete discrQCD_CAT1_;  
  delete discrTTbar_CAT0_;
  delete discrTTbar_CAT1_;
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
bool TTHFlatTreeProducer::isGoodJet(const pat::Jet &jet)
{
  bool res = true; // by default is good, unless fails a cut bellow
  float chf = jet.chargedHadronEnergyFraction();
  float nhf = jet.neutralHadronEnergyFraction();
  float phf = jet.photonEnergyFraction();
  float muf = jet.muonEnergyFraction();
  float elf = jet.electronEnergyFraction();
  int chm   = jet.chargedHadronMultiplicity();
  int npr   = jet.neutralMultiplicity()+jet.chargedMultiplicity();
  float eta = fabs(jet.eta());
  float pt  = jet.pt();
  bool idL = (npr>1 && phf<0.99 && nhf<0.99);
  //bool idM = (idL && ((eta<=2.4 && nhf<0.9 && phf<0.9 && elf<0.99 && muf<0.99 && chf>0 && chm>0) || eta>2.4));
  bool idT = (idL && ((eta<=2.4 && nhf<0.9 && phf<0.9 && elf<0.9 && muf<0.9 && chf>0 && chm>0) || eta>2.4));
  if (!idT) res = false;
  if (pt < ptMin_) res = false;
  if (eta > etaMax_) res = false;
  return res;
}
//////////////////////////////////////////////////////////////////////////////////////////
bool TTHFlatTreeProducer::isGoodMuon(const pat::Muon &mu,const reco::Vertex &vtx,float rho)
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
float TTHFlatTreeProducer::MuonRelIso(const reco::Candidate *cand,float rho)
{
  pat::Muon mu = *((pat::Muon*)cand);
  reco::MuonPFIsolation pfIso = mu.pfIsolationR04();
  float relIso = (float)(pfIso.sumChargedHadronPt+max(0.0,pfIso.sumNeutralHadronEt+pfIso.sumPhotonEt-0.5*pfIso.sumPUPt))/mu.pt();
  return relIso;
}
//////////////////////////////////////////////////////////////////////////////////////////
bool TTHFlatTreeProducer::isGoodElectron(const pat::Electron &el,const reco::Vertex &vtx,float rho)
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
float TTHFlatTreeProducer::ElectronRelIso(const reco::Candidate *cand,float rho)
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
void TTHFlatTreeProducer::cmVariables(const ROOT::Math::PtEtaPhiEVector &jet,const ROOT::Math::XYZVector &cmRef,float &cosThetaStar,float &etStar)
{
  ROOT::Math::PtEtaPhiEVector momJB = ROOT::Math::VectorUtil::boost(jet,cmRef);
  
  cosThetaStar = -999;
  etStar = -999;

  if (momJB.E() > 0.) {
    cosThetaStar = momJB.Pz()/momJB.E();
    etStar = jet.Et()*(1.-cosThetaStar*cosThetaStar);
  }
}
//////////////////////////////////////////////////////////////////////////////////////////
void TTHFlatTreeProducer::computeEventShapes(vector<const reco::Candidate *> myObj)
{
  float sumE(0.0),sumP2(0.0),sumPxx(0.0),sumPxy(0.0),sumPxz(0.0),sumPyy(0.0),sumPyz(0.0),sumPzz(0.0);
  vector<TLorentzVector> vP4;
  float sumPz = 0.;
  float sumEt = 0.;
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

    sumPz += obj->pz();
    sumEt += obj->pt();
  } 
  float sumSqr = sumE*sumE-sumPz*sumPz;
  if (sumSqr > 0.) {
    centrality_ = sumEt/sqrt(sumSqr);
  }
  if (sumE != 0 && myObj.size() > 2) {
    ROOT::Math::XYZVector cmRef;
    cmRef.SetZ(-sumPz/sumE);

    ROOT::Math::PtEtaPhiEVector theJet1(myObj.at(0)->pt(),myObj.at(0)->eta(),myObj.at(0)->phi(),myObj.at(0)->energy());
    ROOT::Math::PtEtaPhiEVector theJet2(myObj.at(1)->pt(),myObj.at(1)->eta(),myObj.at(1)->phi(),myObj.at(1)->energy());

    cmVariables(theJet1,cmRef,cosThetaStar1_,EtStar1_);
    cmVariables(theJet2,cmRef,cosThetaStar2_,EtStar2_);
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
    //---- loop over jet pairs and compute Fox-Wolfram moments ----
    float sumPTij[5] = {0.0,0.0,0.0,0.0,0.0};
    float zeros[5] = {2.4048,5.5201,8.6537,11.7915,14.9309};
    for(unsigned int k1=0;k1<vP4.size();k1++) {
      for(unsigned int k2=k1+1;k2<vP4.size();k2++) {
        float dR = deltaR(vP4[k1].Rapidity(),vP4[k1].Phi(),vP4[k2].Rapidity(),vP4[k2].Phi());
        float factor = vP4[k1].Pt()*vP4[k2].Pt()/pow(sumEt,2);
        for(int i=0;i<5;i++) {
          sumPTij[i] += factor*TMath::BesselJ0(zeros[i]*dR);
        } 
      }
    }
    for(int i=0;i<5;i++) {
      hcMoments_[i] = sumPTij[i];
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
void TTHFlatTreeProducer::analyze(edm::Event const& iEvent, edm::EventSetup const& iSetup) 
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
      if (p.pdgId() == 25) {
        for(unsigned k = 0; k < p.numberOfDaughters(); k++) {
          int daughterID = p.daughter(k)->pdgId();
          if (fabs(daughterID) == 5) {
            HToBB_ = true;
          }
        }
      }
    }// end of particle loop
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
  }
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
  vector<int> vBIdx; 
  vector<TLorentzVector> vBP4,vBP4noTop;
  bool antiBtagFlag(false); 
  for(pat::JetCollection::const_iterator ijet =jets->begin();ijet != jets->end(); ++ijet) {  
    if (isGoodJet(*ijet)) {
      float btag= ijet->bDiscriminator(srcBtag_.c_str());
      if (btag > btagMaxThreshold_) {
        antiBtagFlag = true;
        continue;
      }
      bool isBtag = (btag >= btagMinThreshold_ && btag < btagMaxThreshold_);
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
        if (isBtag) {
          vBP4.push_back(TLorentzVector(ijet->px(),ijet->py(),ijet->pz(),ijet->energy()));
          vBIdx.push_back(nJets_-1);
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
    
  //---- loop over btagged jets and compute variables ----  
  int nPairs(0);
  float sumMbb(0),sumDRbb(0),sumBtag(0);
  dRbbMin_ = 100;
  btagMax_ = -100;
  btagMin_ = 100; 
  for(unsigned int k1=0;k1<vBP4.size();k1++) {
    sumBtag += (*btag_)[vBIdx[k1]];
    if ((*btag_)[vBIdx[k1]] < btagMin_) {
      btagMin_ = (*btag_)[vBIdx[k1]];
    }
    if ((*btag_)[vBIdx[k1]] > btagMax_) {
      btagMax_ = (*btag_)[vBIdx[k1]];
    } 
    for(unsigned int k2=k1+1;k2<vBP4.size();k2++) {
      float m = (vBP4[k1]+vBP4[k2]).M();
      float dR = deltaR(vBP4[k1].Eta(),vBP4[k1].Phi(),vBP4[k2].Eta(),vBP4[k2].Phi());
      sumMbb  += m;
      sumDRbb += dR;
      if (dR < dRbbMin_) {
        dRbbMin_ = dR;
        mbbMin_  = m; 
      }
      nPairs++;
    }
  }
  if (vBP4.size() > 0) {
    btagAve_ = sumBtag/vBP4.size();
  }
  if (nPairs > 0) {
    mbbAve_  = sumMbb/nPairs;
    dRbbAve_ = sumDRbb/nPairs;
  }
        
  rho_    = *rho;
  met_    = (*met)[0].et();
  metPhi_ = (*met)[0].phi();
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
    dRbbTop_  = deltaR((*partonsB)[0].p4(),(*partonsBbar)[0].p4());
    mW_[0]    = ((*partonsQ)[0].p4()+(*partonsQbar)[0].p4()).mass();
    mW_[1]    = ((*partonsP)[0].p4()+(*partonsPbar)[0].p4()).mass();
    mTop_[0]  = ((*partonsQ)[0].p4()+(*partonsQbar)[0].p4()+(*partonsB)[0].p4()).mass();
    mTop_[1]  = ((*partonsP)[0].p4()+(*partonsPbar)[0].p4()+(*partonsBbar)[0].p4()).mass();
    ptTop_[0] = ((*partonsQ)[0].p4()+(*partonsQbar)[0].p4()+(*partonsB)[0].p4()).pt();
    ptTop_[1] = ((*partonsP)[0].p4()+(*partonsPbar)[0].p4()+(*partonsBbar)[0].p4()).pt();
    yTop_[0]  = ((*partonsQ)[0].p4()+(*partonsQbar)[0].p4()+(*partonsB)[0].p4()).Rapidity();
    yTop_[1]  = ((*partonsP)[0].p4()+(*partonsPbar)[0].p4()+(*partonsBbar)[0].p4()).Rapidity();
    LorentzVector p4TTbar(0,0,0,0);
    p4TTbar = (*partonsQ)[0].p4()+(*partonsQbar)[0].p4()+(*partonsB)[0].p4()+(*partonsP)[0].p4()+(*partonsPbar)[0].p4()+(*partonsBbar)[0].p4();
    mTTbar_     = p4TTbar.mass();
    yTTbar_     = p4TTbar.Rapidity();
    ptTTbar_    = p4TTbar.pt();
  }// if kinfit
  
  cutFlowHisto_->Fill("All",1);
  if (passTrigger || !passTrigger) {
    cutFlowHisto_->Fill("trigger",1);
    if (nJets_ >= nJetsMin_) {
      cutFlowHisto_->Fill("nJets",1);
      if (nBJets_ >= nBJetsMin_) {
        cutFlowHisto_->Fill("nBJets",1);
        if (ht_ > htMin_) {
          cutFlowHisto_->Fill("ht",1);
          if (nJets_ > 5 && nBJets_ > 1 && status_>-1) {
            
	   if (nBJets_==2){
	  
               mvaQCD_ = discrQCD_CAT0_ ->eval(nJets_,ht_,(*pt_)[5],mbbMin_,dRbbMin_,qglMedian_,cosThetaStar1_,cosThetaStar2_,sphericity_,aplanarity_,centrality_,foxWolfram_[0],foxWolfram_[1],foxWolfram_[2],foxWolfram_[3],mTop_[0],ptTTbar_,mTTbar_,dRbbTop_,chi2_);  
               mvaTTbar_ = discrTTbar_CAT0_->eval(nJets_,ht_,(*pt_)[5],mbbMin_,dRbbMin_,qglMedian_,cosThetaStar1_,cosThetaStar2_,sphericity_,aplanarity_,centrality_,foxWolfram_[0],foxWolfram_[1],foxWolfram_[2],foxWolfram_[3],mTop_[0],ptTTbar_,mTTbar_,dRbbTop_,chi2_);
          
	    }
	  
	     if (nBJets_>2){
	  
	      mvaQCD_ = discrQCD_CAT1_    ->eval(nJets_,ht_,(*pt_)[5],mbbMin_,dRbbMin_,qglMedian_,cosThetaStar1_,cosThetaStar2_,sphericity_,aplanarity_,centrality_,foxWolfram_[0],foxWolfram_[1],foxWolfram_[2],foxWolfram_[3],mTop_[0],ptTTbar_,mTTbar_,dRbbTop_,chi2_);
              mvaTTbar_ = discrTTbar_CAT1_->eval(nJets_,ht_,(*pt_)[5],mbbMin_,dRbbMin_,qglMedian_,cosThetaStar1_,cosThetaStar2_,sphericity_,aplanarity_,centrality_,foxWolfram_[0],foxWolfram_[1],foxWolfram_[2],foxWolfram_[3],mTop_[0],ptTTbar_,mTTbar_,dRbbTop_,chi2_);
	  
	     } 
            
          
	 
	 
	  }
          if (!antiBtagFlag) {
            cutFlowHisto_->Fill("antiBtag",1);
            outTree_->Fill();     
          }
        }
      }
    }  
  }
}
//////////////////////////////////////////////////////////////////////////////////////////
void TTHFlatTreeProducer::initialize()
{
  mbbAve_         = -999;
  mbbMin_         = -999;
  dRbbAve_        = -999;
  dRbbMin_        = -999;
  qglAve_         = -999;
  qglMin_         = -999;
  qglMedian_      = -999;
  btagAve_        = -999;
  btagMax_        = -999;
  btagMin_        = -999;
  status_         = -999;
  prob_           = -999;
  chi2_           = -999;
  mvaQCD_         = -999;
  mvaTTbar_       = -999;
  dRbbTop_        = -999;
  mW_[0]          = -999;
  mW_[1]          = -999;
  mTop_[0]        = -999;
  mTop_[1]        = -999;
  ptTop_[0]       = -999;
  ptTop_[1]       = -999;
  yTop_[0]        = -999;
  yTop_[1]        = -999;
  mTTbar_         = -999;
  yTTbar_         = -999;
  ptTTbar_        = -999;
  run_            = -999;
  evt_            = -999;
  lumi_           = -999;
  nVtx_           = -999;
  pvRho_          = -999;
  pvz_            = -999;
  pvndof_         = -999;
  pvchi2_         = -999;
  nJets_          = -999;
  nLeptons_       = -999;
  nBJets_         = -999;
  rho_            = -999;
  met_            = -999;
  metPhi_         = -999;
  metSig_         = -999;
  ht_             = -999;
  htBtag_         = -999;
  sphericity_     = -999;
  aplanarity_     = -999;
  centrality_     = -999;
  cosThetaStar1_  = -999;
  cosThetaStar2_  = -999;
  EtStar1_        = -999;
  EtStar2_        = -999;
  for(int i=0;i<4;i++) {
    foxWolfram_[i] = -999;
  }
  for(int i=0;i<5;i++) {
    hcMoments_[i] = -999;
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
    HToBB_ = false;
    decay_ = -1;
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
TTHFlatTreeProducer::~TTHFlatTreeProducer() 
{
}

DEFINE_FWK_MODULE(TTHFlatTreeProducer);
















