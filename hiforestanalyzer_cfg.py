#Loading necessary libraries
import FWCore.ParameterSet.Config as cms
from RecoMuon.TrackingTools.MuonServiceProxy_cff import *
import PhysicsTools.PythonAnalysis.LumiList as LumiList
import FWCore.ParameterSet.Types as CfgTypes
process = cms.Process('HiForest')
process.options = cms.untracked.PSet(SkipEvent = cms.untracked.vstring('ProductNotFound'))

#Number of events: put '-1' unless testing
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

#HiForest script init
process.load("HiForest_cff")
process.HiForest.inputLines = cms.vstring("HiForest V3",)
version = 'no git info'
process.HiForest.HiForestVersion = cms.string(version)

goodJSON = 'Cert_181530-183126_HI7TeV_PromptReco_Collisions11_JSON_MuonPhys.txt'
myLumis = LumiList.LumiList(filename = goodJSON).getCMSSWString().split(',')
import FWCore.Utilities.FileUtils as FileUtils
files2011data = FileUtils.loadListFromFile ('CMS_HIRun2010_HIAllPhysics_ZS-v2_RECO_file_index.txt')
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(*files2011data    
    )
)
process.source.lumisToProcess = CfgTypes.untracked(CfgTypes.VLuminosityBlockRange())
process.source.lumisToProcess.extend(myLumis)

#Global Tag: change the name according to the instructions
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.connect = cms.string('sqlite_file:/cvmfs/cms-opendata-conddb.cern.ch/GR_R_44_V15.db')
process.GlobalTag.globaltag = 'GR_R_44_V15::All'
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load("Configuration.StandardSequences.MagneticField_cff")
process.HiForest.GlobalTagLabel = process.GlobalTag.globaltag

#Define the output root file (change each run not to overwrite previous output)
process.TFileService = cms.Service("TFileService",
                                   fileName=cms.string("HiForestAOD_DATAtest2011.root"))

#Init Trigger Analyzer
process.hltanalysis = cms.EDAnalyzer('TriggerInfoAnalyzer',
                              processName = cms.string("HLT"),
                              triggerName = cms.string("@"),         
                              datasetName = cms.string("HIDiMuon"),  #'HICorePhysics' to look at Core Physics only
                              triggerResults = cms.InputTag("TriggerResults","","HLT"),
                              triggerEvent   = cms.InputTag("hltTriggerSummaryAOD","","HLT")                             
                              )

#Collect event data
process.demo = cms.EDAnalyzer('Analyzer') #present analyzer is for muons - see details in Analyzer.cc for possible modifications
process.dump=cms.EDAnalyzer('EventContentAnalyzer') #easy check of Event structure and names without using the TBrowser

process.ana_step = cms.Path(process.hltanalysis+
		  	    #process.dump+  #uncomment if necessary to check the name. Do not forget to change the number of events to '1'
			    process.demo+
                            process.HiForest 
)

