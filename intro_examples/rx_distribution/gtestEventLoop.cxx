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
#include <TH1.h>
#include <TH3.h>
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

int    gOptNEvt;
string gOptInpFilename;

//___________________________________________________________________
int main(int argc, char ** argv)
{
  GetCommandLineArgs (argc, argv);

  //-- open the ROOT file and get the TTree & its header
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

	//...............File management................
	TFile results_file("rx_distribution_NuMI_C12.root","recreate");
	ofstream Nu_report;
	Nu_report.open("NuMI_C12_report.txt");
	Nu_report.precision(1);
	Nu_report << std::fixed;

	Nu_report << "Event Generation Report for NuMI beam + C12\n\n";
	Nu_report << "Number of analized events: " << nev << "\n";

	//.........General Histogram: Process&Rx.........
	TH3F * h = new TH3F();
	h->SetName("h");
	
	//.............Particles histogram...............
	TH1D * histo_part = new TH1D();
	histo_part->SetName("histo_part");
	
	TH1D * histo_part_cut = new TH1D();
	histo_part_cut->SetName("histo_part_cut");



  //
  // Loop over all events
  //
  for(int i = 0; i < nev; i++)
  
  {
    // get next tree entry
    tree->GetEntry(i);

    // get the GENIE event
    EventRecord &  event = *(mcrec->event);

	//.................Get the Reaction...................
	string reactives = "";
	string products = "";
	string rx = "";
    
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

		histo_part->Fill(fsl->Name().c_str(),1);
		histo_part->Fill(isp->Name().c_str(),1);

	reactives = isl->Name() + " + "+ ist->Name();
	products = fsl->Name() + " + "+ isp->Name();
	rx = reactives + " -> " + products;

	rx = rep_string(rx,"proton","p");
	rx = rep_string(rx,"neutron","n");
	
	//histo_rx->Fill(rx.c_str(),1);

	
	//.............Get Scattering and Interaction...........
	
	Interaction * in = event.Summary();
	const ProcessInfo & proc = in -> ProcInfo();

	string scat = proc.ScatteringTypeAsString();
	string interact = proc.InteractionTypeAsString();
	
	//histo_int->Fill(interact.c_str(),1);
	//histo_scat->Fill(scat.c_str(),1);

	h->Fill(scat.c_str(),interact.c_str(),rx.c_str(),1.);

	// Loop over all particles in this event
    
	/*
	
	GHepParticle * p = 0;
    TIter event_iter(&event);

    while((p=dynamic_cast<GHepParticle *>(event_iter.Next())))
    {
		int coef(1);
		
		if(p->Status()==kIStInitialState)
		{
			reactives += " + ";
			reactives += p->Name();
		}

		if(p->Status()==kIStStableFinalState)
		{
			string name = p->Name();
			int prvpos= products.find(name);
			
			if(prvpos>=0)		//found previous ocurrence
			{
				int dcoef(0);		//coeff # of digits
				while(isdigit(products[prvpos-1-dcoef])) dcoef++;
				
				if(dcoef>0)				//coeff is 2 at least
				{
					int coef = std::stoi(products.substr(prvpos-dcoef,dcoef).c_str()); 
					products=products.erase(prvpos-dcoef,dcoef);
					products=products.insert(prvpos-dcoef,std::to_string(coef+1));
				}
				else					//coeff is 1 (not written)
				{
					products=products.insert(prvpos,"2");
				}
			}
			else			//not previous ocurrnce, just append
			{	
				products += " + ";
				products += name;
			}
		}

		if(p->Status()==kIStFinalStateNuclearRemnant)
		{
			string name = event.Particle(p->FirstMother())->Name();
			int prvpos= products.find(name);
			
			if(prvpos>=0)		//found previous ocurrence
			{
				int dcoef(0);		//coeff # of digits
				while(isdigit(products[prvpos-1-dcoef])) dcoef++;
				
				if(dcoef>0)				//coeff is 2 at least
				{
					int coef = std::stoi(products.substr(prvpos-dcoef,dcoef).c_str()); 
					products=products.erase(prvpos-dcoef,dcoef);
					products=products.insert(prvpos-dcoef,std::to_string(coef+1));
				}
				else					//coeff is 1 (not written)
				{
					products=products.insert(prvpos,"2");
				}
			}
			else			//not previous ocurrnce, just append
			{	
				products += " + ";
				products += name;
			}
		}

    }// end loop over particles	


	//Give the rx string an appropiate form
	reactives=reactives.erase(0,3);		//delete first "+"
	products=products.erase(0,3);		//delete first "+"
	*/
	

	// clear current mc event record
    mcrec->Clear();

	LOG("myAnalysis", pNOTICE) "Event: "<< i << " done";
  }


  // close input GHEP event file_
  file.Close();

	
	LOG("myAnalysis", pNOTICE) << "Reorganizing histograms...";
	gStyle->SetPaintTextFormat("1.2f");

	//..........Main histogram projections.........
	
	h->Scale(1./(float)h->GetEntries());
	
	TH1D * histo_scat = h->ProjectionX("histo_scat");
	TH1D * histo_int = h->ProjectionY("histo_int");
	TH1D * histo_rx = h->ProjectionZ("histo_rx");
	
		//Cut Rx histogram
		TH1D * histo_rx_cut = new TH1D();
		histo_rx_cut->SetName("histo_rx_cut");


	//...........Interaction Histograms.............
	HistReorder(histo_scat);
	HistReorder(histo_int);
	
	Nu_report << "Interactions\n";
	for(int l=1;l<=histo_int->GetNbinsX();l++)
	  Nu_report << "\t"<<histo_int->GetXaxis()->GetBinLabel(l)<<": "<<histo_int->GetBinContent(l)*100.<< "%\n";
	
	Nu_report << "\nScattering types\n";
	for(int l=1;l<=histo_scat->GetNbinsX();l++)
	  Nu_report << "\t"<<histo_scat->GetXaxis()->GetBinLabel(l)<<": "<<histo_scat->GetBinContent(l)*100.<<"%\n";
	
	HistFormat(histo_scat,"Scattering type for NuMI + C12","scattering","frequency");
	HistFormat(histo_int,"Interaction type for NuMI + C12","interaction","frequency");
	
	TCanvas c3;
	c3.Divide(2,1);
	c3.cd(1);
	histo_scat->GetXaxis()->SetLabelSize(0.06);
	histo_scat->Draw();
	histo_scat->Draw("text0 same");

	c3.cd(2);
	histo_int->GetXaxis()->SetLabelSize(0.06);
	histo_int->Draw();
	histo_int->Draw("text0 same");	
	c3.SaveAs("./NuMI_C12_Interactions.png");

	
	//...........Particle Histograms...............
	HistReorder(histo_part);
	histo_part->Scale(1./(float)histo_part->GetEntries());
	TAxis * ax = histo_part->GetXaxis();
	
	Nu_report << "\nProduced Particles\n";

	for(int l=1;l<=histo_part->GetNbinsX();l++)	
	{
		string part = histo_part->GetXaxis()->GetBinLabel(l);
		Nu_report << "\t"<<histo_part->GetBinContent(l)*100.<<"%\t"<<part<<"\n";
		part = rep4tex(part);
		ax->SetBinLabel(l,part.c_str());
	}

	HistFormat(histo_part,"Produced particles for NuMI + C12","","frequency");
	histo_part->LabelsOption("v");
	TCanvas *c1 = new TCanvas();
	HistPlotInNewCanvas(histo_part,c1,"./NuMI_C12_Particles.png",800,400,0.3,0.1,0.1,0.065,"0");
	
   	  //Cut histogram
	int bincut = histo_part->FindLastBinAbove(float(0.5/100),1);
	int nbins = histo_part->GetNbinsX();
	float small_bins_sum(0);
	for(int j=bincut+1;j<=nbins;j++) small_bins_sum+=histo_part->GetBinContent(j);
	
	for(int h=1;h<=bincut;h++) histo_part_cut->Fill(ax->GetBinLabel(h),histo_part->GetBinContent(h));
	histo_part_cut->Fill("Below 0.5%",small_bins_sum);
	histo_part_cut->LabelsDeflate("X");

	HistFormat(histo_part_cut,"Produced particles for NuMI + C12, cut","","frequency");
	histo_part_cut->LabelsOption("v");
	TCanvas *c2 = new TCanvas();
	HistPlotInNewCanvas(histo_part_cut,c2,"./NuMI_C12_ParticlesCut.png",600,400,0.3,0.1,0.1,0.065,"0");


	//................Rx histograms.................
	HistReorder(histo_rx);
	
	
	Nu_report << "\nReaction Channels\n";
	for(int r=1;r<= h->GetNbinsX();r++)
	{
		Nu_report << "\n\t"<<h->GetXaxis()->GetBinLabel(r)<<"\n";
		for(int s=1;s<= h->GetNbinsY();s++)
		{
			for(int u=1; u<= h->GetNbinsZ();u++)
			{
				if (h->GetBinContent(r,s,u)!=0) 
				{
					Nu_report << "\t\t"<<h->GetYaxis()->GetBinLabel(s)<<"\n";
					break;
				}
			}
			
			for(int t=1; t<= h->GetNbinsZ();t++)
			{
				string rx = h->GetZaxis()->GetBinLabel(t);
				float content = h->GetBinContent(r,s,t);
				if (content!=0.) Nu_report << "\t\t\t"<<content*100.<<"%\t"<<rx<<"\n";
			}
		}
	}


	TAxis * ax_rx = histo_rx->GetXaxis();
	for(int l =1; l<=histo_rx->GetNbinsX();l++)
	{
		string rx = histo_rx->GetXaxis()->GetBinLabel(l);
		rx = rep4tex(rx);
		ax_rx->SetBinLabel(l,rx.c_str());
	}

	
	HistFormat(histo_rx,"Reactions produced for NuMI + C12","","frequency");
	TCanvas *c4 = new TCanvas();
	HistPlotInNewCanvas(histo_rx,c4,"./NuMI_C12_Reactions.png",900,400,0.45,0.03,0.03,0.043,"90");
	
		//Cut histogram
	int bincut_rx = histo_rx->FindLastBinAbove(0.5/100.,1);
	int nbins_rx = histo_rx->GetNbinsX();
	float small_bins_sum_rx(0);
	for(int j=bincut_rx+1;j<=nbins_rx;j++) small_bins_sum_rx+=histo_rx->GetBinContent(j);
	
	for(int h=1;h<=bincut_rx;h++) histo_rx_cut->Fill(ax_rx->GetBinLabel(h),histo_rx->GetBinContent(h));
	histo_rx_cut->Fill("Below 0.5%",small_bins_sum_rx);
	histo_rx_cut->LabelsDeflate("X");

	HistFormat(histo_rx_cut,"Reactions produced for NuMI + C12, cut","","frequency");
	histo_rx_cut->LabelsOption("v");
	TCanvas *c5 = new TCanvas();
	HistPlotInNewCanvas(histo_rx_cut,c5,"./NuMI_C12_ReactionsCut.png",650,500,0.45,0.17,0.17,0.056,"90");
	
  //Write results file
  h->Write();
  histo_part->Write();
  histo_part_cut->Write();
  histo_scat->Write();
  histo_int->Write();
  histo_rx->Write();
  histo_rx_cut->Write();
  results_file.Close();
  
  Nu_report.close();

  LOG("myAnalysis", pNOTICE)  << "Done!";

  return 0;
}
//___________________________________________________________________
void GetCommandLineArgs(int argc, char ** argv)
{
  LOG("myAnalysis", pINFO) << "Parsing commad line arguments";

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
	histo->SetFillColor(38);
	histo->SetMarkerSize(1.8);
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
//_________________________________________________________________________________
