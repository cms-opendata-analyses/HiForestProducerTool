
// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

//------ EXTRA HEADER FILES--------------------//
#include "math.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/Common/interface/Ref.h"

// for tracking information
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/HitPattern.h"

// for vertex information 
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

// for muons
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/MuonReco/interface/MuonIsolation.h"

// for electrons uncomment when implemented
//#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
//#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"

//for beamspot information
#include "DataFormats/BeamSpot/interface/BeamSpot.h"

// triggers
#include "DataFormats/Common/interface/TriggerResults.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

// ROOT
#include <TLorentzVector.h>
#include <TFile.h>
#include <TTree.h>

#include <TTree.h>
#include <TDirectory.h>

//
// class declaration
//
class Analyzer : public edm::EDAnalyzer {
   public:
      explicit Analyzer(const edm::ParameterSet&);
      ~Analyzer();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;

      virtual void beginRun(edm::Run const&, edm::EventSetup const&);
      virtual void endRun(edm::Run const&, edm::EventSetup const&);
      virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
      virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
      
      // user routines (detailed description given with the method implementations)
      int SelectEvent(const edm::Event& iEvent);
      int SelectMu(const edm::Handle<reco::TrackCollection>& muons, const reco::VertexCollection::const_iterator& pv);
// Note that muons are taken from the TrackCollection one can collect other necessary data from the input root file by looking
// at its structure in the TBrowser:
// i.e. from the TBrowser we see a folder called:  recoTracks_globalMuons__RECO.
// This means using edm::Handle<reco::TrackCollection> is needed to get hte data (see line just above) and globalMuons will be the 
// relative input tag used in the analyzer function.
   //   int SelectEl(const edm::Handle<reco::GsfElectronCollection>& electrons, const reco::VertexCollection::const_iterator& pv);
      int SelectPrimaryVertex(const edm::Handle<reco::VertexCollection>& primVertex);
      const reco::Candidate* GetFinalState(const reco::Candidate* particle, const int id);
      void FillFourMomentum(const reco::Candidate* particle, float* p);
      void InitBranchVars();

      // input tags
      edm::InputTag _inputTagMuons;
      edm::InputTag _inputTagElectrons;
      edm::InputTag _inputTagBtags;
      edm::InputTag _inputTagPrimaryVertex;

      // general flags and variables
      int _flagMC;
      int _flagRECO;
      int _flagGEN;
      int _nevents;
      int _neventsSelected;
      int _signLeptonP;
      int _signLeptonM;
      
      // storage
      TFile* _file;
      TTree* _tree;
      
      // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
      // >>>>>>>>>>>>>>>> event variables >>>>>>>>>>>>>>>>>>>>>>>
      // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
      // (their description given when tree branches are created)
      // event
      int _evRunNumber;
      int _evEventNumber;
      // muons
      static const int _maxNmu = 10;
      int _Nmu;
      int _Nmu0;
      float _muPt[_maxNmu];
      float _muEta[_maxNmu];
      float _muPhi[_maxNmu];
      float _muC[_maxNmu];
      float _muIso03[_maxNmu];
      float _muIso04[_maxNmu];
      int _muHitsValid[_maxNmu];
      int _muHitsPixel[_maxNmu];
      float _muDistPV0[_maxNmu];
      float _muDistPVz[_maxNmu];
      float _muTrackChi2NDOF[_maxNmu];
      // electrons
      static const int _maxNel = 10;
      int _Nel;
      float _elPt[_maxNel];
      float _elEta[_maxNel];
      float _elPhi[_maxNel];
      float _elIso03[_maxNel];
      float _elIso04[_maxNel];
      int _elConvFlag[_maxNel];
      float _elConvDist[_maxNel];
      float _elConvDcot[_maxNel];
      float _elMissHits[_maxNel];
      float _elDistPV0[_maxNel];
      float _elDistPVz[_maxNel];
      // primary vertex
      int _Npv;
      int _pvNDOF;
      float _pvZ;
      float _pvRho;
};

//
// constants (particle masses)
//
double _massMu = 0.105658;
double _massEl = 0.000511;

