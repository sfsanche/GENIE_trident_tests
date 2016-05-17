//____________________________________________________________________________
/*!

\brief   Example event loop. Use that as a template for your analysis code.

\author  Costas Andreopoulos <costas.andreopoulos \at stfc.ac.uk>
         STFC, Rutherford Appleton Laboratory

\created May 4, 2004

\cpright Copyright (c) 2003-2013, GENIE Neutrino MC Generator Collaboration
         For the full text of the license visit http://copyright.genie-mc.org
         or see $GENIE/LICENSE
*/
//____________________________________________________________________________

#include <string>
#include <iostream>
#include <fstream>
#include <ctype.h>

#include <TSystem.h>
#include <TFile.h>
#include <TTree.h>
#include <TIterator.h>

#include <TCanvas.h>
#include <TROOT.h>
#include <TH1.h>
#include <THStack.h>
#include <TLegend.h>
#include <TAxis.h>
#include <TStyle.h>

#include "EVGCore/EventRecord.h"
#include "GHEP/GHepParticle.h"
#include "GHEP/GHepRecord.h"
#include "Ntuple/NtpMCFormat.h"
#include "Ntuple/NtpMCTreeHeader.h"
#include "Ntuple/NtpMCEventRecord.h"
#include "Messenger/Messenger.h"
#include "PDG/PDGCodes.h"
#include "Utils/CmdLnArgParser.h"

using std::string;
using namespace genie;


void GetCommandLineArgs (int argc, char ** argv);
string rep_string(string,string,string);
string rep4tex(string);
void HistReorder(TH1D*);
void HistFormat(TH1D*, string, string, string);
void HistPlotInNewCanvas(TH1D*, TCanvas*, string, int, int, float, float, float, float, string);
void SetNiceStyle(TROOT*);

int    gOptNEvt;
string gOptInpFilename;
string gOptWeightsFilename;


