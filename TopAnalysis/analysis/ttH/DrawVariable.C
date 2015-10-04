void DrawVariable(TString VAR,bool SHAPE,int REBIN,float XMIN,float XMAX,TString XTITLE)
{
  gROOT->ForceStyle();
  const int N = 10;
  TString SAMPLE[N] = {"JetHT","ttHJetTobb_M125","TT","QCD_HT200to300","QCD_HT300to500",
                       "QCD_HT500to700","QCD_HT700to1000","QCD_HT1000to1500","QCD_HT1500to2000","QCD_HT2000toInf"};
  float XSEC[N]     = {1.0,0.577*0.5085,832,1.74e+6,3.67e+5,2.94e+4,6.524e+03,1.064e+03,121.5,2.542e+01};
  float LUMI(16);
  TFile *inf[N];
  TH1F  *h[N];
  int COLOR[N] = {kBlack,kRed-10,kYellow-10,kBlue-10,kBlue-9,kBlue-8,kBlue-7,kBlue-6,kBlue-5,kBlue-4};

  TCanvas *can = new TCanvas("can_"+VAR,"can_"+VAR,900,600);
  can->SetRightMargin(0.15);

  for(int i=0;i<N;i++) {
    inf[i] = TFile::Open("Histo_"+SAMPLE[i]+".root");
    cout<<inf[i]->GetName()<<endl;
    TH1F *hTriggerPass = (TH1F*)inf[i]->Get("hadtop/TriggerPass");
    h[i]   = (TH1F*)inf[i]->Get("hadtop/h_"+VAR);
    h[i]->Rebin(REBIN);
    h[i]->Sumw2();
    h[i]->SetLineWidth(2);
    h[i]->SetLineColor(COLOR[i]);
    h[i]->SetFillColor(COLOR[i]); 
    if (i>0) {
      h[i]->Scale(LUMI*XSEC[i]/hTriggerPass->GetBinContent(1));
    }
  }

  TH1F *hQCD = (TH1F*)h[3]->Clone("hQCD");
  hQCD->Add(h[4]);
  hQCD->Add(h[5]);
  hQCD->Add(h[6]); 
  hQCD->Add(h[7]);
  //hQCD->Add(h[8]);
  //hQCD->Add(h[9]);

  hQCD->SetLineColor(kBlack);
  hQCD->SetFillColor(kBlue-10);
  hQCD->SetLineWidth(1);
  h[2]->SetLineColor(kBlack);
  h[2]->SetFillColor(kYellow-9);
  h[2]->SetLineWidth(1);
  h[1]->SetLineColor(kRed);
  h[1]->SetFillColor(kRed-10);
  h[1]->SetLineWidth(2);

  float kfactor = 1.0;
  if (hQCD->Integral() > 0) { 
    kfactor = (h[0]->Integral()-h[2]->Integral())/hQCD->Integral();
  }  
  hQCD->Scale(kfactor);
  cout<<"Data events:  "<<h[0]->Integral()<<endl;
  cout<<"QCD events:   "<<hQCD->Integral()<<endl;
  cout<<"TTbar events: "<<h[2]->Integral()<<endl;
  cout<<"TTH events:   "<<h[1]->Integral()<<endl;
  cout<<"kfactor:      "<<kfactor<<endl;

  THStack *hs = new THStack("hs","hs");
  //hs->Add(h[1]);
  hs->Add(h[2]);
  /*
  hs->Add(h[4]);
  hs->Add(h[5]);
  hs->Add(h[6]);
  hs->Add(h[7]);
  hs->Add(h[8]);
  hs->Add(h[9]);        
  */
  hs->Add(hQCD);

  TH1F *hBkg = (TH1F*)hQCD->Clone("hBkg");
  hBkg->Scale(kfactor);
  hBkg->Add(h[2]);

  hBkg->SetFillColor(kGray);

  TH1F *hRatio = (TH1F*)h[0]->Clone("Ratio");
  hRatio->SetLineWidth(1);
  hRatio->Divide(hBkg);

  TLegend *leg = new TLegend(0.86,0.65,0.99,0.9);
  leg->SetFillColor(0);
  leg->SetTextFont(42);
  leg->SetTextSize(0.03);
  leg->AddEntry(hQCD,"QCD","F");
  leg->AddEntry(h[2],"TTbar","F");
  leg->AddEntry(h[1],"ttH","F");
  
  if (SHAPE) {
    h[1]->SetFillStyle(3001);
    h[2]->SetFillStyle(3001);
    h[0]->Scale(1./h[0]->Integral());
    h[1]->Scale(1./h[1]->Integral());
    h[2]->Scale(1./h[2]->Integral());
    hQCD->Scale(1./hQCD->Integral());
    hBkg->Scale(1./hBkg->Integral());
    double max = TMath::Max(h[0]->GetBinContent(h[0]->GetMaximumBin()),hBkg->GetBinContent(hBkg->GetMaximumBin()));
    hBkg->SetMaximum(1.1*max);
    hBkg->GetXaxis()->SetTitle(XTITLE);
    hBkg->GetXaxis()->SetRangeUser(XMIN,XMAX);
    hBkg->Draw("hist");
    h[0]->Draw("same E"); 
    //h[1]->Draw("same hist"); 
    leg->Draw();
    gPad->RedrawAxis();
    can->Print("can_"+VAR+"_norm.pdf"); 
  }
  else {
    can->SetBottomMargin(0.25);
    //gPad->SetLogy(); 
    h[1]->Scale(h[0]->Integral()/h[1]->Integral());
    h[1]->SetFillColor(0);
    TH1F *hAux = (TH1F*)h[0]->Clone("aux");
    hAux->Reset();
    hAux->GetYaxis()->SetRangeUser(0.5,1.1*TMath::Max(h[1]->GetBinContent(h[1]->GetMaximumBin()),h[0]->GetBinContent(h[0]->GetMaximumBin()))); 
    
    hAux->GetXaxis()->SetRangeUser(XMIN,XMAX);
    hAux->GetYaxis()->SetTitle(TString::Format("Number of events / %d fb^{-1}",int(LUMI)/1000));
    hAux->GetXaxis()->SetTitle("");
    hAux->GetXaxis()->SetLabelSize(0.0);
    hAux->Draw();
    hs->Draw("hist same"); 
    h[0]->Draw("same E");
    h[1]->Draw("same hist");
    leg->Draw();
    gPad->RedrawAxis();

    TPad *pad = new TPad("pad","pad",0.,0.,1.,1.);
    pad->SetTopMargin(0.77);
    pad->SetRightMargin(0.15);
    pad->SetFillColor(0);
    pad->SetFillStyle(0);
    pad->Draw();
    pad->cd(0);
    pad->SetGridy();
    hRatio->GetXaxis()->SetTitleOffset(0.95);
    hRatio->GetYaxis()->SetTitleOffset(1.5);
    hRatio->GetYaxis()->SetTickLength(0.06);
    hRatio->GetYaxis()->SetTitleSize(0.03);
    hRatio->GetYaxis()->SetLabelSize(0.03);
    hRatio->GetYaxis()->SetTitle("Data/MC");
    hRatio->GetXaxis()->SetTitle(XTITLE);
    hRatio->GetXaxis()->SetRangeUser(XMIN,XMAX);
    hRatio->GetYaxis()->SetRangeUser(0.5,1.5);
    hRatio->GetYaxis()->SetNdivisions(505);
    hRatio->Draw();

    can->Print("can_"+VAR+"_abs.pdf"); 
  }
}















