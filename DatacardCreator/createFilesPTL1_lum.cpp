/*
createFilesPTL1_lum creates the root files to study how the likelihood scan changes
by varying the luminosity (EFT 6th-order operator: Q_W), using the ptl1 distribution
from the ntuple ntuple_RcW_0p3_HS_2.root (with a fixed background, x_0 = 100 GeV).

c++ -o createFilesPTL1_lum createFilesPTL1_lum.cpp `root-config --glibs --cflags`

./createFilesPTL1_lum

*/

#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <vector>

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
#include <TF1.h>

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
	gStyle->SetLabelSize(.04, "XY");
	TApplication* myapp = new TApplication ("myapp", NULL, NULL);
	TCanvas* cnv = new TCanvas("cnv","cnv",0,0,1200,400);
	cnv->Divide(2,1);

	const char* OPERATOR;
    const char* event_name;
	const char* wc;
	if (argc > 1){
		OPERATOR=argv[1];
        event_name=argv[2];
		wc=argv[3];
	}
	string wc_string(wc);
	replace(wc_string, ".", "p");
	float wilson_coeff=stof(wc);
	cout << wc_string <<"\n"<<wilson_coeff<<endl;

	TH1::SetDefaultSumw2();
	cout << setprecision(7) << fixed;

	vector<TFile*> files;
	TFile* f1 = new TFile ("ptl1_lum_1.root","recreate");
	TFile* f2 = new TFile ("ptl1_lum_2.root","recreate");
	TFile* f3 = new TFile ("ptl1_lum_3.root","recreate");
	TFile* f4 = new TFile ("ptl1_lum_4.root","recreate");
	TFile* f5 = new TFile ("ptl1_lum_5.root","recreate");
	TFile* f6 = new TFile ("ptl1_lum_6.root","recreate");
	TFile* f7 = new TFile ("ptl1_lum_7.root","recreate");
	TFile* f8 = new TFile ("ptl1_lum_8.root","recreate");
	TFile* f9 = new TFile ("ptl1_lum_9.root","recreate");
	TFile* f10 = new TFile ("ptl1_lum_10.root","recreate");
	TFile* f11 = new TFile ("ptl1_lum_11.root","recreate");

	files.push_back(f1);
	files.push_back(f2);
	files.push_back(f3);
	files.push_back(f4);
	files.push_back(f5);
	files.push_back(f6);
	files.push_back(f7);
	files.push_back(f8);
	files.push_back(f9);
	files.push_back(f10);
	files.push_back(f11);

	string name_ntuples[] = {"sm","lin","quad"};
	string name_global_numbers[] = {"sm_nums","lin_nums","quad_nums"};
	string histo_names[] = {"histo_sm", "histo_linear", "histo_quadratic"};
	string histo_titles[] = {"SM", "LIN", "QUAD"};


	//reading RMS values
	string line;
	float RMS_array[13];
	int i=0;
	string filename=string(OPERATOR)+"_"+string(event_name)+"_"+wc_string+"_rms.txt";
	ifstream file(filename.c_str());
	if(file.is_open()){
		while(getline(file,line)){
			RMS_array[i]=stof(line);
			i++;
		}
		file.close();
	}
	//mins and max of PTL1 distribution
	float min = 25;
	float max = 1250;
	float first_limit = 900; //from here bin_width*2
	float second_limit = 1250; //from here bin_width*4
	float RMS = RMS_array[3]; //RMS of PTL1 distribution
	vector<float> bins_edges_vector;
	bins_edges_vector.push_back(min);
	while (true)
	{
		if (bins_edges_vector.back() < first_limit)
		{
			//bin_width in the first region = 1/3*RMS
			bins_edges_vector.push_back(bins_edges_vector.back() + RMS/3.);
		}
		else if (bins_edges_vector.back() < second_limit)
		{
			//bin_width in the first region = 2/3*RMS
			bins_edges_vector.push_back(bins_edges_vector.back() + RMS*2/3.);
		}
		else if (bins_edges_vector.back() < max)
		{
			//bin_width in the first region = RMS
			bins_edges_vector.push_back(bins_edges_vector.back() + RMS);
		}
		else break;
	}

	int Nbins = bins_edges_vector.size() - 1;

	float* bins_edges = bins_edges_vector.data();
	float* bins_edges_bkg_overflow = bins_edges_vector.data();
	bins_edges_bkg_overflow[bins_edges_vector.size()] = 2*max;

	//test values of luminosity
	float luminosity_array[] = {100, 200, 400, 800, 1600, 3200, 6400, 12800, 25600, 51200, 102400};
	vector<TH1F> histos[11];
	float integrals[11][3];
	string path="/Users/giorgio/Desktop/tesi/D6EFTStudies/analysis/"+string(OPERATOR)+"_"+string(event_name)+"_"+wc_string+".root";
	TFile* myfile = new TFile(path.c_str());

	float x_0 = 100; //fixed values of x_0 in the exponential model for the background
	TF1* bkg_model = new TF1("bkg_model", "exp(-x/[0])", min, 2*max);
	bkg_model->SetParameter(0, x_0);
	float cW = 1;

	TH1F* bkg = new TH1F("histo_bkg", "BKG", Nbins, bins_edges);
	TH1F* bkg_overflow = new TH1F("histo_bkg_overflow", "BKG_overflow", Nbins+1, bins_edges_bkg_overflow);


	for (int ntuple_number = 0; ntuple_number < 3; ntuple_number++) // sm, int, bsm
	{
		TH1F* global_numbers = (TH1F*) myfile->Get(name_global_numbers[ntuple_number].c_str()) ;
		float cross_section = global_numbers->GetBinContent(1);
		float sum_weights_total = global_numbers->GetBinContent(2);
		float sum_weights_selected = global_numbers->GetBinContent(3);

		for (int lum_index = 0; lum_index < 11; lum_index++)
		{
			float luminosity = luminosity_array[lum_index];
			float normalization = cross_section*luminosity/sum_weights_total;

			TH1F* histo = new TH1F(histo_titles[ntuple_number].c_str(), histo_titles[ntuple_number].c_str(), Nbins, bins_edges);

			TTreeReader reader (name_ntuples[ntuple_number].c_str(), myfile);
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

			//preselections
			while (reader.Next ())
			{
				if (*met > 30 && *mjj > 500 && *mll > 20 && *ptl1 > 25 && *ptl2 > 20 && *ptj1 > 30
				&& *ptj2 > 30 && abs(*etaj1) < 5 && abs(*etaj2) < 5 && abs(*etaj1 - *etaj2) > 2.5)
				{
					histo->Fill(*ptl1,*weight);
				}
			}

			histo->Scale(normalization);

			if (ntuple_number == 1) histo->Scale(1./0.3);  //linear term
			if (ntuple_number == 2) histo->Scale(1./(0.3*0.3));  //quadratic term

			//overflow events moved to the last bin
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

			histos[lum_index].push_back(*histo);
			integrals[lum_index][ntuple_number] = histo->Integral();

			histo->Reset();
		}
	}

	for (int lum_index = 0; lum_index < 11; lum_index++)
	{
		bkg->SetFillColor(kWhite);
		bkg_overflow->FillRandom("bkg_model", pow(10, 6));

		//integral_bkg : integral_signal = 1 : 1
		bkg_overflow->Scale(histos[lum_index][0].Integral(1, histos[lum_index][0].GetNbinsX()+1)/bkg_overflow->Integral());

		//overflow bin as last bin for the background histogram (see createFilesPTL1_bkg.cpp)
		for (int bin_counter = 1; bin_counter < bkg->GetNbinsX(); bin_counter++)
		{
			bkg->SetBinContent(bin_counter, bkg_overflow->GetBinContent(bin_counter));
		}
		bkg->SetBinContent(bkg->GetNbinsX(), bkg_overflow->GetBinContent(bkg->GetNbinsX()) + bkg_overflow->GetBinContent(bkg->GetNbinsX() + 1));


		//saves the histograms in the root files
		/*files[lum_index]->cd();
		histos[lum_index][0].Write();
		histos[lum_index][1].Write();
		histos[lum_index][2].Write();
		bkg->Write();
		files[lum_index]->Write();
		files[lum_index]->Close();*/


		//prints the integrals (to be inserted in datacard.txt)
		cout << "lum_"<< lum_index + 1 << "--------------------------------------------" << endl;
		cout << integrals[lum_index][0] << "\t" << integrals[lum_index][1] << "\t" << integrals[lum_index][2] << "\t" << bkg->Integral() << endl;


		//PLOTS
		TH1F* histo_sum = new TH1F(histos[lum_index][0] + cW*histos[lum_index][1] + cW*cW*histos[lum_index][2]);
		histo_sum->SetTitle("SM + QUAD + LIN");
		histo_sum->SetFillStyle(3001);
		histo_sum->SetFillColor(kOrange-2);
		bkg->SetTitle("BKG");
		bkg->SetFillStyle(3001);
		bkg->SetFillColor(kGreen + 1);
		TH1F* sm_bkg = new TH1F(histos[lum_index][0] + *bkg);
		sm_bkg->SetTitle("SM + BKG");
		sm_bkg->SetLineColor(kRed);
		sm_bkg->SetLineWidth(2.);

		for (int bin_counter = 0; bin_counter < bkg->GetNbinsX()+1; bin_counter++)
		{
			bkg->SetBinContent(bin_counter, bkg->GetBinContent(bin_counter)/bkg->GetBinWidth(bin_counter));
			histo_sum->SetBinContent(bin_counter, histo_sum->GetBinContent(bin_counter)/histo_sum->GetBinWidth(bin_counter));
			sm_bkg->SetBinContent(bin_counter, sm_bkg->GetBinContent(bin_counter)/sm_bkg->GetBinWidth(bin_counter));
		}

		THStack* h_stack = new THStack("hs","");
		h_stack->Add(bkg);
		h_stack->Add(histo_sum);


		cnv->cd(1);

		h_stack->Draw("hist");
		stringstream ss_cW, ss_lum;
		ss_lum << luminosity_array[lum_index];
		ss_cW << cW;
		string title = "c_{W} = " + ss_cW.str() + ", x_{0} = 100 GeV, L = " + ss_lum.str() + " fb^{-1}";
		h_stack->SetTitle(title.c_str());
		string xlabel = string("p_{tl1} (GeV)");
		string ylabel = string("events / GeV");
		h_stack->GetXaxis()->SetTitle(xlabel.c_str());
		h_stack->GetXaxis()->SetTitleSize(.05);
		h_stack->GetXaxis()->SetTitleOffset(.9);
		h_stack->GetYaxis()->SetTitle(ylabel.c_str());
		h_stack->GetYaxis()->SetTitleSize(.05);
		h_stack->GetYaxis()->SetTitleOffset(1.);
		sm_bkg->Draw("HIST SAME");
		gPad->BuildLegend(0.55,0.65,0.90,0.90,"");

		cnv->Modified();
		cnv->Update();


		cnv->cd(2);

		h_stack->Draw("hist");
		title += " (log scale)";
		h_stack->SetTitle(title.c_str());
		sm_bkg->Draw("HIST SAME");
		gPad->BuildLegend(0.55,0.65,0.90,0.90,"");
		gPad->SetLogy();

		cnv->Modified();
		cnv->Update();


		bkg->Reset();
		bkg_overflow->Reset();

		//To save the plots
		/*stringstream ss_name_png;
		ss_name_png << lum_index+1;
		string name_png = "ptl1_lum_" + ss_name_png.str() + ".png";
		cnv->Print(name_png.c_str(), "png");*/

	}

	myapp->Run();

	return 0 ;
}
