#! /usr/bin/env python
import os
from KKousour.TopAnalysis.eostools import *

path = '/store/cmst3/user/kkousour/ttbar/'
 
sample = [
  '/JetHT/Run2015B-PromptReco-v1/'
]

for ss in sample:
  dirs = ls_EOS(path+"/JetHT/crab_JetHT/")
  timestamps = []
  if len(dirs) == 0: continue
  for dd in dirs:
    timestamps.append(dd.split('/')[-1])     
  timestamps.sort(reverse=True)
  #--- delete older folders -------------
  print "Removing old directories: "
  print timestamps[1:] 
  for ii in range(1,len(timestamps)):
    location = path+"JetHT/crab_JetHT/"+timestamps[ii]
    command = "cmsRm -r "+location
    print command
    os.system(command)
  #--- merge ROOT files from the newest folder ------------- 
  location = path+"/JetHT/crab_JetHT/"+timestamps[0]+"/0000/"
  print 'Adding ROOT files from location: '+location
  files = ls_EOS(location)
  command = 'hadd -f ./flatTree_JetHT.root '
  for ff in files:
    if (ff.find('.root') > 0):
      nn = int((ff.split('_')[-1]).split('.')[0])
      pfn = lfnToPFN(ff)
      print '\"'+pfn+'\",'
      command += pfn + ' '
  os.system(command)
