// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

//include the following headers:
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "FWCore/Common/interface/TriggerResultsByName.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include <cassert>

#include "TTree.h"

//
// class declaration
//

class TriggerInfoAnalyzer : public edm::EDAnalyzer {
   public:
      explicit TriggerInfoAnalyzer(const edm::ParameterSet&);
      ~TriggerInfoAnalyzer();

      virtual void beginRun(edm::Run const&, edm::EventSetup const&);
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual int analyzeTrigger(const edm::Event&, const edm::EventSetup&, const std::string& triggerName);
      //the follwing are not being used here
      virtual void beginJob() ;
      virtual void endJob() ;
      virtual void endRun(edm::Run const&, edm::EventSetup const&);
      virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
      virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
     
      /// module config parameters
      std::string   processName_;
      std::string   triggerName_;
      std::string   datasetName_;
      edm::InputTag triggerResultsTag_;
      edm::InputTag triggerEventTag_;


      // additional class data memebers
      // these are actually the containers where we will store
      // the trigger information
      edm::Handle<edm::TriggerResults>   triggerResultsHandle_;
      edm::Handle<trigger::TriggerEvent> triggerEventHandle_;
      HLTConfigProvider hltConfig_;


  int HltEvtCnt;
  TTree* HltTree;
  int* trigflag;

      // ----------member data ---------------------------
};

//This should match your configuration python file
TriggerInfoAnalyzer::TriggerInfoAnalyzer(const edm::ParameterSet& ps):
processName_(ps.getParameter<std::string>("processName")),
triggerName_(ps.getParameter<std::string>("triggerName")),
datasetName_(ps.getParameter<std::string>("datasetName")),
triggerResultsTag_(ps.getParameter<edm::InputTag>("triggerResults")),
triggerEventTag_(ps.getParameter<edm::InputTag>("triggerEvent"))
{
   //now do what ever initialization is needed
  using namespace std;
  using namespace edm;
  
  //Print the configuration just to check
  cout << "Here is the information passed to the constructor:" <<endl;
  cout << "HLTEventAnalyzerAOD configuration: " << endl
       << "   ProcessName = " << processName_ << endl
       << "   TriggerName = " << triggerName_ << endl
       << "   DataSetName = " << datasetName_ << endl
       << "   TriggerResultsTag = " << triggerResultsTag_.encode() << endl
       << "   TriggerEventTag = " << triggerEventTag_.encode() << endl;

  HltEvtCnt = 0;
  edm::Service<TFileService> fs;
  HltTree = fs->make<TTree>("HltTree", "");
  const int kMaxTrigFlag = 10000;
  trigflag = new int[kMaxTrigFlag];
}


TriggerInfoAnalyzer::~TriggerInfoAnalyzer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//




// ------------ method called when starting to processes a run  ------------
void TriggerInfoAnalyzer::beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup)
//--------------------------------------------------------------------------
{
  using namespace std;
  using namespace edm;


  //If the hltConfig can be initialized, then the below is an example of
  //how to extract the config information for the trigger from the 
  //so-called provenance.

  // The trigger configuration can change from 
  // run to run (during the run is the same),
  // so it needs to be called here.

  ///   "init" return value indicates whether intitialisation has succeeded
  ///   "changed" parameter indicates whether the config has actually changed

  bool changed(true);
  if (hltConfig_.init(iRun,iSetup,processName_,changed)) {
    if (changed) {
      // check if trigger name in (new) config
      if (triggerName_!="@") { // "@" means: analyze all triggers in config
	const unsigned int n(hltConfig_.size());
	const unsigned int triggerIndex(hltConfig_.triggerIndex(triggerName_));
	if (triggerIndex>=n) {
	  cout << "HLTEventAnalyzerAOD::analyze:"
	       << " TriggerName " << triggerName_ 
	       << " not available in (new) config!" << endl;
	  cout << "Available TriggerNames are: " << endl;
	  hltConfig_.dump("Triggers");
	}
      }
	hltConfig_.dump("Datasets");//use to check the Dataset name to analyze the triggers
    }
  } else {
    cout << "HLTEventAnalyzerAOD::analyze:"
	 << " config extraction failure with process name "
	 << processName_ << endl;
  }

}//------------------- beginRun()





