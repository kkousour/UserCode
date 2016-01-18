//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Aug 10 13:45:22 2015 by ROOT version 6.02/05
// from TTree events/events
// found on file: flatTree.root
//////////////////////////////////////////////////////////

#ifndef TreeClassBoosted_h
#define TreeClassBoosted_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"
#include "vector"
#include "vector"

class TreeClassBoosted {
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
   const Int_t kMaxrho = 1;
   const Int_t kMaxht = 1;
   const Int_t kMaxmet = 1;
   const Int_t kMaxmetSig = 1;
   const Int_t kMaxmJJ = 1;
   const Int_t kMaxyJJ = 1;
   const Int_t kMaxptJJ = 1;
   const Int_t kMaxdRJJ = 1;
   const Int_t kMaxdPhiJJ = 1;
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
   Float_t         rho;
   Float_t         ht;
   Float_t         mva;
   Float_t         met;
   Float_t         metSig;
   Float_t         mJJ;
   Float_t         yJJ;
   Float_t         ptJJ;
   Float_t         dRJJ;
   Float_t         dPhiJJ;
   vector<bool>    *jetIsBtag;
   vector<int>     *jetFlavor;
   vector<int>     *jetNSub;
   vector<int>     *jetNBSub;
   vector<float>   *jetPt;
   vector<float>   *jetBtag;
   vector<float>   *jetEta;
   vector<float>   *jetPhi;
   vector<float>   *jetMass;
   vector<float>   *jetMassSoftDrop;
   vector<float>   *jetEnergy;
   vector<float>   *jetChf;
   vector<float>   *jetNhf;
   vector<float>   *jetPhf;
   vector<float>   *jetMuf;
   vector<float>   *jetElf;
   vector<float>   *jetTau1;
   vector<float>   *jetTau2;
   vector<float>   *jetTau3;
   vector<float>   *jetBtagSub0;
   vector<float>   *jetBtagSub1;
   vector<float>   *jetMassSub0;
   vector<float>   *jetMassSub1;
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
   TBranch        *b_rho_;   //!
   TBranch        *b_ht_;   //!
   TBranch        *b_mva_;   //!
   TBranch        *b_met_;   //!
   TBranch        *b_metSig_;   //!
   TBranch        *b_mJJ_;   //!
   TBranch        *b_yJJ_;   //!
   TBranch        *b_ptJJ_;   //!
   TBranch        *b_dRJJ_;   //!
   TBranch        *b_dPhiJJ_;   //!
   TBranch        *b_jetIsBtag;   //!
   TBranch        *b_jetFlavor;   //!
   TBranch        *b_jetNSub;   //!
   TBranch        *b_jetNBSub;   //!
   TBranch        *b_jetPt;   //!
   TBranch        *b_jetBtag;   //!
   TBranch        *b_jetEta;   //!
   TBranch        *b_jetPhi;   //!
   TBranch        *b_jetMass;   //!
   TBranch        *b_jetMassSoftDrop;   //!
   TBranch        *b_jetEnergy;   //!
   TBranch        *b_jetChf;   //!
   TBranch        *b_jetNhf;   //!
   TBranch        *b_jetPhf;   //!
   TBranch        *b_jetMuf;   //!
   TBranch        *b_jetElf;   //!
   TBranch        *b_jetTau1;   //!
   TBranch        *b_jetTau2;   //!
   TBranch        *b_jetTau3;   //!
   TBranch        *b_jetBtagSub0;   //!
   TBranch        *b_jetBtagSub1;   //!
   TBranch        *b_jetMassSub0;   //!
   TBranch        *b_jetMassSub1;   //!
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

   TreeClassBoosted(TTree *tree=0);
   virtual ~TreeClassBoosted();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop(TDirectory *DIR);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef TreeClassBoosted_cxx
TreeClassBoosted::TreeClassBoosted(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("flatTree.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("flatTree.root");
      }
      TDirectory * dir = (TDirectory*)f->Get("flatTree.root:/hadtopBoost");
      dir->GetObject("events",tree);

   }
   Init(tree);
}

