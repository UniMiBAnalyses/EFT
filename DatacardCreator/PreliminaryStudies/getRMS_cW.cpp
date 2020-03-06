/*
Prints the RMS of the total EFT distribution (SM + LIN + QUAD) for the variables met, mjj, mll,
ptl1, ptl2, ptj1, ptj2, etaj1, etaj2, phij1, phij2, deltaetajj and deltaphijj, using the HS ntuple
ntuple_RcW_0p3_HS_2.root (6-th dimension EFT operator: Q_W). No selections are applied.
To create the total EFT distribution a realistic value of cW is used (cW = 0.01).

c++ -o getRMS_cW getRMS_cW.cpp `root-config --glibs --cflags`

es) ./getRMS_cW OPERATOR event_name wilson_coeff

*/

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <fstream>

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

bool replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}
int main (int argc, char** argv)
{
	const char* OPERATOR;
	const char* event_name;
	const char* wc;
	if (argc > 1){
		OPERATOR = argv[1];
		event_name=argv[2];
		wc=argv[3];
	}
	string wc_string(wc);
	replace(wc_string, ".", "p");
	float wilson_coeff=stof(wc);
	cout << wc_string <<"\n"<<wilson_coeff<<endl;

	TH1::SetDefaultSumw2();

	string name_histograms[] = {"SM", "QUAD", "LIN"};

	//the last two variables are actually "deltaetajj" and "deltaphijj", "met" is just to read an existing branch ("deltaetajj"
	//and "deltaphijj" are obtained from the other angular distributions)
	string kinetic_variables[] = {"met","mjj","mll","ptl1","ptl2","ptj1","ptj2","etaj1","etaj2","phij1","phij2","met","met"};

	vector<TH1F*> histos;
	string path="/Users/giorgio/Desktop/tesi/D6EFTStudies/analysis/"+string(OPERATOR)+"_"+string(event_name)+"_"+wc_string+".root";
	TFile* myfile = new TFile(path.c_str());

	string name_ntuples[] = {"sm", "lin", "quad"};
	string name_global_numbers[] = {"sm_nums", "lin_nums", "quad_nums"};

	cout << "------------ RMS values for the distributions in ntuple_RcW_0p3_HS_2.root ------------" << endl;
	int Nbins = 200;

	//opening text file to write rms in it

	ofstream file_rms (string(OPERATOR)+"_"+string(event_name)+"_"+wc_string+"_rms.txt");
    ofstream file_min_max (string(OPERATOR)+"_"+string(event_name)+"_"+wc_string+"_min_max.txt");
	if(file_rms.is_open()){
		for (int var_number = 0; var_number < 13; var_number++) //for loop to scan all the variables
		{
			vector<float> values[3]; // to contain data of SM, QUAD and LIN distributions
			vector<float> weights[3];

			float max = 0., min = 0.;

			//for loop to find max and min of the total EFT distribution, before defining the histograms
			for (int distr_number = 0; distr_number < 3; distr_number++) // j = 0,1,2: SM simulation, BSM (quadratic term), BSM (interference term)
			{
				TTreeReader reader (name_ntuples[distr_number].c_str(), myfile);
				TTreeReaderValue<float> var (reader, kinetic_variables[var_number].c_str());
				TTreeReaderValue<float> weight (reader, "w"); //weights branch
				TTreeReaderValue<float> etaj1 (reader, "etaj1");
				TTreeReaderValue<float> etaj2 (reader, "etaj2");
				TTreeReaderValue<float> phij1 (reader, "phij1");
				TTreeReaderValue<float> phij2 (reader, "phij2");

				while (reader.Next())
				{
					if (var_number < 11) {
						values[distr_number].push_back(*var);
					}
					else if (var_number == 11) values[distr_number].push_back(abs(*etaj1 - *etaj2));
					else {
						if (abs(*phij1 - *phij2) < M_PI) values[distr_number].push_back(abs(*phij1 - *phij2));
						else values[distr_number].push_back(2.*M_PI - abs(*phij1 - *phij2));
					}
					weights[distr_number].push_back(*weight);

				}

				// finds max and min in the three distributions, in order to define the histograms in the range [min, max]
				if (max < *max_element(values[distr_number].begin(), values[distr_number].end()))
					max = *max_element(values[distr_number].begin(), values[distr_number].end());
				if (min > *min_element(values[distr_number].begin(), values[distr_number].end()))
					min = *min_element(values[distr_number].begin(), values[distr_number].end());
			}
            //we want to store this min, max in order to use this information in other histos
            file_min_max << min <<"\t"<< max <<endl;
			TH1F* sm = new TH1F ("sm", "sm", Nbins, min, max);
			TH1F* lin = new TH1F ("int", "int", Nbins, min, max);
			TH1F* quad = new TH1F ("bsm", "bsm", Nbins, min, max);

			histos.push_back(sm);
			histos.push_back(lin);
			histos.push_back(quad);

			for (int distr_number = 0; distr_number < 3; distr_number++)
			{
				TH1F* global_numbers = (TH1F*) myfile->Get(name_global_numbers[distr_number].c_str()) ;
				float cross_section = global_numbers->GetBinContent(1);
				float sum_weights_total = global_numbers->GetBinContent(2);
				float sum_weights_selected = global_numbers->GetBinContent(3);
				float luminosity = 100;
				float normalization = cross_section*luminosity/sum_weights_total;

				for (int event_number = 0; event_number < values[distr_number].size(); event_number++)
				{
					histos[distr_number]->Fill(values[distr_number][event_number],weights[distr_number][event_number]);
				}

				histos[distr_number]->Scale(normalization);

				//rescaling QUAD and LIN distributions with a realistic value of the Wilson coefficient
				//this is for a wilson coefficient=0.3 and getting 0.1
				if (distr_number == 1) histos[distr_number]->Scale(1/wilson_coeff);
				if (distr_number == 2) histos[distr_number]->Scale((1)/(wilson_coeff*wilson_coeff));

				histos[distr_number]->ResetStats();

				values[distr_number].clear();
				weights[distr_number].clear();
			}

			//total EFT distribution
			TH1F* histo_sum = new TH1F(*histos[0] + *histos[1] + *histos[2]);
			/*TApplication* myapp = new TApplication ("myapp", NULL, NULL);
			TCanvas* cnv = new TCanvas("cnv","cnv",0,0,1200,400);
			cnv->cd();
			histo_sum->Draw();
			cnv->Modified();
			cnv->Update();

			myapp->Run();*/
			float RMS = histo_sum->GetRMS(); //RMS of the total EFT distribution

			//good visualization
			cout << RMS <<endl;
			file_rms << RMS <<endl;

			//bad visualization (to copy the values)
			/*if (k != 12) cout << RMS << ", ";
			else cout << RMS << endl;*/

			histos.clear();

			delete sm;
			delete quad;
			delete lin;
		}
		file_rms.close();
        file_min_max.close();
	}


	return 0;

}
