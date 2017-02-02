import FWCore.ParameterSet.Config as cms 
process = cms.Process('myprocess')
process.TFileService=cms.Service("TFileService",fileName=cms.string('flatTree.root'))
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
process.GlobalTag.globaltag = '80X_dataRun2_Prompt_ICHEP16JEC_v0'
##-------------------- Define the source  ----------------------------
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))
process.source = cms.Source("PoolSource",
  fileNames = cms.untracked.vstring(
    #"/store/data/Run2016E/JetHT/MINIAOD/23Sep2016-v1/100000/0248DC22-B889-E611-B75F-0025905A60AA.root"
    '/store/data/Run2016G/SingleMuon/MINIAOD/23Sep2016-v1/1110000/A2C0F697-B19C-E611-A4D8-F04DA275BFF2.root'
    )
)
#############   Format MessageLogger #################
process.load('FWCore.MessageService.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

#############   JEC #################
from CondCore.DBCommon.CondDBSetup_cfi import *
process.jec = cms.ESSource("PoolDBESSource",
      toGet = cms.VPSet(
        cms.PSet(
            record = cms.string('JetCorrectionsRecord'),
            tag    = cms.string('JetCorrectorParametersCollection_Summer16_23Sep2016AllV3_DATA_AK8PFchs'),
            label  = cms.untracked.string('AK8PFchs')
        )  
      ),
      connect = cms.string('sqlite:Summer16_23Sep2016AllV3_DATA.db')
)
## add an es_prefer statement to resolve a possible conflict from simultaneous connection to a global tag
process.es_prefer_jec = cms.ESPrefer('PoolDBESSource','jec')

process.load("PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff")

process.patJetCorrFactorsReapplyJECAK8 = process.updatedPatJetCorrFactors.clone(
  src = cms.InputTag("slimmedJetsAK8"),
  levels = ['L1FastJet','L2Relative','L3Absolute','L2L3Residual'],
  payload = 'AK8PFchs' 
) 

process.patJetsReapplyJECAK8 = process.updatedPatJets.clone(
  jetSource = cms.InputTag("slimmedJetsAK8"),
  jetCorrFactorsSource = cms.VInputTag(cms.InputTag("patJetCorrFactorsReapplyJECAK8"))
)

##-------------------- User analyzer  --------------------------------
process.boosted = cms.EDAnalyzer('BoostedTTbarFlatTreeProducer',
  jets             = cms.InputTag('patJetsReapplyJECAK8'),
  muons            = cms.InputTag('slimmedMuons'),
  electrons        = cms.InputTag('slimmedElectrons'),
  met              = cms.InputTag('slimmedMETs'),
  candidates       = cms.InputTag('packedPFCandidates'),
  vertices         = cms.InputTag('offlineSlimmedPrimaryVertices'),
  rho              = cms.InputTag('fixedGridRhoFastjetAll'),
  massMin          = cms.double(50),
  ptMin            = cms.double(200),
  etaMax           = cms.double(2.4),
  minMuPt          = cms.double(20),
  minElPt          = cms.double(20),
  btagMin          = cms.double(0.8484),
  btagger          = cms.string('pfCombinedInclusiveSecondaryVertexV2BJetTags'),
  xmlFile          = cms.string('boosted_mva_Fisher_new.weights.xml'),
  triggerNames     = cms.vstring(
    'HLT_AK8PFJet360_TrimMass30_v',
    'HLT_AK8DiPFJet250_200_TrimMass30_BTagCSV_p20_v',
    'HLT_AK8DiPFJet280_200_TrimMass30_BTagCSV_p20_v',
    'HLT_AK8DiPFJet250_200_TrimMass30_v',
    'HLT_AK8DiPFJet280_200_TrimMass30_v',
    'HLT_AK8PFJet140_v',
    'HLT_AK8PFJet200_v',
    'HLT_AK8PFJet260_v',
    'HLT_IsoMu27_v',
    'HLT_Mu50_v'
  ),
  triggerResults   = cms.InputTag('TriggerResults','','HLT'),
  triggerPrescales = cms.InputTag('patTrigger')
)

process.p = cms.Path(
   process.patJetCorrFactorsReapplyJECAK8 +
   process.patJetsReapplyJECAK8 +
   process.boosted
)








