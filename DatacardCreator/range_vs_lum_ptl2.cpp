/*
Plots the variation of the confidence ranges (68% and 95%) by varying the integrated luminosity.
The tested variable is ptl2 (6th-order EFT operator: Q_HW), the root files are generated by the
program "crateFilesPTL2_lum.cpp".
Data are contained in the file "range_ptl2_lum.txt", which contains 10 raws (for 10 values of 
luminosity); the 5 values in each raw are: the left endpoint of the 95% range, the left endpoint 
of the 68% range, the right enpoint of the 68% range, the right endpoint of the 95% range and the 
minimum point of the likelihood scan. 

c++ -o range_vs_lum_ptl2 range_vs_lum_ptl2.cpp `root-config --glibs --cflags`

./range_vs_lum_ptl2
*/

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>

#include <TFile.h>
#include <TNtuple.h>
#include <TTreeReader.h>
#include <TH1.h>
#include <TGraph.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLegend.h>
#include <THStack.h>
#include <TText.h>
#include <TF1.h>

using namespace std ;

int main (int argc, char** argv) 
{
	gStyle->SetLabelSize(.04, "XY");
	TApplication* myapp = new TApplication ("myapp", NULL, NULL);
	TCanvas* cnv = new TCanvas("cnv","cnv",0,0,700,500);
	
	float x_0[] = {100, 200, 400, 800, 1600, 3200, 6400, 12800, 25600, 51200};
	int i = 0;
	
	ifstream range_file;
	range_file.open("range_ptl2_lum.txt", ios::in);
	if (range_file.good() == false) cout << "-----Error while opening range.txt-----" << endl;
	
	float range_68[10], range_95[10]; //11 different values of x0
	float sx_95, sx_68, dx_68, dx_95, min;
	i = 0;
	while (true) 
	{
		range_file >> sx_95 >> sx_68 >> dx_68 >> dx_95 >> min;
		range_68[i] = dx_68 - sx_68;
		range_95[i] = dx_95 - sx_95;
		i++;
		if (range_file.eof() == true) break;
	}
	range_file.close();

	TGraph* graph_68 = new TGraph (10, x_0, range_68);
	TGraph* graph_95 = new TGraph (10, x_0, range_95);
	cnv->cd();

	graph_95->SetMarkerSize(1.1);
	graph_95->SetMarkerStyle(20);	
	graph_95->SetMarkerColor(kRed);	
	graph_95->SetTitle("-2#Delta Log(L) confidence ranges vs. integrated luminosity");// (p_{tl1} distribution, c_{W})");
	graph_95->GetXaxis()->SetTitle("Luminosity (/fb)");
	graph_95->GetXaxis()->SetTitleSize(.05);
	graph_95->GetXaxis()->SetTitleOffset(.9);
	graph_95->GetYaxis()->SetTitleSize(.05);
	graph_95->GetYaxis()->SetTitle("range width");
	graph_95->GetYaxis()->SetTitleSize(.05);
	graph_95->GetYaxis()->SetTitleOffset(.8);
	
	graph_95->Draw("AP");
	graph_95->Draw("AP");

	graph_68->SetMarkerSize(1.1);
	graph_68->SetMarkerStyle(20);	
	graph_68->SetMarkerColor(kBlue);	
	graph_68->Draw("P SAME");

	//Fit to find the asymptotic value of the ranges (when the only error is the systematic one)
	TF1 *fit_function = new TF1("f", "[0]  + [1] / sqrt(x)"); 
	fit_function->SetParameter(1, 0.03);
	fit_function->SetParameter(0, 1.9);
	fit_function->SetLineColor(kBlue);
	graph_68->Fit(fit_function);
	fit_function->SetLineColor(kRed);
	graph_95->Fit(fit_function);
	
	
	fit_function->Draw("same");

	TLegend* legend = new TLegend(0.45,0.48,0.83,0.63);
	legend->AddEntry(graph_68,"68% confidence range","p");
	legend->AddEntry(graph_95,"95% confidence range","p");
	legend->Draw();
	gPad->SetLogx();
	cnv->Modified();
	cnv->Update();

	//To save the plot
	//cnv->Print("range_vs_lum_ptl1_cW.png", "png");
	
	myapp->Run();

	return 0;

}

