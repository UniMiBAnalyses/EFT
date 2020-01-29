/*
Plots the differences between the analitic distributions (obtained with scaling relations from
the HighStatistics ntuple ntuple_RcW_0p3_HS.root) and the MC distributions (ntuple_RcW_0p05.root,
ntuple_RcW_0p1.root, ntuple_RcW_0p4.root, ntuple_RcW_1.root). The SM distributions are built
from the HS ntuple ntuple_SMlimit_HS.root. No preselections are applied.

c++ -o MC_vs_analitic MC_vs_analitic.cpp `root-config --glibs --cflags`
*/

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include <TFile.h>
#include <TNtuple.h>
#include <TTreeReader.h>
#include <TH1.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TPaveText.h>
#include <TGaxis.h>
#include <TLatex.h>

using namespace std ;

float weightedMean (vector<float> values, vector<float> errors);
float errorWeightedMean (vector<float> errors);

int main (int argc, char** argv)
{
	gStyle->SetLabelSize(.04, "XY");
	gStyle->SetOptStat(0);
	TGaxis::SetMaxDigits(2);
	const char* kinetic_variable; //possible variables: met, mjj, mll, ptl1, ptl2
	const char* kinetic_variable_tex;
	if (argc == 1)
	{
		kinetic_variable = "met";
	}
	else
	{
		kinetic_variable = argv[1];
	}

	TH1::SetDefaultSumw2();
	TApplication* myapp = new TApplication ("myapp", NULL, NULL);
	TCanvas* cnv = new TCanvas("cnv","cnv",0,0,1200,800);
	TCanvas* cnv2 = new TCanvas("cnv2","cnv2",0,0,1200,800);
	TCanvas* cnv3 = new TCanvas("cnv3","cnv3",0,0,1200,800);

	cnv->Divide(2,2);
	cnv2->Divide(2,2);
	cnv3->Divide(2,2);

	string cW[] = {"0p3","0p05","0p1","0p4","1"};
	string titles[] = {"c_{W} = 0.05", "c_{W} = 0.1", "c_{W} = 0.3","c_{W} = 0.4", "c_{W} = 1"};
	string kinetic_variables[] = {"met","mjj","mll","ptl1","ptl2"};
	string kinetic_variables_tex[] = {"MET","m_{jj}","m_{ll}","p_{tl1} ","p_{tl2} "};

	vector<TH1F*> histos, histos_analitic;
	float max;

	//maxima chosen for the 5 distributions (met, mjj, mll, ptl1, ptl2) with cW = 0.3, used
	//to create the others with scaling relations
	float maxima[] = {1620, 8600, 1950, 1250, 600};

	//RMS values for the 5 kinetic variables' distributions (see getRMScW.cpp)
	float RMS_array[5] = {87.2333, 953.58, 116.004, 70.2788, 31.7565};
	int Nbins;

	for (int i = 0; i < 5; i++)
	{
		kinetic_variable_tex = kinetic_variables_tex[i].c_str();
		if (kinetic_variable == kinetic_variables[i]) //finds the input kinetic variable
		{
			max = maxima[i];

			//bins' width: (1/3)*RMS
			Nbins = floor(max/((1./3.)*RMS_array[i]));
			break;
		}
	}

	//arrays to contain the 'dispersions' of the difference distributions over the y axis
	//(extimated as the weighted mean of bin contents)
	float dispersion_quad[4];
	float dispersion_lin[4];
	float dispersion_tot[4]; //'tot' stands for INT (lin) + BSM (quad)

	for (int k = 0; k < 5; k++) // cW = 0.3, 0.05, 0.1, 0.4, 1.
	{
		string name_files[3];
		if (k != 0) //for cW = 0.3 high statistics (HS)
		{
			name_files[0] = "ntuple_SMlimit_HS.root";
			name_files[1] = "ntuple_RcW_" + cW[k] + ".root";
			name_files[2] = "ntuple_RcW_" + cW[k] + ".root";
		}
		else
		{
			name_files[0] = "ntuple_SMlimit_HS.root";
			name_files[1] = "ntuple_RcW_" + cW[k] + "_HS.root";
			name_files[2] = "ntuple_RcW_" + cW[k] + "_HS.root";
		}

		string name_ntuples[] = {"SSeu_SMlimit", "SSeu_RcW_bsm_" + cW[k], "SSeu_RcW_int_" + cW[k]};
		string name_global_numbers[] = {"SSeu_SMlimit_nums", "SSeu_RcW_bsm_"+ cW[k] +"_nums", "SSeu_RcW_int_" + cW[k] + "_nums"};
		vector<float> values;
		vector<float> weights;


		for (int j = 0; j < 3; j++) // j = 0,1,2: SM simulation, BSM (quadratic term), BSM (interference term)
		{
			TFile* myfile = new TFile(name_files[j].c_str());
			TTreeReader reader (name_ntuples[j].c_str(), myfile);
			TTreeReaderValue<float> var (reader, kinetic_variable);
			TTreeReaderValue<float> weight (reader, "w"); //weights branch
			TH1F* histo = new TH1F ("histo", "histo", Nbins, 0, max);

			while (reader.Next ())
			{
				histo->Fill(*var,*weight);
			}

			TH1F* global_numbers = (TH1F*) myfile->Get(name_global_numbers[j].c_str());
			float cross_section = global_numbers->GetBinContent(1);
			float sum_weights_total = global_numbers->GetBinContent(2);
			float sum_weights_selected = global_numbers->GetBinContent(3);
			float luminosity = 100;
			float normalization = cross_section*luminosity/sum_weights_total;

			/*for (int i = 0; i < values.size(); i++)
			{
				histo->Fill(values[i],weights[i]);
			}*/

			histo->Scale(normalization);
			histo->SetBinContent(histo->GetNbinsX(), histo->GetBinContent(histo->GetNbinsX()) + histo->GetBinContent(histo->GetNbinsX() + 1));
			histo->SetBinContent(histo->GetNbinsX() + 1, 0.);

			if (k == 0) histos_analitic.push_back(histo);
			else histos.push_back(histo);

			values.clear();
			weights.clear();
		}

		float cW_values[] = {0.3, 0.05, 0.1, 0.4, 1};
		if (k > 0) //nothing to compare with cW = 0.3 (distribution used to generate the others)
		{
			TH1F* sm_analitic = new TH1F(*histos_analitic[0]);
			TH1F* quadratic_analitic = new TH1F(*histos_analitic[1]);
			TH1F* linear_analitic = new TH1F(*histos_analitic[2]);

			quadratic_analitic->Scale(cW_values[k]*cW_values[k]/(0.3*0.3)); // quadratic scaling relation
			linear_analitic->Scale(cW_values[k]/0.3);           // linear scaling relation

			TH1F* quad_difference = new TH1F(*histos[1] - *quadratic_analitic);
			TH1F* lin_difference = new TH1F(*histos[2] - *linear_analitic);
			TH1F* quad_plus_lin_difference = new TH1F(*histos[1] + *histos[2] - *quadratic_analitic - *linear_analitic);

			vector<float> bin_contents_quad;
			vector<float> bin_errors_quad;
			vector<float> bin_contents_lin;
			vector<float> bin_errors_lin;
			vector<float> bin_contents_tot;
			vector<float> bin_errors_tot;

			for (int i = 1; i <= quad_difference->GetNbinsX(); i++)
			{
				bin_contents_quad.push_back(quad_difference->GetBinContent(i));
				bin_errors_quad.push_back(quad_difference->GetBinError(i));

				bin_contents_lin.push_back(lin_difference->GetBinContent(i));
				bin_errors_lin.push_back(lin_difference->GetBinError(i));

				bin_contents_tot.push_back(quad_plus_lin_difference->GetBinContent(i));
				bin_errors_tot.push_back(quad_plus_lin_difference->GetBinError(i));
			}

			//computation of 'dispersions' in the y axis as the weighted mean of bin contents
			dispersion_quad[k-1] = errorWeightedMean(bin_errors_quad);
			dispersion_lin[k-1] = errorWeightedMean(bin_errors_lin);
			dispersion_tot[k-1] = errorWeightedMean(bin_errors_tot);

			//PLOT------difference between quadratic distributions: BSM(MC) - BSM(scaling relations)
			cnv->cd(k);
			quad_difference->Draw();
			string title_quad = string("MC - analitic (quadratic distribution, ") + titles[k] + string(")");
			quad_difference->SetTitle(title_quad.c_str());
			string xlabel = string(kinetic_variable_tex) + string(" (GeV)");
			quad_difference->GetXaxis()->SetTitle(xlabel.c_str());
			quad_difference->GetXaxis()->SetTitleSize(.05);
			quad_difference->GetXaxis()->SetTitleOffset(0.92);
 			quad_difference->GetYaxis()->SetTitle("#Delta events");
			quad_difference->GetYaxis()->SetTitleSize(.05);
			quad_difference->GetYaxis()->SetTitleOffset(1.);
			TLegend* legend_quad = new TLegend(0.38,0.77,0.9,0.9);
			stringstream ss_mean_quad, ss_error_quad;
			ss_mean_quad << weightedMean(bin_contents_quad, bin_errors_quad);
			ss_error_quad << dispersion_quad[k-1];
			string mean_quad = ss_mean_quad.str() + string(" #pm ") + ss_error_quad.str();
			string legend_content_quad = "#splitline{Weighted mean of bin contents:}{" + mean_quad + "}";
			legend_quad->SetHeader(legend_content_quad.c_str(),"");
			legend_quad->Draw();
			cnv->Modified();
			cnv->Update();

			//PLOT------difference between linear distributions: INT(MC) - INT(scaling relations)
			cnv2->cd(k);
			lin_difference->Draw();
			string title_lin = string("MC - analitic (linear distribution, ") + titles[k] + string(")");
			lin_difference->SetTitle(title_lin.c_str());
			lin_difference->GetXaxis()->SetTitle(xlabel.c_str());
			lin_difference->GetXaxis()->SetTitleSize(.05);
			lin_difference->GetXaxis()->SetTitleOffset(0.92);
			lin_difference->GetYaxis()->SetTitle("#Delta events");
			lin_difference->GetYaxis()->SetTitleSize(.05);
			lin_difference->GetYaxis()->SetTitleOffset(1.);
			TLegend* legend_lin = new TLegend(0.38,0.77,0.9,0.9);
			stringstream ss_mean_lin, ss_error_lin;
			ss_mean_lin << weightedMean(bin_contents_lin, bin_errors_lin);
			ss_error_lin << dispersion_lin[k-1];
			string mean_lin = ss_mean_lin.str() + string(" #pm ") + ss_error_lin.str();
			string legend_content_lin = "#splitline{Weighted mean of bin contents:}{" + mean_lin + "}";
			legend_lin->SetHeader(legend_content_lin.c_str() ,"");
			legend_lin->Draw();
			cnv2->Modified();
			cnv2->Update();

			//PLOT------difference between quadratic+linear distributions: (BSM+INT(MC)) - (BSM+INT(scaling relations))
			cnv3->cd(k);
			quad_plus_lin_difference->Draw();
			string title_tot = string("MC - analitic (quad + lin distribution, ") + titles[k] + string(")");
			quad_plus_lin_difference->SetTitle(title_tot.c_str());
			quad_plus_lin_difference->GetXaxis()->SetTitle(xlabel.c_str());
			quad_plus_lin_difference->GetXaxis()->SetTitleSize(.05);
			quad_plus_lin_difference->GetXaxis()->SetTitleOffset(0.92);
			quad_plus_lin_difference->GetYaxis()->SetTitle("#Delta events");
			quad_plus_lin_difference->GetYaxis()->SetTitleSize(.05);
			quad_plus_lin_difference->GetYaxis()->SetTitleOffset(1.);
			TLegend* legend_tot = new TLegend(0.38,0.77,0.9,0.9);
			stringstream ss_mean_tot, ss_error_tot;
			ss_mean_tot << weightedMean(bin_contents_tot, bin_errors_tot);
			ss_error_tot << dispersion_tot[k-1];
			string mean_tot = ss_mean_tot.str() + string(" #pm ") + ss_error_tot.str();
			string legend_content_tot = "#splitline{Weighted mean of bin contents:}{" + mean_tot + "}";
			legend_tot->SetHeader(legend_content_tot.c_str() ,"");
			legend_tot->Draw();
			cnv3->Modified();
			cnv3->Update();

			bin_contents_quad.clear();
			bin_errors_quad.clear();
			bin_contents_lin.clear();
			bin_errors_lin.clear();
			bin_contents_tot.clear();
			bin_errors_tot.clear();

		}
		histos.clear();
	}

	float cW_values[] = {0.05, 0.1, 0.4, 1};

	//graphs to plot dispersions vs. cW
	TGraph* dispersion_vs_cW_quad = new TGraph(4, cW_values, dispersion_quad);
	TGraph* dispersion_vs_cW_lin = new TGraph(4, cW_values, dispersion_lin);

	TCanvas* cnv4 = new TCanvas("cnv4","cnv4",0,0,700,500);
	TCanvas* cnv5 = new TCanvas("cnv5","cnv5",0,0,700,500);

	//PLOT------dispersion vs. cW for quadratic (BSM) distributions
	cnv4->cd();
	dispersion_vs_cW_quad->SetMarkerStyle(20);
	dispersion_vs_cW_quad->SetMarkerSize(1.1);
	dispersion_vs_cW_quad->SetMarkerColor(kBlack);
	dispersion_vs_cW_quad->Draw("AP");
	dispersion_vs_cW_quad->Fit("pol2"); //expected quadratic trend
	dispersion_vs_cW_quad->GetXaxis()->SetTitle("c_{W}");
	dispersion_vs_cW_quad->GetXaxis()->SetTitleSize(.05);
	dispersion_vs_cW_quad->GetXaxis()->SetTitleOffset(.8);
	dispersion_vs_cW_quad->GetYaxis()->SetTitle("dispersion");
	dispersion_vs_cW_quad->GetYaxis()->SetTitleSize(.05);
	dispersion_vs_cW_quad->GetYaxis()->SetTitleOffset(.9);
	string graph_title_quad = string("Dispersion vs c_{W} (") + string(kinetic_variable_tex) + string(", quadratic distribution)");
	dispersion_vs_cW_quad->SetTitle(graph_title_quad.c_str());
	cnv4->Modified();
	cnv4->Update();

	//PLOT------dispersion vs. cW for linear (INT) distributions
	cnv5->cd();
	dispersion_vs_cW_lin->SetMarkerStyle(20);
	dispersion_vs_cW_lin->SetMarkerSize(1.1);
	dispersion_vs_cW_lin->SetMarkerColor(kBlack);
	dispersion_vs_cW_lin->Draw("AP");
	dispersion_vs_cW_lin->Fit("pol1"); //expected linear trend
	dispersion_vs_cW_lin->GetXaxis()->SetTitle("c_{W}");
	dispersion_vs_cW_lin->GetXaxis()->SetTitleSize(.05);
	dispersion_vs_cW_lin->GetXaxis()->SetTitleOffset(.8);
	dispersion_vs_cW_lin->GetYaxis()->SetTitle("dispersion");
	dispersion_vs_cW_lin->GetYaxis()->SetTitleSize(.05);
	dispersion_vs_cW_lin->GetYaxis()->SetTitleOffset(.9);
	string graph_title_lin = string("Dispersion vs c_{W} (") + string(kinetic_variable_tex) + string(", linear distribution)");
	dispersion_vs_cW_lin->SetTitle(graph_title_lin.c_str());
	cnv5->Modified();
	cnv5->Update();

	//To save the plots
	/*string name_quad_difference = string(kinetic_variable) + "_quad_diff.png";
	string name_lin_difference = string(kinetic_variable) + "_lin_diff.png";
	string name_tot_difference = string(kinetic_variable) + "_linquad_diff.png";
	string name_dispersion_quad = string(kinetic_variable) + "_quad_cW.png";
	string name_dispersion_lin = string(kinetic_variable) + "_lin_cW.png";

	cnv->Print(name_quad_difference.c_str(),"png");
	cnv2->Print(name_lin_difference.c_str(),"png");
	cnv3->Print(name_tot_difference.c_str(),"png");
	cnv4->Print(name_dispersion_quad.c_str(),"png");
	cnv5->Print(name_dispersion_lin.c_str(),"png");*/

	myapp->Run();

	return 0;
}

float weightedMean (vector<float> values, vector<float> errors)
{
	if (values.size() != errors.size())	return -1;

	float weighted_sum = 0.;
	float sum_of_weights = 0.;
	for (int i = 0; i < values.size(); i++)
	{
		weighted_sum += values[i]/(errors[i]*errors[i]);
		sum_of_weights += 1./(errors[i]*errors[i]);
	}
	return weighted_sum/sum_of_weights;
}

float errorWeightedMean (vector<float> errors)
{
	float sum_of_weights = 0.;
	for (int i = 0; i < errors.size(); i++)
	{
		sum_of_weights += 1./(errors[i]*errors[i]);
	}
	return 1./sqrt(sum_of_weights);
}
