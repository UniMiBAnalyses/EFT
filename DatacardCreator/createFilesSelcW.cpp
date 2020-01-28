/*
createFilesSelcW creates 14 root files for the likelihood scans, using the 11 kinetic variables
in the root file "ntuple_RcW_0p3_HS_2.root" (EFT 6th-order operator: Q_W), two derived distributions
(deltaetajj, deltaphijj) and one "no shape" distribution, filled with ones with the corresponding weights.
Preselections are applied. Every root file contains three histograms, which represent the SM term ("histo_linear"),
the linear term ("histo_linear") and the quadratic term ("histo_quadratic") of the total EFT distribution.

"./createFilesSelcW variable" plots "variable" with a test value of cW

c++ -o createFilesSelcW createFilesSelcW.cpp `root-config --glibs --cflags`

*/

#include <iostream>
#include <string>
#include <iomanip>

#include <TFile.h>
#include <TNtuple.h>
#include <TTreeReader.h>
#include <TH1.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLegend.h>
#include <THStack.h>
#include <TText.h>

using namespace std ;


int main (int argc, char** argv)
{
	TH1::SetDefaultSumw2();
	cout << setprecision(7) << fixed;
	gStyle->SetLabelSize(.04, "XY");
	const char* kinetic_variable_plot = "met";
	if (argc > 1) kinetic_variable_plot = argv[1];

	TApplication* myapp = new TApplication ("myapp", NULL, NULL);
	TCanvas* cnv = new TCanvas("cnv","cnv",0,0,1200,400);
	cnv->Divide(2,1);

	vector<TFile*> files;
	TFile* f_met = new TFile ("histo_0p3_met_sel.root","recreate");
	TFile* f_mjj = new TFile ("histo_0p3_mjj_sel.root","recreate");
	TFile* f_mll = new TFile ("histo_0p3_mll_sel.root","recreate");
	TFile* f_ptl1 = new TFile ("histo_0p3_ptl1_sel.root","recreate");
	TFile* f_ptl2 = new TFile ("histo_0p3_ptl2_sel.root","recreate");
	TFile* f_ptj1 = new TFile ("histo_0p3_ptj1_sel.root","recreate");
	TFile* f_ptj2 = new TFile ("histo_0p3_ptj2_sel.root","recreate");
	TFile* f_etaj1 = new TFile ("histo_0p3_etaj1_sel.root","recreate");
	TFile* f_etaj2 = new TFile ("histo_0p3_etaj2_sel.root","recreate");
	TFile* f_phij1 = new TFile ("histo_0p3_phij1_sel.root","recreate");
	TFile* f_phij2 = new TFile ("histo_0p3_phij2_sel.root","recreate");
	TFile* f_deltaetajj = new TFile ("histo_0p3_deltaetajj_sel.root","recreate");
	TFile* f_deltaphijj = new TFile ("histo_0p3_deltaphijj_sel.root","recreate");
	TFile* f_noshape = new TFile ("histo_0p3_noshape_sel.root","recreate");

	files.push_back(f_met);
	files.push_back(f_mjj);
	files.push_back(f_mll);
	files.push_back(f_ptl1);
	files.push_back(f_ptl2);
	files.push_back(f_ptj1);
	files.push_back(f_ptj2);
	files.push_back(f_etaj1);
	files.push_back(f_etaj2);
	files.push_back(f_phij1);
	files.push_back(f_phij2);
	files.push_back(f_deltaetajj);
	files.push_back(f_deltaphijj);
	files.push_back(f_noshape);

	string name_ntuples[] = {"sm","lin","quad"};
	string name_global_numbers[] = {"sm_nums","lin_nums","quad_nums"};

	//the last two variables are actually "deltaetajj" and "deltaphijj", "met" is just to read an existing branch ("deltaetajj"
	//and "deltaphijj" are obtained from the other angular distributions)
	string kinetic_variables[] = {"met","mjj","mll","ptl1","ptl2","ptj1","ptj2","etaj1","etaj2",
		"phij1","phij2","met","met","met"};
	string kinetic_variables_real[] = {"met","mjj","mll","ptl1","ptl2","ptj1","ptj2","etaj1","etaj2",
		"phij1","phij2","deltaetajj","deltaphijj", "no shape histo"};
	string histo_names[] = {"histo_sm", "histo_linear", "histo_quadratic"};
	string histo_titles[] = {"SM", "LIN", "QUAD"};

	//mins and maxs of the distributions
	float min[] = {30, 500, 20, 25, 20, 30, 30, -5., -5., -M_PI, -M_PI, 2.5, 0, 0};
	float max[] = {1620, 8600, 1950, 1250, 600, 2000, 1100, 5., 5., M_PI, M_PI, 10., M_PI, 2};

	//RMS values for the 11 kinetic variables' distributions (see getRMS_cW.cpp)
	string line;
	float RMS_array[13];
	int i=0;
	ifstream file("OPERATOR_rms.txt");
	if(file.is_open()){
		while(getline(file,line)){
			RMS_array[i]=stof(line);
			i++;
		}
		file.close();
	}
	//float RMS_array[] = {87.2333, 953.58, 116.004, 70.2788, 31.7565, 139.601, 88.028, 1.98489, 2.35187, 1.81397, 1.81276, 1.99129, 0.806861};

	//variable width binning for the first 7 variables: met, mjj, mll, ptl1, ptl2, ptj1 (fixed width binning for the angular variables)
	vector<float> bins_edges_vectors[14];
	float first_limit[] = {1000, 8000, 1350, 900, 380, 1600, 900}; //from here bin_width*2
	float second_limit[] = {1300, 8600, 1700, 1250, 430, 1800, 1000}; //from here bin_width*4
	int Nbins[14];
	int var_number_plot = 0;

	//binning
	for (int var_number = 0; var_number < 14; var_number++)
	{
		if (kinetic_variable_plot == kinetic_variables_real[var_number]) var_number_plot = var_number;
		bins_edges_vectors[var_number].push_back(min[var_number]);
		if (var_number < 7) //variable width binning
		{
			while (true)
			{
				if (bins_edges_vectors[var_number].back() < first_limit[var_number])
				{
					//bin_width in the first region = 1/3*RMS
					bins_edges_vectors[var_number].push_back(bins_edges_vectors[var_number].back() + RMS_array[var_number]/3.);
				}
				else if (bins_edges_vectors[var_number].back() < second_limit[var_number])
				{
					//bin_width in the second region = 2/3*RMS
					bins_edges_vectors[var_number].push_back(bins_edges_vectors[var_number].back() + RMS_array[var_number]*2/3.);
				}
				else if (bins_edges_vectors[var_number].back() < max[var_number])
				{
					//bin_width in the third region = RMS
					bins_edges_vectors[var_number].push_back(bins_edges_vectors[var_number].back() + RMS_array[var_number]);
				}
				else break;
			}
			Nbins[var_number] = bins_edges_vectors[var_number].size() - 1;
		}
		else if (var_number != 13) //fixed width binning for the angular variables
		{
			Nbins[var_number] = floor((max[var_number]-min[var_number])/((1./3.)*RMS_array[var_number]));
			float width_bin = (max[var_number]-min[var_number])/Nbins[var_number];
			while (bins_edges_vectors[var_number].back() < max[var_number])
			{
				bins_edges_vectors[var_number].push_back(bins_edges_vectors[var_number].back() + width_bin);
			}
		}
		else //one bin histogram, defined in the range [0, 2] and filled with ones
		{
			bins_edges_vectors[var_number].push_back(2.);
			Nbins[var_number] = 1;
		}
	}

	vector<TH1F> histos[14];
	float integrals[14][3];
	TFile* myfile = new TFile("/Users/giorgio/Desktop/tesi/D6EFTStudies/analysis/VBS_e+_mu+.root");

	for (int ntuple_number = 0; ntuple_number < 3; ntuple_number++) // sm, lin, quad
	{
		TH1F* global_numbers = (TH1F*) myfile->Get(name_global_numbers[ntuple_number].c_str()) ;
		float cross_section = global_numbers->GetBinContent(1);
		float sum_weights_total = global_numbers->GetBinContent(2);
		float sum_weights_selected = global_numbers->GetBinContent(3);
		float luminosity = 100;
		float normalization = cross_section*luminosity/sum_weights_total;

		for (int var_number = 0; var_number < 14; var_number++)
		{
			float* bins_edges = bins_edges_vectors[var_number].data();
			TH1F* histo = new TH1F(kinetic_variables_real[var_number].c_str(), histo_titles[ntuple_number].c_str(), Nbins[var_number], bins_edges);

			TTreeReader reader (name_ntuples[ntuple_number].c_str(), myfile);
			TTreeReaderValue<float> var (reader, kinetic_variables[var_number].c_str());
			TTreeReaderValue<float> weight (reader, "w"); //weights branch
			TTreeReaderValue<float> met (reader, "met");
			TTreeReaderValue<float> mjj (reader, "mjj");
			TTreeReaderValue<float> mll (reader, "mll");
			TTreeReaderValue<float> ptl1 (reader, "ptl1");
			TTreeReaderValue<float> ptl2 (reader, "ptl2");
			TTreeReaderValue<float> ptj1 (reader, "ptj1");
			TTreeReaderValue<float> ptj2 (reader, "ptj2");
			TTreeReaderValue<float> etaj1 (reader, "etaj1");
			TTreeReaderValue<float> etaj2 (reader, "etaj2");
			TTreeReaderValue<float> phij1 (reader, "phij1");
			TTreeReaderValue<float> phij2 (reader, "phij2");

			//selections
			while (reader.Next ())
			{
				if (*met > 30 && *mjj > 500 && *mll > 20 && *ptl1 > 25 && *ptl2 > 20 && *ptj1 > 30
				&& *ptj2 > 30 && abs(*etaj1) < 5 && abs(*etaj2) < 5 && abs(*etaj1 - *etaj2) > 2.5)
				{
					if (var_number < 11) histo->Fill(*var,*weight);
					else if (var_number == 11) histo->Fill(abs(*etaj1 - *etaj2), *weight); //deltaetajj
					else if (var_number == 12) //deltaphijj
					{
						if (abs(*phij1 - *phij2) < M_PI) histo->Fill(abs(*phij1 - *phij2),*weight);
						else histo->Fill(2.*M_PI - abs(*phij1 - *phij2), *weight);
					}
					else histo->Fill(1, *weight); //no shape distribution
				}
			}

			histo->Scale(normalization);

			if (ntuple_number == 1) histo->Scale(1./0.3);  //linear term
			if (ntuple_number == 2) histo->Scale(1./(0.3*0.3));  //quadratic term

			//overflow bin events moved in the last bin
			histo->SetBinContent(histo->GetNbinsX(), histo->GetBinContent(histo->GetNbinsX()) + histo->GetBinContent(histo->GetNbinsX() + 1));
			histo->SetBinContent(histo->GetNbinsX() + 1, 0.);

			histo->SetName(histo_names[ntuple_number].c_str());

			//To check if there are no empty bins in the sm distribution
			/*if (ntuple_number == 0)
			{
				for (int bin_counter = 0; bin_counter < histo->GetNbinsX() + 2 ; bin_counter++) {
					if (histo->GetBinContent(bin_counter) != 0)
						cout << histo->GetBinContent(bin_counter) << endl;
					else
						cout << "XXXXXXX" << endl;
				}
				cout << endl;
			}*/

			histos[var_number].push_back(*histo);
			integrals[var_number][ntuple_number] = histo->Integral();

			histo->Reset();
		}

		delete global_numbers;
	}

	//prints the integrals (to be inserted in datacard.txt) and saves the histograms in the root file
	for (int var_number = 0; var_number < 14; var_number ++)
	{
		files[var_number]->cd();
		histos[var_number][0].Write();
		histos[var_number][1].Write();
		histos[var_number][2].Write();
		files[var_number]->Write();
		files[var_number]->Close();
		cout << kinetic_variables_real[var_number].c_str() << "---------------------------------" << endl;
		for (int ntuple_number = 0; ntuple_number < 3; ntuple_number++) cout << integrals[var_number][ntuple_number] << "\t";
		cout << endl;
	}

	string kinetic_variables_tex[] = {"MET","m_{jj}","m_{ll}","p_{tl1}","p_{tl2}","p_{tj1}","p_{tj2}",
		"#eta_{j1}","#eta_{j2}","#phi_{j1}","#phi_{j2}","#Delta#eta_{jj}","#Delta#phi_{jj}"};
	THStack* h_stack = new THStack("hs","");

	float cW = 1; //cW for the plot
	histos[var_number_plot][2].Scale(cW*cW); // quadratic scaling relation
	histos[var_number_plot][1].Scale(cW);	 // linear scaling relation

	//PLOT

	//plots only the sm distribution, to check the binning
	/*sm->SetLineColor(kBlue);
	sm->SetLineWidth(2.);
	sm->Draw("hist");
	gPad->SetLogy();*/

	//change of variable in the y axis, from "number of events" to "(number of events)/(bin width)"
	for (int j = 0; j < 3; j++) {
		for (int i = 0; i < histos[var_number_plot][j].GetNbinsX()+1; i++)
		{
			histos[var_number_plot][j].SetBinContent(i, histos[var_number_plot][j].GetBinContent(i)/histos[var_number_plot][j].GetBinWidth(i));
		}
	}

	histos[var_number_plot][0].SetFillStyle(3001);
	histos[var_number_plot][0].SetFillColor(kOrange+1);
	histos[var_number_plot][2].SetFillStyle(3001);
	histos[var_number_plot][2].SetFillColor(kBlue-7);
	histos[var_number_plot][1].SetFillColor(kGreen+1);
	histos[var_number_plot][1].SetFillStyle(3001);

	h_stack->Add(&histos[var_number_plot][0]);
	h_stack->Add(&histos[var_number_plot][2]);
	h_stack->Add(&histos[var_number_plot][1]);

	TH1F* histo_sum = new TH1F(histos[var_number_plot][0] + histos[var_number_plot][1] + histos[var_number_plot][2]);
	histo_sum->SetTitle("SM + BSM + INT");
	histo_sum->SetLineColor(kRed);
	histo_sum->SetFillColor(kWhite);
	histo_sum->SetLineWidth(2.);

	cnv->cd(1);

	h_stack->Draw("HIST");
	histo_sum->Draw("same hist");
	stringstream ss_title;
	ss_title << cW;
	string title = kinetic_variables_tex[var_number_plot] + string(" (c_{W} = ") + ss_title.str() + string(")");
	h_stack->SetTitle(title.c_str());
	string xlabel = string(kinetic_variables_tex[var_number_plot]);
	string ylabel = "events / ";
	if (var_number_plot < 7)
	{
		xlabel += string(" (Gev)");
		ylabel += string("Gev");
	}

	else if (var_number_plot == 7 || var_number_plot == 8 || var_number_plot == 11) ylabel += string("#eta unit");
	else if (var_number_plot == 9 || var_number_plot == 10 || var_number_plot == 12) ylabel += string("#phi unit");
	h_stack->GetXaxis()->SetTitle(xlabel.c_str());
	h_stack->GetXaxis()->SetTitleSize(.05);
	h_stack->GetXaxis()->SetTitleOffset(.9);
	h_stack->GetYaxis()->SetTitle(ylabel.c_str());
	h_stack->GetYaxis()->SetTitleSize(.05);
	h_stack->GetYaxis()->SetTitleOffset(.9);
	if (var_number_plot < 7 || var_number_plot == 11) gPad->BuildLegend(0.60,0.70,0.90,0.90,"");
	else if (var_number_plot != 12) gPad->BuildLegend(0.55,0.14,0.85,0.34,"");
	else gPad->BuildLegend(0.15,0.44,0.45,0.64,"");

	cnv->Modified();
	cnv->Update();

	cnv->cd(2);

	//LOGARITHMIC PLOT
	gPad->SetLogy();
	h_stack->Draw("HIST");
	histo_sum->Draw("same hist");
	string title_log = title + " (log scale)";
	h_stack->SetTitle(title_log.c_str());
	if (var_number_plot < 7 || var_number_plot == 11) gPad->BuildLegend(0.60,0.70,0.90,0.90,"");
	else if (var_number_plot != 12) gPad->BuildLegend(0.55,0.14,0.85,0.34,"");
	else gPad->BuildLegend(0.15,0.44,0.45,0.64,"");

	cnv->Modified();
	cnv->Update();

	//To save the plots
	/*string plot_name = kinetic_variable_plot + string("_sel_cW.png");
	cnv->Print(plot_name.c_str(), "png");*/

	myapp->Run();

	return 0 ;
}
