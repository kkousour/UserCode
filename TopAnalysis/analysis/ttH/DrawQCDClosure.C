void DrawQCDClosure(TString VAR,TString XTITLE)
{
  gROOT->ForceStyle();
  TString FileName[7] = {"QCD_HT200to300","QCD_HT300to500","QCD_HT500to700","QCD_HT700to1000","QCD_HT1000to1500","QCD_HT1500to2000","QCD_HT2000toInf"};
  float XSEC[7]       = {1.74e+6,3.67e+5,2.94e+4,6.524e+03,1.064e+03,121.5,2.542e+01};
  TFile *inf[7];
  TH1F  *h[7],*h1[7];

  TCanvas *can = new TCanvas("can_QCDClosure_"+VAR,"can_QCDClosure_"+VAR,900,600);
  can->cd(1);
  can->SetBottomMargin(0.3);
  can->SetRightMargin(0.15);

  for(int i=0;i<7;i++) {
    inf[i] = TFile::Open("Histo_"+FileName[i]+".root");
    TH1F *hTriggerPass = (TH1F*)inf[i]->Get("hadtopL/TriggerPass");
    h[i]   = (TH1F*)inf[i]->Get("hadtopL/h_"+VAR);
    h1[i]   = (TH1F*)inf[i]->Get("hadtop/h_"+VAR);
    h[i]->Sumw2();
    h1[i]->Sumw2();
    h[i]->Rebin(5);
    h1[i]->Rebin(5);
    h[i]->Scale(XSEC[i]/hTriggerPass->GetBinContent(1));
    h1[i]->Scale(XSEC[i]/hTriggerPass->GetBinContent(1));
    cout<<hTriggerPass->GetBinContent(1)<<endl;
  }

  TH1F *hQCD  = (TH1F*)h[0]->Clone("hQCD");
  TH1F *hQCD1 = (TH1F*)h1[0]->Clone("hQCD1");
  for(int i=0;i<7;i++) {
    hQCD->Add(h[i]);
    hQCD1->Add(h1[i]);
  } 
  hQCD->SetFillColor(kGray);

  hQCD->Scale(1./hQCD->Integral());
  hQCD1->Scale(1./hQCD1->Integral());
  hQCD->GetXaxis()->SetLabelSize(0.0);
  double max = 1.1*TMath::Max(hQCD->GetBinContent(hQCD->GetMaximumBin()),hQCD1->GetBinContent(hQCD1->GetMaximumBin()));
  hQCD->SetMinimum(1e-5);
  hQCD->SetMaximum(max);
  hQCD->Draw("hist");
  hQCD1->Draw("sameE");
  gPad->RedrawAxis();

  TLegend *leg = new TLegend(0.86,0.65,0.99,0.9);
  leg->SetFillColor(0);
  leg->SetTextFont(42);
  leg->SetTextSize(0.03);
  leg->AddEntry(hQCD,"Control","F");
  leg->AddEntry(hQCD1,"Signal","LP");
  leg->Draw();

  TH1F *hRatio = (TH1F*)hQCD1->Clone("Ratio");
  hRatio->Divide(hQCD);

  TPad* pad = new TPad("pad", "pad", 0., 0., 1., 1.);
  pad->SetTopMargin(0.7);
  pad->SetRightMargin(0.15);
  pad->SetFillColor(0);
  pad->SetFillStyle(0);
  pad->Draw();
  pad->cd(0);
  gPad->SetGridy();
  hRatio->GetXaxis()->SetTitle(XTITLE);
  hRatio->GetYaxis()->SetNdivisions(505);
  hRatio->GetYaxis()->SetRangeUser(0,2);
  hRatio->GetYaxis()->SetLabelSize(0.04);
  hRatio->Draw();
}















