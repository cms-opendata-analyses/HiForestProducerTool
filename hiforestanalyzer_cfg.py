#Loading necessary libraries
import FWCore.ParameterSet.Config as cms
from RecoMuon.TrackingTools.MuonServiceProxy_cff import *
import PhysicsTools.PythonAnalysis.LumiList as LumiList
import FWCore.ParameterSet.Types as CfgTypes
process = cms.Process('HiForest')
process.options = cms.untracked.PSet(SkipEvent = cms.untracked.vstring('ProductNotFound'))

#Number of events: put '-1' unless testing
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100) )

#HiForest script init
process.load("HiForest_cff")
process.HiForest.inputLines = cms.vstring("HiForest V3",)
version = 'no git info'
process.HiForest.HiForestVersion = cms.string(version)

goodJSON = '/home/cms-opendata/CMSSW_3_9_2_patch5/src/TriggerInfo/TriggerInfoAnalyzer/Cert_150436-152957_HI7TeV_StreamExpress_Collisions10_JSON_MuonPhys_v2.txt'
myLumis = LumiList.LumiList(filename = goodJSON).getCMSSWString().split(',')
import FWCore.Utilities.FileUtils as FileUtils
files2010data = FileUtils.loadListFromFile ('/home/cms-opendata/Opendata/CMSSW_3_9_2_patch5/src/HiForest/HiForestAnalyzer/InputList.txt')
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(*files2010data    
    )
)
process.source.lumisToProcess = CfgTypes.untracked(CfgTypes.VLuminosityBlockRange())
process.source.lumisToProcess.extend(myLumis)

#Global Tag: change the name according to the instructions
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.connect = cms.string('sqlite_file:/cvmfs/cms-opendata-conddb.cern.ch/GR_R_39X_V6B.db')
process.GlobalTag.globaltag = 'GR_R_39X_V6B::All'
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load("Configuration.StandardSequences.MagneticField_cff")
process.HiForest.GlobalTagLabel = process.GlobalTag.globaltag

#Define the output root file (change each run not to overwrite previous output)
process.TFileService = cms.Service("TFileService",
                                   fileName=cms.string("HiForestAOD_DATAtest.root"))

#Init Trigger Analyzer
process.hltanalysis = cms.EDAnalyzer('TriggerInfoAnalyzer',
                              processName = cms.string("HLT"),
                              triggerName = cms.string("@"),         
                              datasetName = cms.string("HIAllPhysics"),  #'HICorePhysics' to look at Core Physics only
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

