#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include "KKousour/TopAnalysis/plugins/ApplyJSON.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"

using namespace std;

ApplyJSON::ApplyJSON(edm::ParameterSet const& cfg) 
{
  jsonFile_ = cfg.getParameter<std::string> ("jsonFile");
  parse();
}
//////////////////////////////////////////////////////////////////////////////////////////
void ApplyJSON::parse()
{
  ifstream inf(jsonFile_);
  stringstream buffer;
  
  if (inf.is_open()) {
    buffer << inf.rdbuf();
    string test = buffer.str(); 
    int pos1(0),pos2(0),block_pos1(0),block_pos2(0);
    while(pos1<int(test.length())) {
      pos1 = test.find("\"",pos2+1);
      pos2 = test.find("\"",pos1+1);
      if (pos1<0 || pos2<0) break; 
      int run = atoi((test.substr(pos1+1,(pos2-pos1-1))).c_str());
      //cout<<"======================================"<<endl;
      //cout<<"run: "<<run<<endl;
      bool end_block(false);
      block_pos2 = pos2;
      vector<pair<int,int> > vlumi; 
      while(!end_block) {
        block_pos1 = test.find("[",block_pos2+1);
        if (test[block_pos1+1] == '[') {
          block_pos1++;
        }
        block_pos2 = test.find("]",block_pos1+1);
        int lumi_pos1 = block_pos1+1;
        int comma = test.find(",",lumi_pos1);
        int lumi_pos2 = block_pos2-1;
        int lumi1 = atoi((test.substr(lumi_pos1,comma-lumi_pos1)).c_str());
        int lumi2 = atoi((test.substr(comma+1,lumi_pos2-comma)).c_str());
        pair<int,int> lumi_pair;
        lumi_pair.first  = lumi1;
        lumi_pair.second = lumi2;
        vlumi.push_back(lumi_pair);
        //int lumi2 = test[block_pos1+4];
        //cout<<"lumi1 = "<<lumi1<<" lumi2 = "<<lumi2<<endl;
        if (test[block_pos2+1] == ']') {
          block_pos2++;
          end_block = true;
        }
      }
      pair<int,vector<pair<int,int> > > entry;
      entry.first = run;
      entry.second = vlumi;
      vrun_.push_back(entry);
    }
  }
  for(unsigned i=0;i<vrun_.size();i++) {
    cout<<vrun_[i].first<<": ";
    for(unsigned j=0;j<(vrun_[i].second).size();j++) {
      cout<<"["<<(vrun_[i].second)[j].first<<", "<<(vrun_[i].second)[j].second<<"]";
      if (j < (vrun_[i].second).size()-1) cout<<",";
      else cout<<endl; 
    }
  }
  inf.close();
}
//////////////////////////////////////////////////////////////////////////////////////////
bool ApplyJSON::filter(edm::Event & iEvent, edm::EventSetup const& iSetup) 
{
  bool result(false);
          
  int run  = iEvent.id().run();
  int lumi = iEvent.id().luminosityBlock();

  if (!vrun_.empty()) {
    for(unsigned i=0;i<vrun_.size();i++) {
      if (run == vrun_[i].first) {
        for(unsigned j=0;j<(vrun_[i].second).size();j++) {
          if (lumi >= (vrun_[i].second)[j].first && lumi <= (vrun_[i].second)[j].second) {
            result = true;
            continue; 
          }
        } 
        continue;
      }
    }
  }
  //cout<<run<<" "<<lumi<<" "<<result<<endl;
  return result;
}
//////////////////////////////////////////////////////////////////////////////////////////
ApplyJSON::~ApplyJSON() 
{
}

DEFINE_FWK_MODULE(ApplyJSON);
















