/*
Plots a bar graph with two bars for every kinetic variable, one for the width of the 68% confidence
range and the other for the 95% confidence range. Data are contained in the files "range_cW.txt" and
"range_cHW.txt", where every raw represents a kinetic variable and contains 5 values: the left endpoint
of the 95% range, the left endpoint of the 68% range, the right enpoint of the 68% range, the right
endpoint of the 95% range and the minimum point of the likelihood scan. The last raw contains the data
of the "no shape" distribution (histogram with one bin), which gives un upper limit to the ranges.

c++ -o range_vs_kineticvar range_vs_kineticvar.cpp `root-config --glibs --cflags`

./range_vs_kineticvar

*/

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <numeric>

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
#include <TLine.h>
#include <TLatex.h>

using namespace std ;

//to order the values of a vector from the smallest to the greatest,
//keeping the information on the indices
template <typename T>
vector<size_t> sort_indexes(const vector<T> &v) {

  // initialize original index locations
  vector<size_t> idx(v.size());
  iota(idx.begin(), idx.end(), 0);

  // sort indexes based on comparing values in v
  sort(idx.begin(), idx.end(),
       [&v](size_t i1, size_t i2) {return v[i1] < v[i2];});

  return idx;
}

int main (int argc, char** argv)
{
	gStyle->SetLabelSize(0.04, "XY");
	TApplication* myapp = new TApplication ("myapp", NULL, NULL);

	string x_axis[9]   = {"met","m_{jj}","m_{ll}","p_{tl1}","p_{tl2}","p_{tj1}","p_{tj2}",
		"#Delta#eta_{jj}","#Delta#phi_{jj}"};

	ifstream range_file;

	//for cW
	//range_file.open("range_cW.txt", ios::in);

	//for cHW
	range_file.open(argv[1], ios::in);

	if (range_file.good() == false) cout << "-----Error while opening range_no_sel.txt-----" << endl;
    cout <<"Bella"<<endl;
	vector<float> range_68;
	vector <float> range_95;
	float sx_95, sx_68, dx_68, dx_95, min;
	float sx_95_noshape, sx_68_noshape, dx_68_noshape, dx_95_noshape;
	int i = 1;
	while (i <=10) //14 raws in the file (13 variables + "no shape" distribution)
	{
		if (i < 10) {
			range_file >>min>> sx_95 >> sx_68 >> dx_68 >> dx_95  ;
            cout<<dx_68 - sx_68<<endl;
			range_68.push_back((dx_68 - sx_68));
			range_95.push_back((dx_95 - sx_95));
		}
		else range_file>> min >> sx_95_noshape >> sx_68_noshape >> dx_68_noshape >> dx_95_noshape ;

		i++;
	}
	range_file.close();

	//sorts the variables from the most discriminant to the less discriminant
	vector<long unsigned int> sorted_indexes = sort_indexes(range_68);

	float range_68_sorted[9];
	float range_95_sorted[9];
	string x_axis_sorted[9];

	for (int i = 0; i < range_68.size(); i++)
	{
		range_68_sorted[i] = range_68[sorted_indexes[i]];
		range_95_sorted[i] = range_95[sorted_indexes[i]];
		x_axis_sorted[i] = x_axis[sorted_indexes[i]];
	}


	//PLOT
    cout <<"ciao"<<endl;
	TCanvas *cnv = new TCanvas("cnv","cnv",1000,500);
	cnv->SetGrid();
	TH1F *h1 = new TH1F("h1","Widths of confidence ranges",9,0,9);
	h1->SetFillColor(kBlue-7);
	h1->SetBarWidth(0.08);
	h1->SetBarOffset(0.4);
	h1->SetStats(0);
	h1->SetMinimum(0);

	//for cW
	//h1->SetMaximum(2.);

	//for cHW


	for (i=1; i<=9; i++) {
		h1->SetBinContent(i, range_68_sorted[i-1]);
		h1->GetXaxis()->SetBinLabel(i,x_axis_sorted[i-1].c_str());
	}
	h1->GetXaxis()->SetLabelSize(.06);
	h1->GetYaxis()->SetTitle("range width");
	h1->GetYaxis()->SetTitleSize(0.05);
	h1->GetYaxis()->SetTitleOffset(0.8);
	h1->Draw("b");

	TH1F *h2 = new TH1F("h2","h2",9,0,9);
	h2->SetFillColor(kRed-7);
	h2->SetBarWidth(0.08);
	h2->SetBarOffset(0.55);
	h2->SetStats(0);
	for (i=1;i<=9;i++){
            h2->SetBinContent(i, range_95_sorted[i-1]);

            cout <<"&\t"<<range_68[i-1]<<endl;
            //cout << x_axis_sorted[i-1] <<"\t" <<range_68_sorted[i-1]<<"\t"<<range_95_sorted[i-1] <<endl;
    }
    cout << x_axis_sorted[0].c_str() <<endl;
    /*for (i=1;i<=9;i++){
        cout << "&\t"<<range_68_sorted[i-1];
    }*/

    h1->SetMaximum(1.1*range_95_sorted[8]);
	h2->Draw("b same");

	float range_68_noshape = dx_68_noshape - sx_68_noshape;
	float range_95_noshape = dx_95_noshape - sx_95_noshape;

	TLine* line_68 = new TLine(0, range_68_noshape, 9, range_68_noshape);
	line_68->SetLineColor(kBlue);
	line_68->SetLineWidth(2.);
	line_68->Draw("same");

	TLine* line_95 = new TLine(0, range_95_noshape, 9, range_95_noshape);
	line_95->SetLineColor(kRed);
	line_95->SetLineWidth(2.);
	line_95->Draw("same");

    //da aggiungere la luminosità!!
    float luminosity=100;
    TString nameLabel = Form ("L = %.1f fb^{-1}   (13 TeV)", luminosity);
    auto tex3 = new TLatex(0.62,0.905,nameLabel.Data());
    tex3->SetNDC();
    tex3->SetTextFont(52);
    tex3->SetTextSize(0.035);
    tex3->SetLineWidth(2);
    tex3->Draw("same");

	TLegend* legend = new TLegend(0.1,0.7,0.48,0.9);
	legend->AddEntry(h1,"68% confidence range","f");
	legend->AddEntry(h2,"95% confidence range","f");
    legend->AddEntry(line_68,"68% confidence range integral","l");
    legend->AddEntry(line_95,"95% confidence range integral","l");
	legend->Draw();

	//To save the plot
	//cnv->Print("range_vs_kineticvar_cW.png", "png");
	cnv->Print("range_vs_kineticvar_cHbox.png", "png");
    cnv->Modified();
    cnv->Update();
	myapp->Run();

	return 0;

}
