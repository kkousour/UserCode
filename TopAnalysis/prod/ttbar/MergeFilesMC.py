#! /usr/bin/env python
import os
from KKousour.TopAnalysis.eostools import *

path = '/store/cmst3/user/kkousour/ttbar/'
 
sample = [
  "/WJetsToQQ_HT-600ToInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8"
  #"/TT_TuneCUETP8M1_13TeV-powheg-pythia8",
  #"/QCD_HT200to300_TuneCUETP8M1_13TeV-madgraphMLM-pythia8",
  #"/QCD_HT300to500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8",
  #"/QCD_HT500to700_TuneCUETP8M1_13TeV-madgraphMLM-pythia8",
  #"/QCD_HT700to1000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8",
  #"/QCD_HT1000to1500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8",
  #"/QCD_HT1500to2000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8",
  #"/QCD_HT2000toInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8" 
]

for ss in sample:
  tag = (ss.split("_TuneCUETP8M1_")[0]).replace("/","")
  print tag
  dirs = ls_EOS(path+ss+"/crab_"+tag)
  timestamps = []
  if len(dirs) == 0: continue
  for dd in dirs:
    timestamps.append(dd.split('/')[-1])     
  timestamps.sort(reverse=True)
  #--- delete older folders -------------
  print "Removing old directories: "
  print timestamps[1:] 
  for ii in range(1,len(timestamps)):
    location = path+ss+"/crab_"+tag+"/"+timestamps[ii]
    command = "cmsRm -r "+location
    print command
    os.system(command)
  #--- merge ROOT files from the newest folder ------------- 
  location = path+ss+"/crab_"+tag+"/"+timestamps[0]+"/0000/"
  print 'Adding ROOT files from location: '+location
  files = ls_EOS(location)
  command = 'hadd -f ./flatTree_'+tag+'.root '
  for ff in files:
    if (ff.find('.root') > 0):
      nn = int((ff.split('_')[-1]).split('.')[0])
      pfn = lfnToPFN(ff)
      print '\"'+pfn+'\",'
      command += pfn + ' '
  os.system(command)
