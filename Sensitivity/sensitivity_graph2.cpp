// c++ sensitivity_graph2.cpp -o sensitivity_graph2 `root-config --cflags --glibs`
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <numeric>

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
#include <TGraphAsymmErrors.h>
#include <TGraphErrors.h>
#include <TH2F.h>
#include <TFrame.h>
#include <TLine.h>
using namespace std ;

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
    TApplication* myapp = new TApplication ("myapp", NULL, NULL);
	TCanvas* cnv = new TCanvas("c1","gerrors2",200,10,1000,500);
    gStyle->SetOptStat(0);



   //we read data from sensitivity.txt
   string line;
   vector<float> x68_v,y_v,yerrl68_v,yerrh68_v;
   vector<float> x95_v,yerrl95_v,yerrh95_v,range_68;
   for(int i=0;i<15;i++) x68_v.push_back(i+0.2);
   for(int i=0;i<15;i++) x95_v.push_back(i+0.5);
   vector<string> operators;
   string op;
   float x, y, yerrl95, yerrl68, yerrh68, yerrh95;
   int i=0;
   ifstream file("sensitivity3.txt");
   float max_err=-1;
   float m;
   while(i<15){

       file >> y >> yerrl95 >> yerrl68 >>yerrh68 >>yerrh95 >> op;
       cout << y <<"\t"<<  yerrl95 <<"\t"<< yerrl68 << "\t"<< yerrh68<<"\t"<<yerrh95<<endl;
       y_v.push_back(y);
       yerrl95_v.push_back(abs(yerrl95));
       yerrl68_v.push_back(abs(yerrl68));
       yerrh68_v.push_back(yerrh68);
       yerrh95_v.push_back(yerrh95);
       range_68.push_back(yerrh68-yerrl68);
       operators.push_back(op);
       m=max(yerrh95,yerrl95);
       max_err=max(m,max_err);

       cout << max_err<<"\t"<<m<<endl;
       i++;

   }
   m=3./2.*max_err;


   vector<float> x_noshape_v,noshapel_v,noshapeh_v,range_noshape;
   for(int i=0;i<15;i++) x_noshape_v.push_back(i+0.7);

   i=0;
   ifstream file1("noshape.txt");
   while(i<15){

       file1 >> y >> yerrl95 >> yerrl68 >>yerrh68 >>yerrh95 >> op;
       cout << y <<"\t"<<  yerrl95 <<"\t"<< yerrl68 << "\t"<< yerrh68<<"\t"<<yerrh95<<endl;
       y_v.push_back(y);
       noshapel_v.push_back(abs(yerrl68));
       noshapeh_v.push_back(abs(yerrh68));
       m=max(yerrh95,yerrl95);
       max_err=max(m,max_err);

       cout << max_err<<"\t"<<m<<endl;
       i++;

   }

  //now we want to order operators from the smallest confidence interval at 68%, to the biggest
  vector<long unsigned int> sorted_indexes = sort_indexes(range_68);

  float y_o[15],yerrl68_o[15],yerrh68_o[15];
  float yerrl95_o[15],yerrh95_o[15];
  string operators_o[15];
  float noshapel_o[15],noshapeh_o[15];
  for (int i = 0; i < range_68.size(); i++)
  {
      operators_o[i]=operators[sorted_indexes[i]];
      //must be at 0 the minimum
      y_o[i]=0.0;
      yerrl68_o[i]=yerrl68_v[sorted_indexes[i]];
      yerrh68_o[i]=yerrh68_v[sorted_indexes[i]];
      yerrl95_o[i]=yerrl95_v[sorted_indexes[i]];
      yerrh95_o[i]=yerrh95_v[sorted_indexes[i]];

      noshapel_o[i]=noshapel_v[sorted_indexes[i]];
      noshapeh_o[i]=noshapeh_v[sorted_indexes[i]];

      cout << operators[sorted_indexes[i]] <<endl;
  }


  m=1;
  TH2F *hr = new TH2F("hr","Sensitivity",15,0,15,2,-m,m);
  hr->GetXaxis()->SetLabelOffset(1);
  hr->GetXaxis()->SetNdivisions(15);
  hr->SetYTitle("Y title");
  hr->Draw();
  cnv->GetFrame()->SetFillColor(21);
  cnv->GetFrame()->SetBorderSize(12);
   vector<float> xerr_v;
   for(int i=0;i<15;i++) xerr_v.push_back(0);
   //constructor of TGraphAsymmErrors is (N,x,y,x_err_low,x_err_high,y_err_low,y_err_high)
   TGraphAsymmErrors * gr1 = new TGraphAsymmErrors(15,&x68_v[0],y_o,&xerr_v[0],&xerr_v[0],yerrl68_o,yerrh68_o);
   TGraphAsymmErrors * gr2 = new TGraphAsymmErrors(15,&x95_v[0],y_o,&xerr_v[0],&xerr_v[0],yerrl95_o,yerrh95_o);
   TGraphAsymmErrors * gr3 = new TGraphAsymmErrors(15,&x_noshape_v[0],y_o,&xerr_v[0],&xerr_v[0],noshapel_o,noshapeh_o);
   gr1->SetLineColor(3);
   gr1->SetLineWidth(4);
   gr1->SetMarkerStyle(34);
   gr1->SetMarkerColor(1);
   gr1->Draw("P");
   cnv->SetGrid();

   gr2->SetLineColor(6);
   gr2->SetLineWidth(4);
   gr2->SetMarkerStyle(6);
   gr2->SetMarkerColor(0);
   gr2->Draw("P");

   gr3->SetLineColor(9);
   gr3->SetLineWidth(4);
   gr3->SetMarkerStyle(6);
   gr3->SetMarkerColor(0);
   gr3->Draw("P");


   //adding labels on the x-axis
   TText *t = new TText();
   t->SetTextAlign(21);
   t->SetTextSize(0.035);
   t->SetTextFont(72);
   //char * operators[15] = {"cHD","cHbox","cW","cHW","cHWB","cll","cHl1","cHl3","cHq1","cHq3","cll1","cqq1","cqq11","cqq3","cqq31"};
   for (Int_t i=0;i<15;i++) {
      t->DrawText(x68_v[i]+0.1,-m-0.4,operators_o[i].c_str());
  }
  TLine *line1 = new TLine( 0,0,15,0);
  line1->SetLineWidth(2);
  line1->SetLineStyle(2);
  line1->SetLineColor(kRed);
  line1->Draw();
  TLegend* legend = new TLegend(0.1,0.75,0.25,0.9);
  legend->AddEntry(gr1,"68% confidence range","l");
  legend->AddEntry(gr2,"95% confidence range","l");
  legend->AddEntry(gr3,"68% limit confidence range","l");
  legend->Draw();


    cnv->Modified();
	cnv->Update();
    cnv->Print("sensitivity3.png","png");
    myapp->Run();
}
