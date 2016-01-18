#include "FitData.C"
void FitDataAll(TString TITLE, int NBINS)
{
  gROOT->ForceStyle();
  float NSIG[10],NBKG[10],ESIG[10],EBKG[10];
  
  for(int i=0;i<1;i++) {
    FitData(5,"",NSIG[i],NBKG[i],ESIG[i],EBKG[i]);
  }
  
  TH1F *hSig = new TH1F("hSig","hSig",NBINS,0,NBINS);
  TH1F *hBkg = new TH1F("hBkg","hBkg",NBINS,0,NBINS);
  TH1F *hPur = new TH1F("hPur","hPur",NBINS,0,NBINS);
  
  for(int i=0;i<1;i++) {
    hSig->SetBinContent(i+1,NSIG[i]);
    hSig->SetBinError(i+1,ESIG[i]);
    hBkg->SetBinContent(i+1,NBKG[i]);
    hBkg->SetBinError(i+1,EBKG[i]);
    float p = NSIG[i]/(NSIG[i]+NBKG[i]);
    float e = (NSIG[i]*NBKG[i]/pow(NSIG[i]+NBKG[i],2))*sqrt(pow(ESIG[i]/NSIG[i],2)+pow(EBKG[i]/NBKG[i],2));
    hPur->SetBinContent(i+1,p);
    hPur->SetBinError(i+1,e);
    if (i == 0) {
      hPur->GetXaxis()->SetBinLabel(i+1,"All");
      hSig->GetXaxis()->SetBinLabel(i+1,"All");
    } 
    else {
      hPur->GetXaxis()->SetBinLabel(i+1,TString::Format("%d",i));
      hSig->GetXaxis()->SetBinLabel(i+1,TString::Format("%d",i));
    }
  }
  hSig->SetMarkerStyle(20);
  hSig->SetMarkerColor(kRed);
  hSig->SetLineColor(kRed);
  hBkg->SetMarkerStyle(21);
  hBkg->SetMarkerColor(kBlack);
  hBkg->SetLineColor(kBlack);
  hPur->SetLineColor(kBlack);
  hPur->SetMarkerColor(kBlack);
  hPur->SetMarkerSize(1.3);
  hPur->SetLineWidth(2);
  
  TCanvas *can = new TCanvas("FittedYields_"+BINVAR,"FittedYields_"+BINVAR,900,600);
  gPad->SetLogy();
  hSig->SetMinimum(10);
  hSig->GetXaxis()->CenterLabels();
  hSig->GetYaxis()->SetTitle("Fitted yield");
  hSig->GetXaxis()->SetTitle("Bin");
  hSig->Draw();
  hBkg->Draw("same");
  TLegend *leg = new TLegend(0.2,0.2,0.5,0.4);
  leg->SetHeader(TITLE);
  leg->AddEntry(hSig,"Signal","LP");
  leg->AddEntry(hBkg,"Bkg","LP");
  leg->SetFillColor(0);
  leg->SetBorderSize(0);
  leg->SetTextFont(42);
  leg->SetTextSize(0.05);
  leg->Draw();

  TCanvas *canPur = new TCanvas("Purity_"+BINVAR,"Purity_"+BINVAR,900,600);
  gPad->SetGridx();
  hPur->SetMaximum(1);
  hPur->SetMinimum(0);
  hPur->GetXaxis()->SetNdivisions(110);
  hPur->GetYaxis()->SetNdivisions(505);
  hPur->GetXaxis()->CenterLabels();
  hPur->GetXaxis()->SetTitle("Bin");
  hPur->GetYaxis()->SetTitle("Signal purity");
  hPur->Draw();

  TPaveText *pave = new TPaveText(0.2,0.2,0.4,0.3,"NDC");
  pave->AddText(TITLE);
  pave->SetTextFont(62);
  pave->SetFillColor(0);
  pave->Draw();

  TFile *outf = TFile::Open("FittedYields.root","RECREATE");
  outf->cd();
  hSig->Write("signal");
  hBkg->Write("background");
  outf->Close();
  
}
