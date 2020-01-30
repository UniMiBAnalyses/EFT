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
using namespace std ;

int main (int argc, char** argv)
{
    TApplication* myapp = new TApplication ("myapp", NULL, NULL);
	TCanvas* cnv = new TCanvas("c1","gerrors2",200,10,1000,500);
    gStyle->SetOptStat(0);

    TH2F *hr = new TH2F("hr","Sensitivity",15,0,15,2,-0.5,0.5);
   hr->GetXaxis()->SetLabelOffset(1);
   hr->GetXaxis()->SetNdivisions(15);
   hr->SetYTitle("Y title");
   hr->Draw();
   cnv->GetFrame()->SetFillColor(21);
   cnv->GetFrame()->SetBorderSize(12);

   //we read data from sensitivity.txt
   string line;
   vector<float> x_v,y_v,yerrl_v,yerrh_v;
   float x,y,yerrl, yerrh;
   int i=0;
   ifstream file("sensitivity.txt");
   while(true){
       file >> x >> y >> yerrl >>yerrh;
       x_v.push_back(x);
       y_v.push_back(y);
       yerrl_v.push_back(yerrl);
       yerrh_v.push_back(yerrh);
       cout << x <<"\t" << y<<"\t" <<yerrl<<"\t" << yerrh<<"\t"<<endl;
       if(file.eof()==true){
           break;
       }
   }

   vector<float> xerr_v;
   for(int i=0;i<14;i++) xerr_v.push_back(0);
   TGraphAsymmErrors * gr1 = new TGraphAsymmErrors(14,&x_v[0],&y_v[0],&xerr_v[0],&xerr_v[0],&yerrl_v[0],&yerrh_v[0]);
   gr1->SetMarkerColor(kBlue);
   gr1->SetMarkerStyle(21);
   gr1->Draw("P");

   //adding labels on the x-axis
   TText *t = new TText();
   t->SetTextAlign(21);
   t->SetTextSize(0.035);
   t->SetTextFont(72);
   char * operators[14] = {"cHD","cHbox","cW","cHB","cHW","cHWB","cll","cHl1","cHl3","cHq1","cHq3","cHe","cHu","cHd"};
   for (Int_t i=0;i<14;i++) {
      t->DrawText(x_v[i],-0.4,operators[i]);
  }



    cnv->Modified();
	cnv->Update();
    myapp->Run();
}
