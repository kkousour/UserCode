#include <iomanip>
void GetTreeSize(TString FileName)
{
  gROOT->ForceStyle();
  TFile *inf = TFile::Open(FileName);
  TIter nextKey(inf->GetListOfKeys());
  TKey *key;
  TH1F *hTreeSize = new TH1F("TreeSize","TreeSize",inf->GetListOfKeys()->Capacity(),0,inf->GetListOfKeys()->Capacity());
  cout<<"File contains "<<inf->GetListOfKeys()->Capacity()<<" directories"<<endl;
  while ((key = (TKey*)nextKey())) {
    TString dirName(key->GetName());
    cout<<"Found directory "<<dirName<<endl;
    TTree *tr = (TTree*)inf->Get(dirName+"/events");
    if (!tr) continue;
    TObjArray *branches = (TObjArray*)tr->GetListOfBranches();
    int size(0);
    cout.setf(ios::right);
    int N(branches->GetEntries());
    TH1F *hBranchSize = new TH1F("BranchSize_"+dirName,"BranchSize_"+dirName,N,0,N);
    for(int ib=0;ib<N;ib++) {
      TString name(branches->At(ib)->GetName());
      TBranch *br = (TBranch*)tr->GetBranch(name);
      hBranchSize->Fill(name,br->GetZipBytes()/1e+6); 
      size += br->GetZipBytes();
    } 
    hTreeSize->Fill(dirName,size/1e+6);
    cout<<"Total size: "<<size/1e+6<<" MB"<<endl;
    for(int ib=0;ib<N;ib++) {
      TString name(branches->At(ib)->GetName());
      TBranch *br = (TBranch*)tr->GetBranch(name);
      float percent = TMath::Ceil(1000*float(br->GetZipBytes())/float(size))/10;
      //cout<<ib<<setw(20)<<name<<setw(15)<<br->GetZipBytes()<<" "<<percent<<"%"<<endl;
    }

    TCanvas *can = new TCanvas("can_BranchSize_"+dirName,"can_BranchSize_"+dirName,1000,400);
    hBranchSize->GetXaxis()->SetTitle("Branch Name");
    hBranchSize->GetXaxis()->SetLabelSize(0.04);
    hBranchSize->GetYaxis()->SetTitle("Size (MB)");
    hBranchSize->SetFillColor(kGray);
    hBranchSize->Draw("hist");
  }
  TCanvas *can1 = new TCanvas("can_TreeSize","can_TreeSize",1000,400);
  hTreeSize->GetXaxis()->SetTitle("Tree Name");
  hTreeSize->GetYaxis()->SetTitle("Size (MB)");
  hTreeSize->SetFillColor(kGray);
  hTreeSize->Draw("hist");
}
