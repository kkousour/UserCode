from CRABClient.UserUtilities import config, getUsernameFromSiteDB

config = config()
config.General.requestName = 'JetHT_Run2015C'
config.General.transferOutputs = True
config.General.transferLogs = False
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'flat-data-cfg.py'
config.JobType.maxJobRuntimeMin = 2800
#config.JobType.inputFiles = ['Summer15_50nsV2_DATA.db']
config.Data.inputDataset = '/JetHT/Run2015C-PromptReco-v1/MINIAOD'
config.Data.ignoreLocality = True
config.Data.inputDBS = 'global'
config.Data.splitting = 'LumiBased'
config.Data.lumiMask = 'Cert_246908-256869_13TeV_PromptReco_Collisions15_25ns_JSON.txt'
config.Data.unitsPerJob = 10
config.Data.outLFNDirBase = '/store/group/cmst3/user/kkousour/ttH/'
config.Data.publication = False
config.Site.storageSite = 'T2_CH_CERN'
