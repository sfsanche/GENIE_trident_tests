void plot_joint(void)
{
	//Generic Canvas
	TCanvas c;
	c.SetLogy();
	
	//Get Pointers to files
	TFile * f_all = new TFile("all_events/all_events_NuMI_C12.root","read");
	TFile * f_dilep = new TFile("dilepton/dilepton_background_NuMI_C12.root","read");
	TFile * f_dimuon = new TFile("dimuons/dimuons_NuMI_C12.root","read");

	//Get pointers to relevant histograms

		//Particles
		TH1D * all_h_part   =   (TH1D*)f_all->Get("histo_part");
		TH1D * dilep_h_part =   (TH1D*)f_dilep->Get("histo_part");
		TH1D * dimuon_part  =   (TH1D*)f_dimuon->Get("histo_part");

		//Particles Cut
		TH1D * all_h_part_cut   =   (TH1D*)f_all->Get("histo_part_cut");
		TH1D * dilep_h_part_cut =   (TH1D*)f_dilep->Get("histo_part_cut");
		TH1D * dimuon_part_cut  =   (TH1D*)f_dimuon->Get("histo_part_cut");


		//Reactions
		TH1D * all_h_rx   =   (TH1D*)f_all->Get("histo_rx");
		TH1D * dilep_h_rx =   (TH1D*)f_dilep->Get("histo_rx");
		TH1D * dimuon_rx  =   (TH1D*)f_dimuon->Get("histo_rx");

		//Reactions Cut
		TH1D * all_h_rx_cut   =   (TH1D*)f_all->Get("histo_rx_cut");
		TH1D * dilep_h_rx_cut =   (TH1D*)f_dilep->Get("histo_rx_cut");
		TH1D * dimuon_rx_cut  =   (TH1D*)f_dimuon->Get("histo_rx_cut");

		//Scattering Type
		TH1D * all_h_scat   =   (TH1D*)f_all->Get("histo_scat");
		TH1D * dilep_h_scat =   (TH1D*)f_dilep->Get("histo_scat");
		TH1D * dimuon_scat  =   (TH1D*)f_dimuon->Get("histo_scat");

		//Interaction Type
		TH1D * all_h_int   =   (TH1D*)f_all->Get("histo_int");
		TH1D * dilep_h_int =   (TH1D*)f_dilep->Get("histo_int");
		TH1D * dimuon_int  =   (TH1D*)f_dimuon->Get("histo_int");



	//Nicely Format the histograms

		//Particles
		hist_format(all_h_part  ,  kCyan, all_h_part  -> GetEntries() );
		hist_format(dilep_h_part,  kBlue, dilep_h_part-> GetEntries() );
		hist_format(dimuon_part ,  kRed , dimuon_part -> GetEntries() );

		//Particles Cut
		hist_format(all_h_part_cut   ,  kCyan, all_h_part  -> GetEntries() );
		hist_format(dilep_h_part_cut ,  kBlue, dilep_h_part-> GetEntries() );
		hist_format(dimuon_part_cut  ,  kRed , dimuon_part -> GetEntries() );

		//Reactions
		hist_format(all_h_rx   ,  kCyan, all_h_rx   -> GetEntries() );
		hist_format(dilep_h_rx ,  kBlue, dilep_h_rx -> GetEntries() );
		hist_format(dimuon_rx  ,  kRed , dimuon_rx  -> GetEntries() );

		//Reactions Cut
		hist_format(all_h_rx_cut   ,  kCyan, all_h_rx  -> GetEntries() );
		hist_format(dilep_h_rx_cut ,  kBlue, dilep_h_rx-> GetEntries() );
		hist_format(dimuon_rx_cut  ,  kRed , dimuon_rx -> GetEntries() );

		//Scattering Type
		hist_format(all_h_scat   ,  kCyan, all_h_scat   -> GetEntries());
		hist_format(dilep_h_scat ,  kBlue, dilep_h_scat -> GetEntries());
		hist_format(dimuon_scat  ,  kRed , dimuon_scat  -> GetEntries());

		//Interaction Type
		hist_format(all_h_int   ,  kCyan, all_h_int   -> GetEntries());
		hist_format(dilep_h_int ,  kBlue, dilep_h_int -> GetEntries());
		hist_format(dimuon_int  ,  kRed , dimuon_int  -> GetEntries());
	
	//Now draw everything in canvases
		
		//Particles
		all_h_part  -> Draw();
		dilep_h_part-> Draw("same");
		dimuon_part -> Draw("same");
		c.BuildLegend();
		c.SaveAs("particles.png");

		//Particles Cut
		all_h_part_cut   -> Draw();
		dilep_h_part_cut -> Draw("same");
		dimuon_part_cut  -> Draw("same");
		c.SaveAs("particles_cut.png");


		//Reactions
		all_h_rx   -> Draw();
		dilep_h_rx -> Draw("same");
		dimuon_rx  -> Draw("same");
		c.SaveAs("reactions.png");

		//Reactions Cut
		all_h_rx_cut  -> Draw();
		dilep_h_rx_cut-> Draw("same");
		dimuon_rx_cut -> Draw("same");
		c.SaveAs("reactions_cut.png");

		//Scattering Type
		all_h_scat   -> Draw();
		dilep_h_scat -> Draw("same");
		dimuon_scat  -> Draw("same");
		c.SaveAs("scattering.png");

		//Interaction Type
		all_h_int   -> Draw();
		dilep_h_int -> Draw("same");
		dimuon_int  -> Draw("same");
		c.SaveAs("interaction.png");



}

//Format histograms in a nice way
void hist_format(TH1D* h, int color, Double_t factor)
{
	h->Scale(factor);
	h->SetFillColor(0);
	h->SetLineWidth(3);
	h->SetLineColor(color);
	return;
}

