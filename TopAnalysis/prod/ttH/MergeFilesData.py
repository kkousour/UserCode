#! /usr/bin/env python
import os
from KKousour.TopAnalysis.eostools import *

path = '/store/cmst3/user/kkousour/ttH/'
 
era_list = [
  "Run2016B-23Sep2016-v3",
  "Run2016C-23Sep2016-v1",
  "Run2016D-23Sep2016-v1",
  "Run2016E-23Sep2016-v1",
  "Run2016F-23Sep2016-v1",
  "Run2016G-23Sep2016-v1",
  "Run2016H-PromptReco-v1",
  "Run2016H-PromptReco-v2",
  "Run2016H-PromptReco-v3"
]

for era in era_list:
  dirs = ls_EOS(path+"/JetHT/crab_JetHT_"+era+"/")
  timestamps = []
  if len(dirs) == 0: continue
  for dd in dirs:
    timestamps.append(dd.split('/')[-1])     
  timestamps.sort(reverse=True)
  #--- delete older folders -------------
  print "Removing old directories: "
  print timestamps[1:] 
  for ii in range(1,len(timestamps)):
    location = path+"JetHT/crab_JetHT_"+era+"/"+timestamps[ii]
    command = "/afs/cern.ch/project/eos/installation/cms/bin/eos.select rm -r "+location
    print command
    os.system(command)
  #--- find the folders ------------------------------------
  dirs = ls_EOS(path+"/JetHT/crab_JetHT_"+era+"/"+timestamps[0])
  print dirs
  if len(dirs) == 0: continue
  for dd in dirs:
    files = ls_EOS(dd)
    nfiles = len(files)
    print 'Adding '+str(nfiles)+' ROOT files from location: '+dd
    ss = str(dd.split('/')[-1])
    command1 = 'hadd -f ./flatTree_JetHT_'+era+'_'+ss+'_1.root '
    command2 = 'hadd -f ./flatTree_JetHT_'+era+'_'+ss+'_2.root '
    k = 0
    for ff in files:
      if (ff.find('.root') > 0):
        #nn = int((ff.split('_')[-1]).split('.')[0])
        pfn = lfnToPFN(ff)
        print '\"'+pfn+'\",'
        if (k < 500):
          command1 += pfn + ' '
        else:
          command2 += pfn + ' '
        k += 1
    if (k > 0):
      os.system(command1)
    if (k >= 500):
      os.system(command2)




