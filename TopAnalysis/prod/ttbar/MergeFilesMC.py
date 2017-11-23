#! /usr/bin/env python
import os
from KKousour.TopAnalysis.eostools import *

path = '/store/cmst3/user/kkousour/ttbar/'

sample = [

  #--- ttH ---------
  "ttHJetTobb_M125_13TeV_amcatnloFXFX_madspin_pythia8_ext3",
  #"ttHJetToNonbb_M125_13TeV_amcatnloFXFX_madspin_pythia8_mWCutfix_ext1",
  #"TTZToQQ_TuneCUETP8M1_13TeV-amcatnlo-pythia8",
  #"TTWJetsToQQ_TuneCUETP8M1_13TeV-amcatnloFXFX-madspin-pythia8",
  #"/WZ_TuneCUETP8M1_13TeV-pythia8/RunIISummer16MiniAODv2-PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v1/MINIAODSIM",
  #"WWTo4Q_4f_13TeV_amcatnloFXFX_madspin_pythia8",
  #"ZZTo4Q_13TeV_amcatnloFXFX_madspin_pythia8",
 # "DYJetsToQQ_HT180_13TeV-madgraphMLM-pythia8",
 # "WJetsToQQ_HT180_13TeV-madgraphMLM-pythia8",
 # "TT_TuneEE5C_13TeV-powheg-herwigpp",
 # "TT_TuneCUETP8M2T4_13TeV-powheg-pythia8",
 # "TT_TuneCUETP8M2T4down_13TeV-powheg-pythia8",
 # "TT_TuneCUETP8M2T4up_13TeV-powheg-pythia8",
 # "TT_TuneCUETP8M2T4_13TeV-powheg-fsrdown-pythia8",
 # "TT_TuneCUETP8M2T4_13TeV-powheg-fsrup-pythia8",
 # "TT_TuneCUETP8M2T4_13TeV-powheg-isrdown-pythia8",
 # "TT_TuneCUETP8M2T4_13TeV-powheg-isrup-pythia8_ext1",
 # "TT_TuneCUETP8M2T4_mtop1665_13TeV-powheg-pythia8",
 # "TT_TuneCUETP8M2T4_mtop1695_13TeV-powheg-pythia8",
 # "TT_TuneCUETP8M2T4_mtop1715_13TeV-powheg-pythia8",
 # "TT_TuneCUETP8M2T4_mtop1735_13TeV-powheg-pythia8",
 # "TT_TuneCUETP8M2T4_mtop1755_13TeV-powheg-pythia8_ext1",
 # "TT_TuneCUETP8M2T4_mtop1785_13TeV-powheg-pythia8",
 # "TTJets_TuneCUETP8M2T4_13TeV-amcatnloFXFX-pythia8",
 # "ST_t-channel_antitop_4f_inclusiveDecays_13TeV-powhegV2-madspin-pythia8_TuneCUETP8M1",
 # "ST_t-channel_top_4f_inclusiveDecays_13TeV-powhegV2-madspin-pythia8_TuneCUETP8M1",
 # "ST_tW_antitop_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M1_ext1",
 # "ST_tW_top_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M1_ext1",
 # "QCD_HT200to300_TuneCUETP8M1_13TeV-madgraphMLM-pythia8",
 # "QCD_HT200to300_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_ext1",
 # "QCD_HT300to500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8",
 # "QCD_HT300to500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_ext1",
 # "QCD_HT500to700_TuneCUETP8M1_13TeV-madgraphMLM-pythia8",
 # "QCD_HT500to700_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_ext1", 
 # "QCD_HT700to1000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8",
 # "QCD_HT700to1000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_ext1", 
 # "QCD_HT1000to1500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8",
 # "QCD_HT1000to1500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_ext1",
 # "QCD_HT1500to2000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8", 
 # "QCD_HT1500to2000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_ext1",
 # "QCD_HT2000toInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8",
 # "QCD_HT2000toInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_ext1"
  #"WJetsToLNu_HT-100To200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_ext1",
  #"WJetsToLNu_HT-200To400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_ext1",
  #"WJetsToLNu_HT-400To600_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_ext1",
  #"WJetsToLNu_HT-600To800_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_ext1",
  #"WJetsToLNu_HT-800To1200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_ext1",
  #"WJetsToLNu_HT-1200To2500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_ext1",
  #"WJetsToLNu_HT-2500ToInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8_ext1" 
]
total_size = 0
for ss in sample:
  print "====== "+ss+" ==============="
  ss_core = ss.split('_ext')[0]
  #command = "/afs/cern.ch/project/eos/installation/cms/bin/eos.select ls "+path+ss_core+"/crab_"+ss
  #result = os.system(command)
  #dirs = list(result.split('\n'))
  #print dirs
  #print "hello"
  dirs = ls_EOS(path+ss_core+"/crab_"+ss)
  print "size = "+str(eosDirSize(path+ss_core+"/crab_"+ss))
  total_size += eosDirSize(path+ss_core+"/crab_"+ss)
  timestamps = []
  if len(dirs) == 0: continue
  for dd in dirs:
    timestamps.append(dd.split('/')[-1])     
  timestamps.sort(reverse=True)
  #print timestamps
  #--- delete older folders -------------
  if len(timestamps)>1:
    #print "Removing old directories: "
    #print timestamps[1:]
    for ii in range(1,len(timestamps)):
      location = path+ss_core+"/crab_"+ss+"/"+timestamps[ii]
      command = "/afs/cern.ch/project/eos/installation/cms/bin/eos.select rm -r "+location
      #print command
      os.system(command)
  #--- find the folders ------------------------------------
  #command = "/afs/cern.ch/project/eos/installation/cms/bin/eos.select ls "+path+ss_core+"/crab_"+ss+"/"+timestamps[0]
  #dirs = os.system(command)
  dirs = ls_EOS(path+ss_core+"/crab_"+ss+"/"+timestamps[0])
  print dirs
  if len(dirs) == 0: continue
  for dd in dirs:
    files = ls_EOS(dd)
    nfiles = len(files)
    ng = nfiles/500 + 1
    print 'Adding ROOT files from location: '+dd
    tag = str(dd.split('/')[-1])
    nn_min = int(tag)*1000
    print 'Found '+str(nfiles)+' files, will be split in '+str(ng)+' groups'
    command = []
    for ig in range(ng):
      command.append('hadd -f ./flatTree_'+ss+'_'+tag+'_'+str(ig)+'.root ')
    for ff in files:
      if (ff.find('.root') > 0):
        nn = int((ff.split('_')[-1]).split('.')[0])
        pfn = lfnToPFN(ff)
        #print '\"'+pfn+'\",'
        command[(nn-nn_min)/500] += pfn + ' '
    for ig in range(ng):
      os.system(command[ig])


print "Total size = "+str(total_size)+" GB"

