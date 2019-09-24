/*
Plots the variation of the confidence ranges (68% and 95%) by varying the parameter x_0
in the exponential model of the background. The tested variable is ptl1 (6th-order EFT
operator: Q_W). Data are contained in the file "range_ptl1_bkg.txt", which contains 12 raws 
(for 12 values of x_0 tested); the 5 values in each raw are: the left endpoint of the
95% range, the left endpoint of the 68% range, the right enpoint of the 68% range, the right
endpoint of the 95% range and the minimum point of the likelihood scan. The 12 tested values
of x_0 are contained in the file "x_0_ptl1.txt".

c++ -o range_vs_x0 range_vs_x0.cpp `root-config --glibs --cflags`

./range_vs_x0
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

using namespace std ;

int main (int argc, char** argv) 
{
	gStyle->SetLabelSize(.04, "XY");
	TApplication* myapp = new TApplication ("myapp", NULL, NULL);
	TCanvas* cnv = new TCanvas("cnv","cnv",0,0,700,500);
	
	ifstream x_0_file;
	x_0_file.open("x_0_ptl1.txt", ios::in);
	if (x_0_file.good() == false) cout << "-----Error while opening x_0_ptl1.txt-----" << endl;
	float x_0[12]; 
	int i = 0;
	while (true) 
	{
		x_0_file >> x_0[i];
		i++;
		if (x_0_file.eof() == true) break;
	}
	x_0_file.close();

	ifstream range_file;
	range_file.open("range_ptl1_bkg.txt", ios::in);
	if (range_file.good() == false) cout << "-----Error while opening range.txt-----" << endl;
	
	float range_68[12], range_95[12]; //12 different values of x0
	float sx_95, sx_68, dx_68, dx_95, min;
	i = 0;
	for (int i = 0; i < 12; i++) 
	{
		range_file >> sx_95 >> sx_68 >> dx_68 >> dx_95 >> min;
		range_68[i] = dx_68 - sx_68;
		range_95[i] = dx_95 - sx_95;		
	}
	range_file.close();

	//PLOT
	TGraph* graph_68 = new TGraph (12, x_0, range_68);
	TGraph* graph_95 = new TGraph (12, x_0, range_95);
	cnv->cd();

	graph_95->SetMarkerSize(1.1);
	graph_95->SetMarkerStyle(20);	
	graph_95->SetMarkerColor(kRed);	
	graph_95->SetTitle("-2#Delta Log(L) confidence ranges vs. x_{0} (p_{tl1} distribution)");
	graph_95->GetXaxis()->SetTitle("x_{0} (Gev)");
	graph_95->GetXaxis()->SetTitleSize(.05);
	graph_95->GetXaxis()->SetTitleOffset(.9);
	graph_95->GetYaxis()->SetTitleSize(.05);
	graph_95->GetYaxis()->SetTitle("range width");
	graph_95->GetYaxis()->SetTitleSize(.05);
	graph_95->GetYaxis()->SetTitleOffset(.9);
	graph_95->GetYaxis()->SetRangeUser(0.15, 1.2);
	graph_95->Draw("APL");

	graph_68->SetMarkerSize(1.1);
	graph_68->SetMarkerStyle(20);	
	graph_68->SetMarkerColor(kBlue);	

	graph_68->Draw("PL SAME");

	TLegend* legend = new TLegend(0.45,0.18,0.83,0.33);
	legend->AddEntry(graph_68,"68% confidence range","p");
	legend->AddEntry(graph_95,"95% confidence range","p");
	legend->Draw();

	cnv->Modified();
	cnv->Update();

	//To save the plot
	//cnv->Print("range_vs_x0_ptl1.png", "png");
	
	myapp->Run();

	return 0;

}

