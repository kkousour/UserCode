void DrawBoosted(bool SHAPE)
{
  const int N = 12;
  const float LUMI = 1000;
  float XSEC[N] = {832,471100.,117276.,7823.,648.2,186.9,32.293,9.4183,0.84265,0.114943,0.00682981,0.000165445};

  TString SAMPLE[N] = {
    "TT",
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
  TTree *tr[N];
  TH1F  *h[N];
  TH1F  *hQCD;
  TCut SEL("triggerBit[1] && jetPt>450 && jetTau3/jetTau2<0.7 && jetNBSub>0");
  //TString VAR = "TMath::Max(jetMassSub0[0],jetMassSub1[0])";
  TString VAR = "jetMassSoftDrop"; 
  //TString VAR = "jetTau3[0]/jetTau1[0]";
  int NBINS = 50;
  float XMIN(0),XMAX(300);

  TCanvas *can = new TCanvas("Boosted","Boosted",900,600);
  for(int k=0;k<N;k++) {
    inf[k]      = TFile::Open("flatTree_"+SAMPLE[k]+".root");
    tr[k]       = (TTree*)inf[k]->Get("hadtopBoost/events");
    h[k]        = new TH1F("h_"+SAMPLE[k],"h_"+SAMPLE[k],NBINS,XMIN,XMAX);
    h[k]->Sumw2();
    tr[k]->Draw(VAR+">>h_"+SAMPLE[k],SEL);
    h[k]->Scale(LUMI*XSEC[k]/((TH1F*)inf[k]->Get("hadtopBoost/CutFlow"))->GetBinContent(1)); 
    cout<<SAMPLE[k]<<": "<<h[k]->GetEntries()<<" "<<h[k]->Integral()<<endl;
    if (k == 1) {
      hQCD = (TH1F*)h[k]->Clone();
    }
    if (k > 1) {
      hQCD->Add(h[k]);
    }
  } 
  if (SHAPE) {
    hQCD->Scale(1/hQCD->Integral());
    h[0]->Scale(1/h[0]->Integral());
  }
  double max1 = TMath::Max(hQCD->GetBinContent(hQCD->GetMaximumBin()),h[0]->GetBinContent(h[0]->GetMaximumBin()));
  hQCD->SetMaximum(1.2*max1);
  hQCD->Draw("HIST");
  h[0]->Draw("sameE");
}




