//___________________________________________________________________
int main(int argc, char ** argv)
{
  GetCommandLineArgs (argc, argv);

  //-- open the ROOT Events file and get the TTree & its header
  TTree *           tree = 0;
  NtpMCTreeHeader * thdr = 0;

  TFile file(gOptInpFilename.c_str(),"READ");

  tree = dynamic_cast <TTree *>           ( file.Get("gtree")  );
  thdr = dynamic_cast <NtpMCTreeHeader *> ( file.Get("header") );

  if(!tree) return 1;

  NtpMCEventRecord * mcrec = 0;
  tree->SetBranchAddress("gmcrec", &mcrec);

  int nev = (gOptNEvt > 0) ?
        TMath::Min(gOptNEvt, (int)tree->GetEntries()) :
        (int) tree->GetEntries();



  //-- open the ROOT Weights file and get the TTree & its header
  TTree *           w_tree = 0;
//Uncomment this line and delete the subsequent one to specify weights file as an argument, ' -w /path/to/weights/file.root'
//  TFile weights_file(gOptWeightsFilename.c_str(),"READ");
  TFile weights_file("./weights_MACCQE_t5.root","READ");		

  weights_file.GetObject("MaCCQE",w_tree);
  if(!w_tree) return 1;

  TArrayF * weights = 0;
  TArrayF * twkdials = 0;

  w_tree->SetBranchAddress("weights", &weights);
  w_tree->SetBranchAddress("twkdials", &twkdials);


	//...............File management................
	TFile results_file("QES_events_distribution_NuMI_C12.root","recreate");

	int nbins = 200;

	//........Histograms: QES event frequency.........
	TH1D * histo_qescc_p3 = new TH1D("histo_qescc_p3","M_{A}+#delta",nbins,0.,20.);
	TH1D * histo_qescc_p2 = new TH1D("histo_qescc_p2","M_{A}+#frac{2}{3}#delta",nbins,0.,20.);
	TH1D * histo_qescc_p1 = new TH1D("histo_qescc_p1","M_{A}+#frac{1}{3}#delta",nbins,0.,20.);
	TH1D * histo_qescc_0 = new TH1D("histo_qescc_0","M_{A} nominal",nbins,0.,20.);
	TH1D * histo_qescc_m1 = new TH1D("histo_qescc_m1","M_{A}-#frac{1}{3}#delta",nbins,0.,20.);
	TH1D * histo_qescc_m2 = new TH1D("histo_qescc_m2","M_{A}-#frac{2}{3}#delta",nbins,0.,20.);
	TH1D * histo_qescc_m3 = new TH1D("histo_qescc_m3","M_{A}-#delta",nbins,0.,20.);

	THStack * histo_qescc = new THStack("histo_qescc","CCQE event frequency");
	histo_qescc->Add(histo_qescc_p3);
	histo_qescc->Add(histo_qescc_p2);
	histo_qescc->Add(histo_qescc_p1);
	histo_qescc->Add(histo_qescc_0);
	histo_qescc->Add(histo_qescc_m1);
	histo_qescc->Add(histo_qescc_m2);
	histo_qescc->Add(histo_qescc_m3);

	//.......Histograms: Weights distribution.................
	TH1D * histo_weights_p3 = new TH1D("histo_weights_p3","M_{A}+#delta",120,0.6,2.2);
	TH1D * histo_weights_p2 = new TH1D("histo_weights_p2","M_{A}+#frac{2}{3}#delta",120,0.6,2.2);
	TH1D * histo_weights_p1 = new TH1D("histo_weights_p1","M_{A}+#frac{1}{3}#delta",120,0.6,2.2);
	TH1D * histo_weights_0 = new TH1D("histo_weights_0","M_{A} nominal",120,0.6,2.2);
	TH1D * histo_weights_m1 = new TH1D("histo_weights_m1","M_{A}-#frac{1}{3}#delta",120,0.6,2.2);
	TH1D * histo_weights_m2 = new TH1D("histo_weights_m2","M_{A}-#frac{2}{3}#delta",120,0.6,2.2);
	TH1D * histo_weights_m3 = new TH1D("histo_weights_m3","M_{A}-#delta",120,0.6,2.2);

	THStack * histo_weights = new THStack("histo_weights","CCQE Weights Distribution");
	histo_weights->Add(histo_weights_p3);
	histo_weights->Add(histo_weights_p2);
	histo_weights->Add(histo_weights_p1);
	histo_weights->Add(histo_weights_0);
	histo_weights->Add(histo_weights_m1);
	histo_weights->Add(histo_weights_m2);
	histo_weights->Add(histo_weights_m3);

	//......Histograms: Q2 distribution by interaction type...........
	TH1D * histo_Q2_QES = new TH1D("histo_Q2_QES","Quasielastic",nbins,0.,10.);
	TH1D * histo_Q2_DIS = new TH1D("histo_Q2_DIS","Deep Inelastic",nbins,0.,10.);
	TH1D * histo_Q2_RES = new TH1D("histo_Q2_RES","Resonant",nbins,0.,10.);
	TH1D * histo_Q2_COH = new TH1D("histo_Q2_COH","Coherent",nbins,0.,10.);
	TH1D * histo_Q2_NUEEL = new TH1D("histo_Q2_NUEEL","#nu-e^{-} elastic",nbins,0.,10.);
	TH1D * histo_Q2_IMD = new TH1D("histo_Q2_IMD","Inverse #mu Decay",nbins,0.,10.);

	THStack * histo_Q2 = new THStack("histo_Q2","Q^{2} Distribution by Scattering type");
	histo_Q2->Add(histo_Q2_QES);
	histo_Q2->Add(histo_Q2_RES);
	histo_Q2->Add(histo_Q2_DIS);
	histo_Q2->Add(histo_Q2_COH);
	histo_Q2->Add(histo_Q2_NUEEL);
	histo_Q2->Add(histo_Q2_IMD);

	//......Histograms: W distribution by interaction type...........
	TH1D * histo_W_QES = new TH1D("histo_W_QES","Quasielastic",nbins,0.,10.);
	TH1D * histo_W_DIS = new TH1D("histo_W_DIS","Deep Inelastic",nbins,0.,10.);
	TH1D * histo_W_RES = new TH1D("histo_W_RES","Resonant",nbins,0.,10.);
	TH1D * histo_W_COH = new TH1D("histo_W_COH","Coherent",nbins,0.,10.);
	TH1D * histo_W_NUEEL = new TH1D("histo_W_NUEEL","#nu-e^{-} elastic",nbins,0.,10.);
	TH1D * histo_W_IMD = new TH1D("histo_W_IMD","Inverse #mu Decay",nbins,0.,10.);

	THStack * histo_W = new THStack("histo_W","Hadronic Invariant Mass (W) Distribution by Scattering type");
	histo_W->Add(histo_W_QES);
	histo_W->Add(histo_W_RES);
	histo_W->Add(histo_W_DIS);
	histo_W->Add(histo_W_COH);
	histo_W->Add(histo_W_NUEEL);
	histo_W->Add(histo_W_IMD);
  //
  // Loop over all events
  //
  //for(int i = 0; i < nev; i++)		// Original
  for(int i = 0; i < 50000; i++)		// Modified to fixed 50000 events (no more in weights file)
  {
    // get next tree entry
    tree->GetEntry(i);

    // get the GENIE event
    EventRecord &  event = *(mcrec->event);

	//.............Get Scattering and Interaction...........
	
	Interaction * in = event.Summary();
	const ProcessInfo & proc = in -> ProcInfo();

	string scat = proc.ScatteringTypeAsString();
	string interact = proc.InteractionTypeAsString();
		
	//..............Get Kinematical variables..............
	const Kinematics  & kine = in->Kine();
	double Q2s	= kine.Q2(true);			//This gets 'selected' Q2, as opposed to 'running' Q2
	double Ws	= kine.W(true);			//This gets 'selected' W, as opposed to 'running' W
	
	//................Event Selection.......................
	  if(scat=="DIS") {histo_Q2_DIS->Fill(Q2s); histo_W_DIS->Fill(Ws);}
	  else if(scat=="RES") {histo_Q2_RES->Fill(Q2s); histo_W_RES->Fill(Ws);}
	  else if(scat=="COH") {histo_Q2_COH->Fill(Q2s); histo_W_COH->Fill(Ws);}
	  else if(scat=="NUEEL") {histo_Q2_NUEEL->Fill(Q2s); histo_W_NUEEL->Fill(Ws);}
	  else if(scat=="IMD") {histo_Q2_IMD->Fill(Q2s); histo_W_IMD->Fill(Ws);}
	  else if(scat=="QES") {histo_Q2_QES->Fill(Q2s); histo_W_QES->Fill(Ws);}
	
	  //Require Specifically Quasielastic CC scattering for the rest of analysis
	  if(scat!="QES" || interact!="Weak[CC]") continue;
	
		//Ingoing particles
		GHepParticle * isl = event.Probe();
		GHepParticle * ist;
		
		if(event.HitElectron()>0) ist = event.HitElectron();
		else if(event.HitNucleon()>0) ist = event.HitNucleon();
		else ist = event.TargetNucleus();


		//..............Get outgoing particles................. 
		GHepParticle * fsl = event.FinalStatePrimaryLepton();
		GHepParticle * isp;
		if(event.HitElectron()>0) isp = event.Particle(event.HitElectron()->FirstDaughter());
		else if(event.HitNucleon()>0)isp = event.Particle(event.HitNucleon()->LastDaughter());
		else isp = event.Particle(event.TargetNucleus()->FirstDaughter());


		//.............Get Corresponding 3 weights................
		w_tree->GetEntry(i);

		//...................Fill Histograms.....................	
		histo_qescc_m3->Fill(fsl->Energy(),weights->GetAt(0)); //Weights for Ma-delta
		histo_qescc_m2->Fill(fsl->Energy(),weights->GetAt(1)); //Weights for Ma-delta
		histo_qescc_m1->Fill(fsl->Energy(),weights->GetAt(2)); //Weights for Ma-delta
		histo_qescc_0->Fill(fsl->Energy(),weights->GetAt(3));  //Weights should be one... cheched, they are
		histo_qescc_p1->Fill(fsl->Energy(),weights->GetAt(4)); //Weights for Ma+delta
		histo_qescc_p2->Fill(fsl->Energy(),weights->GetAt(5)); //Weights for Ma+delta
		histo_qescc_p3->Fill(fsl->Energy(),weights->GetAt(6)); //Weights for Ma+delta

		histo_weights_m3->Fill(weights->GetAt(0));
		histo_weights_m2->Fill(weights->GetAt(1));
		histo_weights_m1->Fill(weights->GetAt(2));
		histo_weights_0->Fill(weights->GetAt(3));
		histo_weights_p1->Fill(weights->GetAt(4));
		histo_weights_p2->Fill(weights->GetAt(5));
		histo_weights_p3->Fill(weights->GetAt(6));
	

	// clear current mc event record
   	mcrec->Clear();

	//LOG("myAnalysis", pNOTICE) "Event: "<< i << " done";
  }


  // close input GHEP event file_
  file.Close();
  weights_file.Close();

  //Nicely Plot and save the results
  LOG("myAnalysis", pNOTICE) << "Plotting results...";
//  SetNiceStyle(gROOT);


  TCanvas * c1 = new TCanvas("c1","c1",1024,740);
  histo_qescc_p3->SetLineColor(kRed+5);
  histo_qescc_p2->SetLineColor(kRed+3);
  histo_qescc_p1->SetLineColor(kRed);
  histo_qescc_0->SetLineColor(kBlue);
  histo_qescc_m1->SetLineColor(kGreen);
  histo_qescc_m2->SetLineColor(kGreen-3);
  histo_qescc_m3->SetLineColor(kGreen-5);

  histo_qescc->Draw("nostack");			//Draw must be called to define axis before defining its titles
  histo_qescc->GetXaxis()->SetTitle("Energy (GeV)");
  histo_qescc->GetYaxis()->SetTitle("events");
  TLegend * leg_histo_qescc = (TLegend*) c1->BuildLegend();
  leg_histo_qescc->SetNColumns(2);
  leg_histo_qescc->SetTextSize(0.03);

  c1->SaveAs("CCQE_Axial_Mass_Reweighting_results.png");  

  //..
  TCanvas * c2 = new TCanvas("c2","c2",1024,740);
  histo_weights_p3->SetLineColor(kRed+5);
  histo_weights_p2->SetLineColor(kRed+3);
  histo_weights_p1->SetLineColor(kRed);
  histo_weights_m1->SetLineColor(kGreen);
  histo_weights_m2->SetLineColor(kGreen-3);
  histo_weights_m3->SetLineColor(kGreen-5);

  histo_weights->Draw("nostack");
  histo_weights->GetXaxis()->SetTitle("Weight factor");
  histo_weights->GetYaxis()->SetTitle("");
  TLegend * leg_histo_weights= (TLegend*) c2->BuildLegend();
  leg_histo_weights->SetNColumns(2);
  leg_histo_weights->SetTextSize(0.03);

  c2->SaveAs("CCQE_Axial_Mass_Reweighting_Weights.png");  



  //..
  TCanvas * c3 = new TCanvas("c3","c3",1024,740);
  histo_Q2_QES->SetLineColor(kRed);	
  histo_Q2_DIS->SetLineColor(kBlue);	
  histo_Q2_RES->SetLineColor(kGreen);	
  histo_Q2_COH->SetLineColor(kOrange+3);	
  histo_Q2_NUEEL->SetLineColor(kTeal);	
  histo_Q2_IMD->SetLineColor(kOrange);	

  histo_Q2_QES->SetLineWidth(3);	
  histo_Q2_DIS->SetLineWidth(3);	
  histo_Q2_RES->SetLineWidth(3);	
  histo_Q2_COH->SetLineWidth(3);	
  histo_Q2_NUEEL->SetLineWidth(3);	
  histo_Q2_IMD->SetLineWidth(3);	

/*histo_Q2_QES->SetFillColor(30);	
  histo_Q2_DIS->SetFillColor(38);	
  histo_Q2_RES->SetFillColor(40);	
  histo_Q2_COH->SetFillColor(42);	
  histo_Q2_NUEEL->SetFillColor(46);	
  histo_Q2_IMD->SetFillColor(48);	
*/  
  histo_Q2->Draw("nostack");
  histo_Q2->GetXaxis()->SetTitle("GeV^{2}");
  histo_Q2->GetYaxis()->SetTitle("");
  c3->BuildLegend();
  c3->SaveAs("Q2_distribution.png");  

  //..
  TCanvas * c4 = new TCanvas("c4","c4",1024,740);
  histo_W_QES->SetLineColor(kRed);	
  histo_W_DIS->SetLineColor(kBlue);	
  histo_W_RES->SetLineColor(kGreen);	
  histo_W_COH->SetLineColor(kOrange+3);	
  histo_W_NUEEL->SetLineColor(kTeal);	
  histo_W_IMD->SetLineColor(kOrange);	

  histo_W_QES->SetLineWidth(3);	
  histo_W_DIS->SetLineWidth(3);	
  histo_W_RES->SetLineWidth(3);	
  histo_W_COH->SetLineWidth(3);	
  histo_W_NUEEL->SetLineWidth(3);	
  histo_W_IMD->SetLineWidth(3);	

/*histo_W_QES->SetFillColor(30);	
  histo_W_DIS->SetFillColor(38);	
  histo_W_RES->SetFillColor(40);	
  histo_W_COH->SetFillColor(42);	
  histo_W_NUEEL->SetFillColor(46);	
  histo_W_IMD->SetFillColor(48);	
*/  
  histo_W->Draw("nostack");
  histo_W->GetXaxis()->SetTitle("GeV");
  histo_W->GetYaxis()->SetTitle("");
  c4->BuildLegend();
  c4->SaveAs("W_distribution.png");  


  //Save all histograms 
  c1->Write();
  c2->Write();
  c3->Write();
  c4->Write();
  
  results_file.Write();
  results_file.Close();
  
  LOG("myAnalysis", pNOTICE)  << "Done!";

  return 0;
}
//___________________________________________________________________
void GetCommandLineArgs(int argc, char ** argv)
{
  LOG("myAnalysis", pINFO) << "Parsing command line arguments";

  CmdLnArgParser parser(argc,argv);

  // get GENIE event sample
  if( parser.OptionExists('f') ) {
    LOG("myAnalysis", pINFO) 
       << "Reading event sample filename";
    gOptInpFilename = parser.ArgAsString('f');
  } else {
    LOG("myAnalysis", pFATAL) 
        << "Unspecified input filename - Exiting";
    exit(1);
  }


  // get GENIE event sample
  if( parser.OptionExists('w') ) {
    LOG("myAnalysis", pINFO) 
       << "Reading event weights filename";
    gOptWeightsFilename = parser.ArgAsString('w');
  } 
 /* 
  else {
    LOG("myAnalysis", pFATAL) 
        << "Unspecified input filename - Exiting";
    exit(1);
  }
*/

  // number of events to analyse
  if( parser.OptionExists('n') ) {
    LOG("myAnalysis", pINFO) 
      << "Reading number of events to analyze";
    gOptNEvt = parser.ArgAsInt('n');
  } else {
    LOG("myAnalysis", pINFO)
      << "Unspecified number of events to analyze - Use all";
    gOptNEvt = -1;
  }
}

