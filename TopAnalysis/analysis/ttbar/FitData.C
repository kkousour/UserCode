void FitData(int REBIN)
{
  gROOT->ForceStyle();
  
  RooMsgService::instance().setSilentMode(kTRUE);
  RooMsgService::instance().setStreamStatus(0,kFALSE);
  RooMsgService::instance().setStreamStatus(1,kFALSE);
  
  TFile *inf = TFile::Open("Histo_JetHT.root");
  TH1F *h = (TH1F*)inf->Get("hadtopBoost/h_jetMassSoftDrop0_Cut_sig");
  h->Rebin(REBIN);
  RooRealVar *nData = new RooRealVar("nData","nData",h->Integral());
  // -----------------------------------------
      
  TFile *fTemplates = TFile::Open("templates_boosted_workspace.root");
  RooWorkspace *wTemplates = (RooWorkspace*)fTemplates->Get("w");
  RooRealVar *x   = (RooRealVar*)wTemplates->var("mTop");
  RooWorkspace *wData = new RooWorkspace("w","workspace");

  RooDataHist *roohist_data = new RooDataHist("roohist_data","roohist_data",RooArgList(*x),h);
  
  wData->import(*roohist_data);
  wData->import(*nData);
  wData->writeToFile("data_boosted_workspace.root");

  RooPlot *frame = x->frame();
  
  roohist_data->plotOn(frame);
      
  RooAbsPdf *pdf_signal = (RooAbsPdf*)wTemplates->pdf("ttbar_pdf");
  RooAbsPdf *pdf_bkg    = (RooAbsPdf*)wTemplates->pdf("qcd_pdf");
       
  RooRealVar *nFitSig = new RooRealVar("nFitSig","nFitSig",2000,0,10000);
  RooRealVar *nFitBkg = new RooRealVar("nFitBkg","nFitBkg",2000,0,10000);
        
  RooAddPdf *model = new RooAddPdf("model","model",RooArgList(*pdf_signal,*pdf_bkg),RooArgList(*nFitSig,*nFitBkg)); 
  RooFitResult *res = model->fitTo(*roohist_data,RooFit::Save(),RooFit::Extended(kTRUE));
  res->Print();
  cout<<"Signal = "<<nFitSig->getVal()<<" +/- "<<nFitSig->getError()<<endl;
  cout<<"Bkg    = "<<nFitBkg->getVal()<<" +/- "<<nFitBkg->getError()<<endl;

  //float NSIG = nFitSig->getVal();
  //float NBKG = nFitBkg->getVal();
  //float ESIG = nFitSig->getError();
  //float EBKG = nFitBkg->getError();
        
  model->plotOn(frame);

  RooHist *hpull = frame->pullHist();
  model->plotOn(frame,RooFit::Components("qcd_pdf"),RooFit::LineColor(kGreen+1),RooFit::LineWidth(2),RooFit::LineStyle(kDashed));
  model->plotOn(frame,RooFit::Components("signal_pdf"),RooFit::LineColor(kRed),RooFit::LineWidth(2),RooFit::LineStyle(1));

  TCanvas *can = new TCanvas(TString::Format("Fitted_mTop"),TString::Format("Fitted_mTop"),900,600);
  can->cd(1)->SetBottomMargin(0.3);
  frame->SetMinimum(0.1);
  frame->GetYaxis()->SetNdivisions(505);
  frame->GetXaxis()->SetTitle("");
  frame->GetXaxis()->SetLabelSize(0.0);
  frame->Draw(); 

  RooHist  *hist_data = (RooHist*)frame->findObject("h_roohist_data",RooHist::Class());
  RooCurve *curve_all = (RooCurve*)frame->findObject("model_Norm[mTop]",RooCurve::Class());
  RooCurve *curve_sig = (RooCurve*)frame->findObject("model_Norm[mTop]_Comp[signal_pdf]",RooCurve::Class());
  RooCurve *curve_bkg = (RooCurve*)frame->findObject("model_Norm[mTop]_Comp[qcd_pdf]",RooCurve::Class());

  //---- compute G --------------------------- 
  if (curve_all) {
    double xl,xh,Exp,Obs,G(0.0);
    int nbins(0);
    for(int i=0;i<h->GetNbinsX();i++) {
      xl  = h->GetBinLowEdge(i+1);
      xh  = h->GetBinLowEdge(i+1)+h->GetBinWidth(i+1);
      Obs = h->GetBinContent(i+1);
      if (xl < x->getMin() || xh > x->getMax()) continue;
      nbins++;
      Exp = curve_all->average(xl,xh);
      //cout<<i<<" "<<xl<<" "<<xh<<" "<<Obs<<" "<<Exp<<endl;
      if (Obs > 0 && Exp > 0) {
        G += 2*Obs*log(Obs/Exp);
      }
    }
    cout<<"chi2/ndf = "<<frame->chiSquare(2)<<", G = "<<G/(nbins-2)<<", Prob = "<<TMath::Prob(G,nbins-2)<<endl;
  }

  TLegend *leg = new TLegend(0.75,0.7,0.92,0.9);
  leg->AddEntry(hist_data,"data","P");
  leg->AddEntry(curve_all,"signal + qcd","L");
  leg->AddEntry(curve_sig,"signal","L");
  leg->AddEntry(curve_bkg,"qcd","L");
  leg->SetFillColor(0);
  leg->SetTextFont(42);
  leg->SetTextSize(0.04);
  leg->Draw();
        
  RooPlot *frame2 = x->frame();
  frame2->addPlotable(hpull,"p");
  
  TPad *pad = new TPad("pad","pad",0.,0.,1.,1.);
  pad->SetTopMargin(0.7);
  //pad->SetGridy();
  pad->SetFillColor(0);
  pad->SetFillStyle(0);
  pad->Draw();
  pad->cd(0);
  pad->SetGridy();
  frame2->SetMinimum(-5);
  frame2->SetMaximum(5);
  frame2->GetYaxis()->SetNdivisions(505);
  frame2->GetXaxis()->SetTitleOffset(0.9);
  frame2->GetYaxis()->SetTitleOffset(0.8);
  frame2->GetYaxis()->SetTickLength(0.06);
  frame2->GetYaxis()->SetTitleSize(0.05);
  frame2->GetYaxis()->SetTitleSize(0.03);
  frame2->GetYaxis()->SetLabelSize(0.03);
  frame2->GetYaxis()->SetTitle("(Data-Fit)/Error");
  frame2->GetXaxis()->SetTitle("Softdrop mass (GeV)");
  frame2->Draw();   
}
