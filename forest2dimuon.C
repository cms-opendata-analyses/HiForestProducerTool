#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TLorentzVector.h>
#include <iostream>
/*#include <algorithm>
#include <vector>

using std::stringstream;
using std::vector;
using std::abs;
*/
void forest2dimuon(){  

	using namespace std;
	TString fname = "HiForestAOD_DATAtest2011.root";
	TString trig = "HLT_HIL1DoubleMuOpen_v2"; //check the name of the trigger in the output root file and put what you want to use
	TString Collection = "demo"; 
	Int_t nevt=-1;
	TFile *f1 = new TFile(fname.Data());
	Float_t mumass=0.105658;
	TTree *HltTree = (TTree*)f1->Get("hltanalysis/HltTree");
	TTree *MuTree = (TTree*)f1->Get(Form("%s/Muons",Collection.Data()));
	TCanvas *c1 = new TCanvas("c1","DiMuone",1200,800);
	TH1F *dimu_h = new TH1F("dimu_h","dimu_h",50,0,10);
//	dimu_h->SetStats(kFALSE);
        dimu_h->GetXaxis()->SetTitle("M_{Inv} [GeV]");
        dimu_h->GetYaxis()->SetTitle("Events");
        c1->SetFrameLineColor(1);
        c1->SetFrameFillColor(0);
	c1->SetFillColor(10);
	dimu_h->Sumw2();
	MuTree->AddFriend(HltTree);
	Int_t           trigBit;
	TBranch        *b_trigBit;
	if (trig == "" ) {  cout << " No Trigger selection! " << endl ;}     
	else { 
		HltTree->SetBranchAddress(trig.Data(), &trigBit, &b_trigBit);
	}
	/// Muon inputs : 
	    Int_t           nMu;
	    Float_t MuPt[10];
	    Float_t MuC[10];
	    Float_t MuEta[10];
	    Float_t MuPhi[10];
	    Int_t MuHitsV[10];
	    Int_t MuHitsP[10];
	    Float_t MuTrackChi[10];
	    Float_t MuDistPVz[10];
	    TBranch        *b_nMu;   //!
	    TBranch        *b_MuPt;   //!
	    TBranch        *b_MuC;   //!
	    TBranch        *b_MuEta;   //!
	    TBranch        *b_MuPhi;   //!
	    TBranch        *b_MuHitsV;   //!
	    TBranch        *b_MuHitsP;   //!
	    TBranch        *b_MuTrackChi;   //!
	    TBranch        *b_MuDistPVz;   //!

	    MuTree->SetBranchAddress("Nmu", &nMu, &b_nMu);
	    MuTree->SetBranchAddress("muPt", MuPt, &b_MuPt);
	    MuTree->SetBranchAddress("muC", MuC, &b_MuC);
	    MuTree->SetBranchAddress("muEta", MuEta, &b_MuEta);
	    MuTree->SetBranchAddress("muPhi", MuPhi, &b_MuPhi);
	    MuTree->SetBranchAddress("muHitsValid", MuHitsV, &b_MuHitsV);
	    MuTree->SetBranchAddress("muHitsPixel", MuHitsP, &b_MuHitsP);
	    MuTree->SetBranchAddress("muTrackChi2NDOF", MuTrackChi, &b_MuTrackChi);
	    MuTree->SetBranchAddress("muDistPV0", MuDistPVz, &b_MuDistPVz);

	////////////////////////////////////////////////////////////////////////
	//////////////////  dijet tree 
	////////////////////////////////////////////////////////////////////////
	TLorentzVector v1, v2, dimu; //4-vectors for muons and dimuon
	if(nevt == -1) nevt = HltTree->GetEntries(); 
	cout << "Events to Analyze: "<<nevt<<endl;
	for (Int_t iev=0; iev<nevt; iev++) {
		if(iev%100000==0)
		{ 
			cout << ">>>>> EVENT " << iev << endl; 
		}
		HltTree->GetEntry(iev);
		if ( (trig != "" ) && (!trigBit) ) {
		continue;					//check if the trigger is fired
		}
		MuTree->GetEntry(iev);
		if (nMu<2)continue;				//We need at least 2 muons in an event
	for (Int_t i=1;i<nMu;i++){
//		if (MuHitsV[i]<12) continue;			//Muon Selections
//		if (MuHitsP[i]<2) continue;			//
//		if (MuTrackChi[i]>4.0) continue;		//
//		if (MuDistPVz[i]>0.05) continue;		//
//		if (MuPt[i]<1.4) continue;			//
//		if (abs(MuEta[i])>2.4) continue;		//
		for (Int_t j=0;j<i;j++){				//loop over 2nd muond
//			if (MuHitsV[j]<12) continue;		//Muon Selections
//			if (MuHitsP[j]<2) continue;		//
//			if (MuTrackChi[j]>4.0) continue;	//
//			if (MuDistPVz[j]>0.05) continue;	//
//			if (MuPt[j]<1.4) continue;		//
//			if (abs(MuEta[j])>2.4) continue;	//
			if (MuC[i]>0&&MuC[j]>0) continue;	//Only opposite charge muons
			if (MuC[i]<0&&MuC[j]<0) continue;	//
		v1.SetPtEtaPhiM( MuPt[i], MuEta[i], MuPhi[i], mumass );
		v2.SetPtEtaPhiM( MuPt[j], MuEta[j], MuPhi[j], mumass );
		dimu=v1+v2;
		dimu_h->Fill(dimu.M());
		}
	}
	} //end of event loop
	dimu_h->Draw("P");
} 