string rep_string(string haystack, string needle, string nail)
{
	int hitpos(0);
	
	while(1)
	{
		hitpos = haystack.find(needle);
		if(hitpos>=0)
			haystack = haystack.replace(hitpos,needle.size(),nail);
		else break;
	}
	
	return haystack;
}

string rep4tex(string texifyme)
{
		texifyme = rep_string(texifyme,"proton","p");
		texifyme = rep_string(texifyme,"neutron","n");
		texifyme = rep_string(texifyme,"nu_e_bar","#bar{#nu}_{e}");
		texifyme = rep_string(texifyme,"nu_mu_bar","#bar{#nu}_{#mu}");
		texifyme = rep_string(texifyme,"nu_e","#nu_{e}");
		texifyme = rep_string(texifyme,"nu_mu","#nu_{#mu}");
		texifyme = rep_string(texifyme,"nu_tau","#nu_{#tau}");
		texifyme = rep_string(texifyme,"mu+","#mu^{+}");
		texifyme = rep_string(texifyme,"e+","e^{+}");
		texifyme = rep_string(texifyme,"mu-","#mu");
		texifyme = rep_string(texifyme,"e-","e");
		texifyme = rep_string(texifyme,"tau-","#tau");
		texifyme = rep_string(texifyme,"pi0","#pi^{0}");
		texifyme = rep_string(texifyme,"pi+","#pi^{+}");
		texifyme = rep_string(texifyme,"pi-","#pi^{-}");
		texifyme = rep_string(texifyme,"K0_bar","#bar{#kappa^{0}}");
		texifyme = rep_string(texifyme,"K0","#kappa^{0}");
		texifyme = rep_string(texifyme,"K+","#kappa^{+}");
		texifyme = rep_string(texifyme,"K-","#kappa^{-}");
		texifyme = rep_string(texifyme,"C12","{}^{12}C");
		texifyme = rep_string(texifyme,"C11","{}^{11}C");
		texifyme = rep_string(texifyme,"B11","{}^{11}B");
		texifyme = rep_string(texifyme,"Ar40","{}^{40}Ar");
		texifyme = rep_string(texifyme,"Ar39","{}^{39}Ar");
		texifyme = rep_string(texifyme,"Cl39","{}^{39}Cl");
		texifyme = rep_string(texifyme,"Sigma_c++","#Sigma^{++}_{c}");
		texifyme = rep_string(texifyme,"Sigma_c+","#Sigma^{+}_{c}");
		texifyme = rep_string(texifyme,"Sigma_c0","#Sigma^{0}_{c}");
		texifyme = rep_string(texifyme,"Sigma_c-","#Sigma^{-}_{c}");
		texifyme = rep_string(texifyme,"Delta++","#Delta^{++}");
		texifyme = rep_string(texifyme,"Delta+","#Delta^{+}");
		texifyme = rep_string(texifyme,"Delta0","#Delta^{0}");
		texifyme = rep_string(texifyme,"Delta-","#Delta^{-}");
		texifyme = rep_string(texifyme,"N0","N^{0}");
		texifyme = rep_string(texifyme,"N+","N^{+}");
		texifyme = rep_string(texifyme,"N-","N^{-}");
		texifyme = rep_string(texifyme,"Lambda0","#Lambda^{0}");
		texifyme = rep_string(texifyme,"Lambda_c+","#Lambda^{+}_{c}");
		texifyme = rep_string(texifyme,"gamma","photon");
		texifyme = rep_string(texifyme,"photon","#gamma");
		texifyme = rep_string(texifyme,"D0","D_{0}");
		texifyme = rep_string(texifyme,"D_s","D_{s}");
		texifyme = rep_string(texifyme,"->","#rightarrow");

		return texifyme;
}

