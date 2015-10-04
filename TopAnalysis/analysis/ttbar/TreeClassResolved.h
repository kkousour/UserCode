//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Aug 10 13:45:05 2015 by ROOT version 6.02/05
// from TTree events/events
// found on file: flatTree.root
//////////////////////////////////////////////////////////

#ifndef TreeClassResolved_h
#define TreeClassResolved_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"
#include "vector"
#include "vector"

class TreeClassResolved {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.
   const Int_t kMaxrun = 1;
   const Int_t kMaxevt = 1;
   const Int_t kMaxlumi = 1;
   const Int_t kMaxnVtx = 1;
   const Int_t kMaxnJets = 1;
   const Int_t kMaxnLeptons = 1;
   const Int_t kMaxnBJets = 1;
   const Int_t kMaxstatus = 1;
   const Int_t kMaxprob = 1;
   const Int_t kMaxchi2 = 1;
   const Int_t kMaxrho = 1;
   const Int_t kMaxht = 1;
   const Int_t kMaxhtBtag = 1;
   const Int_t kMaxmet = 1;
   const Int_t kMaxmetSig = 1;
   const Int_t kMaxsphericity = 1;
   const Int_t kMaxaplanarity = 1;
   const Int_t kMaxfoxWolfram = 1;
   const Int_t kMaxqglAve = 1;
   const Int_t kMaxqglMin = 1;
   const Int_t kMaxqglMedian = 1;
   const Int_t kMaxidxQ = 1;
   const Int_t kMaxidxQbar = 1;
   const Int_t kMaxidxB = 1;
   const Int_t kMaxidxP = 1;
   const Int_t kMaxidxPbar = 1;
   const Int_t kMaxidxBbar = 1;
   const Int_t kMaxmW = 1;
   const Int_t kMaxmTop = 1;
   const Int_t kMaxptTop = 1;
   const Int_t kMaxyTop = 1;
   const Int_t kMaxmTTbar = 1;
   const Int_t kMaxyTTbar = 1;
   const Int_t kMaxptTTbar = 1;
   const Int_t kMaxdRbbTop = 1;
   const Int_t kMaxmWReco = 1;
   const Int_t kMaxmTopReco = 1;
   const Int_t kMaxdRbbTopReco = 1;
   const Int_t kMaxdecay = 1;
   const Int_t kMaxnpu = 1;

   // Declaration of leaf types
   Int_t           runNo;
   Int_t           evtNo;
   Int_t           lumi;
   Int_t           nvtx;
   Int_t           nJets;
   Int_t           nLeptons;
   Int_t           nBJets;
   Int_t           status;
   Float_t         prob;
   Float_t         chi2;
   Float_t         rho;
   Float_t         ht;
   Float_t         htBtag;
   Float_t         met;
   Float_t         metSig;
   Float_t         sphericity;
   Float_t         aplanarity;
   Float_t         foxWolfram[4];
   Float_t         qglAve;
   Float_t         qglMin;
   Float_t         qglMedian;
   Int_t           idxQ;
   Int_t           idxQbar;
   Int_t           idxB;
   Int_t           idxP;
   Int_t           idxPbar;
   Int_t           idxBbar;
   Float_t         mW[2];
   Float_t         mTop[2];
   Float_t         ptTop[2];
   Float_t         yTop[2];
   Float_t         mTTbar;
   Float_t         yTTbar;
   Float_t         ptTTbar;
   Float_t         dRbbTop;
   Float_t         mWReco[2];
   Float_t         mTopReco[2];
   Float_t         dRbbTopReco;
   vector<bool>    *jetIsBtag;
   vector<int>     *jetFlavor;
   vector<float>   *jetPt;
   vector<float>   *jetBtag;
   vector<float>   *jetQGL;
   vector<float>   *jetEta;
   vector<float>   *jetPhi;
   vector<float>   *jetMass;
   vector<float>   *jetEnergy;
   vector<float>   *jetChf;
   vector<float>   *jetNhf;
   vector<float>   *jetPhf;
   vector<float>   *jetMuf;
   vector<float>   *jetElf;
   vector<float>   *jetPuMva;
   vector<int>     *lepId;
   vector<float>   *lepPt;
   vector<float>   *lepEta;
   vector<float>   *lepPhi;
   vector<float>   *lepEnergy;
   vector<float>   *lepIso;
   vector<bool>    *triggerBit;
   vector<int>     *triggerPre;
   Int_t           decay;
   Int_t           npu;

