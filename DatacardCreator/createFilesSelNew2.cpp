/*
createFilesSelNew2 creates the root files to make the likelihood scans with the 13 kinetic variables 
in the root file "ntuple_RcHW_0p3_2.root" (EFT 6th-order operator: cHW). The sm distribution is from
the root file "ntuple_RcW_0p3_HS_2.root"

c++ -o createFilesSelNew2 createFilesSelNew2.cpp `root-config --glibs --cflags` 

./createFilesSelNew2
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

	vector<TFile*> files;
	TFile* f_met = new TFile ("histo_0p3_noshape_sel_2.root","recreate");
	TFile* f_mjj = new TFile ("histo_0p3_mjj_META_2.root","recreate");
	TFile* f_mll = new TFile ("histo_0p3_mll_sel_2.root","recreate");
	TFile* f_ptl1 = new TFile ("histo_0p3_ptl1_sel_2.root","recreate");
	TFile* f_ptl2 = new TFile ("histo_0p3_ptl2_sel_2.root","recreate");
	TFile* f_ptj1 = new TFile ("histo_0p3_ptj1_sel_2.root","recreate");
	TFile* f_ptj2 = new TFile ("histo_0p3_ptj2_sel_2.root","recreate");
	TFile* f_etaj1 = new TFile ("histo_0p3_etaj1_sel_2.root","recreate");
	TFile* f_etaj2 = new TFile ("histo_0p3_etaj2_sel_2.root","recreate");
	TFile* f_phij1 = new TFile ("histo_0p3_phij1_sel_2.root","recreate");
	TFile* f_phij2 = new TFile ("histo_0p3_phij2_sel_2.root","recreate");
	TFile* f_deltaetajj = new TFile ("histo_0p3_deltaetajj_sel_2.root","recreate");
	TFile* f_deltaphijj = new TFile ("histo_0p3_deltaphijj_sel_2.root","recreate");
	

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
	
	string name_files[] = {"ntuple_RcW_0p3_HS_2.root", "ntuple_RcHW_0p3_2.root", "ntuple_RcHW_0p3_2.root"};
	string name_ntuples[] = {"SSeu_SMlimit","SSeu_RcHW_int_0p3","SSeu_RcHW_bsm_0p3"};
	string name_global_numbers[] = {"SSeu_SMlimit_nums","SSeu_RcHW_int_0p3_nums","SSeu_RcHW_bsm_0p3_nums"};
	string kinetic_variables[] = {"met","mjj","mll","ptl1","ptl2","ptj1","ptj2","etaj1","etaj2",
		"phij1","phij2","deltaetajj","deltaphijj"};
	string kinetic_variable_delta1, kinetic_variable_delta2;

	//mins and maxs of the distributions (cuts included)
	float min[] = {30, 500, 20, 25, 20, 30, 0, -5., -5., -M_PI, -M_PI, 2.5, 0};
	float max[] = {1620, 8600, 1950, 1250, 600, 2500, 1800, 5., 5., M_PI, M_PI, 10., M_PI}; 

	//RMS values for the 11 kinetic variables' distributions (see getRMS_new1.cpp)
	float RMS_array[] = {87.2333, 953.58, 116.004, 70.2788, 31.7565, 139.601, 88.028, 1.98489, 2.35187, 1.81397, 1.81276, 1.99129, 0.806861};    		
	
	//variable width binning for the first 7 variables: met, mjj, mll, ptl1, ptl2, ptj1 (angular variables excluced)
	vector<float> bins_edges_vectors[13];
	float first_limit[] = {1000, 8000, 1350, 900, 380, 1550, 1150}; //from here bin_width*2
	float second_limit[] = {1300, 8600, 1700, 1250, 430, 2000, 1350}; //from here bin_width*4
	int Nbins[13];

	for (int var_number = 0; var_number < 13; var_number++) 	
	{
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
					bins_edges_vectors[var_number].push_back(bins_edges_vectors[var_number].back() + RMS_array[var_number]*2/3.);
				}
				else if (bins_edges_vectors[var_number].back() < max[var_number]) 
				{
					bins_edges_vectors[var_number].push_back(bins_edges_vectors[var_number].back() + RMS_array[var_number]);
				}
				else break;
			}
			Nbins[var_number] = bins_edges_vectors[var_number].size() - 1;		
		}
		else 
		{
			Nbins[var_number] = floor((max[var_number]-min[var_number])/((1./3.)*RMS_array[var_number]));
			float width_bin = (max[var_number]-min[var_number])/Nbins[var_number];
			while (bins_edges_vectors[var_number].back() < max[var_number])
			{
				bins_edges_vectors[var_number].push_back(bins_edges_vectors[var_number].back() + width_bin);	
			}
		}
	}

	vector<TH1F*> histos;
	vector<float> values;
	vector<float> weights;
		
	//for (int k = 0; k < 13; k++) // 13 kinetics variables
	//{
	int k = 1; //metà bin mjj
		float* bins_edges = bins_edges_vectors[k].data();		
		
		TH1F* sm = new TH1F ("histo_sm", "", Nbins[k], bins_edges);
		TH1F* lin = new TH1F ("histo_linear", "", Nbins[k], bins_edges);
		TH1F* quad = new TH1F ("histo_quadratic", "", Nbins[k], bins_edges);

			sm->Rebin();
			lin->Rebin();
			quad->Rebin();
		
		histos.push_back(sm);
		histos.push_back(lin);
		histos.push_back(quad);


		cout << kinetic_variables[k].c_str() << "---------------------------------" << endl;

		
		if (k < 11) //deltaetajj and deltaphijj in the 'else'
		{
			for (int j = 0; j < 3; j++) // j = 0,1,2: SM simulation, BSM (interference term), BSM (quadratic term)
			{ 

				TFile* myfile = new TFile(name_files[j].c_str());
				TTreeReader reader (name_ntuples[j].c_str(), myfile);
				TTreeReaderValue<float> var1 (reader, kinetic_variables[k].c_str());
				TTreeReaderValue<float> var2 (reader, "w"); //weights branch
				
				while (reader.Next ()) 
				{	
					values.push_back(*var1);
					weights.push_back(*var2);
				}		

				TH1F* global_numbers = (TH1F*) myfile->Get(name_global_numbers[j].c_str()) ;
				float cross_section = global_numbers->GetBinContent(1);
				float sum_weights_total = global_numbers->GetBinContent(2);
				float sum_weights_selected = global_numbers->GetBinContent(3);
				float luminosity = 100;
				float normalization = cross_section*luminosity/sum_weights_total;	

				for (int i = 0; i < values.size(); i++) 
				{
					histos[j]->Fill(values[i],weights[i]);
				}

				histos[j]->Scale(normalization);
				
				if (j == 1) histos[j]->Scale(1./0.3); //linear term
				if (j == 2) histos[j]->Scale(1./(0.3*0.3)); //quadratic term
				
				histos[j]->SetBinContent(histos[j]->GetNbinsX(), histos[j]->GetBinContent(histos[j]->GetNbinsX()) + histos[j]->GetBinContent(histos[j]->GetNbinsX() + 1));
				histos[j]->SetBinContent(histos[j]->GetNbinsX() + 1, 0.);

				//To check if there are no empty bins in the sm distribution
				/*for (int i = 0; i < Nbins[k] + 2 ; i++) {
					if (histos[j]->GetBinContent(i) != 0)
						cout << histos[j]->GetBinContent(i) << endl;
					else
						cout << "XXXXXXX" << endl;
				}
				cout << endl;*/

				values.clear();
				weights.clear();
			}
		}
		else //deltaetajj and deltaphijj
		{
			if (k == 11)
			{
				kinetic_variable_delta1 = "etaj1";
				kinetic_variable_delta2 = "etaj2";
			}
			else 
			{
				kinetic_variable_delta1 = "phij1";
				kinetic_variable_delta2 = "phij2";
			}
			for (int j = 0; j < 3; j++) // SM simulation, BSM (quadratic term), BSM (interference term)
			{
				TFile* myfile = new TFile(name_files[j].c_str());
				TTreeReader reader (name_ntuples[j].c_str(), myfile);
				TTreeReaderValue<float> var1 (reader, kinetic_variable_delta1.c_str());
				TTreeReaderValue<float> var2 (reader, kinetic_variable_delta2.c_str());
				TTreeReaderValue<float> w (reader, "w"); //weights branch
				
				while (reader.Next()) 
				{	
					if (k == 11) values.push_back(abs(*var1 - *var2)); //deltaetajj := abs(etaj1 - etaj2)
					else {
						//deltaphijj: smaller angle between the jets (in [0, pi])
						if (abs(*var1 - *var2) < M_PI) values.push_back(abs(*var1 - *var2));
						else values.push_back(2.*M_PI - abs(*var1 - *var2));
					}
					weights.push_back(*w);	
				}		

				TH1F* global_numbers = (TH1F*) myfile->Get(name_global_numbers[j].c_str()) ;
				float cross_section = global_numbers->GetBinContent(1);
				float sum_weights_total = global_numbers->GetBinContent(2);
				float sum_weights_selected = global_numbers->GetBinContent(3);
				float luminosity = 100;
				float normalization = cross_section*luminosity/sum_weights_total;	

				for (int i = 0; i < values.size(); i++) 
				{
					histos[j]->Fill(values[i],weights[i]);
				}

				histos[j]->Scale(normalization);

				if (j == 1) histos[j]->Scale(1./0.3);
				if (j == 2) histos[j]->Scale(1./(0.3*0.3));

				histos[j]->SetBinContent(histos[j]->GetNbinsX(), histos[j]->GetBinContent(histos[j]->GetNbinsX()) + histos[j]->GetBinContent(histos[j]->GetNbinsX() + 1));
				histos[j]->SetBinContent(histos[j]->GetNbinsX() + 1, 0.);

				//To check if there are no empty bins in the sm distribution
				/*for (int i = 0; i < Nbins[k] + 2 ; i++) {
					if (histos[j]->GetBinContent(i) != 0)
						cout << histos[j]->GetBinContent(i) << endl;
					else
						cout << "XXXXXXX" << endl;
				}
				cout << endl;*/
				
				values.clear();		
				weights.clear();
			}
		}
		
		//creates the root files
		for (int j = 0; j < 3; j++) 
		{
			files[k]->cd(); 
			histos[j]->Write();
			cout << histos[j]->Integral() << "\t"; //prints the integrals to be inserted in datacard.txt
			files[k]->Write();	
		}

		files[k]->Close();
		cout << endl << endl;
		histos.clear();
		delete sm;
		delete quad;
		delete lin;
	//}


	return 0 ;
}

