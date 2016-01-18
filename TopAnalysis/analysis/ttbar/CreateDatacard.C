void CreateDatacard()
{
  const int NF = 6;
  TFile *fData = TFile::Open("data_boosted_workspace.root");
  TFile *fTemp = TFile::Open("templates_boosted_workspace.root");

  RooWorkspace *wData = (RooWorkspace*)fData->Get("w");
  RooWorkspace *wTemp = (RooWorkspace*)fTemp->Get("w");

  char name[1000];
  
  double nData = ((RooRealVar*)wData->var("nData"))->getValV();
  double nTop  = ((RooRealVar*)wTemp->var("YieldTT"))->getValV();
   
  ofstream datacard;
  sprintf(name,"datacard_boosted.txt");
  cout<<"======================================="<<endl; 
  cout<<"Creating datacard: "<<name<<endl;
  cout<<"======================================="<<endl; 
  datacard.open(name);
  datacard.setf(ios::right);
  datacard<<"imax 1"<<"\n";
  datacard<<"jmax 1"<<"\n";
  datacard<<"kmax *"<<"\n";
  datacard<<"----------------"<<"\n";
  datacard<<"shapes data_obs * "<<fData->GetName()<<" w:roohist_data"<<"\n";
  datacard<<"shapes qcd      * "<<fTemp->GetName()<<" w:qcd_pdf"<<"\n";
  datacard<<"shapes ttbar    * "<<fTemp->GetName()<<" w:ttbar_pdf"<<"\n";
  datacard<<"----------------"<<"\n";
  datacard<<"bin           1"<<"\n";
  datacard<<"observation  -1"<<"\n";
  datacard<<"----------------"<<"\n";
  datacard<<"bin           1      1"<<"\n";
  datacard<<"process       ttbar  qcd"<<"\n";
  datacard<<"process       0      1"<<"\n";
  datacard<<"rate          "<<nTop<<"   "<<nData<<"\n";
  datacard<<"----------------"<<"\n";
  datacard<<"lumi          lnN  1.10  -"<<"\n";
  datacard<<"qcd_norm      lnU  -     1.50"<<"\n";
  datacard<<"\n";
  datacard<<"#--- ttbar shape parameters ------ \n";
  datacard<<"\n";
  datacard<<"kJES          param 1.0 0.05 \n";
  datacard<<"kJER          param 1.0 0.2 \n";
  datacard.close();
  
  fData->Close();
  fTemp->Close();
}
