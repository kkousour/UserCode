#define TreeClassBoosted_cxx
#include "TreeClassBoosted.h"
#include <iostream>
#include <TString.h>
#include <TH1F.h>
#include <TDirectory.h>
using std::cin;
using std::cout;
using std::endl;

void TreeClassBoosted::Loop(TDirectory *DIR)
{
//---- define histograms ------------------
  char name[1000];
  const int NVAR = 5;
  TString var[NVAR] = {"ht","nvtx","nJets","nBJets","met"}; 
  double dX[NVAR]   = {10,1,1,1,1};
  double XMIN[NVAR] = {0,0,0,0,0};
  double XMAX[NVAR] = {3000,50,15,10,150};
  
  TH1F *hVar[NVAR];
  cout<<"Booking histograms.........."<<endl;
  for(int ivar=0;ivar<NVAR;ivar++) {
    int NBINS = (XMAX[ivar]-XMIN[ivar])/dX[ivar];
    sprintf(name,"h_%s",var[ivar].Data()); 
    hVar[ivar] = new TH1F(name,name,NBINS,XMIN[ivar],XMAX[ivar]);
  }
  cout<<"Booking jet histograms.........."<<endl;
  const int NJETVAR = 5;
  TString varJet[NJETVAR] = {"jetPt","jetEta","jetBtag","jetMassSoftDrop","jetTau32"}; 
  double dXJET[NJETVAR]   = {1,0.1,0.01,0.5,0.01};
  double XMINJET[NJETVAR] = {0,-3,-10,0,0};
  double XMAXJET[NJETVAR] = {2000,3,1,1000,1};
  TH1F *hJetVar[NJETVAR][2];
  
  for(int ivar=0;ivar<NJETVAR;ivar++) {
    int NBINS = (XMAXJET[ivar]-XMINJET[ivar])/dXJET[ivar];
    for(int j=0;j<2;j++) {
      sprintf(name,"h_%s%d",varJet[ivar].Data(),j);
      hJetVar[ivar][j] = new TH1F(name,name,NBINS,XMINJET[ivar],XMAXJET[ivar]);
    }
  }
  if (fChain == 0) return;
  Long64_t nentries = fChain->GetEntriesFast();
  cout<<"Reading "<<nentries<<" events"<<endl;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    if (jentry % (nentries/10) == 0) cout<<100*jentry/nentries<<"%"<<endl;
    fChain->GetEntry(jentry);
    bool cut_trigger = ((*triggerBit)[1]);
    bool cut_nJets   = (nJets > 1);
    bool cut_leptons = (nLeptons == 0); 
    bool cut_met     = (met < 80);
    bool cut_jetPt   = ((*jetPt)[0] > 450);

    if (!cut_trigger)  continue;
    if (!cut_nJets)    continue;
    if (!cut_leptons)  continue;
    if (!cut_met)      continue;
    if (!cut_jetPt)    continue;

    float x[NVAR] = {ht,float(nvtx),float(nJets),float(nBJets),met};
     
    for(int ivar=0;ivar<NVAR;ivar++) {
      hVar[ivar]->Fill(x[ivar]);
    }
    for(int j=0;j<2;j++) {
      float xJet[NJETVAR] = {(*jetPt)[j],(*jetEta)[j],(*jetBtag)[j],(*jetMassSoftDrop)[j],(*jetTau3)[j]/(*jetTau2)[j]}; 
      for(int ivar=0;ivar<NJETVAR;ivar++) {
        bool cut_nBSub = ((*jetNBSub)[j] > -1);
        bool cut_tau32 = ((*jetTau3)[j]/(*jetTau2)[j] < 0.7);
        if (!cut_nBSub) continue; 
        if (!cut_tau32) continue; 
        hJetVar[ivar][j]->Fill(xJet[ivar]);
      }
    }    
  }
  DIR->Write();
}
