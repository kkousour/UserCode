#! /usr/bin/env python
import os
from KKousour.TopAnalysis.eostools import *

path = '/store/cmst3/user/kkousour/ttbar/'
 
era_list = ['Run2015Dv4']

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
    command = "eos rm -r "+location
    print command
    os.system(command)
  #--- merge ROOT files from the newest folder ------------- 
  #command = 'hadd -f ./flatTree_JetHT_'+era+'.root '
  #--- find the folders ------------------------------------
  dirs = ls_EOS(path+"/JetHT/crab_JetHT_"+era+"/"+timestamps[0])
  print dirs
  if len(dirs) == 0: continue
  k = 0
  for dd in dirs:
    print 'Adding ROOT files from location: '+dd
    files = ls_EOS(dd)
    command = 'hadd -f ./flatTree_JetHT_'+era+'_'+str(k)+'.root '
    for ff in files:
      if (ff.find('.root') > 0):
        nn = int((ff.split('_')[-1]).split('.')[0])
        pfn = lfnToPFN(ff)
        print '\"'+pfn+'\",'
        command += pfn + ' '
    os.system(command)
    k += 1