   // List of branches
   TBranch        *b_run_;   //!
   TBranch        *b_evt_;   //!
   TBranch        *b_lumi_;   //!
   TBranch        *b_nVtx_;   //!
   TBranch        *b_nJets_;   //!
   TBranch        *b_nLeptons_;   //!
   TBranch        *b_nBJets_;   //!
   TBranch        *b_status_;   //!
   TBranch        *b_prob_;   //!
   TBranch        *b_chi2_;   //!
   TBranch        *b_rho_;   //!
   TBranch        *b_ht_;   //!
   TBranch        *b_htBtag_;   //!
   TBranch        *b_met_;   //!
   TBranch        *b_metSig_;   //!
   TBranch        *b_sphericity_;   //!
   TBranch        *b_aplanarity_;   //!
   TBranch        *b_foxWolfram_;   //!
   TBranch        *b_qglAve_;   //!
   TBranch        *b_qglMin_;   //!
   TBranch        *b_qglMedian_;   //!
   TBranch        *b_idxQ_;   //!
   TBranch        *b_idxQbar_;   //!
   TBranch        *b_idxB_;   //!
   TBranch        *b_idxP_;   //!
   TBranch        *b_idxPbar_;   //!
   TBranch        *b_idxBbar_;   //!
   TBranch        *b_mW_;   //!
   TBranch        *b_mTop_;   //!
   TBranch        *b_ptTop_;   //!
   TBranch        *b_yTop_;   //!
   TBranch        *b_mTTbar_;   //!
   TBranch        *b_yTTbar_;   //!
   TBranch        *b_ptTTbar_;   //!
   TBranch        *b_dRbbTop_;   //!
   TBranch        *b_mWReco_;   //!
   TBranch        *b_mTopReco_;   //!
   TBranch        *b_dRbbTopReco_;   //!
   TBranch        *b_jetIsBtag;   //!
   TBranch        *b_jetFlavor;   //!
   TBranch        *b_jetPt;   //!
   TBranch        *b_jetBtag;   //!
   TBranch        *b_jetQGL;   //!
   TBranch        *b_jetEta;   //!
   TBranch        *b_jetPhi;   //!
   TBranch        *b_jetMass;   //!
   TBranch        *b_jetEnergy;   //!
   TBranch        *b_jetChf;   //!
   TBranch        *b_jetNhf;   //!
   TBranch        *b_jetPhf;   //!
   TBranch        *b_jetMuf;   //!
   TBranch        *b_jetElf;   //!
   TBranch        *b_jetPuMva;   //!
   TBranch        *b_lepId;   //!
   TBranch        *b_lepPt;   //!
   TBranch        *b_lepEta;   //!
   TBranch        *b_lepPhi;   //!
   TBranch        *b_lepEnergy;   //!
   TBranch        *b_lepIso;   //!
   TBranch        *b_triggerBit;   //!
   TBranch        *b_triggerPre;   //!
   TBranch        *b_decay_;   //!
   TBranch        *b_npu_;   //!

   TreeClassResolved(TTree *tree=0);
   virtual ~TreeClassResolved();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop(TDirectory *DIR);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef TreeClassResolved_cxx
TreeClassResolved::TreeClassResolved(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("flatTree.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("flatTree.root");
      }
      TDirectory * dir = (TDirectory*)f->Get("flatTree.root:/hadtop");
      dir->GetObject("events",tree);

   }
   Init(tree);
}