void HistReorder (TH1D* histo)
{
	histo->LabelsOption(">","X");
	histo->LabelsDeflate("X");
	//histo->Scale(1./(float)histo->GetEntries());
	return;
}

void HistFormat(TH1D* histo, string title, string xaxis, string yaxis)
{
	histo->SetTitle(title.c_str());
	histo->SetStats(kFALSE);
	histo->SetXTitle(xaxis.c_str());
	histo->SetYTitle(yaxis.c_str());
	//histo->SetFillColor(38);
	//histo->SetMarkerSize(1.8);
	return;
}

void HistPlotInNewCanvas (TH1D* histo, TCanvas* canvas, string name,int width,int height,float botmarg, float rightmarg, float leftmarg, float labelsize, string textangle)
{
	canvas->SetCanvasSize(width,height);
	canvas->SetBottomMargin(botmarg);
	canvas->SetRightMargin(rightmarg);
	canvas->SetLeftMargin(leftmarg);
	histo->GetXaxis()->SetLabelSize(labelsize);
	histo->Draw();
	string fmt = "text"+textangle+" same";
	histo->Draw(fmt.c_str());
	canvas->SaveAs(name.c_str());
	return;
}


void SetNiceStyle(TROOT* gROOT)
{
	int fontcode = 132;

	TStyle *myStyle  = new TStyle("MyStyle","My Root Styles");


    myStyle->SetPaintTextFormat("1.2f");

	// from ROOT plain style
	myStyle->SetCanvasBorderMode(0);
	myStyle->SetPadBorderMode(0);
	myStyle->SetFrameBorderMode(0);
	myStyle->SetPadColor(0);
	myStyle->SetCanvasColor(0);
	myStyle->SetTitleColor(0);
	myStyle->SetTitleFillColor(0);
	myStyle->SetStatColor(0);

	myStyle->SetLabelSize(0.03,"xyz"); // size of axis values

	// default canvas positioning
	myStyle->SetCanvasDefX(900);
	myStyle->SetCanvasDefY(20);
	myStyle->SetCanvasDefH(550);
	myStyle->SetCanvasDefW(540);

	myStyle->SetPadBottomMargin(0.1);
	myStyle->SetPadTopMargin(0.1);
	myStyle->SetPadLeftMargin(0.1);
	myStyle->SetPadRightMargin(0.1);


	//myStyle->SetPadTickX(1);
	//myStyle->SetPadTickY(1);

	//For Histograms
	myStyle->SetHistLineWidth(40);
    myStyle->SetLabelFont(fontcode,"xyz");
    myStyle->SetLegendFont(fontcode);
    myStyle->SetStatFont(fontcode);
    myStyle->SetTitleFont(fontcode);

	// US letter
	//myStyle->SetPaperSize(20, 24);


	gROOT->SetStyle("MyStyle"); //uncomment to set this style

}

//_________________________________________________________________________________