//
// constructor
//
Analyzer::Analyzer(const edm::ParameterSet& iConfig)
{
  // for proper log files writing (immediate output)
  setbuf(stdout, NULL);
  
  // input tags
  _inputTagMuons = edm::InputTag("globalMuons");
  //_inputTagElectrons = edm::InputTag("gsfElectrons"); //use this to Analyze electrons
  //_inputTagPrimaryVertex = edm::InputTag("offlinePrimaryVertices*"); //vertex input tag used for pp collisions
  _inputTagPrimaryVertex = edm::InputTag("hiSelectedVertex"); //'hiSelectedVertex' is generally used for PbPb collisions
  
  // read configuration parameters
  _flagMC = 0;//iConfig.getParameter<int>("mc"); // true for MC, false for data
  _flagRECO = 1;//iConfig.getParameter<int>("reco"); // if true, RECO level processed
  _flagGEN = 0;//iConfig.getParameter<int>("gen"); // if true, generator level processed (works only for MC)
  _nevents = 0; // number of processed events
  _neventsSelected = 0; // number of selected events
  edm::Service<TFileService> fs;
  _tree = fs->make<TTree>("Muons", "Muons"); //make output tree

  // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  // >>>>>>> tree branches >>>>>>>>>>>>
  // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  //
  // event
  _tree->Branch("evRunNumber", &_evRunNumber, "evRunNumber/I"); // run number
  _tree->Branch("evEventNumber", &_evEventNumber, "evEventNumber/I"); // event number

  if(_flagRECO)
  {
    // muons
    _tree->Branch("Nmu", &_Nmu, "Nmu/I"); // number of muons 
    _tree->Branch("muPt", _muPt, "muPt[Nmu]/F"); // muon pT
    _tree->Branch("muEta", _muEta, "muEta[Nmu]/F"); // muon pseudorapidity
    _tree->Branch("muPhi", _muPhi, "muPhi[Nmu]/F"); // muon phi
    _tree->Branch("muC", _muC, "muC[Nmu]/F"); // muon phi
    _tree->Branch("muIso03", _muIso03, "muIso03[Nmu]/F"); // muon isolation, delta_R=0.3
    _tree->Branch("muIso04", _muIso04, "muIso04[Nmu]/F"); // muon isolation, delta_R=0.4
    _tree->Branch("muHitsValid", _muHitsValid, "muHitsValid[Nmu]/I"); // muon valid hits number
    _tree->Branch("muHitsPixel", _muHitsPixel, "muHitsPixel[Nmu]/I"); // muon pixel hits number
    _tree->Branch("muDistPV0", _muDistPV0, "muDistPV0[Nmu]/F"); // muon distance to the primary vertex (projection on transverse plane)
    _tree->Branch("muDistPVz", _muDistPVz, "muDistPVz[Nmu]/F"); // muon distance to the primary vertex (z projection)
    _tree->Branch("muTrackChi2NDOF", _muTrackChi2NDOF, "muTrackChi2NDOF[Nmu]/F"); // muon track number of degrees of freedom
    // primary vertex
    _tree->Branch("Npv", &_Npv, "Npv/I"); // total number of primary vertices
    _tree->Branch("pvNDOF", &_pvNDOF, "pvNDOF/I"); // number of degrees of freedom of the primary vertex
    _tree->Branch("pvZ", &_pvZ, "pvZ/F"); // z component of the primary vertex
    _tree->Branch("pvRho", &_pvRho, "pvRho/F"); // rho of the primary vertex (projection on transverse plane)
  }

}


// destructor
Analyzer::~Analyzer()
{
}


//
// member functions
//

// initialise event variables with needed default (zero) values; called in the beginning of each event
void Analyzer::InitBranchVars()
{
  _evRunNumber = 0;
  _evEventNumber = 0;
  _Nmu = 0;
  _Npv= 0;
  _pvNDOF = 0;
  _pvZ = 0;
  _pvRho = 0;
}

// Store event info (fill corresponding tree variables)
int Analyzer::SelectEvent(const edm::Event& iEvent)
{
  _evRunNumber = iEvent.id().run();
  _evEventNumber = iEvent.id().event();
  return 0;
}

