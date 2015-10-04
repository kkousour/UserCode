void DrawQCDClosure(TString VAR,int NBINS,float XMIN,float XMAX,TString XTITLE)
{
  gROOT->ForceStyle();
  
  const int N = 11;
  float XSEC[N] = {471100.,117276.,7823.,648.2,186.9,32.293,9.4183,0.84265,0.114943,0.00682981,0.000165445};

  TString SAMPLE[N] = {
    "QCD_Pt_120to170",
    "QCD_Pt_170to300",
    "QCD_Pt_300to470",
    "QCD_Pt_470to600",
    "QCD_Pt_600to800",
    "QCD_Pt_800to1000",
    "QCD_Pt_1000to1400",
    "QCD_Pt_1400to1800",
    "QCD_Pt_1800to2400",
    "QCD_Pt_2400to3200",
    "QCD_Pt_3200toInf" 
  }; 

  TFile *inf[N];
  TTree *tr0[N],*tr1[N],*tr2[N];
  TH1F  *h0[N],*h1[N],*h2[N];

  TCanvas *can = new TCanvas("can_QCDClosure_"+VAR,"can_QCDClosure_"+VAR,900,600);
  can->cd(1);
  can->SetBottomMargin(0.3);
  can->SetRightMargin(0.15);

  for(int i=0;i<N;i++) {
    inf[i] = TFile::Open("flatTree_"+SAMPLE[i]+".root");
    tr0[i] = (TTree*)inf[i]->Get("hadtopNoBtag/events");
    tr1[i] = (TTree*)inf[i]->Get("hadtopOneBtag/events");
    tr2[i] = (TTree*)inf[i]->Get("hadtop/events");
    TH1F *hpu = (TH1F*)inf[i]->Get("hadtop/pileup");
    h0[i]  = new TH1F("h0_"+SAMPLE[i],"h0_"+SAMPLE[i],NBINS,XMIN,XMAX);
    h1[i]  = new TH1F("h1_"+SAMPLE[i],"h1_"+SAMPLE[i],NBINS,XMIN,XMAX);
    h2[i]  = new TH1F("h2_"+SAMPLE[i],"h2_"+SAMPLE[i],NBINS,XMIN,XMAX);
    h0[i]->Sumw2();
    h1[i]->Sumw2();
    h2[i]->Sumw2();
    tr0[i]->Draw(VAR+">>"+"h0_"+SAMPLE[i],"triggerBit[0] && prob>0.05 && nBJets==0 && dRbbTop>2");
    tr1[i]->Draw(VAR+">>"+"h1_"+SAMPLE[i],"triggerBit[0] && prob>0.05 && nBJets==1 && dRbbTop>2");
    tr2[i]->Draw(VAR+">>"+"h2_"+SAMPLE[i],"triggerBit[0] && prob>0.05 && nBJets>1 && dRbbTop>2");
    h0[i]->Scale(XSEC[i]/hpu->GetEntries());
    h1[i]->Scale(XSEC[i]/hpu->GetEntries());
    h2[i]->Scale(XSEC[i]/hpu->GetEntries()); 
    cout<<SAMPLE[i]<<" "<<hpu->GetEntries()<<" "<<h0[i]->GetEntries()<<" "<<h1[i]->GetEntries()<<" "<<h2[i]->GetEntries()<<endl;
  }

  TH1F *hQCD0 = (TH1F*)h0[0]->Clone("hQCD0");
  TH1F *hQCD1 = (TH1F*)h1[0]->Clone("hQCD1");
  TH1F *hQCD2 = (TH1F*)h2[0]->Clone("hQCD2");

  for(int i=1;i<N;i++) {
    hQCD0->Add(h0[1]);
    hQCD1->Add(h1[1]);
    hQCD2->Add(h2[1]);
  }  
   
  hQCD0->SetFillColor(kGray);
  hQCD1->SetLineColor(kBlack);
  hQCD1->SetMarkerColor(kBlack);
  hQCD1->SetMarkerStyle(21);
  hQCD2->SetLineColor(kRed);
  hQCD2->SetMarkerColor(kRed);
  hQCD2->SetMarkerStyle(20);

  hQCD0->Scale(1./hQCD0->Integral());
  hQCD1->Scale(1./hQCD1->Integral());
  hQCD2->Scale(1./hQCD2->Integral());

  hQCD0->GetXaxis()->SetLabelSize(0.0);
  double max = 1.1*TMath::Max(hQCD0->GetBinContent(hQCD0->GetMaximumBin()),hQCD2->GetBinContent(hQCD2->GetMaximumBin()));
  hQCD0->SetMinimum(1e-5);
  hQCD0->SetMaximum(max);
  hQCD0->Draw("hist");
  hQCD1->Draw("sameE");
  hQCD2->Draw("sameE");
  gPad->RedrawAxis();

  TLegend *leg = new TLegend(0.86,0.65,0.99,0.9);
  leg->SetFillColor(0);
  leg->SetTextFont(42);
  leg->SetTextSize(0.03);
  leg->AddEntry(hQCD0,"Zero btag","F");
  leg->AddEntry(hQCD1,"One btag","LP");
  leg->AddEntry(hQCD2,"Two btag","LP");
  leg->Draw();

  TH1F *hRatio0 = (TH1F*)hQCD2->Clone("Ratio0");
  hRatio0->Divide(hQCD0);
  TH1F *hRatio1 = (TH1F*)hQCD2->Clone("Ratio1");
  hRatio1->Divide(hQCD1);

  hRatio0->SetLineColor(kBlack);
  hRatio0->SetMarkerColor(kBlack);

  TPad* pad = new TPad("pad", "pad", 0., 0., 1., 1.);
  pad->SetTopMargin(0.7);
  pad->SetRightMargin(0.15);
  pad->SetFillColor(0);
  pad->SetFillStyle(0);
  pad->Draw();
  pad->cd(0);
  gPad->SetGridy();
  hRatio0->GetXaxis()->SetTitle(XTITLE);
  hRatio0->GetYaxis()->SetNdivisions(505);
  hRatio0->GetYaxis()->SetRangeUser(0,2);
  hRatio0->GetYaxis()->SetLabelSize(0.04);
  hRatio0->Draw();
  hRatio1->Draw("same");
}















