// c++ sensitivity_graph.cpp -o sensitivity_graph `root-config --cflags --glibs`
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
#include <TGraphAsymmErrors.h>
#include <TGraphErrors.h>
#include <TH2F.h>
#include <TFrame.h>
#include <TLine.h>
using namespace std ;

int main (int argc, char** argv)
{
    TApplication* myapp = new TApplication ("myapp", NULL, NULL);
	TCanvas* cnv = new TCanvas("c1","gerrors2",200,10,1000,500);
    gStyle->SetOptStat(0);



   //we read data from sensitivity.txt
   string line;
   vector<float> x_v,y_v,yerrl_v,yerrh_v;
   float x,y,yerrl, yerrh;
   int i=0;
   ifstream file("sensitivity.txt");
   float max_err=-1;
   float m;
   while(true){
       file >> x >> y >> yerrl >>yerrh;
       cout << x <<"\t"<< y <<"\t"<< yerrl <<"\t"<<yerrh<<endl;
       x_v.push_back(x);
       y_v.push_back(y);
       yerrl_v.push_back(yerrl);
       yerrh_v.push_back(yerrh);
       m=max(yerrh,yerrl);
       max_err=max(m,max_err);
       cout << max_err<<"\t"<<m <<endl;
       if(file.eof()==true){
           break;
       }
   }
   m=3./2.*max_err;

   vector<float> x_v2,y_v2,yerrl_v2,yerrh_v2;
   i=0;
   ifstream file2("sensitivity2.txt");
   float max_err2=-1;
   float m2;
   while(true){
       file2 >> x >> y >> yerrl >>yerrh;
       x_v2.push_back(x+0.2);
       y_v2.push_back(y);
       yerrl_v2.push_back(yerrl);
       yerrh_v2.push_back(yerrh);
       m2=max(yerrh,yerrl);
       max_err2=max(m2,max_err2);
       cout << max_err2<<"\t"<<m2 <<endl;
       if(file2.eof()==true){
           break;
       }
   }
   m2=3./2.*max_err2;


   TH2F *hr = new TH2F("hr","Sensitivity",15,0,15,2,-m,m);
  hr->GetXaxis()->SetLabelOffset(1);
  hr->GetXaxis()->SetNdivisions(15);
  hr->SetYTitle("Y title");
  hr->Draw();
  cnv->GetFrame()->SetFillColor(21);
  cnv->GetFrame()->SetBorderSize(12);
   vector<float> xerr_v;
   for(int i=0;i<14;i++) xerr_v.push_back(0);
   vector<float> xerr_v2;
   for(int i=0;i<14;i++) xerr_v2.push_back(0);
   TGraphAsymmErrors * gr1 = new TGraphAsymmErrors(14,&x_v[0],&y_v[0],&xerr_v[0],&xerr_v[0],&yerrl_v[0],&yerrh_v[0]);
   TGraphAsymmErrors * gr2 = new TGraphAsymmErrors(14,&x_v2[0],&y_v2[0],&xerr_v2[0],&xerr_v2[0],&yerrl_v2[0],&yerrh_v2[0]);
   gr1->SetLineColor(3);
   gr1->SetLineWidth(4);
   gr1->SetMarkerStyle(24);
   gr1->SetMarkerColor(2);
   gr1->Draw("P");
   cnv->SetGrid();

   gr2->SetLineColor(6);
   gr2->SetLineWidth(4);
   gr2->SetMarkerStyle(24);
   gr2->SetMarkerColor(5);
   gr2->Draw("P");
   cnv->SetGrid();

   //adding labels on the x-axis
   TText *t = new TText();
   t->SetTextAlign(21);
   t->SetTextSize(0.035);
   t->SetTextFont(72);
   char * operators[14] = {"cHD","cHbox","cW","cHB","cHW","cHWB","cll","cHl1","cHl3","cHq1","cHq3","cHe","cHu","cHd"};
   for (Int_t i=0;i<14;i++) {
      t->DrawText(x_v[i],-m-0.2,operators[i]);
  }
  TLine *line1 = new TLine( 0,0,15,0);
  line1->SetLineWidth(2);
  line1->SetLineStyle(2);
  line1->SetLineColor(kRed);
  line1->Draw();
  TLegend* legend = new TLegend(0.1,0.7,0.48,0.9);
  legend->AddEntry(gr2,"68% confidence range","f");
  legend->AddEntry(gr1,"95% confidence range","f");
  legend->Draw();


    cnv->Modified();
	cnv->Update();
    cnv->Print("sensitivity.png","png");
    myapp->Run();
}
