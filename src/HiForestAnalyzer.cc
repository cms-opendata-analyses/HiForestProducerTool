// -*- C++ -*-
//
// Package:    HiForestAnalyzer
// Class:      HiForestAnalyzer
// 
/**\class HiForestAnalyzer HiForestAnalyzer.cc HiForest/HiForestAnalyzer/src/HiForestAnalyzer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  
//         Created:  Tue Nov 10 17:26:21 CST 2020
// $Id$
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
//
// class declaration
//

class HiForestAnalyzer : public edm::EDAnalyzer {
   public:
      explicit HiForestAnalyzer(const edm::ParameterSet&);
      ~HiForestAnalyzer();


   private:
      virtual void beginJob() ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;

      // ----------member data ---------------------------
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
HiForestAnalyzer::HiForestAnalyzer(const edm::ParameterSet& iConfig)

{
   //now do what ever initialization is needed

}


HiForestAnalyzer::~HiForestAnalyzer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called to for each event  ------------
void
HiForestAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;



#ifdef THIS_IS_AN_EVENT_EXAMPLE
   Handle<ExampleData> pIn;
   iEvent.getByLabel("example",pIn);
#endif
   
#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
   ESHandle<SetupData> pSetup;
   iSetup.get<SetupRecord>().get(pSetup);
#endif
}


// ------------ method called once each job just before starting event loop  ------------
void 
HiForestAnalyzer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
HiForestAnalyzer::endJob() {
}

//define this as a plug-in
DEFINE_FWK_MODULE(HiForestAnalyzer);
