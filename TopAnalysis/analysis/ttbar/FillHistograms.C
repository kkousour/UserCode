#include "TreeClassResolved.C"
#include "TreeClassBoosted.C"
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TString.h> 
#include <TCollection.h>
#include <TKey.h>
void FillHistograms(TString SAMPLE)
{
  cout<<"Processing sample "<<SAMPLE<<endl;
  TString PATH("");
  TFile *inf  = TFile::Open(PATH+"flatTree_"+SAMPLE+".root");
  TFile *outf = TFile::Open(TString::Format("Histo_%s.root",SAMPLE.Data()),"RECREATE");

  TIter nextKey(inf->GetListOfKeys());
  TKey *key;
  while ((key = (TKey*)nextKey())) {
    TString dirName(key->GetName());
    cout<<"Found directory "<<dirName<<endl;
    
    TH1F *hPass = (TH1F*)inf->Get(dirName+"/TriggerPass");
    outf->mkdir(dirName);  
    TDirectory *dir = (TDirectory*)outf->Get(dirName); 
    TTree *tr   = (TTree*)inf->Get(dirName+"/events");
    if (dirName.Contains("Boost")) {
      TreeClassBoosted myTree(tr);
      dir->cd();
      hPass->Write("TriggerPass");
      myTree.Loop(dir);
      cout<<"Loop finished"<<endl;
      dir->Close();
      cout<<"directory closed"<<endl;
      delete tr;
      cout<<"Tree deleted"<<endl;
    }
    else {
      TreeClassResolved myTree(tr);
      dir->cd();
      hPass->Write("TriggerPass");
      myTree.Loop(dir);
      cout<<"Loop finished"<<endl;
      dir->Close();
      cout<<"directory closed"<<endl;
      delete tr;
      cout<<"Tree deleted"<<endl;
    }
  }
  outf->Close();
  inf->Close();
}
