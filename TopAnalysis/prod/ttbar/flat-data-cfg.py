import FWCore.ParameterSet.Config as cms 
process = cms.Process('myprocess')
process.TFileService=cms.Service("TFileService",fileName=cms.string('flatTree.root'))
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
process.GlobalTag.globaltag = '74X_dataRun2_v2'
##-------------------- Define the source  ----------------------------
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))
process.source = cms.Source("PoolSource",
  fileNames = cms.untracked.vstring(
   "/store/data/Run2015C/JetHT/MINIAOD/PromptReco-v1/000/254/790/00000/06947E9F-204A-E511-B627-02163E0137BA.root",
   "/store/data/Run2015C/JetHT/MINIAOD/PromptReco-v1/000/254/790/00000/18D18896-204A-E511-B82D-02163E01190D.root",
   "/store/data/Run2015C/JetHT/MINIAOD/PromptReco-v1/000/254/790/00000/260A1195-204A-E511-8627-02163E014125.root",
   "/store/data/Run2015C/JetHT/MINIAOD/PromptReco-v1/000/254/790/00000/6A03E597-204A-E511-B2EA-02163E01418B.root",
   "/store/data/Run2015C/JetHT/MINIAOD/PromptReco-v1/000/254/790/00000/7086989C-204A-E511-B943-02163E013409.root",
   "/store/data/Run2015C/JetHT/MINIAOD/PromptReco-v1/000/254/790/00000/7CA25D9B-204A-E511-BF7A-02163E011955.root",
   "/store/data/Run2015C/JetHT/MINIAOD/PromptReco-v1/000/254/790/00000/823FA0A1-204A-E511-887F-02163E01453E.root"
    )
)
#############   Format MessageLogger #################
process.load('FWCore.MessageService.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

#############   JEC #################
#process.load("CondCore.DBCommon.CondDBCommon_cfi")
#from CondCore.DBCommon.CondDBSetup_cfi import *
#process.jec = cms.ESSource("PoolDBESSource",
#      DBParameters = cms.PSet(
#        messageLevel = cms.untracked.int32(0)
#        ),
#      timetype = cms.string('runnumber'),
#      toGet = cms.VPSet(
#      cms.PSet(
#            record = cms.string('JetCorrectionsRecord'),
#            tag    = cms.string('JetCorrectorParametersCollection_Summer15_50nsV2_DATA_AK4PFchs'),
#            label  = cms.untracked.string('AK4PFchs')
#            ) 
#      ), 
#      connect = cms.string('sqlite:Summer15_50nsV2_DATA.db')
#)
## add an es_prefer statement to resolve a possible conflict from simultaneous connection to a global tag
#process.es_prefer_jec = cms.ESPrefer('PoolDBESSource','jec')

process.load("PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff")
process.patJetCorrFactorsReapplyJEC = process.patJetCorrFactorsUpdated.clone(
  src = cms.InputTag("slimmedJets"),
  levels = ['L1FastJet','L2Relative','L3Absolute','L2L3Residual'],
  payload = 'AK4PFchs' 
) 

process.load("PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff")
process.patJetsReapplyJEC = process.patJetsUpdated.clone(
  jetSource = cms.InputTag("slimmedJets"),
  jetCorrFactorsSource = cms.VInputTag(cms.InputTag("patJetCorrFactorsReapplyJEC"))
)

process.patJetCorrFactorsReapplyJECAK8 = process.patJetCorrFactorsUpdated.clone(
  src = cms.InputTag("slimmedJetsAK8"),
  levels = ['L1FastJet','L2Relative','L3Absolute','L2L3Residual'],
  payload = 'AK4PFchs' 
) 

process.load("PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff")
process.patJetsReapplyJECAK8 = process.patJetsUpdated.clone(
  jetSource = cms.InputTag("slimmedJetsAK8"),
  jetCorrFactorsSource = cms.VInputTag(cms.InputTag("patJetCorrFactorsReapplyJECAK8"))
)

from PhysicsTools.PatAlgos.selectionLayer1.jetSelector_cfi import selectedPatJets
process.goodJets = selectedPatJets.clone(src='patJetsReapplyJEC',cut='pt>30 & abs(eta)<2.4')

############# QGL #################
process.load('RecoJets.JetProducers.QGTagger_cfi')
process.QGTagger.srcJets   = cms.InputTag('goodJets')
process.QGTagger.jetsLabel = cms.string('QGL_AK4PFchs')

##-------------------- User analyzer  --------------------------------
process.hadtopBoost = cms.EDAnalyzer('BoostedTTbarFlatTreeProducer',
  jets             = cms.InputTag('patJetsReapplyJECAK8'),
  muons            = cms.InputTag('slimmedMuons'),
  electrons        = cms.InputTag('slimmedElectrons'),
  met              = cms.InputTag('slimmedMETs'),
  vertices         = cms.InputTag('offlineSlimmedPrimaryVertices'),
  rho              = cms.InputTag('fixedGridRhoFastjetAll'),
  massMin          = cms.double(50),
  ptMin            = cms.double(30),
  ptMinLeading     = cms.double(300),
  etaMax           = cms.double(2.4),
  btagMinThreshold = cms.double(0.89),
  btagMaxThreshold = cms.double(1.1),
  btagger          = cms.string('pfCombinedInclusiveSecondaryVertexV2BJetTags'),
  xmlFile          = cms.string('factory_mva_Boosted_QCD__BDT_Category.weights.xml'),
  triggerNames     = cms.vstring(
    'HLT_AK8PFJet360_TrimMass30_v',
    'HLT_PFJet200_v',
    'HLT_PFJet260_v',
    'HLT_PFJet320_v',
    'HLT_PFJet400_v',
    'HLT_PFJet450_v'
  ),
  triggerResults   = cms.InputTag('TriggerResults','','HLT'),
  triggerPrescales = cms.InputTag('patTrigger')
)

process.hadtop = cms.EDAnalyzer('TTbarFlatTreeProducer',
  jets             = cms.InputTag('goodJets'),
  muons            = cms.InputTag('slimmedMuons'),
  electrons        = cms.InputTag('slimmedElectrons'),
  met              = cms.InputTag('slimmedMETs'),
  vertices         = cms.InputTag('offlineSlimmedPrimaryVertices'),
  rho              = cms.InputTag('fixedGridRhoFastjetAll'),
  nJetsMin         = cms.int32(6),
  nBJetsMin        = cms.int32(2),
  ptMin            = cms.double(30),
  htMin            = cms.double(400),
  etaMax           = cms.double(2.4),
  kinfit           = cms.string('kinFitTtFullHadEvent'),
  btagMinThreshold = cms.double(0.89),
  btagMaxThreshold = cms.double(1.1),
  btagger          = cms.string('pfCombinedInclusiveSecondaryVertexV2BJetTags'),
  qgtagger         = cms.InputTag('QGTagger','qgLikelihood'),
  
  triggerNames     = cms.vstring(
   'HLT_PFHT450_SixJet40_PFBTagCSV0p72_v',
   'HLT_PFHT450_SixJet40_v',
   'HLT_PFHT400_SixJet30_BTagCSV0p55_2PFBTagCSV0p72_v',
   'HLT_PFHT400_SixJet30_v',
   'HLT_PFHT200_v',
   'HLT_PFHT250_v',
   'HLT_PFHT300_v',
   'HLT_PFHT350_v',
   'HLT_PFHT400_v',
   'HLT_PFJet60_v',
   'HLT_PFJet80_v',
   'HLT_PFJet140_v',
   'HLT_DiPFJetAve60_v',
   'HLT_DiPFJetAve80_v',
   'HLT_DiPFJetAve140_v'
  ),
  triggerResults   = cms.InputTag('TriggerResults','','HLT'),
  triggerPrescales = cms.InputTag('patTrigger')
)

process.hadtopOneBtag = process.hadtop.clone(nBJetsMin = 1, kinfit = "kinFitTtFullHadEventOneBtag")

process.load('TopQuarkAnalysis.TopKinFitter.TtFullHadKinFitProducer_cfi')

process.kinFitTtFullHadEvent.jets                = 'goodJets'
process.kinFitTtFullHadEvent.bTagAlgo            = 'pfCombinedInclusiveSecondaryVertexV2BJetTags'
process.kinFitTtFullHadEvent.minBTagValueBJet    = 0.89
process.kinFitTtFullHadEvent.maxBTagValueNonBJet = 0.89
process.kinFitTtFullHadEvent.bTags               = 2
process.kinFitTtFullHadEvent.maxNJets            = 8

process.kinFitTtFullHadEventOneBtag = process.kinFitTtFullHadEvent.clone(bTags = 1)

#process.json = cms.EDFilter("ApplyJSON",
#  jsonFile = cms.string("Cert_246908-257599_13TeV_PromptReco_Collisions15_25ns_JSON.txt")
#)

process.p = cms.Path(
   #process.json +
   process.patJetCorrFactorsReapplyJEC +
   process.patJetCorrFactorsReapplyJECAK8 +
   process.patJetsReapplyJEC +
   process.patJetsReapplyJECAK8 +
   process.goodJets + 
   process.QGTagger + 
   process.kinFitTtFullHadEvent + 
   process.kinFitTtFullHadEventOneBtag +
   process.hadtop +
   process.hadtopOneBtag +
   process.hadtopBoost
)








