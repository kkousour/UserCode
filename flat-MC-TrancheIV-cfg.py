import FWCore.ParameterSet.Config as cms 
process = cms.Process('myprocess')
process.TFileService=cms.Service("TFileService",fileName=cms.string('flatTree.root'))
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
process.GlobalTag.globaltag = '80X_mcRun2_asymptotic_2016_miniAODv2_v1'
##-------------------- Define the source  ----------------------------
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))
process.source = cms.Source("PoolSource",
  fileNames = cms.untracked.vstring(
    "/store/mc/RunIISummer16MiniAODv2/ttHJetTobb_M125_13TeV_amcatnloFXFX_madspin_pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext3-v1/60000/22C73FD9-5AE2-E611-A6EE-001E674FB149.root"
    #"/store/mc/RunIISummer16MiniAODv2/ttHJetTobb_M125_13TeV_amcatnloFXFX_madspin_pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext3-v1/110000/2299238C-15E2-E611-9D02-001E67A40604.root"
    #"/store/mc/RunIISummer16MiniAODv2/ttHJetTobb_M125_13TeV_amcatnloFXFX_madspin_pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext3-v1/110000/38345EDA-E5E1-E611-AC6B-24BE05C47B22.root"
    #'/store/mc/RunIISummer16MiniAODv2/TTZToQQ_TuneCUETP8M1_13TeV-amcatnlo-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/100000/103521A4-3ED3-E611-A69C-0CC47A7C3424.root'
    )
)
#############   Format MessageLogger #################
process.load('FWCore.MessageService.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 1000


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
process.boosted = cms.EDAnalyzer('BoostedTTHFlatTreeProducer',
  jets             = cms.InputTag('patJetsReapplyJECAK8'),
  muons            = cms.InputTag('slimmedMuons'),
  electrons        = cms.InputTag('slimmedElectrons'),
  met              = cms.InputTag('slimmedMETs'),
  candidates       = cms.InputTag('packedPFCandidates'),
  vertices         = cms.InputTag('offlineSlimmedPrimaryVertices'),
  rho              = cms.InputTag('fixedGridRhoFastjetAll'),
  massMin          = cms.double(50),
  ptMin            = cms.double(200),
  ptMinLeading     = cms.double(200),
  etaMax           = cms.double(2.4),
  minMuPt          = cms.double(20),
  minElPt          = cms.double(20),
  GenptMin         = cms.double(200),
  GenetaMax        = cms.double(2.4),
  btagMin          = cms.double(0.8484),
  btagger          = cms.string('pfCombinedInclusiveSecondaryVertexV2BJetTags'),
  doubleBtagMin	   = cms.double(0.3),
  doubleBtagger	   = cms.string('pfBoostedDoubleSecondaryVertexAK8BJetTags'),
  xmlFile          = cms.string('boosted_mva_Fisher_new.weights.xml'),
  xmlFisherFile    = cms.string('BoostedttH_SignalandBkgJets_mva_Fisher.weights.xml'),
  isMC             = cms.untracked.bool(True),
  genjets          = cms.untracked.InputTag('slimmedGenJetsAK8'),
  jetFlavourInfos  = cms.InputTag("ak8genJetFlavourInfos"),                 
  isPrint          = cms.untracked.bool(False),
  saveWeights      = cms.untracked.bool(False),
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
    'HLT_Mu50_v',
    'HLT_AK8PFHT600_TrimMass50_v',
    'HLT_AK8PFHT750_TrimMass50_v',
    'HLT_AK8PFHT800_TrimMass50_v',
    'HLT_AK8PFHT850_TrimMass50_v',
    'HLT_AK8PFHT900_TrimMass50_v',
    'HLT_AK8PFHT950_TrimMass50_v'
    #'HLT_AK8PFHT600_TrimR0p1PT0p03Mass50_BTagCSV_p20_v',
    #'HLT_AK8PFHT650_TrimR0p1PT0p03Mass50_v',
    #'HLT_AK8PFHT700_TrimR0p1PT0p03Mass50_v'
  ),
  triggerResults   = cms.InputTag('TriggerResults','','HLT'),
  triggerPrescales = cms.InputTag('patTrigger')
)


process.eventCounter = cms.EDAnalyzer("EventCounter")

process.reapplyjec = cms.Sequence(
   process.patJetCorrFactorsReapplyJECAK8 + 
   process.patJetsReapplyJECAK8
)




process.boostedanalyzer = cms.Sequence(
   process.boosted 
   
)

process.p = cms.Path(
   process.eventCounter *
   process.selectedHadronsAndPartons*process.ak8genJetFlavourInfos*#*process.matchGenBHadron*
   process.reapplyjec *    
   process.boostedanalyzer
)
