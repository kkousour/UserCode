void DrawBoosted(TString VAR, float XMIN, float XMAX, int REBIN, TString XTITLE)
{
  gROOT->ForceStyle();
  const int N = 7;
  float XSEC[N] = {1.74e+6,3.67e+5,2.94e+4,6.524e+03,1.064e+03,121.5,2.542e+01};

  TString SAMPLE[N] = {
    "QCD_HT200to300",
    "QCD_HT300to500",
    "QCD_HT500to700", 
    "QCD_HT700to1000",
    "QCD_HT1000to1500",
    "QCD_HT1500to2000",
    "QCD_HT2000toInf" 
  };
 
  TFile *inf[N];
  TH1F  *h0[N],*h1[N];
  TH1F  *hQCD0,*hQCD1; 

  TCanvas *can = new TCanvas("Boosted","Boosted",900,600);
  can->cd(1);
  can->SetBottomMargin(0.3);
  //can->SetRightMargin(0.15);
  for(int k=0;k<N;k++) {
    inf[k] = TFile::Open("Histo_"+SAMPLE[k]+".root");
    h0[k]  = (TH1F*)inf[k]->Get("hadtopBoost/h_"+VAR+"_Cut_ctl");
    h0[k]->Sumw2();
    h0[k]->Rebin(REBIN);
    h1[k]  = (TH1F*)inf[k]->Get("hadtopBoost/h_"+VAR+"_Cut_sig");
    h1[k]->Sumw2();
    h1[k]->Rebin(REBIN);
    h0[k]->Scale(XSEC[k]/((TH1F*)inf[k]->Get("hadtopBoost/TriggerPass"))->GetBinContent(1));
    h1[k]->Scale(XSEC[k]/((TH1F*)inf[k]->Get("hadtopBoost/TriggerPass"))->GetBinContent(1)); 
    cout<<SAMPLE[k]<<": "<<h0[k]->GetEntries()<<" "<<h0[k]->Integral()<<endl;
    if (k == 0) {
      hQCD0 = (TH1F*)h0[k]->Clone();
      hQCD1 = (TH1F*)h1[k]->Clone();
    }
    if (k > 0) {
      hQCD0->Add(h0[k]);
      hQCD1->Add(h1[k]);
    }
  } 
  hQCD0->Scale(1/hQCD0->Integral());
  hQCD1->Scale(1/hQCD1->Integral());
  double max1 = TMath::Max(hQCD0->GetBinContent(hQCD0->GetMaximumBin()),hQCD1->GetBinContent(hQCD1->GetMaximumBin()));
  hQCD0->SetMaximum(1.2*max1);
  hQCD0->SetMinimum(1e-4);
  hQCD0->GetXaxis()->SetLabelSize(0.0);
  hQCD0->GetXaxis()->SetRangeUser(XMIN,XMAX); 
  hQCD0->Draw("HIST");
  hQCD1->Draw("sameE");

  TLegend *leg = new TLegend(0.6,0.7,0.9,0.9);
  leg->SetHeader("QCD Closure");
  leg->SetBorderSize(0);
  leg->SetTextFont(42);
  leg->SetTextSize(0.05);
  leg->AddEntry(hQCD1,"Signal sample","P");
  leg->AddEntry(hQCD0,"Control sample","F");
  leg->Draw();

  TH1F *hRatio = (TH1F*)hQCD1->Clone("Ratio");
  hRatio->Divide(hQCD0);

  hRatio->SetLineColor(kBlack);
  hRatio->SetMarkerColor(kBlack);

  TPad* pad = new TPad("pad", "pad", 0., 0., 1., 1.);
  pad->SetTopMargin(0.7);
  //pad->SetRightMargin(0.15);
  pad->SetFillColor(0);
  pad->SetFillStyle(0);
  pad->Draw();
  pad->cd(0);
  gPad->SetGridy();
  hRatio->GetXaxis()->SetTitle(XTITLE);
  hRatio->GetXaxis()->SetRangeUser(XMIN,XMAX); 
  hRatio->GetYaxis()->SetNdivisions(505);
  hRatio->GetYaxis()->SetRangeUser(0,2);
  hRatio->GetYaxis()->SetLabelSize(0.04);
  hRatio->Draw();
  hRatio->Draw("same");
}




















