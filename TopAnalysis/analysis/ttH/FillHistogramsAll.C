#include "FillHistograms.C"

void FillHistogramsAll()
{
  FillHistograms("JetHT");
  FillHistograms("TT");
  FillHistograms("ttHJetTobb_M125");
  FillHistograms("QCD_HT200to300");
  FillHistograms("QCD_HT300to500");
  FillHistograms("QCD_HT500to700");
  FillHistograms("QCD_HT700to1000");
  FillHistograms("QCD_HT1000to1500");
  FillHistograms("QCD_HT1500to2000");
  FillHistograms("QCD_HT2000toInf");
}
