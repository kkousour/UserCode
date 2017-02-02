import FWCore.ParameterSet.Config as cms 
process = cms.Process('myprocess')
process.TFileService=cms.Service("TFileService",fileName=cms.string('flatTree.root'))
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
process.GlobalTag.globaltag = '80X_mcRun2_asymptotic_2016_miniAODv2'
##-------------------- Define the source  ----------------------------
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))
process.source = cms.Source("PoolSource",
  fileNames = cms.untracked.vstring(
    "/store/mc/RunIISummer16MiniAODv2/QCD_HT200to300_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/120000/02FDEBBA-9ABD-E611-888C-001E67E6F7F6.root"
    )
)
#############   Format MessageLogger #################
process.load('FWCore.MessageService.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

#############   JEC #################
from CondCore.DBCommon.CondDBSetup_cfi import CondDBSetup

process.jec = cms.ESSource('PoolDBESSource',
    CondDBSetup,
    #connect = cms.string('sqlite_fip:KKousour/TopAnalysis/prod/ttH/Summer16_23Sep2016V3_MC.db'),
    connect = cms.string('sqlite:Summer16_23Sep2016V3_MC.db'),
    toGet = cms.VPSet(
        cms.PSet(
            record = cms.string('JetCorrectionsRecord'),
            tag    = cms.string('JetCorrectorParametersCollection_Summer16_23Sep2016V3_MC_AK4PFchs'),
            label  = cms.untracked.string('AK4PFchs')
        )
    )
)

## add an es_prefer statement to resolve a possible conflict from simultaneous connection to a global tag
process.es_prefer_jec = cms.ESPrefer('PoolDBESSource','jec')

#--- first re-apply JEC from the GT -------------------------
process.load("PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff")
process.patJetCorrFactorsReapplyJEC = process.updatedPatJetCorrFactors.clone(
  src = cms.InputTag("slimmedJets"),
  levels = ['L1FastJet','L2Relative','L3Absolute'],
  payload = 'AK4PFchs' 
) 

process.load("PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff")
process.patJetsReapplyJEC = process.updatedPatJets.clone(
  jetSource = cms.InputTag("slimmedJets"),
  jetCorrFactorsSource = cms.VInputTag(cms.InputTag("patJetCorrFactorsReapplyJEC"))
)

#--- clean jets from leptons -----------------------------------
process. cleanedJets = cms.EDProducer('JetCleanedProducer',
  jets        = cms.InputTag('patJetsReapplyJEC'),
  rho         = cms.InputTag('fixedGridRhoFastjetAll'),
  muons       = cms.InputTag('slimmedMuons'),
  electrons   = cms.InputTag('slimmedElectrons'),
  vertices    = cms.InputTag('offlineSlimmedPrimaryVertices'),
  minMuPt     = cms.double(10),
  minElPt     = cms.double(10)
)

#--- finally define the good jets -------------------------------
from PhysicsTools.PatAlgos.selectionLayer1.jetSelector_cfi import selectedPatJets
process.goodJets = selectedPatJets.clone(src='cleanedJets',cut='pt>30 & abs(eta)<2.4')

process.load('RecoJets.JetProducers.QGTagger_cfi')
process.QGTagger.srcJets   = cms.InputTag('goodJets')
process.QGTagger.jetsLabel = cms.string('QGL_AK4PFchs')

##-------------------- User analyzers  --------------------------------
process.load('TopQuarkAnalysis.TopKinFitter.TtFullHadKinFitProducer_cfi')

process.kinFitTtFullHadEvent.jets                = 'goodJets'
process.kinFitTtFullHadEvent.jetCorrectionLevel  = 'L3Absolute'
process.kinFitTtFullHadEvent.bTagAlgo            = 'pfCombinedInclusiveSecondaryVertexV2BJetTags'
process.kinFitTtFullHadEvent.minBTagValueBJet    = 0.8484
process.kinFitTtFullHadEvent.maxBTagValueNonBJet = 0.8484
process.kinFitTtFullHadEvent.bTags               = 2
process.kinFitTtFullHadEvent.maxNJets            = 8
process.kinFitTtFullHadEvent.jetEnergyResolutionScaleFactors = cms.vdouble(1.061,1.088,1.106,1.126,1.343)
process.kinFitTtFullHadEvent.jetEnergyResolutionEtaBinning = cms.vdouble(0.0,0.8,1.3,1.9,2.5,-1)

process.kinFitTtFullHadEventLoose = process.kinFitTtFullHadEvent.clone(minBTagValueBJet = 0.5426, maxBTagValueNonBJet = 0.5426)

process.ttH = cms.EDAnalyzer('TTHFlatTreeProducer',
  jets              = cms.InputTag('goodJets'),
  muons             = cms.InputTag('slimmedMuons'),
  electrons         = cms.InputTag('slimmedElectrons'),
  met               = cms.InputTag('slimmedMETs'),
  vertices          = cms.InputTag('offlineSlimmedPrimaryVertices'),
  rho               = cms.InputTag('fixedGridRhoFastjetAll'),
  nJetsMin          = cms.int32(6),
  nBJetsMin         = cms.int32(2),
  ptMin             = cms.double(30),
  htMin             = cms.double(400),
  etaMax            = cms.double(2.4),
  minMuPt           = cms.double(10),
  minElPt           = cms.double(10),
  kinfit            = cms.string('kinFitTtFullHadEvent'),
  xmlFileQCD_CAT0   = cms.string('TTH_mva_CAT0_QCD_BDT.weights.xml'),
  xmlFileQCD_CAT1   = cms.string('TTH_mva_CAT1_QCD_BDT.weights.xml'),
  xmlFileTTbar_CAT0 = cms.string('TTH_mva_CAT0_TTbar_BDT.weights.xml'), 
  xmlFileTTbar_CAT1 = cms.string('TTH_mva_CAT1_TTbar_BDT.weights.xml'),
  btagMinThreshold  = cms.double(0.8484),
  btagMaxThreshold  = cms.double(1.1),
  btagger           = cms.string('pfCombinedInclusiveSecondaryVertexV2BJetTags'),
  qgtagger          = cms.InputTag('QGTagger','qgLikelihood'),
  isMC              = cms.untracked.bool(True),
  triggerNames      = cms.vstring(
     'HLT_PFHT450_SixJet40_BTagCSV_p056_v',
     'HLT_PFHT450_SixJet40_v',
     'HLT_PFHT400_SixJet30_DoubleBTagCSV_p056_v',
     'HLT_PFHT400_SixJet30_v',
     'HLT_PFHT350_v'
  ),
  triggerResults   = cms.InputTag('TriggerResults','','HLT'),
  triggerPrescales = cms.InputTag('patTrigger')
)

process.ttHLoose   = process.ttH.clone(btagMinThreshold = 0.5426, btagMaxThreshold = 0.8484, kinfit = 'kinFitTtFullHadEventLoose')

process.reapplyjec = cms.Sequence(
   process.patJetCorrFactorsReapplyJEC +
   process.patJetsReapplyJEC
)

process.selectjets = cms.Sequence(
   process.cleanedJets +
   process.goodJets
)

process.qgtagging = cms.Sequence(
   process.QGTagger
)

process.kinfit = cms.Sequence(
   process.kinFitTtFullHadEvent + 
   process.kinFitTtFullHadEventLoose
)

process.ttHanalyzer = cms.Sequence(
   process.ttH +
   process.ttHLoose
)

process.eventCounter = cms.EDAnalyzer("EventCounter")

process.p = cms.Path(
   process.eventCounter *
   process.reapplyjec *    
   process.selectjets *
   process.qgtagging *
   process.kinfit *
   process.ttHanalyzer
)








