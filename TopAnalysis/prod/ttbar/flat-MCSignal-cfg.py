import FWCore.ParameterSet.Config as cms 
process = cms.Process('myprocess')
process.TFileService=cms.Service("TFileService",fileName=cms.string('flatTree.root'))
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
process.GlobalTag.globaltag = '80X_mcRun2_asymptotic_2016_TrancheIV_v8'
##-------------------- Define the source  ----------------------------
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))
process.source = cms.Source("PoolSource",
  fileNames = cms.untracked.vstring(
    "/store/mc/RunIISummer16MiniAODv2/TT_TuneCUETP8M2T4_13TeV-powheg-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/50000/0693E0E7-97BE-E611-B32F-0CC47A78A3D8.root"
    )
)
#############   Format MessageLogger #################
process.load('FWCore.MessageService.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

#############   JEC #################
#from CondCore.DBCommon.CondDBSetup_cfi import *
#process.jec = cms.ESSource("PoolDBESSource",
#      toGet = cms.VPSet(
#        cms.PSet(
#            record = cms.string('JetCorrectionsRecord'),
#            tag    = cms.string('JetCorrectorParametersCollection_Summer16_23Sep2016V3_MC_AK8PFchs'),
#            label  = cms.untracked.string('AK8PFchs')
#        ) 
#      ),
#      connect = cms.string('sqlite:Summer16_23Sep2016V3_MC.db')
#)
## add an es_prefer statement to resolve a possible conflict from simultaneous connection to a global tag
#process.es_prefer_jec = cms.ESPrefer('PoolDBESSource','jec')

#--- first re-apply JEC from the GT -------------------------
process.load("PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff")

process.patJetCorrFactorsReapplyJECAK8 = process.updatedPatJetCorrFactors.clone(
  src = cms.InputTag("slimmedJetsAK8"),
  levels = ['L1FastJet','L2Relative','L3Absolute'],
  payload = 'AK8PFchs' 
) 

process.patJetsReapplyJECAK8 = process.updatedPatJets.clone(
  jetSource = cms.InputTag("slimmedJetsAK8"),
  jetCorrFactorsSource = cms.VInputTag(cms.InputTag("patJetCorrFactorsReapplyJECAK8"))
)

#--- then smear MC jets to match the JER in data --------------
process.smearedJetsAK8 = cms.EDProducer('JetShiftProducer',
  jets        = cms.InputTag('patJetsReapplyJECAK8'),
  rho         = cms.InputTag('fixedGridRhoFastjetAll'),
  payload     = cms.untracked.string('AK8PFchs'),
  resSFFile   = cms.untracked.string('Spring16_25nsV10_MC_SF_AK8PFchs.txt'),
  shiftJES    = cms.untracked.double(0.0),
  shiftJER    = cms.untracked.double(0.0),
  doSmear     = cms.untracked.bool(True),
  doShift     = cms.untracked.bool(False)
)

process.smearedJetsAK8Up   = process.smearedJetsAK8.clone(shiftJER = 1.0)
process.smearedJetsAK8Down = process.smearedJetsAK8.clone(shiftJER = -1.0)

#--- JES variations -------------------------------------
process.shiftedJetsAK8Up = cms.EDProducer('JetShiftProducer',
  jets        = cms.InputTag('patJetsReapplyJECAK8'),
  rho         = cms.InputTag('fixedGridRhoFastjetAll'),
  payload     = cms.untracked.string('AK8PFchs'),
  shiftJES    = cms.untracked.double(1.0),
  doShift     = cms.untracked.bool(True)
)

process.shiftedJetsAK8Down = process.shiftedJetsAK8Up.clone(shiftJES = -1.0)

genParticleCollection = 'prunedGenParticles'
genJetCollection = 'slimmedGenJetsAK8'

from PhysicsTools.JetMCAlgos.HadronAndPartonSelector_cfi import selectedHadronsAndPartons
process.selectedHadronsAndPartons = selectedHadronsAndPartons.clone(particles = genParticleCollection)

from PhysicsTools.JetMCAlgos.AK4PFJetsMCFlavourInfos_cfi import ak4JetFlavourInfos
process.ak8genJetFlavourInfos = ak4JetFlavourInfos.clone(
    jets = genJetCollection,
    rParam = cms.double(0.8),
)

#only needed if information of the associated b hadrons are required
from PhysicsTools.JetMCAlgos.GenHFHadronMatcher_cff import matchGenBHadron
process.matchGenBHadron = matchGenBHadron.clone(
    genParticles = genParticleCollection,
    jetFlavourInfos = cms.InputTag("ak8genJetFlavourInfos"),
    flavour = cms.int32(5),
    onlyJetClusteredHadrons = cms.bool(True),
    noBBbarResonances = cms.bool(False),
)


##-------------------- User analyzers  --------------------------------
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
  xmlFile          = cms.string('boosted_mva_MLP.weights.xml'),
  isMC             = cms.untracked.bool(True),
  genjets          = cms.untracked.InputTag('slimmedGenJetsAK8'),
  jetFlavourInfos  = cms.untracked.InputTag("ak8genJetFlavourInfos"),                 
  isPrint          = cms.untracked.bool(False),
  saveWeights      = cms.untracked.bool(True),
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

process.boostedSmeared     = process.boosted.clone(jets = 'smearedJetsAK8',    saveWeights = False)
process.boostedSmearedUp   = process.boosted.clone(jets = 'smearedJetsAK8Up',  saveWeights = False)
process.boostedSmearedDown = process.boosted.clone(jets = 'smearedJetsAK8Down',saveWeights = False)
process.boostedShiftedUp   = process.boosted.clone(jets = 'shiftedJetsAK8Up',  saveWeights = False)
process.boostedShiftedDown = process.boosted.clone(jets = 'shiftedJetsAK8Down',saveWeights = False)

process.eventCounter = cms.EDAnalyzer("EventCounter",
   ptTopMin  = cms.double(400),
   etaTopMax = cms.double(5.0)
)

process.reapplyjec = cms.Sequence(
   process.patJetCorrFactorsReapplyJECAK8 + 
   process.patJetsReapplyJECAK8
)

process.smearjets = cms.Sequence(
   process.smearedJetsAK8 +
   process.smearedJetsAK8Up +
   process.smearedJetsAK8Down
)

process.shiftjets = cms.Sequence(
   process.shiftedJetsAK8Up +
   process.shiftedJetsAK8Down
)

process.boostedanalyzer = cms.Sequence(
   process.boosted +
   process.boostedSmeared +
   process.boostedSmearedUp +
   process.boostedSmearedDown +
   process.boostedShiftedUp +
   process.boostedShiftedDown
)

process.p = cms.Path(
   process.eventCounter *
   process.selectedHadronsAndPartons*process.ak8genJetFlavourInfos*#*process.matchGenBHadron*
   process.reapplyjec *    
   process.smearjets *
   process.shiftjets * 
   process.boostedanalyzer
)








