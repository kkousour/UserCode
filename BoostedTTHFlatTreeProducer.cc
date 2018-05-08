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
#include "fastjet/contrib/Njettiness.hh"
#include "fastjet/tools/MassDropTagger.hh"
#include "fastjet/contrib/SoftDrop.hh"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"

#include "UserCode/TopAnalysis/plugins/BoostedTTHFlatTreeProducer.h"

using namespace std;
using namespace reco;
using namespace fastjet;

BoostedTTHFlatTreeProducer::BoostedTTHFlatTreeProducer(edm::ParameterSet const& cfg)
{ 
  jetsToken             = consumes<pat::JetCollection>(cfg.getParameter<edm::InputTag>("jets"));
  genjetsToken          = consumes<GenJetCollection>(cfg.getUntrackedParameter<edm::InputTag>("genjets",edm::InputTag("")));
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
  genParticlesToken     = consumes<edm::View<reco::GenParticle> >(edm::InputTag("prunedGenParticles"));
  lheEvtInfoToken       = consumes<LHEEventProduct>(edm::InputTag("externalLHEProducer"));
  runInfoToken          = consumes<LHERunInfoProduct>(edm::InputTag("externalLHEProducer"));
  srcBtag_              = cfg.getParameter<std::string>("btagger");
  srcDoubleBtag_        = cfg.getParameter<std::string>("doubleBtagger");
  xmlFile_              = cfg.getParameter<std::string>("xmlFile");
  xmlFisherFile_        = cfg.getParameter<std::string>("xmlFisherFile");
  triggerNames_         = cfg.getParameter<std::vector<std::string> >("triggerNames");
  etaMax_               = cfg.getParameter<double>("etaMax");
  ptMin_                = cfg.getParameter<double>("ptMin");
  ptMinLeading_         = cfg.getParameter<double>("ptMinLeading");
  massMin_              = cfg.getParameter<double>("massMin");
  btagMin_              = cfg.getParameter<double>("btagMin");
  doubleBtagMin_        = cfg.getParameter<double>("doubleBtagMin");
  minMuPt_              = cfg.getParameter<double>("minMuPt");
  minElPt_              = cfg.getParameter<double>("minElPt");
  isMC_                 = cfg.getUntrackedParameter<bool>("isMC",false);
  isPrint_              = cfg.getUntrackedParameter<bool>("isPrint",false);
  saveWeights_          = cfg.getUntrackedParameter<bool>("saveWeights",false);
  debug_                = cfg.getUntrackedParameter<bool>("debug",false);
  GenptMin_             = cfg.getParameter<double>("GenptMin");
  GenetaMax_            = cfg.getParameter<double>("GenetaMax");

  jetFlavourInfosToken_ = consumes<reco::JetFlavourInfoMatchingCollection>( cfg.getParameter<edm::InputTag>("jetFlavourInfos"));
  
  //Gen Jet information
  fAKJetDef = new fastjet::JetDefinition(fastjet::antikt_algorithm, 0.8);
  int activeAreaRepeats = 1;
  double ghostArea      = 0.01;
  double ghostEtaMax    = 7.0;
  fActiveArea           = new fastjet::ActiveAreaSpec (ghostEtaMax,activeAreaRepeats,ghostArea);
  fAreaDefinition       = new fastjet::AreaDefinition (fastjet::active_area_explicit_ghosts, *fActiveArea );
  sd = new fastjet::contrib::SoftDrop(0.0,0.1,0.8);//beta_, zCut_, R0 );
}
//////////////////////////////////////////////////////////////////////////////////////////
void BoostedTTHFlatTreeProducer::beginJob() 
{
   printTriggerMenu_ = true;
   
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
  outTree_->Branch("nGenJets"             ,&nGenJets_          ,"nGenJets_/I");
  outTree_->Branch("nLeptons"             ,&nLeptons_          ,"nLeptons_/I");
  
  outTree_->Branch("nBJets"               ,&nBJets_            ,"nBJets_/I");
  outTree_->Branch("mvaValueMax"          ,&mvaValueMax_       ,"mvaValueMax/F");
  outTree_->Branch("highestMvaJet"        ,&highestMvaJet_     ,"highestMvaJet/I");
  outTree_->Branch("pvRho"                ,&pvRho_             ,"pvRho_/F");
  outTree_->Branch("pvz"                  ,&pvz_               ,"pvz_/F");
  outTree_->Branch("pvchi2"               ,&pvchi2_            ,"pvchi2_/F");
  outTree_->Branch("pvndof"               ,&pvndof_            ,"pvndof_/F");
  outTree_->Branch("rho"                  ,&rho_               ,"rho_/F");
  outTree_->Branch("ht"                   ,&ht_                ,"ht_/F");
  outTree_->Branch("mva"                  ,&mva_               ,"mva_/F");
  outTree_->Branch("met"                  ,&met_               ,"met_/F");
  outTree_->Branch("metGen"               ,&metGen_               ,"metGen_/F");
  outTree_->Branch("metSig"               ,&metSig_            ,"metSig_/F");
  outTree_->Branch("metGenSig"            ,&metGenSig_            ,"metGenSig_/F");
  outTree_->Branch("mJJ"                  ,&mJJ_               ,"mJJ_/F");
  outTree_->Branch("yJJ"                  ,&yJJ_               ,"yJJ_/F");
  outTree_->Branch("ptJJ"                 ,&ptJJ_              ,"ptJJ_/F");
  outTree_->Branch("dRJJ"                 ,&dRJJ_              ,"dRJJ_/F");
  outTree_->Branch("dPhiJJ"               ,&dPhiJJ_            ,"dPhiJJ_/F");
  outTree_->Branch("dPhiLJ"               ,&dPhiLJ_            ,"dPhiLJ_/F");
  
  //------------------------------------------------------------------
  
  
  isDoubleBtag_   = new std::vector<bool>;
  isBtag_         = new std::vector<bool>;
  flavor_         = new std::vector<int>;
  flavorHadron_   = new std::vector<int>;
  nSubJets_       = new std::vector<int>;
  nSubGenJets_    = new std::vector<int>;
  nBSubJets_      = new std::vector<int>;
  pt_             = new std::vector<float>;
  btag_           = new std::vector<float>;  
  
  doubleBtag_     = new std::vector<float>;
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
  ptSub0_         = new std::vector<float>;
  ptSub1_         = new std::vector<float>;
  jetMva_         = new std::vector<float>;
  etaSub0_        = new std::vector<float>;
  etaSub1_        = new std::vector<float>;
  phiSub0_        = new std::vector<float>;
  phiSub1_        = new std::vector<float>;
  flavorSub0_     = new std::vector<int>;
  flavorSub1_     = new std::vector<int>;
  flavorHadronSub0_     = new std::vector<int>;
  flavorHadronSub1_     = new std::vector<int>;
  lId_            = new std::vector<int>;
  lPt_            = new std::vector<float>;
  lEta_           = new std::vector<float>;
  lPhi_           = new std::vector<float>;
  lE_             = new std::vector<float>;
  lIso_           = new std::vector<float>;

  
  
  
  
  

  outTree_->Branch("jetIsBtag"            ,"vector<bool>"      ,&isBtag_); 
  outTree_->Branch("jetIsDoubleBtag"      ,"vector<bool>"      ,&isDoubleBtag_);
  outTree_->Branch("jetFlavor"            ,"vector<int>"       ,&flavor_);
  outTree_->Branch("jetFlavorHadron"      ,"vector<int>"       ,&flavorHadron_);
  outTree_->Branch("jetNSub"              ,"vector<int>"       ,&nSubJets_);
  outTree_->Branch("jetNSubGen"           ,"vector<int>"       ,&nSubGenJets_);
  outTree_->Branch("jetNBSub"             ,"vector<int>"       ,&nBSubJets_);
  outTree_->Branch("jetPt"                ,"vector<float>"     ,&pt_);
  outTree_->Branch("jetBtag"              ,"vector<float>"     ,&btag_);  
  outTree_->Branch("jetDoubleBtag"        ,"vector<float>"     ,&doubleBtag_);
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
  outTree_->Branch("jetPtSub0"            ,"vector<float>"     ,&ptSub0_);
  outTree_->Branch("jetPtSub1"            ,"vector<float>"     ,&ptSub1_);
  outTree_->Branch("jetEtaSub0"           ,"vector<float>"     ,&etaSub0_);
  outTree_->Branch("jetEtaSub1"           ,"vector<float>"     ,&etaSub1_);
  outTree_->Branch("jetPhiSub0"           ,"vector<float>"     ,&phiSub0_);
  outTree_->Branch("jetPhiSub1"           ,"vector<float>"     ,&phiSub1_);
  outTree_->Branch("jetFlavorSub0"        ,"vector<int>"       ,&flavorSub0_);
  outTree_->Branch("jetFlavorSub1"        ,"vector<int>"       ,&flavorSub1_);
  outTree_->Branch("jetFlavorHadronSub0"  ,"vector<int>"       ,&flavorHadronSub0_);
  outTree_->Branch("jetFlavorHadronSub1"  ,"vector<int>"       ,&flavorHadronSub1_);
  outTree_->Branch("jetMva"               ,"vector<float>"     ,&jetMva_);
  outTree_->Branch("lepId"                ,"vector<int>"       ,&lId_);
  outTree_->Branch("lepPt"                ,"vector<float>"     ,&lPt_);
  outTree_->Branch("lepEta"               ,"vector<float>"     ,&lEta_);
  outTree_->Branch("lepPhi"               ,"vector<float>"     ,&lPhi_);
  outTree_->Branch("lepE"                 ,"vector<float>"     ,&lE_);
  outTree_->Branch("lepIso"               ,"vector<float>"     ,&lIso_);

 
  //------------------------------------------------------------------
  triggerBit_ = new std::vector<bool>;
  triggerPre_ = new std::vector<int>;
  outTree_->Branch("triggerBit"           ,"vector<bool>"      ,&triggerBit_);
  outTree_->Branch("triggerPre"           ,"vector<int>"       ,&triggerPre_);
  //------------------------------------------------------------------
  discr_     = new BoostedDiscriminatorMVA("UserCode/TopAnalysis/data/"+xmlFile_);
  discrJets_ = new DiscriminatorFisher("UserCode/TopAnalysis/data/"+xmlFisherFile_);

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
    
    partonId_       = new std::vector<int>;
    partonSt_       = new std::vector<int>;
    partonMatchIdx_ = new std::vector<int>;
    partonMatchDR_  = new std::vector<float>;
    partonPt_       = new std::vector<float>;
    partonEta_      = new std::vector<float>;
    partonPhi_      = new std::vector<float>;
    partonE_        = new std::vector<float>;
    partonMass_     = new std::vector<float>;
    
    
    outTree_->Branch("partonId"       ,"vector<int>"   ,&partonId_);
    outTree_->Branch("partonSt"       ,"vector<int>"   ,&partonSt_);
    outTree_->Branch("partonMatchIdx" ,"vector<int>"   ,&partonMatchIdx_);
    outTree_->Branch("partonMatchDR"  ,"vector<float>" ,&partonMatchDR_);
    outTree_->Branch("partonPt"       ,"vector<float>" ,&partonPt_);
    outTree_->Branch("partonEta"      ,"vector<float>" ,&partonEta_);
    outTree_->Branch("partonPhi"      ,"vector<float>" ,&partonPhi_);
    outTree_->Branch("partonE"        ,"vector<float>" ,&partonE_);
    outTree_->Branch("partonMass"     ,"vector<float>" ,&partonMass_);
    
    //gen jets
    GenSoftDropMass_ = new std::vector<float>;
    GenSoftDropTau_  = new std::vector<float>;
    GenJetpt_   = new std::vector<float>;
    GenJetphi_  = new std::vector<float>;
    GenJeteta_  = new std::vector<float>;
    GenJetenergy_   = new std::vector<float>;
    GenJetmass_   = new std::vector<float>;
    GenJettau1_   = new std::vector<float>;
    GenJettau2_   = new std::vector<float>;
    GenJettau3_   = new std::vector<float>;
    GenSDSimmetry_   = new std::vector<float>;
    isBJetGen_   = new std::vector<bool>;

    outTree_->Branch("GenSoftDropMass"       ,"vector<float>"   ,&GenSoftDropMass_);
    outTree_->Branch("GenSoftDropTau"       ,"vector<float>"   ,&GenSoftDropTau_);
    outTree_->Branch("GenJetpt"       ,"vector<float>"   ,&GenJetpt_);
    outTree_->Branch("GenJeteta"       ,"vector<float>"   ,&GenJeteta_);
    outTree_->Branch("GenJettau1"       ,"vector<float>"   ,&GenJettau1_);
    outTree_->Branch("GenJettau2"       ,"vector<float>"   ,&GenJettau2_);
    outTree_->Branch("GenJettau3"       ,"vector<float>"   ,&GenJettau3_);
    outTree_->Branch("GenJetphi"       ,"vector<float>"   ,&GenJetphi_);
    outTree_->Branch("GenJetenergy"       ,"vector<float>"   ,&GenJetenergy_);
    outTree_->Branch("GenJetmass"       ,"vector<float>"   ,&GenJetmass_);   
    outTree_->Branch("GenSDSimmetry"       ,"vector<float>"   ,&GenSDSimmetry_);   
    outTree_->Branch("isBJetGen"       ,"vector<bool>"   ,&isBJetGen_);   

    GenSubJet1Pt_= new std::vector<float>;
    GenSubJet2Pt_= new std::vector<float>;
    GenSubJet1Eta_= new std::vector<float>;
    GenSubJet2Eta_= new std::vector<float>;
    GenSubJet1Phi_= new std::vector<float>;
    GenSubJet2Phi_= new std::vector<float>;
    GenSubJet1Mass_= new std::vector<float>;
    GenSubJet2Mass_= new std::vector<float>;
    GenSubJetsDeltaR_= new std::vector<float>;
    GenSubJetsMu_= new std::vector<float>;

    outTree_->Branch("GenSubJet1Pt"       ,"vector<float>"   ,&GenSubJet1Pt_);
    outTree_->Branch("GenSubJet2Pt"       ,"vector<float>"   ,&GenSubJet2Pt_);
    outTree_->Branch("GenSubJet1Eta"      ,"vector<float>"   ,&GenSubJet1Eta_);
    outTree_->Branch("GenSubJet2Eta"      ,"vector<float>"   ,&GenSubJet2Eta_);
    outTree_->Branch("GenSubJet1Phi"      ,"vector<float>"   ,&GenSubJet1Phi_);
    outTree_->Branch("GenSubJet2Phi"      ,"vector<float>"   ,&GenSubJet2Phi_);
    outTree_->Branch("GenSubJet1Mass"     ,"vector<float>"   ,&GenSubJet1Mass_);
    outTree_->Branch("GenSubJet2Mass"     ,"vector<float>"   ,&GenSubJet2Mass_);
    outTree_->Branch("GenSubJetsDeltaR"   ,"vector<float>"   ,&GenSubJetsDeltaR_);
    outTree_->Branch("GenSubJetsMu"       ,"vector<float>"   ,&GenSubJetsMu_);

  }
  cout<<"Begin job finished"<<endl;
}
//////////////////////////////////////////////////////////////////////////////////////////
void BoostedTTHFlatTreeProducer::endJob() 
{  
  delete isBtag_;
  delete isDoubleBtag_;
  delete flavor_;
  delete flavorHadron_;
  delete nSubJets_;
  delete nSubGenJets_;
  delete nBSubJets_;
  delete pt_;
  delete btag_;
  delete doubleBtag_;
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
  delete ptSub0_;
  delete ptSub1_;
  delete jetMva_;
  delete etaSub0_;
  delete etaSub1_;
  delete phiSub0_;
  delete phiSub1_;
  delete flavorSub0_;
  delete flavorSub1_;
  delete flavorHadronSub0_;
  delete flavorHadronSub1_;
  delete triggerBit_;
  delete triggerPre_;
  delete lId_;
  delete lIso_;
  delete lPt_;
  delete lEta_;
  delete lPhi_;
  delete lE_;
  
  
  
  delete discr_;
  delete discrJets_; 
  
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
    delete partonMass_;

   
    delete GenSoftDropMass_;
    delete GenSoftDropTau_;
    delete GenJetpt_;
    delete GenJetphi_;
    delete GenJeteta_;
    delete GenJetenergy_;
    delete GenJetmass_;
    delete GenJettau1_;
    delete GenJettau2_;
    delete GenJettau3_;
    delete GenSDSimmetry_;
    delete isBJetGen_;

    delete GenSubJet1Pt_;
    delete GenSubJet2Pt_;
    delete GenSubJet1Eta_;
    delete GenSubJet2Eta_;
    delete GenSubJet1Phi_;
    delete GenSubJet2Phi_;
    delete GenSubJet1Mass_;
    delete GenSubJet2Mass_;
    delete GenSubJetsDeltaR_;
    delete GenSubJetsMu_;


  }
}
//////////////////////////////////////////////////////////////////////////////////////////
void BoostedTTHFlatTreeProducer::beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup) 
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
void BoostedTTHFlatTreeProducer::endRun(edm::Run const& iRun, edm::EventSetup const& iSetup) 
{
}
//////////////////////////////////////////////////////////////////////////////////////////
bool BoostedTTHFlatTreeProducer::isGoodJet(const pat::Jet &jet)
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
  if (jet.userFloat("ak8PFJetsPuppiSoftDropMass") < massMin_) res = false;
  if ((jet.subjets("SoftDropPuppi")).size() < 2) res = false;
  return res;
}
//////////////////////////////////////////////////////////////////////////////////////////
float BoostedTTHFlatTreeProducer::getPFMiniIsolation(edm::Handle<pat::PackedCandidateCollection> pfcands,const reco::Candidate *cand)
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
bool BoostedTTHFlatTreeProducer::isGoodMuon(const pat::Muon &mu,edm::Handle<pat::PackedCandidateCollection> pfcands)
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
bool BoostedTTHFlatTreeProducer::isGoodElectron(const pat::Electron &el,const reco::Vertex &vtx,edm::Handle<pat::PackedCandidateCollection> pfcands)
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
void BoostedTTHFlatTreeProducer::analyze(edm::Event const& iEvent, edm::EventSetup const& iSetup) 
{
  initialize();

  if(isPrint_) cout<<"**** EVENT ****"<<endl;

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
  
  
  //-------------- Trigger Info -----------------------------------
  triggerPassHisto_->Fill("totalEvents",1);
  const edm::TriggerNames &names = iEvent.triggerNames(*triggerResults);  
  bool passTrigger(false);
  for(unsigned int k=0;k<triggerNames_.size();k++) {
    bool bit(false);
    int pre(1);
    if (k>0) printTriggerMenu_ = false;
    for(unsigned int itrig=0;itrig<triggerResults->size();itrig++) {
      string trigger_name = string(names.triggerName(itrig));
      if (printTriggerMenu_) cout<<trigger_name<<endl;
      //--- erase the last character, i.e. the version number----
      
      //starting from the end find first not numerical character
      std::size_t last_index = trigger_name.find_last_not_of("0123456789");
      //increment by +1 since the index we have is for the non-numeric character
      //and erase everything starting from that index
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
  nGenJets_  = 0;
  nBJets_ = 0;
  ht_     = 0.0;
  float mvaValue   = 0;
  mvaValueMax_= -8;
  highestMvaJet_= -1;
  vector<LorentzVector> vP4; 
  for(pat::JetCollection::const_iterator ijet =jets->begin();ijet != jets->end(); ++ijet) {  
    if (isGoodJet(*ijet)) {
      float btag= ijet->bDiscriminator(srcBtag_.c_str());
      bool isBtag = (btag >= btagMin_);
      float doubleBtag    = ijet->bDiscriminator(srcDoubleBtag_.c_str());
      bool  isDoubleBtag  = (doubleBtag>=doubleBtagMin_);
      
      bool isLeptonMatched = false;
      float DRmax = 0.4;
      if(isPrint_) {if (isBtag==true) cout<<"RECO "<<ijet->pt()<<" "<<ijet->eta()<<" "<<ijet->phi()<<endl;}
      for(auto & lep: myLeptons) if( deltaR(lep->eta(),lep->phi(),ijet->eta(),ijet->phi()) < DRmax ) isLeptonMatched = true;
      if (!isLeptonMatched) {
        flavor_        ->push_back(ijet->partonFlavour());
	flavorHadron_  ->push_back(ijet->hadronFlavour());
        chf_           ->push_back(ijet->chargedHadronEnergyFraction());
        nhf_           ->push_back(ijet->neutralHadronEnergyFraction());
        phf_           ->push_back(ijet->photonEnergyFraction());
        elf_           ->push_back(ijet->electronEnergyFraction());
        muf_           ->push_back(ijet->muonEnergyFraction());
        pt_            ->push_back(ijet->pt());
        phi_           ->push_back(ijet->phi());
        eta_           ->push_back(ijet->eta());
        mass_          ->push_back(ijet->mass());
        massSoftDrop_  ->push_back(ijet->userFloat("ak8PFJetsPuppiSoftDropMass"));
        energy_        ->push_back(ijet->energy());
        btag_          ->push_back(btag);
        isBtag_        ->push_back(isBtag);
        doubleBtag_    ->push_back(doubleBtag);
        isDoubleBtag_  ->push_back(isDoubleBtag);
	tau1_          ->push_back(ijet->userFloat("NjettinessAK8Puppi:tau1"));
        tau2_          ->push_back(ijet->userFloat("NjettinessAK8Puppi:tau2"));
        tau3_          ->push_back(ijet->userFloat("NjettinessAK8Puppi:tau3"));
	
	
        vP4.push_back(ijet->p4());
        ht_ += ijet->pt();
        nJets_++;
        if (isBtag) {
          nBJets_++;
        } 
        //---- subjets --------------------
        int nSub((ijet->subjets("SoftDropPuppi")).size());
        int nBSub(0);
        if (nSub > 0) {
          btagSub0_->push_back((ijet->subjets("SoftDropPuppi"))[0]->bDiscriminator(srcBtag_.c_str()));
          massSub0_->push_back((ijet->subjets("SoftDropPuppi"))[0]->mass());
          ptSub0_->push_back((ijet->subjets("SoftDropPuppi"))[0]->pt());
          etaSub0_->push_back((ijet->subjets("SoftDropPuppi"))[0]->eta());
	  phiSub0_->push_back((ijet->subjets("SoftDropPuppi"))[0]->phi());
          flavorSub0_->push_back((ijet->subjets("SoftDropPuppi"))[0]->partonFlavour());
	  flavorHadronSub0_->push_back((ijet->subjets("SoftDropPuppi"))[0]->hadronFlavour());
          if ((ijet->subjets("SoftDropPuppi"))[0]->bDiscriminator(srcBtag_.c_str()) >= btagMin_) {
            nBSub++;
          }
          if (nSub > 1) {
            btagSub1_->push_back((ijet->subjets("SoftDropPuppi"))[1]->bDiscriminator(srcBtag_.c_str()));
            massSub1_->push_back((ijet->subjets("SoftDropPuppi"))[1]->mass());
            ptSub1_->push_back((ijet->subjets("SoftDropPuppi"))[1]->pt());
            etaSub1_->push_back((ijet->subjets("SoftDropPuppi"))[1]->eta());
	    phiSub1_->push_back((ijet->subjets("SoftDropPuppi"))[1]->phi());
            flavorSub1_->push_back((ijet->subjets("SoftDropPuppi"))[1]->partonFlavour());
	    flavorHadronSub1_->push_back((ijet->subjets("SoftDropPuppi"))[1]->hadronFlavour());
            if ((ijet->subjets("SoftDropPuppi"))[1]->bDiscriminator(srcBtag_.c_str()) >= btagMin_) {
              nBSub++;
            
	    
	    
	    }
          } 
        }
        nSubJets_->push_back(nSub);
        nBSubJets_->push_back(nBSub);
	
	// eval mva for each jet to get mva score
  
        jetMva_->push_back(discrJets_->eval(ijet->userFloat("NjettinessAK8Puppi:tau1"),ijet->userFloat("NjettinessAK8Puppi:tau2"),ijet->userFloat("NjettinessAK8Puppi:tau3"),nBSub,nJets_-1,ijet->bDiscriminator(srcDoubleBtag_.c_str())));
	  
      }// if not matched with leptons
    }// if good jet
  
  
  
 }// jet loop  
 
 
 //after having the mvaScore for all jet ->Find the jet with the hightest mvaScore
 
 
  for(int ijet=0; ijet<nJets_; ++ijet) {
 //for(pat::JetCollection::unsigned int ijet =jets->begin();ijet != jets->end(); ++ijet){
  if ((*pt_)[ijet] >300){
  mvaValue = (*jetMva_)[ijet];
 
   if(mvaValue > mvaValueMax_){
    
    mvaValueMax_   = mvaValue;
    highestMvaJet_ = (int) ijet;
    
 
   }
  } 
 } 
  ///////////////////    
 
  rho_    = *rho;
  met_    = (*met)[0].et();
  if ((*met)[0].sumEt() > 0) {
    metSig_ = (*met)[0].et()/(*met)[0].sumEt();
  }

  if(isMC_){
    metGen_ = (*met)[0].genMET()->et();
    if ((*met)[0].genMET()->sumEt() > 0) {
      metGenSig_ = (*met)[0].genMET()->et()/(*met)[0].genMET()->sumEt();
    }
  }

  if(isPrint_) cout<<"MET "<<metGen_<<" "<<met_<<endl;

  nVtx_   = recVtxs->size();
  run_    = iEvent.id().run();
  evt_    = iEvent.id().event();
  lumi_   = iEvent.id().luminosityBlock();
  if (nLeptons_ > 0 && nJets_ > 0) {
    dPhiLJ_ = fabs(deltaPhi((*lPhi_)[0],(*phi_)[0])); 
  }

  if (nJets_ > 1) {
    dPhiJJ_ = fabs(deltaPhi(vP4[0].phi(),vP4[1].phi())); 
    dRJJ_   = deltaR(vP4[0],vP4[1]);
    mJJ_    = (vP4[0]+vP4[1]).mass();
    yJJ_    = (vP4[0]+vP4[1]).Rapidity();
    ptJJ_   = (vP4[0]+vP4[1]).pt();
    mva_ = discr_->eval((*tau3_)[0]/(*tau2_)[0],(*tau3_)[0]/(*tau1_)[0],(*tau3_)[1]/(*tau2_)[1],(*tau3_)[1]/(*tau1_)[1]);
  }

  //---------- mc -----------------------

  if (!iEvent.isRealData()) { 
    iEvent.getByToken(genEvtInfoToken,genEvtInfo);
    iEvent.getByToken(lheEvtInfoToken,lheEvtInfo);
    iEvent.getByToken(genParticlesToken,genParticles);
    iEvent.getByToken(pupInfoToken,pupInfo);
    iEvent.getByToken(genjetsToken,genjets);

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
      
  // loop over gen particles and find pdg or t, tbar , higgs
  
      if ((fabs(p.pdgId()) == 6 && p.isLastCopy()) || (fabs(p.pdgId())==25 && p.isLastCopy())) {
        
        partonId_ ->push_back(p.pdgId());
        partonSt_ ->push_back(p.status()); 
        partonPt_ ->push_back(p.pt());
        partonEta_->push_back(p.eta());
        partonPhi_->push_back(p.phi());
        partonE_   ->push_back(p.energy());
        partonMass_->push_back(p.mass());
	//----- match partons with jets ------------
        float dRmin(1000);  
        int imatch(-1);
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

    vector<LorentzVector> vP4Gen;

    for(GenJetCollection::const_iterator i_gen = genjets->begin(); i_gen != genjets->end(); i_gen++) {
      if (i_gen->pt() > GenptMin_ && fabs(i_gen->y()) < GenetaMax_ ) {

	nGenJets_++;

	GenJetpt_->push_back(i_gen->pt());
	GenJetphi_->push_back(i_gen->phi());
	GenJeteta_->push_back(i_gen->y());
	GenJetenergy_->push_back(i_gen->energy());
	GenJetmass_->push_back(i_gen->mass());

	vP4Gen.push_back(i_gen->p4());
        
        std::vector<fastjet::PseudoJet>  lClusterParticles;
        for(unsigned int ic=0; ic<i_gen->numberOfDaughters(); ic++) {
          const reco::Candidate* gencand = i_gen->daughter(ic);
          fastjet::PseudoJet   pPart(gencand->px(),gencand->py(),gencand->pz(),gencand->energy());
          lClusterParticles.push_back(pPart);
        }
	
        fClustering = new fastjet::ClusterSequenceArea(lClusterParticles, *fAKJetDef, *fAreaDefinition);
        std::vector<fastjet::PseudoJet>  lOutJets = fClustering->inclusive_jets(20.0);
	
        if(lOutJets.size() == 0) {
          delete fClustering;
          return;
        }
	
	fastjet::PseudoJet pT1JetSD = (*sd)( lOutJets[0]);
        float iMSoftDrop = pT1JetSD.m();
	float iSimmetry = pT1JetSD.structure_of<contrib::SoftDrop>().symmetry();
	
        fastjet::contrib::NormalizedMeasure          normalizedMeasure        (1.0,0.8);
        fastjet::contrib::Njettiness routine(fastjet::contrib::Njettiness::onepass_kt_axes,normalizedMeasure);
        float iTau1 = routine.getTau(1.,lClusterParticles);                                                                                                                   
        float iTau2 = routine.getTau(2.,lClusterParticles);
        float iTau3 = routine.getTau(3.,lClusterParticles);

	GenJettau1_->push_back(iTau1);
	GenJettau2_->push_back(iTau2);
	GenJettau3_->push_back(iTau3);

	GenSDSimmetry_->push_back(iSimmetry);
	
	float Tau32=iTau3/iTau2;
	GenSoftDropTau_->push_back(Tau32);
        GenSoftDropMass_->push_back(iMSoftDrop);

	//subjets for Gen jets
	vector<PseudoJet> pieces = pT1JetSD.pieces();

	float deltaRgensubjets=pT1JetSD.structure_of<contrib::SoftDrop>().delta_R();
	float mugensubjets=pT1JetSD.structure_of<contrib::SoftDrop>().mu();

	if(pieces.size()>1){

	  float ptfirstconst = pieces[0].perp();
	  float ptsecondconst = pieces[1].perp();
	  
	  float etafirstconst = pieces[0].eta();
	  float etasecondconst = pieces[1].eta();
	  
	  float phifirstconst = pieces[0].phi();
	  float phisecondconst = pieces[1].phi();
	  
	  float jet_mass_1=pieces[0].m();
	  float jet_mass_2=pieces[1].m();
	
	  GenSubJet1Pt_->push_back(ptfirstconst);
	  GenSubJet2Pt_->push_back(ptsecondconst);
	  GenSubJet1Eta_->push_back(etafirstconst);
	  GenSubJet2Eta_->push_back(etasecondconst);
	  GenSubJet1Phi_->push_back(phifirstconst);
	  GenSubJet2Phi_->push_back(phisecondconst);
	  GenSubJet1Mass_->push_back(jet_mass_1);
	  GenSubJet2Mass_->push_back(jet_mass_2);
	}
	      
	else {

	  GenSubJet1Pt_->push_back(-10);
	  GenSubJet2Pt_->push_back(-10);
	  GenSubJet1Eta_->push_back(-10);
	  GenSubJet2Eta_->push_back(-10);
	  GenSubJet1Phi_->push_back(-10);
	  GenSubJet2Phi_->push_back(-10);
	  GenSubJet1Mass_->push_back(-10);
	  GenSubJet2Mass_->push_back(-10);
   
	}

	GenSubJetsDeltaR_->push_back(deltaRgensubjets);
	GenSubJetsMu_->push_back(mugensubjets);
	nSubGenJets_->push_back(pieces.size());

        lOutJets.clear();
        delete fClustering;
        lClusterParticles.clear();

      }
    }

    edm::Handle<reco::JetFlavourInfoMatchingCollection> theJetFlavourInfos;
    iEvent.getByToken(jetFlavourInfosToken_, theJetFlavourInfos );
    
    for ( reco::JetFlavourInfoMatchingCollection::const_iterator j  = theJetFlavourInfos->begin();j != theJetFlavourInfos->end();++j ) {
      const reco::Jet *aJet = (*j).first.get();
      reco::JetFlavourInfo aInfo = (*j).second;

      if (aJet->pt() > GenptMin_ && fabs(aJet->y()) < GenetaMax_ ) {
        
	int FlavourGenHadron = aInfo.getHadronFlavour();
	if(abs(FlavourGenHadron)==5){ 
	  isBJetGen_->push_back(true);
	  if(isPrint_) cout<<"GEN "<<aJet->pt()<<" "<<aJet->y()<<" "<<aJet->phi()<<endl;	
	}
	else isBJetGen_->push_back(false);
      }
    }

   
  }//--- end of MC -------

  cutFlowHisto_->Fill("All",1); if (passTrigger || !passTrigger) {
cutFlowHisto_->Fill("trigger",1); if (nJets_ > 1) {
cutFlowHisto_->Fill("nJets",1); if ((*pt_)[0] > ptMinLeading_ && (*pt_)[1] >
ptMinLeading_) { cutFlowHisto_->Fill("ptMinLeading",1); outTree_->Fill();   } }
} }
//////////////////////////////////////////////////////////////////////////////////////////
void BoostedTTHFlatTreeProducer::initialize() { dPhiLJ_         = -1;
dPhiJJ_         = -1; dRJJ_           = -1; mJJ_            = -1;
yJJ_            = -1; ptJJ_           = -1; dPhiGenJJ_      = -1;
dPhiGenLJ_      = -1; dRGenJJ_        = -1; mGenJJ_         = -1;
yGenJJ_         = -1; ptGenJJ_        = -1; run_            = -1;
evt_            = -1; lumi_           = -1; nVtx_           = -1;
nJets_          = -1; nGenJets_       = -1; nLeptons_       = -1;
nBJets_         = -1; mvaValueMax_    = -8; highestMvaJet_  = -1;
rho_            = -1; met_            = -1; metGen_         = -1;
metSig_         = -1; metGenSig_      = -1; ht_             = -1;
mva_            = -999; pvRho_          = -999; pvz_            = -999;
pvndof_         = -999; pvchi2_         = -999; flavor_         ->clear();
flavorHadron_   ->clear(); nSubJets_       ->clear(); nSubGenJets_    ->clear();
nBSubJets_      ->clear(); pt_             ->clear(); eta_            ->clear();
phi_            ->clear(); mass_           ->clear(); massSoftDrop_   ->clear();
energy_         ->clear(); chf_            ->clear(); nhf_            ->clear();
phf_            ->clear(); elf_            ->clear(); muf_            ->clear();
tau1_           ->clear(); tau2_           ->clear(); tau3_           ->clear();
btagSub0_       ->clear(); btagSub1_       ->clear(); massSub0_       ->clear();
massSub1_       ->clear(); ptSub0_         ->clear(); ptSub1_         ->clear();
jetMva_         ->clear(); etaSub0_        ->clear(); etaSub1_       
->clear();  phiSub0_        ->clear(); phiSub1_        ->clear();
flavorSub0_     ->clear(); flavorSub1_     ->clear(); flavorHadronSub0_    
->clear(); flavorHadronSub1_     ->clear(); btag_           ->clear();
isBtag_         ->clear(); doubleBtag_	   ->clear(); isDoubleBtag_   
->clear(); triggerBit_     ->clear(); triggerPre_     ->clear(); lId_           
->clear(); lIso_           ->clear(); lPt_            ->clear(); lEta_          
->clear(); lPhi_           ->clear(); lE_             ->clear();


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
    
    partonSt_->clear();
    partonId_->clear();
    partonMatchIdx_->clear();
    partonMatchDR_->clear();
    partonPt_->clear();
    partonEta_->clear();
    partonPhi_->clear();
    partonE_->clear(); 
    partonMass_->clear();
    isBJetGen_->clear();
    GenSDSimmetry_->clear();
    GenSoftDropMass_->clear();
    GenSoftDropTau_->clear();
    GenJetpt_->clear();
    GenJetphi_->clear();
    GenJeteta_->clear();
    GenJetenergy_->clear();
    GenJetmass_->clear();
    GenJettau1_->clear();
    GenJettau2_->clear();
    GenJettau3_->clear();

    GenSubJet1Pt_->clear();
    GenSubJet2Pt_->clear();
    GenSubJet1Eta_->clear();
    GenSubJet2Eta_->clear();
    GenSubJet1Phi_->clear();
    GenSubJet2Phi_->clear();
    GenSubJet1Mass_->clear();
    GenSubJet2Mass_->clear();
    GenSubJetsDeltaR_->clear();
    GenSubJetsMu_->clear();

  }
}
//////////////////////////////////////////////////////////////////////////////////////////
BoostedTTHFlatTreeProducer::~BoostedTTHFlatTreeProducer() 
{
}

DEFINE_FWK_MODULE(BoostedTTHFlatTreeProducer);
