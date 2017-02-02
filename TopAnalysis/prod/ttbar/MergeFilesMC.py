#! /usr/bin/env python
import os
from KKousour.TopAnalysis.eostools import *

path = '/store/cmst3/user/kkousour/ttbar/'

sample = [
  "DYJetsToQQ_HT180_13TeV-madgraphMLM-pythia8",
  "WJetsToQQ_HT180_13TeV-madgraphMLM-pythia8",
  "TT_TuneCUETP8M1_13TeV-powheg-pythia8_ext3",
  "ST_t-channel_antitop_4f_inclusiveDecays_13TeV-powhegV2-madspin-pythia8_TuneCUETP8M1",
  "ST_t-channel_top_4f_inclusiveDecays_13TeV-powhegV2-madspin-pythia8_TuneCUETP8M1",
  "ST_tW_antitop_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M1",
  "ST_tW_top_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M1",
  "QCD_HT200to300_TuneCUETP8M1_13TeV-madgraphMLM-pythia8",
  "QCD_HT200to300_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_ext1",
  "QCD_HT300to500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8",
  "QCD_HT300to500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_ext1",
  "QCD_HT500to700_TuneCUETP8M1_13TeV-madgraphMLM-pythia8",
  "QCD_HT500to700_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_ext1", 
  "QCD_HT700to1000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8",
  "QCD_HT700to1000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_ext1", 
  "QCD_HT1000to1500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8",
  "QCD_HT1000to1500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_ext1",
  "QCD_HT1500to2000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8", 
  "QCD_HT1500to2000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_ext1",
  "QCD_HT2000toInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8",
  "QCD_HT2000toInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_ext1",
  "WJetsToLNu_HT-100To200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_ext1",
  "WJetsToLNu_HT-200To400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_ext1",
  "WJetsToLNu_HT-400To600_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_ext1",
  "WJetsToLNu_HT-600To800_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_ext1",
  "WJetsToLNu_HT-800To1200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_ext1",
  "WJetsToLNu_HT-1200To2500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_ext1",
  "WJetsToLNu_HT-2500ToInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_ext1" 
]

for ss in sample:
  ss_core = ss.split('_ext')[0]
  dirs = ls_EOS(path+ss_core+"/crab_"+ss)
  timestamps = []
  if len(dirs) == 0: continue
  for dd in dirs:
    timestamps.append(dd.split('/')[-1])     
  timestamps.sort(reverse=True)
  #--- delete older folders -------------
  print "Removing old directories: "
  print timestamps[1:] 
  for ii in range(1,len(timestamps)):
    location = path+ss_core+"/crab_"+ss+"/"+timestamps[ii]
    command = "/afs/cern.ch/project/eos/installation/cms/bin/eos.select rm -r "+location
    print command
    os.system(command)
  #--- find the folders ------------------------------------
  dirs = ls_EOS(path+ss_core+"/crab_"+ss+"/"+timestamps[0])
  print dirs
  if len(dirs) == 0: continue
  for dd in dirs:
    files = ls_EOS(dd)
    nfiles = len(files)
    ng = nfiles/500 + 1
    print 'Adding ROOT files from location: '+dd
    tag = str(dd.split('/')[-1])
    print 'Found '+str(nfiles)+' files, will be split in '+str(ng)+' groups'
    command = []
    for ig in range(ng):
      command.append('hadd -f ./flatTree_'+ss+'_'+tag+'_'+str(ig)+'.root ')
    for ff in files:
      if (ff.find('.root') > 0):
        nn = int((ff.split('_')[-1]).split('.')[0])
        pfn = lfnToPFN(ff)
        print '\"'+pfn+'\",'
        command[nn/500] += pfn + ' '
    for ig in range(ng):
      os.system(command[ig])