TreeClassResolved::~TreeClassResolved()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t TreeClassResolved::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t TreeClassResolved::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void TreeClassResolved::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   jetIsBtag = 0;
   jetFlavor = 0;
   jetPt = 0;
   jetBtag = 0;
   jetQGL = 0;
   jetEta = 0;
   jetPhi = 0;
   jetMass = 0;
   jetEnergy = 0;
   jetChf = 0;
   jetNhf = 0;
   jetPhf = 0;
   jetMuf = 0;
   jetElf = 0;
   jetPuMva = 0;
   lepId = 0;
   lepPt = 0;
   lepEta = 0;
   lepPhi = 0;
   lepEnergy = 0;
   lepIso = 0;
   triggerBit = 0;
   triggerPre = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("runNo", &runNo, &b_run_);
   fChain->SetBranchAddress("evtNo", &evtNo, &b_evt_);
   fChain->SetBranchAddress("lumi", &lumi, &b_lumi_);
   fChain->SetBranchAddress("nvtx", &nvtx, &b_nVtx_);
   fChain->SetBranchAddress("nJets", &nJets, &b_nJets_);
   fChain->SetBranchAddress("nLeptons", &nLeptons, &b_nLeptons_);
   fChain->SetBranchAddress("nBJets", &nBJets, &b_nBJets_);
   fChain->SetBranchAddress("status", &status, &b_status_);
   fChain->SetBranchAddress("prob", &prob, &b_prob_);
   fChain->SetBranchAddress("chi2", &chi2, &b_chi2_);
   fChain->SetBranchAddress("rho", &rho, &b_rho_);
   fChain->SetBranchAddress("ht", &ht, &b_ht_);
   fChain->SetBranchAddress("htBtag", &htBtag, &b_htBtag_);
   fChain->SetBranchAddress("met", &met, &b_met_);
   fChain->SetBranchAddress("metSig", &metSig, &b_metSig_);
   fChain->SetBranchAddress("sphericity", &sphericity, &b_sphericity_);
   fChain->SetBranchAddress("aplanarity", &aplanarity, &b_aplanarity_);
   fChain->SetBranchAddress("foxWolfram", foxWolfram, &b_foxWolfram_);
   fChain->SetBranchAddress("qglAve", &qglAve, &b_qglAve_);
   fChain->SetBranchAddress("qglMin", &qglMin, &b_qglMin_);
   fChain->SetBranchAddress("qglMedian", &qglMedian, &b_qglMedian_);
   fChain->SetBranchAddress("idxQ", &idxQ, &b_idxQ_);
   fChain->SetBranchAddress("idxQbar", &idxQbar, &b_idxQbar_);
   fChain->SetBranchAddress("idxB", &idxB, &b_idxB_);
   fChain->SetBranchAddress("idxP", &idxP, &b_idxP_);
   fChain->SetBranchAddress("idxPbar", &idxPbar, &b_idxPbar_);
   fChain->SetBranchAddress("idxBbar", &idxBbar, &b_idxBbar_);
   fChain->SetBranchAddress("mW", mW, &b_mW_);
   fChain->SetBranchAddress("mTop", mTop, &b_mTop_);
   fChain->SetBranchAddress("ptTop", ptTop, &b_ptTop_);
   fChain->SetBranchAddress("yTop", yTop, &b_yTop_);
   fChain->SetBranchAddress("mTTbar", &mTTbar, &b_mTTbar_);
   fChain->SetBranchAddress("yTTbar", &yTTbar, &b_yTTbar_);
   fChain->SetBranchAddress("ptTTbar", &ptTTbar, &b_ptTTbar_);
   fChain->SetBranchAddress("dRbbTop", &dRbbTop, &b_dRbbTop_);
   fChain->SetBranchAddress("mWReco", mWReco, &b_mWReco_);
   fChain->SetBranchAddress("mTopReco", mTopReco, &b_mTopReco_);
   fChain->SetBranchAddress("dRbbTopReco", &dRbbTopReco, &b_dRbbTopReco_);
   fChain->SetBranchAddress("jetIsBtag", &jetIsBtag, &b_jetIsBtag);
   fChain->SetBranchAddress("jetFlavor", &jetFlavor, &b_jetFlavor);
   fChain->SetBranchAddress("jetPt", &jetPt, &b_jetPt);
   fChain->SetBranchAddress("jetBtag", &jetBtag, &b_jetBtag);
   fChain->SetBranchAddress("jetQGL", &jetQGL, &b_jetQGL);
   fChain->SetBranchAddress("jetEta", &jetEta, &b_jetEta);
   fChain->SetBranchAddress("jetPhi", &jetPhi, &b_jetPhi);
   fChain->SetBranchAddress("jetMass", &jetMass, &b_jetMass);
   fChain->SetBranchAddress("jetEnergy", &jetEnergy, &b_jetEnergy);
   fChain->SetBranchAddress("jetChf", &jetChf, &b_jetChf);
   fChain->SetBranchAddress("jetNhf", &jetNhf, &b_jetNhf);
   fChain->SetBranchAddress("jetPhf", &jetPhf, &b_jetPhf);
   fChain->SetBranchAddress("jetMuf", &jetMuf, &b_jetMuf);
   fChain->SetBranchAddress("jetElf", &jetElf, &b_jetElf);
   fChain->SetBranchAddress("jetPuMva", &jetPuMva, &b_jetPuMva);
   fChain->SetBranchAddress("lepId", &lepId, &b_lepId);
   fChain->SetBranchAddress("lepPt", &lepPt, &b_lepPt);
   fChain->SetBranchAddress("lepEta", &lepEta, &b_lepEta);
   fChain->SetBranchAddress("lepPhi", &lepPhi, &b_lepPhi);
   fChain->SetBranchAddress("lepEnergy", &lepEnergy, &b_lepEnergy);
   fChain->SetBranchAddress("lepIso", &lepIso, &b_lepIso);
   fChain->SetBranchAddress("triggerBit", &triggerBit, &b_triggerBit);
   fChain->SetBranchAddress("triggerPre", &triggerPre, &b_triggerPre);
   fChain->SetBranchAddress("decay", &decay, &b_decay_);
   fChain->SetBranchAddress("npu", &npu, &b_npu_);
   Notify();
}

Bool_t TreeClassResolved::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void TreeClassResolved::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t TreeClassResolved::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef TreeClassResolved_cxx