// ------------ method called for each event  ------------------------------
// As with any EDAnalyzer, this method is the heart of the analysis
void TriggerInfoAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
//--------------------------------------------------------------------------
{
   using namespace edm;
   using namespace std;

   //Get event products: 
   // In the following, the code is trying to access the information 
   // from the ROOT files and point the containers (that we created), 
   // namely triggerResultsHandle_ aed triggerEVentHandle_, 
   // to the correct "address", given at configuration time 
   // and assigned to triggerResultsTag_ and triggerEventTag_
 
   // After that, a simple sanity check is done.
 
   iEvent.getByLabel(triggerResultsTag_,triggerResultsHandle_);
   if (!triggerResultsHandle_.isValid()) {
     cout << "HLTEventAnalyzerAOD::analyze: Error in getting TriggerResults product from Event!" << endl;
     return;
   }
   iEvent.getByLabel(triggerEventTag_,triggerEventHandle_);
   if (!triggerEventHandle_.isValid()) {
     cout << "HLTEventAnalyzerAOD::analyze: Error in getting TriggerEvent product from Event!" << endl;
     return;
   }
   // sanity check
   assert(triggerResultsHandle_->size()==hltConfig_.size());
   

   //The following two examples should be used separately or somehow
   //combine them according the analysis needs
   
  bool saveEvent = 1;
	Int_t accept;
	TString trigName;
   const vector<string> triggerNamesInDS = hltConfig_.datasetContent(datasetName_);
   for (unsigned i = 0; i < triggerNamesInDS.size(); i++) {
	accept=0;
       trigName = triggerNamesInDS[i];
     trigflag[i]=0;
  if (HltEvtCnt==0){
      HltTree->Branch(trigName,&trigflag[i],trigName+"/I");
    }
  else {
	if (!HltTree->GetBranch(trigName))HltTree->Branch(trigName,&trigflag[i],trigName+"/I");	
	}
  accept=  analyzeTrigger(iEvent,iSetup,triggerNamesInDS[i]);
if(accept){
    trigflag[i] = 1;
	cout << trigName<<endl;
}
  }
    HltEvtCnt++;
  if(saveEvent) HltTree->Fill();
  return;

}//---------------------------analyze()




//---------------------------Actual trigger analysis-------------
int TriggerInfoAnalyzer::analyzeTrigger(const edm::Event& iEvent, const edm::EventSetup& iSetup, const std::string& triggerName) 
//-----------------------------------------------------------------
{

  using namespace std;
  using namespace edm;
  using namespace reco;
  using namespace trigger;
  Int_t trig_accept=0;
  //cout<<"Currently analyzing trigger "<<triggerName<<endl;

  //Check the current configuration to see how many total triggers there are
  const unsigned int n(hltConfig_.size());
  //Get the trigger index for the current trigger
  const unsigned int triggerIndex(hltConfig_.triggerIndex(triggerName));
  //check that the trigger in the event and in the configuration agree
  assert(triggerIndex==iEvent.triggerNames(*triggerResultsHandle_).triggerIndex(triggerName));
  // abort on invalid trigger name
  if (triggerIndex>=n) {
    cout << "HLTEventAnalyzerAOD::analyzeTrigger: path "
	 << triggerName << " - not found!" << endl;
    return 0;
  }
  
//  const std::pair<int,int> prescales(hltConfig_.prescaleValues(iEvent,iSetup,triggerName));
//  cout << "HLTEventAnalyzerAOD::analyzeTrigger: path "
//      << triggerName << " [" << triggerIndex << "] "
//      << "prescales L1T,HLT: " << prescales.first << "," << prescales.second
//      << endl;
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  // EXAMPLE: Find out if the trigger was active, accepted, or in error.
  // We could also find out whether the trigger was active (wasrun), 
  // if it accepted the event (accept) or if it gave an error (error).
  // Results from TriggerResults product
  //Uncomment the lines below
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if(triggerResultsHandle_->accept(triggerIndex)){
cout <<triggerName<<"\t";
  cout << " Trigger path status:"
       << " WasRun=" << triggerResultsHandle_->wasrun(triggerIndex)
       << " Accept=" << triggerResultsHandle_->accept(triggerIndex)
       << " Error =" << triggerResultsHandle_->error(triggerIndex)
       << endl;
   trig_accept = 1;
}
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  const unsigned int m(hltConfig_.size(triggerIndex));
  const unsigned int moduleIndex(triggerResultsHandle_->index(triggerIndex));
  assert (moduleIndex<m);
  
  return trig_accept;
}//--------------------------analyzeTrigger()



// ------------ method called once each job just before starting event loop  ------------
void 
TriggerInfoAnalyzer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
TriggerInfoAnalyzer::endJob() 
{
}


// ------------ method called when ending the processing of a run  ------------
void TriggerInfoAnalyzer::endRun(edm::Run const&, edm::EventSetup const&)
{
}


// ------------ method called when starting to processes a luminosity block  ------------
void 
TriggerInfoAnalyzer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a luminosity block  ------------
void 
TriggerInfoAnalyzer::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
TriggerInfoAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(TriggerInfoAnalyzer);
