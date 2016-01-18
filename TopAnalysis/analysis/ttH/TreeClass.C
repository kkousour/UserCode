#define TreeClass_cxx
#include "TreeClass.h"
#include <iostream>
#include <TString.h>
#include <TH1F.h>
#include <TDirectory.h>
using std::cin;
using std::cout;
using std::endl;

void TreeClass::Loop(TDirectory *DIR)
{
  //---- Number of categories ---------------
  const int NCAT = 2;
  //---- define histograms ------------------
  char name[1000];
  const int NVAR = 23;
  TString var[NVAR] = {"mva","ht","htBtag","nvtx","nJets","nBJets","met",
                       "sphericity","aplanarity","FW0","FW1","FW2","FW3",
                       "qglAve","qglMedian","qglMin","dRbbMin","mbbMin",
                       "mTop[0]","dRbbTop","mTTbar","ptTTbar","chi2"}; 
  double dX[NVAR]   = {0.01,10,10,1,1,1,1,
                       0.01,0.005,0.005,0.005,0.005,0.005,
                       0.01,0.01,0.01,0.1,5,
                       1,0.05,1,1,0.5};
  double XMIN[NVAR] = {-1,0,0,0,0,0,0,
                       0,0,0.2,-0.3,-0.1,-0.3,
                       0,0,0,0,0,
                       0,0,0,0,0};
  double XMAX[NVAR] = {1,3000,1000,50,15,10,150,
                       1,0.5,0.5,0.2,0.5,0.3,
                       1,1,1,6,500,
                       3000,7,2000,2000,200};
  TH1F *hCat = new TH1F("hCat","hCat",NCAT+1,0,NCAT+1);
  TH1F *hVar[NVAR][NCAT+1];
  cout<<"Booking histograms.........."<<endl;
  for(int ivar=0;ivar<NVAR;ivar++) {
    int NBINS = (XMAX[ivar]-XMIN[ivar])/dX[ivar];
    for(int icat=0;icat<=NCAT;icat++) {
      sprintf(name,"h_%s_CAT%d",var[ivar].Data(),icat); 
      hVar[ivar][icat] = new TH1F(name,name,NBINS,XMIN[ivar],XMAX[ivar]);
    }
  }
  cout<<"Booking jet histograms.........."<<endl;
  const int NJETVAR = 4;
  TString varJet[NJETVAR] = {"jetPt","jetEta","jetBtag","jetQGL"}; 
  double dXJET[NJETVAR]   = {2,0.1,0.01,0.01};
  double XMINJET[NJETVAR] = {0,-3,-10,0};
  double XMAXJET[NJETVAR] = {500,3,1,1};
  TH1F *hJetVar[NJETVAR][6][NCAT+1];
  
  for(int ivar=0;ivar<NJETVAR;ivar++) {
    int NBINS = (XMAXJET[ivar]-XMINJET[ivar])/dXJET[ivar];
    for(int j=0;j<6;j++) {
      for(int icat=0;icat<=NCAT;icat++) {
        sprintf(name,"h_%s%d_CAT%d",varJet[ivar].Data(),j,icat);
        hJetVar[ivar][j][icat] = new TH1F(name,name,NBINS,XMINJET[ivar],XMAXJET[ivar]);
      }
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
    bool cut_ht      = (ht > 450);
    bool cut_jetPt   = ((*jetPt)[5] > 40);
    bool cut_nJets   = (nJets > 5);
    bool cut_nBJets  = (nBJets > 1);
    bool cut_status  = (status == 0);

    if (!cut_trigger)  continue;
    if (!cut_leptons)  continue;
    if (!cut_ht)       continue;
    if (!cut_jetPt)    continue;
    if (!cut_nJets)    continue;
    if (!cut_nBJets)   continue;
    if (!cut_status)   continue;

    int category(0);
    
    if (nBJets == 2) {
      category = 1;
    } 
    if (nBJets > 2) {
      category = 2;
    }
    
    hCat->Fill(category);
    float x[NVAR] = {mva,ht,htBtag,float(nvtx),float(nJets),float(nBJets),met,
                     sphericity,aplanarity,foxWolfram[0],foxWolfram[1],foxWolfram[2],foxWolfram[3],
                     qglAve,qglMedian,qglMin,dRbbMin,mbbMin,mTop[0],dRbbTop,mTTbar,ptTTbar,chi2};
     
    for(int ivar=0;ivar<NVAR;ivar++) {
      hVar[ivar][0]->Fill(x[ivar]);
      hVar[ivar][category]->Fill(x[ivar]); 
    }
    for(int j=0;j<6;j++) {
      float xJet[NJETVAR] = {(*jetPt)[j],(*jetEta)[j],(*jetBtag)[j],(*jetQGL)[j]}; 
      for(int ivar=0;ivar<NJETVAR;ivar++) {
        hJetVar[ivar][j][0]->Fill(xJet[ivar]);
        hJetVar[ivar][j][category]->Fill(xJet[ivar]);
      }
    }    
  }
  DIR->Write();
}