// muon selection
int Analyzer::SelectMu(const edm::Handle<reco::TrackCollection>& muons, const reco::VertexCollection::const_iterator& pv)
{
  using namespace std;
  _Nmu = 0;
  _Nmu0 = 0;
  // loop over muons
  for (reco::TrackCollection::const_iterator it = muons->begin(); it != muons->end(); it++)
  {
	_Nmu0++;
    if(_Nmu == _maxNmu)
    {
      printf("Maximum number of muons %d reached, skipping the rest\n", _maxNmu);
      return 0;
    }
    _muHitsValid[_Nmu] = 0;
    _muHitsPixel[_Nmu] = 0;
    const reco::HitPattern& p = it->hitPattern();
    for (int i = 0; i < p.numberOfHits(); i++) 
    {
      uint32_t hit = p.getHitPattern(i);
      if (p.validHitFilter(hit) && p.pixelHitFilter(hit))
        _muHitsPixel[_Nmu]++;
      if (p.validHitFilter(hit))
        _muHitsValid[_Nmu]++;
    }
    // fill three momentum (pT, eta, phi)
    _muPt[_Nmu] = it->pt();// * it->charge();
    _muEta[_Nmu] = it->eta();
    _muPhi[_Nmu] = it->phi();
    _muC[_Nmu]=it->charge();
    // fill chi2/ndof
    if (it->ndof()) _muTrackChi2NDOF[_Nmu] = it->chi2() / it->ndof();
    // fill distance to primary vertex
    _muDistPV0[_Nmu] = TMath::Sqrt(TMath::Power(pv->x() - it->vx(), 2.0) + TMath::Power(pv->y() - it->vy(), 2.0));
    _muDistPVz[_Nmu] = TMath::Abs(pv->z() - it->vz());
    // store muon
    _Nmu++;
    // determine muon sign (in the end the event will be stored only there are opposite signed leptons)
    if(it->charge() == +1)
        _signLeptonP = 1;
    if(it->charge() == -1)
        _signLeptonM = 1;
  }
	cout<<"Muons before selection: "<<_Nmu0<<endl;
  return 0;
}

// select primary vertex
int Analyzer::SelectPrimaryVertex(const edm::Handle<reco::VertexCollection>& primVertex)
{
  // if no primary vertices in the event, return false status
  if(primVertex->size() == 0)
    return false;
  // take the first primary vertex
  reco::VertexCollection::const_iterator pv = primVertex->begin();
  // fill z and rho (projection on transverse plane)
  _pvZ = pv->z();
  _pvRho = TMath::Sqrt(TMath::Power(pv->x(), 2.0) + TMath::Power(pv->y(), 2.0));
  // fill number of primary veritces
  _Npv = primVertex->size();
  // fill number of degrees of freedom
  _pvNDOF = pv->ndof();
  // return true status
  return true;
}

// fill 4-momentum (p) with provided particle pointer
void Analyzer::FillFourMomentum(const reco::Candidate* particle, float* p)
{
  // if NULL pointer provided, initialise with default (zero)
  if(particle == NULL)
  {
    p[0] = p[1] = p[2] = p[3] = 0.0;
    return;
  }
  
  p[0] = particle->px();
  p[1] = particle->py();
  p[2] = particle->pz();
  p[3] = particle->mass();
}

// select MC generator level information
// (analysis specific ttbar dileptonic decay)

// ------------ method called for each event  ------------
void Analyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
	using namespace reco;
	using namespace std;

  // event counting, printout after each 1K processed events
  _nevents++;
  //printf("*** EVENT %6d ***\n", _nevents);
  if( (_nevents % 1000) == 0)
  {
    //printf("*****************************************************************\n");
    printf("************* NEVENTS = %d K, selected = %d *************\n", _nevents / 1000, _neventsSelected);
    //printf("*****************************************************************\n");
  }
  //return;
  
  // declare event contents
  Handle<reco::VertexCollection> primVertex;
  edm::Handle<reco::TrackCollection> muons;

  // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  // >>>>>>>>> event selection >>>>>>>>>
  // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  //
  // initialise event variables with default values
  InitBranchVars();
  // process generator level, if needed
  // process reco level, if needed
  if(_flagRECO)
  {
    // primary vertex
    iEvent.getByLabel(_inputTagPrimaryVertex, primVertex);
    reco::VertexCollection::const_iterator pv = primVertex->begin();
    // muons
    iEvent.getByLabel(_inputTagMuons, muons);
    SelectMu(muons, pv);
    // fill primary vertex
    SelectPrimaryVertex(primVertex);
  }
  // fill event info
  SelectEvent(iEvent);
  // all done: store event
  _tree->Fill();
  _neventsSelected++;
}


// ------------ method called when starting to processes a run  ------------
void Analyzer::beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup)
{
}

// below is some default stuff, was not modified

// ------------ method called once each job just before starting event loop  ------------
void Analyzer::beginJob() {;}

// ------------ method called once each job just after ending the event loop  ------------
void Analyzer::endJob() {;}

// ------------ method called when ending the processing of a run  ------------
void Analyzer::endRun(edm::Run const& run, edm::EventSetup const& setup) {;}

// ------------ method called when starting to processes a luminosity block  ------------
void Analyzer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) {;}

// ------------ method called when ending the processing of a luminosity block  ------------
void Analyzer::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) {;}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void Analyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(Analyzer);
