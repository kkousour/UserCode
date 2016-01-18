import FWCore.ParameterSet.Config as cms 
process = cms.Process('myprocess')
process.TFileService=cms.Service("TFileService",fileName=cms.string('flatTree.root'))
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
process.GlobalTag.globaltag = '74X_mcRun2_asymptotic_v2'
##-------------------- Define the source  ----------------------------
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))
process.source = cms.Source("PoolSource",
  fileNames = cms.untracked.vstring(
    "/store/mc/RunIISpring15DR74/QCD_HT300to500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/Asympt25ns_MCRUN2_74_V9-v2/00000/00F9B1F1-3B18-E511-B17D-A0369F30FFD2.root",
    "/store/mc/RunIISpring15DR74/QCD_HT300to500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/Asympt25ns_MCRUN2_74_V9-v2/00000/02B1D2D1-3A18-E511-8C1D-0002C92A1024.root",
    "/store/mc/RunIISpring15DR74/QCD_HT300to500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/Asympt25ns_MCRUN2_74_V9-v2/00000/06B31B3D-4E18-E511-8476-0002C92DB46C.root",
    "/store/mc/RunIISpring15DR74/QCD_HT300to500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/Asympt25ns_MCRUN2_74_V9-v2/00000/08148218-3618-E511-92C6-1CC1DE0570A0.root"
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
#            tag    = cms.string('JetCorrectorParametersCollection_Summer15_50nsV2_MC_AK4PFchs'),
#            label  = cms.untracked.string('AK4PFchs')
#            ) 
#      ), 
#      connect = cms.string('sqlite:Summer15_50nsV2_MC.db')
#)
## add an es_prefer statement to resolve a possible conflict from simultaneous connection to a global tag
#process.es_prefer_jec = cms.ESPrefer('PoolDBESSource','jec')

process.load("PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff")
process.patJetCorrFactorsReapplyJEC = process.patJetCorrFactorsUpdated.clone(
  src = cms.InputTag("slimmedJets"),
  levels = ['L1FastJet','L2Relative','L3Absolute'],
  payload = 'AK4PFchs' 
) 

process.load("PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff")
process.patJetsReapplyJEC = process.patJetsUpdated.clone(
  jetSource = cms.InputTag("slimmedJets"),
  jetCorrFactorsSource = cms.VInputTag(cms.InputTag("patJetCorrFactorsReapplyJEC"))
)

process.patJetCorrFactorsReapplyJECAK8 = process.patJetCorrFactorsUpdated.clone(
  src = cms.InputTag("slimmedJetsAK8"),
  levels = ['L1FastJet','L2Relative','L3Absolute'],
  payload = 'AK4PFchs' 
) 

process.load("PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff")
process.patJetsReapplyJECAK8 = process.patJetsUpdated.clone(
  jetSource = cms.InputTag("slimmedJetsAK8"),
  jetCorrFactorsSource = cms.VInputTag(cms.InputTag("patJetCorrFactorsReapplyJECAK8"))
)

from PhysicsTools.PatAlgos.selectionLayer1.jetSelector_cfi import selectedPatJets
process.goodJets = selectedPatJets.clone(src='patJetsReapplyJEC',cut='pt>30 & abs(eta)<2.4')

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
  pu               = cms.untracked.string("addPileupInfo"),
  genparticles     = cms.untracked.InputTag('prunedGenParticles'),
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
  pu               = cms.untracked.string("addPileupInfo"),
  genparticles     = cms.untracked.InputTag('prunedGenParticles'),
  
  triggerNames     = cms.vstring(
    'HLT_PFHT450_SixJet40_PFBTagCSV_v',
    'HLT_PFHT450_SixJet40_v',
    'HLT_PFHT400_SixJet30_BTagCSV0p5_2PFBTagCSV_v',
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

process.load('TopQuarkAnalysis.TopKinFitter.TtFullHadKinFitProducer_cfi')

process.kinFitTtFullHadEvent.jets                = 'goodJets'
process.kinFitTtFullHadEvent.bTagAlgo            = 'pfCombinedInclusiveSecondaryVertexV2BJetTags'
process.kinFitTtFullHadEvent.minBTagValueBJet    = 0.89
process.kinFitTtFullHadEvent.maxBTagValueNonBJet = 0.89
process.kinFitTtFullHadEvent.bTags               = 2
process.kinFitTtFullHadEvent.maxNJets            = 8

process.kinFitTtFullHadEventNoBtag = process.kinFitTtFullHadEvent.clone(bTags = 0)
process.hadtopNoBtag = process.hadtop.clone(kinfit = "kinFitTtFullHadEventNoBtag",nBJetsMin=0)

process.kinFitTtFullHadEventOneBtag = process.kinFitTtFullHadEvent.clone(bTags = 1)
process.hadtopOneBtag = process.hadtop.clone(kinfit = "kinFitTtFullHadEventOneBtag",nBJetsMin=1)

process.p = cms.Path(
   process.patJetCorrFactorsReapplyJEC +
   process.patJetCorrFactorsReapplyJECAK8 +
   process.patJetsReapplyJEC +
   process.patJetsReapplyJECAK8 +
   process.goodJets + 
   process.QGTagger + 
   process.kinFitTtFullHadEvent + 
   #process.kinFitTtFullHadEventNoBtag +
   process.kinFitTtFullHadEventOneBtag +
   process.hadtop +
   #process.hadtopNoBtag +
   process.hadtopOneBtag +
   process.hadtopBoost
)








