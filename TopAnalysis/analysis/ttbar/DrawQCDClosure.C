void DrawQCDClosure(TString VAR,int NBINS,float XMIN,float XMAX,TString XTITLE)
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
  TTree *tr1[N],*tr2[N];
  TH1F  *h1[N],*h2[N];

  TCanvas *can = new TCanvas("can_QCDClosure_"+VAR,"can_QCDClosure_"+VAR,900,600);
  can->cd(1);
  can->SetBottomMargin(0.3);
  can->SetRightMargin(0.15);

  for(int i=0;i<N;i++) {
    inf[i] = TFile::Open("../../prod/ttbar/flatTree_"+SAMPLE[i]+".root");
    tr1[i] = (TTree*)inf[i]->Get("hadtopOneBtag/events");
    tr2[i] = (TTree*)inf[i]->Get("hadtop/events");
    TH1F *hpu = (TH1F*)inf[i]->Get("hadtop/pileup");
    h1[i]  = new TH1F("h1_"+SAMPLE[i],"h1_"+SAMPLE[i],NBINS,XMIN,XMAX);
    h2[i]  = new TH1F("h2_"+SAMPLE[i],"h2_"+SAMPLE[i],NBINS,XMIN,XMAX);
    h1[i]->Sumw2();
    h2[i]->Sumw2();
    tr1[i]->Draw(VAR+">>"+"h1_"+SAMPLE[i],"ht> 450 && jetPt[5]>40 && prob>0.05 && nBJets==1 && dRbbTop>1.0 && ptTop[0]>0");
    tr2[i]->Draw(VAR+">>"+"h2_"+SAMPLE[i],"ht> 450 && jetPt[5]>40 && prob>0.05 && nBJets>1 && dRbbTop>1.0 && ptTop[0]>0");
    h1[i]->Scale(XSEC[i]/hpu->GetEntries());
    h2[i]->Scale(XSEC[i]/hpu->GetEntries()); 
    cout<<SAMPLE[i]<<" "<<hpu->GetEntries()<<" "<<h1[i]->GetEntries()<<" "<<h2[i]->GetEntries()<<endl;
  }

  TH1F *hQCD1 = (TH1F*)h1[0]->Clone("hQCD1");
  TH1F *hQCD2 = (TH1F*)h2[0]->Clone("hQCD2");

  for(int i=1;i<N;i++) {
    hQCD1->Add(h1[i]);
    hQCD2->Add(h2[i]);
  }  
   
  hQCD1->SetLineColor(kBlack);
  hQCD1->SetMarkerColor(kBlack);
  hQCD1->SetMarkerStyle(21);
  hQCD2->SetLineColor(kRed);
  hQCD2->SetMarkerColor(kRed);
  hQCD2->SetMarkerStyle(20);

  hQCD1->Scale(1./hQCD1->Integral());
  hQCD2->Scale(1./hQCD2->Integral());

  hQCD1->GetXaxis()->SetLabelSize(0.0);
  double max = 1.1*TMath::Max(hQCD1->GetBinContent(hQCD1->GetMaximumBin()),hQCD2->GetBinContent(hQCD2->GetMaximumBin()));
  hQCD1->SetMinimum(1e-5);
  hQCD1->SetMaximum(max);
  hQCD1->Draw("hist");
  hQCD2->Draw("sameE");
  gPad->RedrawAxis();

  TLegend *leg = new TLegend(0.86,0.65,0.99,0.9);
  leg->SetFillColor(0);
  leg->SetTextFont(42);
  leg->SetTextSize(0.03);
  leg->AddEntry(hQCD1,"One btag","LP");
  leg->AddEntry(hQCD2,"Two btag","LP");
  leg->Draw();

  TH1F *hRatio1 = (TH1F*)hQCD2->Clone("Ratio1");
  hRatio1->Divide(hQCD1);

  hRatio1->SetLineColor(kBlack);
  hRatio1->SetMarkerColor(kBlack);

  TPad* pad = new TPad("pad", "pad", 0., 0., 1., 1.);
  pad->SetTopMargin(0.7);
  pad->SetRightMargin(0.15);
  pad->SetFillColor(0);
  pad->SetFillStyle(0);
  pad->Draw();
  pad->cd(0);
  gPad->SetGridy();
  hRatio1->GetXaxis()->SetTitle(XTITLE);
  hRatio1->GetYaxis()->SetNdivisions(505);
  hRatio1->GetYaxis()->SetRangeUser(0,2);
  hRatio1->GetYaxis()->SetLabelSize(0.04);
  hRatio1->Draw();
}