TreeClassBoosted::~TreeClassBoosted()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t TreeClassBoosted::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t TreeClassBoosted::LoadTree(Long64_t entry)
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

void TreeClassBoosted::Init(TTree *tree)
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
   jetNSub = 0;
   jetNBSub = 0;
   jetPt = 0;
   jetBtag = 0;
   jetEta = 0;
   jetPhi = 0;
   jetMass = 0;
   jetMassSoftDrop = 0;
   jetEnergy = 0;
   jetChf = 0;
   jetNhf = 0;
   jetPhf = 0;
   jetMuf = 0;
   jetElf = 0;
   jetTau1 = 0;
   jetTau2 = 0;
   jetTau3 = 0;
   jetBtagSub0 = 0;
   jetBtagSub1 = 0;
   jetMassSub0 = 0;
   jetMassSub1 = 0;
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
   fChain->SetBranchAddress("rho", &rho, &b_rho_);
   fChain->SetBranchAddress("ht", &ht, &b_ht_);
   fChain->SetBranchAddress("mva", &mva, &b_mva_);
   fChain->SetBranchAddress("met", &met, &b_met_);
   fChain->SetBranchAddress("metSig", &metSig, &b_metSig_);
   fChain->SetBranchAddress("mJJ", &mJJ, &b_mJJ_);
   fChain->SetBranchAddress("yJJ", &yJJ, &b_yJJ_);
   fChain->SetBranchAddress("ptJJ", &ptJJ, &b_ptJJ_);
   fChain->SetBranchAddress("dRJJ", &dRJJ, &b_dRJJ_);
   fChain->SetBranchAddress("dPhiJJ", &dPhiJJ, &b_dPhiJJ_);
   fChain->SetBranchAddress("jetIsBtag", &jetIsBtag, &b_jetIsBtag);
   fChain->SetBranchAddress("jetFlavor", &jetFlavor, &b_jetFlavor);
   fChain->SetBranchAddress("jetNSub", &jetNSub, &b_jetNSub);
   fChain->SetBranchAddress("jetNBSub", &jetNBSub, &b_jetNBSub);
   fChain->SetBranchAddress("jetPt", &jetPt, &b_jetPt);
   fChain->SetBranchAddress("jetBtag", &jetBtag, &b_jetBtag);
   fChain->SetBranchAddress("jetEta", &jetEta, &b_jetEta);
   fChain->SetBranchAddress("jetPhi", &jetPhi, &b_jetPhi);
   fChain->SetBranchAddress("jetMass", &jetMass, &b_jetMass);
   fChain->SetBranchAddress("jetMassSoftDrop", &jetMassSoftDrop, &b_jetMassSoftDrop);
   fChain->SetBranchAddress("jetEnergy", &jetEnergy, &b_jetEnergy);
   fChain->SetBranchAddress("jetChf", &jetChf, &b_jetChf);
   fChain->SetBranchAddress("jetNhf", &jetNhf, &b_jetNhf);
   fChain->SetBranchAddress("jetPhf", &jetPhf, &b_jetPhf);
   fChain->SetBranchAddress("jetMuf", &jetMuf, &b_jetMuf);
   fChain->SetBranchAddress("jetElf", &jetElf, &b_jetElf);
   fChain->SetBranchAddress("jetTau1", &jetTau1, &b_jetTau1);
   fChain->SetBranchAddress("jetTau2", &jetTau2, &b_jetTau2);
   fChain->SetBranchAddress("jetTau3", &jetTau3, &b_jetTau3);
   fChain->SetBranchAddress("jetBtagSub0", &jetBtagSub0, &b_jetBtagSub0);
   fChain->SetBranchAddress("jetBtagSub1", &jetBtagSub1, &b_jetBtagSub1);
   fChain->SetBranchAddress("jetMassSub0", &jetMassSub0, &b_jetMassSub0);
   fChain->SetBranchAddress("jetMassSub1", &jetMassSub1, &b_jetMassSub1);
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

Bool_t TreeClassBoosted::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void TreeClassBoosted::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t TreeClassBoosted::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef TreeClassBoosted_cxx
