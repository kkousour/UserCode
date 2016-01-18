#define TreeClassResolved_cxx
#include "TreeClassResolved.h"
#include <iostream>
#include <TString.h>
#include <TH1F.h>
#include <TDirectory.h>
using std::cin;
using std::cout;
using std::endl;

void TreeClassResolved::Loop(TDirectory *DIR)
{
  //---- define histograms ------------------
  char name[1000];
  const int NVAR = 19;
  TString var[NVAR] = {"ht","htBtag","nvtx","nJets","nBJets","met",
                       "sphericity","aplanarity","FW0","FW1","FW2","FW3",
                       "mTop[0]","dRbbTop","mTTbar","ptTTbar","chi2","prob","mWReco"}; 
  double dX[NVAR]   = {10,10,1,1,1,1,
                       0.01,0.005,0.005,0.005,0.005,0.005,
                       1,0.05,1,1,0.5,0.01,1.0};
  double XMIN[NVAR] = {0,0,0,0,0,0,
                       0,0,0.2,-0.3,-0.1,-0.3,
                       0,0,0,0,0,0,0};
  double XMAX[NVAR] = {3000,1000,50,15,10,150,
                       1,0.5,0.5,0.2,0.5,0.3,
                       3000,7,2000,2000,200,1,200};
  
  TH1F *hVar[NVAR];
  cout<<"Booking histograms.........."<<endl;
  for(int ivar=0;ivar<NVAR;ivar++) {
    int NBINS = (XMAX[ivar]-XMIN[ivar])/dX[ivar];
    sprintf(name,"h_%s",var[ivar].Data()); 
    hVar[ivar] = new TH1F(name,name,NBINS,XMIN[ivar],XMAX[ivar]);
  }
  cout<<"Booking jet histograms.........."<<endl;
  const int NJETVAR = 4;
  TString varJet[NJETVAR] = {"jetPt","jetEta","jetBtag","jetQGL"}; 
  double dXJET[NJETVAR]   = {2,0.1,0.01,0.01};
  double XMINJET[NJETVAR] = {0,-3,-10,0};
  double XMAXJET[NJETVAR] = {500,3,1,1};
  TH1F *hJetVar[NJETVAR][6];
  
  for(int ivar=0;ivar<NJETVAR;ivar++) {
    int NBINS = (XMAXJET[ivar]-XMINJET[ivar])/dXJET[ivar];
    for(int j=0;j<6;j++) {
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
    bool cut_trigger = ((*triggerBit)[0] || (*triggerBit)[2]);
    bool cut_leptons = (nLeptons == 0); 
    //bool cut_met     = (met < 80);
    bool cut_ht      = (ht > 450);
    bool cut_jetPt   = ((*jetPt)[5] > 40);
    bool cut_nJets   = (nJets > 5);
    bool cut_nBJets  = (nBJets > 1);
    bool cut_prob    = (prob > 0.1);
    bool cut_dRbb    = (dRbbTop > 1.5);

    if (!cut_trigger)  continue;
    if (!cut_leptons)  continue;
    //if (!cut_met)      continue;
    if (!cut_ht)       continue;
    if (!cut_jetPt)    continue;
    if (!cut_nJets)    continue;
    if (!cut_nBJets)   continue;
    if (!cut_prob)     continue;
    if (!cut_dRbb)     continue;

    float x[NVAR] = {ht,htBtag,float(nvtx),float(nJets),float(nBJets),met,
                     sphericity,aplanarity,foxWolfram[0],foxWolfram[1],foxWolfram[2],foxWolfram[3],
                     mTop[0],dRbbTop,mTTbar,ptTTbar,chi2,prob,static_cast<float>(0.5*(mWReco[0]+mWReco[1]))};
     
    for(int ivar=0;ivar<NVAR;ivar++) {
      hVar[ivar]->Fill(x[ivar]);
    }
    for(int j=0;j<6;j++) {
      float xJet[NJETVAR] = {(*jetPt)[j],(*jetEta)[j],(*jetBtag)[j],(*jetQGL)[j]}; 
      for(int ivar=0;ivar<NJETVAR;ivar++) {
        hJetVar[ivar][j]->Fill(xJet[ivar]);
      }
    }    
  }
  DIR->Write();
}
