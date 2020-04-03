// c++ -o datacard_twoOp_creator `root-config --glibs --cflags` CfgParser.cc  -lm datacard_twoOp_creator.cpp
/*
one should pass the program a config file like file.cfg
to run: ./datacard_creator file.cfg

the difference between this program and datacard_creator is only the list of distributions handled
we don't anymore sm, lin and quad, but we have sm, lin1,quad1,lin2,quad2 and interf
the datacard will then contain sm, linear_1, quad_1,linear_2, quad_2 and linear_mixed_12
this also requires that wilson_coeff is the same for both operators(meaning they have the same value in the generation process)

procedure used to generate interference term is importa model in which two wilson coeff are non zero, generate process NP=2 NP^2==1
Meanwhile the oders distribution were taken from normal generations.

*/


#include <iomanip>
#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include <iterator>
#include <cstdlib>
#include <cassert>
#include <fstream>
#include <algorithm>


#include <TFile.h>
#include <TNtuple.h>
#include <TTreeReader.h>
#include <TH1.h>
#include <TH1F.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLegend.h>
#include <THStack.h>
#include <TText.h>
#include <TLine.h>
#include <TLatex.h>


#include <TLorentzVector.h>


#include "CfgParser.h"

using namespace std ;


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


bool replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

int main (int argc, char ** argv)
{
    TH1::SetDefaultSumw2();
    if (argc < 2)
    {
      cerr << "Forgot to put the cfg file --> exit " << endl ;
      return 1 ;
    }

      CfgParser * gConfigParser = new CfgParser (argv[1]);

      //tells the program to plot all the variables passed
      string plot = gConfigParser->readStringOpt ("general::plot");

      //tells the program which variables to use
      vector<string> variables = gConfigParser->readStringListOpt ("general::variables") ;
      //let's retrive info about histograms we wanto to create
      float histos_info[variables.size()][3];
      for(int i=0;i<variables.size();i++){
          histos_info[i][0]=gConfigParser->readFloatOpt (variables[i]+"::bins") ;
          histos_info[i][1]=gConfigParser->readFloatOpt (variables[i]+"::min") ;
          histos_info[i][2]=gConfigParser->readFloatOpt (variables[i]+"::max") ;
      }

      //wilson_coeff should be a float like 0.3, it will be processed after
      string wc = gConfigParser->readStringOpt ("general::wilson_coeff") ;
      float wilson_coeff_plot = gConfigParser->readFloatOpt ("general::wilson_coeff_plot") ;
      string wc_string(wc);
  	  replace(wc_string, ".", "p");
  	  float wilson_coeff=stof(wc);

      cout << "Wilson coeff: "<<wc_string<<"\t"<<to_string(wilson_coeff)<<endl;
      string g_syntax = gConfigParser->readStringOpt ("general::g_syntax");
      string input_files[6];

      //using input_files instead
      for(int i=0;i<6;i++) input_files[i]=gConfigParser->readStringListOpt ("general::input_files")[i];



      float luminosity = stof(gConfigParser->readStringOpt ("general::luminosity"));
      //otherwise if you don't want to use my syntax(which I highly reccomend not to do)
      //string infile = gConfigParser->readStringListOpt ("general::infile") ;

      //name of ntuples in this order: sm, linear(interference), quadratic(BSM)
      //also the global_numbers should be in the format name_ntuple_nums(which also Vittorio used)
      vector<string> name_ntuples = gConfigParser->readStringListOpt ("general::name_ntuples") ;
      vector<string> name_global_numbers;
      for(int i=0;i<name_ntuples.size();i++) name_global_numbers.push_back(name_ntuples[i]+"_nums");

      //reading cuts
      map <string, float> cuts;
      string kinetic_variables[] = {"met","mjj","mll","ptl1","ptl2","ptj1","ptj2","etaj1","etaj2",
          "phij1","phij2","deltaetajj","deltaphijj", "no shape histo"};


      for(int i=0;i<12;i++){
          if(i!=9 && i!=10){
              //no bounds on phij
              cuts[kinetic_variables[i]]=stof(gConfigParser->readStringOpt ("cuts::"+kinetic_variables[i]));
          }
      }

      //this is for output names only
      string histo_names[] = {"histo_sm", "histo_linear_1", "histo_quad_1", "histo_linear_2", "histo_quad_2", "histo_linear_mixed_12"};
      string dist_names[]={"sm", "linear_1", "quad_1", "linear_2", "quad_2", "linear_mixed_12"};

      /*

      //A nice spatafiatta on variable binning and RMS, apparently useless
      float min[] = {30, 500, 20, 25, 20, 30, 30, -5., -5., -M_PI, -M_PI, 2.5, 0, 0};
  	  float max[] = {1620, 8600, 1950, 1250, 600, 2000, 1100, 5., 5., M_PI, M_PI, 10., M_PI, 2};


  	//RMS values for the 11 kinetic variables' distributions (see getRMS_cW.cpp)
  	string line;
  	float RMS_array[13];
  	int i=0;
      string filename_rms="../"+string(OPERATOR)+"_"+string(event_name)+"_"+wc_string+"_rms.txt";
  	ifstream file_rms(filename_rms.c_str());
  	if(file_rms.is_open()){
  		while(getline(file_rms,line)){
  			RMS_array[i]=stof(line);
  			i++;
  		}
  		file_rms.close();
  	}
      string filename_min_max="../"+string(OPERATOR)+"_"+string(event_name)+"_"+wc_string+"_min_max.txt";
      cout << filename_min_max<<endl;
  	ifstream file_min_max(filename_min_max.c_str());
      i=0;
      float mi,ma;
      while(true){
          file_min_max >> mi >> ma;
          min[i]=mi;
          max[i]=ma;
          //cout << min[i] << max[i] <<endl;
          if(file_min_max.eof()==true){
              break;
          }
          i++;
      }

  	//float RMS_array[] = {87.2333, 953.58, 116.004, 70.2788, 31.7565, 139.601, 88.028, 1.98489, 2.35187, 1.81397, 1.81276, 1.99129, 0.806861};

  	//variable width binning for the first 7 variables: met, mjj, mll, ptl1, ptl2, ptj1 (fixed width binning for the angular variables)
  	vector<float> bins_edges_vectors[variables.size()];
  	float first_limit[] = {300, 8000, 1350, 900, 380, 1600, 900}; //from here bin_width*2
  	float second_limit[] = {400, 8600, 1700, 1250, 430, 1800, 1000}; //from here bin_width*4
      for(int i=0;i<7;i++){
          first_limit[i]=(max[i]-min[i])*1/2+min[i];
          second_limit[i]=(max[i]-min[i])*3/4+min[i];
      }
  	int Nbins[variables.size()];
  	//int var_number_plot = 0;

  	//binning
    int j=0;
  	for (int i = 0; i < 13; i++)
  	{
        if(kinetic_variables[i]==variables[j]){
            int var_number=i;
            //do something
            //if (kinetic_variable_plot == kinetic_variables_real[var_number]) var_number_plot = var_number;
      		bins_edges_vectors[j].push_back(min[var_number]);

            //check if the variable is one of the first seven
      		if (var_number < 7) //variable width binning
      		{
      			while (true)
      			{
      				if (bins_edges_vectors[j].back() < first_limit[var_number])
      				{
      					//bin_width in the first region = 1/3*RMS
      					bins_edges_vectors[j].push_back(bins_edges_vectors[j].back() + RMS_array[var_number]/3.);
      				}
      				else if (bins_edges_vectors[j].back() < second_limit[var_number])
      				{
      					//bin_width in the second region = 2/3*RMS
      					bins_edges_vectors[j].push_back(bins_edges_vectors[j].back() + RMS_array[var_number]*2/3.);
      				}
      				else if (bins_edges_vectors[j].back() < max[var_number])
      				{
      					//bin_width in the third region = RMS
      					bins_edges_vectors[j].push_back(bins_edges_vectors[j].back() + RMS_array[var_number]);
      				}
      				else break;
      			}
      			Nbins[j] = bins_edges_vectors[j].size() - 1;
      		}
      		else if (var_number != 13) //fixed width binning for the angular variables
      		{
      			Nbins[j] = floor((max[var_number]-min[var_number])/((1./3.)*RMS_array[var_number]));
      			float width_bin = (max[var_number]-min[var_number])/Nbins[var_number];
      			while (bins_edges_vectors[j].back() < max[var_number])
      			{
      				bins_edges_vectors[j].push_back(bins_edges_vectors[j].back() + width_bin);
      			}
      		}
      		else //one bin histogram, defined in the range [0, 2] and filled with ones
      		{
      			bins_edges_vectors[j].push_back(2.);
      			Nbins[j] = 1;
      		}

            if(j<variables.size()-1) j++;
            else break;
        }


  	}
*/



    vector <TH1F> histos[variables.size()]; //array of vectors
    float integrals[variables.size()][6];

    //generally there might be three different files
    TFile * myfile[6];
    //again if you prefer using the infile the line below is just path=infile

    for(int i=0;i<6;i++) myfile[i]=new TFile(input_files[i].c_str());


    for(int ntuple_number=0; ntuple_number < 6; ntuple_number++){
            TH1F* global_numbers = (TH1F*) myfile[ntuple_number]->Get(name_global_numbers[ntuple_number].c_str()) ;
      		float cross_section = global_numbers->GetBinContent(1);
      		float sum_weights_total = global_numbers->GetBinContent(2);
      		float sum_weights_selected = global_numbers->GetBinContent(3);
              cout <<"xs:"<< cross_section << "\nsum_weights_tot:"<<sum_weights_total<<"\nsum_weights_selected"<<sum_weights_selected<<endl;

      		float normalization = cross_section*luminosity/sum_weights_total;

      		for (int var_number = 0; var_number < variables.size(); var_number++)
      		{
                cout << "ntuple num:\t"<<ntuple_number<<"\nvar_number:\t"<<var_number<<endl;
                //instead of using the RMS one can use min, max and nbins in order to have a bin width
                //the plot will be divided into 3 zones, the first with a bin width of 2/3bw, the second
                //with bw, and third with  4/3 width in order to prevent empty bins at the tails.
                vector<float> bin_edges;
                bin_edges.clear();
                int nbins=histos_info[var_number][0];
                float min=histos_info[var_number][1];
                float max=histos_info[var_number][2];
                TH1F* histo;
                if(variables[var_number]!="deltaetajj" && variables[var_number]!="deltaphijj" && variables[var_number]!="noshape"){
                    //calculate the first limit, second
                    float first_limit=(max-min)*3/4.+min;
                    float second_limit=(max-min)*4/5.+min;
                    float bin_width=(max-min)/(float)nbins;
                    bin_edges.push_back(min);

                    while (true)
          			{
          				if (bin_edges.back() < first_limit)
          				{
          					//bin_width in the first region
          					bin_edges.push_back(bin_edges.back() + bin_width/3.);
          				}
          				else if (bin_edges.back() < second_limit)
          				{
          					//bin_width in the second region
          					bin_edges.push_back(bin_edges.back() + bin_width*2/3);
          				}
          				else if (bin_edges.back() < max)
          				{
          					//bin_width in the third region
          					bin_edges.push_back(bin_edges.back() + bin_width);
          				}
          				else break;
          			}
          			int Nbins = bin_edges.size() - 1;
                    histo = new TH1F(variables[var_number].c_str(), histo_names[ntuple_number].c_str(), Nbins, bin_edges.data());

            }
            else{
                histo = new TH1F(variables[var_number].c_str(), histo_names[ntuple_number].c_str(), nbins, min,max);
            }
      			//float* bins_edges = bins_edges_vectors[var_number].data();
      			//TH1F* histo = new TH1F(variables[var_number].c_str(), histo_names[ntuple_number].c_str(), Nbins[var_number], bins_edges);
                TTreeReader reader (name_ntuples[ntuple_number].c_str(), myfile[ntuple_number]);
                TTreeReaderValue<float> var (reader, "w");
                if(variables[var_number]!="deltaetajj" && variables[var_number]!="deltaphijj" && variables[var_number]!="noshape"){
                //this can't be read if the variables are delta...
        		      var=TTreeReaderValue<float>(reader, variables[var_number].c_str());
                }
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

    			//selections
    			while (reader.Next ())
    			{
                    //here are the cuts
                    //one should provide lower bound for met, mjj, mll, ptl1, ptl2, ptj1, ptj2,deltaetajj
                    //symmetric bounds for etaj1 and etaj2

                    if (*met > cuts["met"] && *mjj > cuts["mjj"] && *mll > cuts["mll"] && *ptl1 > cuts["ptl1"] && *ptl2 > cuts["ptl2"] && *ptj1 > cuts["ptj1"] && *ptj2 > cuts["ptj2"] && abs(*etaj1) < cuts["etaj1"] &&
                      abs(*etaj2) < cuts["etaj2"] && abs(*etaj1 - *etaj2) > cuts["deltaetajj"])
    				{
                        if (variables[var_number]!="deltaetajj" && variables[var_number]!="deltaphijj" && variables[var_number]!="noshape"){
                            histo->Fill(*var,*weight);
                        }
    					else if (variables[var_number]=="deltaetajj") histo->Fill(abs(*etaj1 - *etaj2), *weight); //deltaetajj
    					else if (variables[var_number]=="deltaphijj") //deltaphijj
    					{
    						if (abs(*phij1 - *phij2) < M_PI){
                                    histo->Fill(abs(*phij1 - *phij2),*weight);
                            }
    						else{
                                    histo->Fill(2.*M_PI - abs(*phij1 - *phij2), *weight);
                            }
    					}
                        else histo->Fill(1, *weight); //no shape distribution
    				}
                    //histo->Fill(*var,*weight);


    			}

    			histo->Scale(normalization);

    			if (ntuple_number == 1) histo->Scale(1./wilson_coeff);  //linear term
    			if (ntuple_number == 2) histo->Scale(1./(wilson_coeff*wilson_coeff));  //quadratic term

                if (ntuple_number == 3) histo->Scale(1./wilson_coeff);  //linear term
    			if (ntuple_number == 4) histo->Scale(1./(wilson_coeff*wilson_coeff));  //quadratic term
                if (ntuple_number == 5) histo->Scale(1./(wilson_coeff*wilson_coeff));

    			//overflow bin events moved in the last bin
                //last bin content=last bin content + content of overflow bin
    			histo->SetBinContent(histo->GetNbinsX(), histo->GetBinContent(histo->GetNbinsX()) + histo->GetBinContent(histo->GetNbinsX() + 1));
    			histo->SetBinContent(histo->GetNbinsX() + 1, 0.);

    			histo->SetName(histo_names[ntuple_number].c_str());

    			//To check if there are no empty bins in the sm distribution
    			/*if (ntuple_number !=-1)
    			{
    				for (int bin_counter = 0; bin_counter < histo->GetNbinsX() + 2 ; bin_counter++) {
    					if (histo->GetBinContent(bin_counter) != 0)
    						cout << histo->GetBinContent(bin_counter) << endl;
    					else
    						cout << "XXXXXXX" << endl;
    				}
    				cout << endl;
    			}*/
                cout << "arrivato "<<var_number+ntuple_number<<endl;
    			histos[var_number].push_back(*histo);
    			integrals[var_number][ntuple_number] = histo->Integral();
                cout << "arrivato "<<var_number+ntuple_number<<endl;
    			histo->Reset();
            }
            delete global_numbers;
      }

      //generic tfile variable, will be necessary to create each file for the variables
      TFile * f;
      vector<TFile*> files;
      string filename;
      for(int i=0;i<variables.size();i++){
          //doing stuff for this kinetic variable
          //creating all the root files
          filename = "data_"+wc_string+"_"+variables[i]+"_twoOp.root";
          cout << "Creating this file:\t"<<filename<<endl;
          f = new TFile (filename.c_str(),"recreate");
          files.push_back(f);
      }

    //prints the integrals (to be inserted in datacard.txt) and saves the histograms in the root file
    string separator= "----------------------------------------------------------------------------------------------------------------------------------";
    for (int var_number = 0; var_number < variables.size(); var_number ++)
  	{
  		files[var_number]->cd();
  		histos[var_number][0].Write();
  		histos[var_number][1].Write();
  		histos[var_number][2].Write();
        histos[var_number][3].Write();
  		histos[var_number][4].Write();
  		histos[var_number][5].Write();
  		files[var_number]->Write();
  		files[var_number]->Close();

  		cout << variables[var_number].c_str() << "---------------------------------" << endl;
  		for (int ntuple_number = 0; ntuple_number < 6; ntuple_number++) cout << integrals[var_number][ntuple_number] << "\t";
  		cout << endl;

        filename="datacard_"+wc_string+"_"+variables[var_number]+"_twoOp.txt";
        ofstream output_datacard(filename.c_str());
        output_datacard << separator <<endl;
        filename = "data_"+wc_string+"_"+variables[var_number]+"_twoOp.root";
        //* all data, * all phase space
        output_datacard << separator <<endl;
        output_datacard << "imax *" <<endl;
        output_datacard << "jmax *" <<endl;
        output_datacard << "kmax *" <<endl;
        output_datacard << separator <<endl;
        output_datacard <<"shapes *\t* "+filename+" histo_$PROCESS $PROCESS_$SYSTEMATIC"<<endl;
        output_datacard <<"shapes data_obs\t* "+filename+" "+histo_names[0]<<endl;
        output_datacard << separator <<endl;
        output_datacard << "bin\t test"<<endl;
        output_datacard <<"observation\t"<<integrals[var_number][0]<<endl;
        output_datacard <<separator<<endl;
        output_datacard << "bin\t";
        for (int ntuple_number = 0; ntuple_number < 6; ntuple_number++) output_datacard <<"test"<<"\t";
        output_datacard <<endl;
        output_datacard << "process\t";
        for (int ntuple_number = 0; ntuple_number < 6; ntuple_number++) output_datacard << dist_names[ntuple_number] << "\t";
        output_datacard <<endl;
        output_datacard << "process\t";
        for (int ntuple_number = 0; ntuple_number < 6; ntuple_number++) output_datacard << ntuple_number<< "\t";
        output_datacard <<endl;
        output_datacard << "rate\t";
        for (int ntuple_number = 0; ntuple_number < 6; ntuple_number++) output_datacard << integrals[var_number][ntuple_number] << "\t";
        output_datacard <<endl;
        output_datacard << separator <<endl;
        output_datacard <<"lumi"<<"\t"<<"lnN"<<"\t";
        //2% of uncertanty on signal and background
        for (int ntuple_number = 0; ntuple_number < 6; ntuple_number++) output_datacard << "1.02" << "\t";
        output_datacard <<endl;
        //some other nuissance
        output_datacard <<"bla"<<"\t"<<"lnN"<<"\t";

        for (int ntuple_number = 0; ntuple_number < 6; ntuple_number++){
            if(ntuple_number!=2){
                output_datacard << "-" << "\t";
            }
        } output_datacard << "1.05" << "\t";
        output_datacard <<endl;
        output_datacard.close();


  	}

    if(plot=="true"){
        //TApplication* myapp = new TApplication ("myapp", NULL, NULL);

        /*auto * c = new TCanvas();
        c->cd();
        histos[1][2].Scale(wilson_coeff*wilson_coeff); // quadratic scaling relation
        histos[1][1].Scale(wilson_coeff);	 // linear scaling relation
        histos[1][0].SetFillStyle(3001);
        histos[1][0].SetFillColor(kOrange+1);
        histos[1][2].SetFillStyle(3001);
        histos[1][2].SetFillColor(kBlue-7);
        histos[1][1].SetFillColor(kGreen+1);
        histos[1][1].SetFillStyle(3001);
        histos[1][0].Draw("hist same");
        histos[1][2].Draw("hist same");
        histos[1][1].Draw("hist same");
        gPad->BuildLegend(0.60,0.70,0.90,0.90,"");*/

      	vector <TCanvas*> cnv;
        //TCanvas * p,p1,p2,p3;
        for(int var_number=0;var_number<variables.size();var_number++){
            string canvas_name="cnv"+to_string(var_number);
            //p=new TCanvas(canvas_name.c_str(),canvas_name.c_str(),0,0,1800,800);
            TCanvas * p1 = new TCanvas("uno","uno",0,0,1000,800);
            TCanvas * p2 = new TCanvas("due","due",0,0,1000,800);
            TCanvas * p3 = new TCanvas("tre","tre",0,0,1000,800);
            /*cnv.push_back(p1);
            cnv.push_back(p2);
            cnv.push_back(p3);*/

            //cnv.push_back(p);
            //p->Divide(3,1);

          	string kinetic_variables_tex[] = {"MET","m_{jj}","m_{ll}","p_{tl1}","p_{tl2}","p_{tj1}","p_{tj2}",
          		"#eta_{j1}","#eta_{j2}","#phi_{j1}","#phi_{j2}","#Delta#eta_{jj}","#Delta#phi_{jj}"};
          	THStack* h_stack = new THStack("hs","");


            float wc = wilson_coeff_plot;

          	histos[var_number][2].Scale(wc*wc); // quadratic scaling relation
          	histos[var_number][1].Scale(wc);	 // linear scaling relation

            histos[var_number][4].Scale(wc*wc); // quadratic scaling relation
          	histos[var_number][3].Scale(wc);	 // linear scaling relation

            histos[var_number][5].Scale(wc*wc); //interf two Ops




          	//PLOT

            //not needed probably:

          	//change of variable in the y axis, from "number of events" to "(number of events)/(bin width)"
          	/*for (int j = 0; j < 3; j++) {
          		for (int i = 0; i < histos[var_number][j].GetNbinsX()+1; i++)
          		{
          			histos[var_number][j].SetBinContent(i, histos[var_number][j].GetBinContent(i)/histos[var_number][j].GetBinWidth(i));
          		}
          	}*/

            //get max of all histos
            float maxm = 0;
            for(int i=0;i<6;i++){
                    maxm+= abs(histos[var_number][i].GetMaximum());
                    cout << histos[var_number][i].GetMaximum() <<endl;
            }


            histos[var_number][0].SetTitle("SM");
          	histos[var_number][0].SetFillStyle(3001);
            histos[var_number][1].SetTitle("INT1");
          	histos[var_number][1].SetFillStyle(3001);
            histos[var_number][2].SetTitle("BSM2");
            histos[var_number][2].SetFillStyle(3001);
            histos[var_number][3].SetTitle("INT2");
          	histos[var_number][3].SetFillStyle(3001);
            histos[var_number][4].SetTitle("BSM2");
          	histos[var_number][4].SetFillStyle(3001);
            histos[var_number][5].SetTitle("INT12");
          	histos[var_number][5].SetFillStyle(3001);

            histos[var_number][0].SetFillColor(kBlack);
            histos[var_number][1].SetFillColor(kRed);
            histos[var_number][2].SetFillColor(kBlue);
            histos[var_number][3].SetFillColor(kGreen);
            histos[var_number][4].SetFillColor(kOrange);
            histos[var_number][5].SetFillColor(kYellow);

            TH1F * histo_sum_c[3];
            double k_my_1[]={0.07,0.04};
            double k_my_2[]={0.1,0.06};
            histo_sum_c[0]= new TH1F(histos[var_number][0] + k_my_1[0]*histos[var_number][1] + k_my_1[0]*k_my_1[0]*histos[var_number][2]);
            histo_sum_c[1]= new TH1F(histos[var_number][0] + k_my_2[0]*histos[var_number][3] + k_my_2[0]*k_my_2[0]*histos[var_number][4]);
            histo_sum_c[2]= new TH1F(histos[var_number][0] + k_my_1[1]*histos[var_number][1] + k_my_1[1]*k_my_1[1]*histos[var_number][2] +k_my_2[1]*histos[var_number][3] + k_my_2[1]*k_my_2[1]*histos[var_number][4] + k_my_1[1]*k_my_2[1]*histos[var_number][5]);


            p1->cd();
            // Upper plot will be in pad1
           TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1.0);
           pad1->SetBottomMargin(0); // Upper and lower plot are joined
           pad1->SetGrid();         // Vertical grid
           pad1->Draw();             // Draw the upper pad: pad1
           pad1->cd();               // pad1 becomes the current pad


         	stringstream ss_title;
         	ss_title << setprecision(2)<<wc;
            string a= ss_title.str();
            replace(a, ".", "p");

         	string title = variables[var_number] + " (cqq3_cqq31)";
            histos[var_number][0].SetTitle(title.c_str());
         	string xlabel = string(variables[var_number]);
            xlabel += string(" (Gev)");
         	string ylabel = "N events";

            //histos[var_number][0].SetMaximum(1.05*maxm);
            histos[var_number][0].SetStats(0);
            histos[var_number][0].Draw("hist");


            TString nameLabel = Form ("L = %.1f fb^{-1}   (13 TeV)", luminosity);
            auto tex3 = new TLatex(0.62,0.905,nameLabel.Data());
            tex3->SetNDC();
            tex3->SetTextFont(52);
            tex3->SetTextSize(0.035);
            tex3->SetLineWidth(2);
            tex3->Draw("same");

         	if (variables[var_number]=="deltaetajj" ) xlabel = string("#eta units");
            else if (variables[var_number]=="deltaphijj") xlabel = string("#phi (rad)");
         	//else if (var_number_plot == 9 || var_number_plot == 10 || var_number_plot == 12) ylabel += string("#phi unit");

         	histos[var_number][0].GetXaxis()->SetTitle(xlabel.c_str());
         	histos[var_number][0].GetXaxis()->SetTitleSize(.05);
         	histos[var_number][0].GetXaxis()->SetTitleOffset(.9);
         	histos[var_number][0].GetYaxis()->SetTitle(ylabel.c_str());
         	histos[var_number][0].GetYaxis()->SetTitleSize(.05);
         	histos[var_number][0].GetYaxis()->SetTitleOffset(.9);

            TLegend * legend;
            if (variables[var_number] !="deltaphijj") legend = new TLegend(0.30,0.70,0.60,0.90);
            else legend= new TLegend(0.1,0.7,0.48,0.9,"");
            legend->AddEntry(&histos[var_number][0],"SM","f");



            //creating the EFT distribution for different values of Wilson coeff

            int number;
            number=3;

            TH1F *ratio[3];
            for(int i=0;i<number;i++)  ratio[i]=(TH1F*)histos[var_number][0].Clone(("h"+to_string(i+3)).c_str());

            /*histo_sum_c[0]=new TH1F(histos[var_number][0] + histos[var_number][1] );
            legend->AddEntry(histo_sum_c[i],("EFT = SM + BSM + INT c= "+ss_title.str()).c_str(),"l");
            histo_sum_c[0]->SetLineColor(kWhite+1+0);
            histo_sum_c[0]->SetFillColor(kWhite);
            histo_sum_c[0]->SetLineStyle(10);
            histo_sum_c[0]->SetLineWidth(2.);
            histo_sum_c[0]->Draw("hist same");*/


            //histo_sum_c[0]= new TH1F(histos[var_number][0] + histos[var_number][1]);

            legend->AddEntry(histo_sum_c[0],"SM + INT1  + QUAD1","l");
          	histo_sum_c[0]->SetLineColor(kWhite+1+0);
          	histo_sum_c[0]->SetFillColor(kWhite);
            histo_sum_c[0]->SetLineStyle(10);
          	histo_sum_c[0]->SetLineWidth(2.);
            histo_sum_c[0]->Draw("hist same");

            //histo_sum_c[1]= new TH1F(histos[var_number][0] + histos[var_number][1] + histos[var_number][2]);

            legend->AddEntry(histo_sum_c[1],"SM + INT2 + QUAD2","l");
          	histo_sum_c[1]->SetLineColor(kWhite+1+1);
          	histo_sum_c[1]->SetFillColor(kWhite);
            histo_sum_c[1]->SetLineStyle(10);
          	histo_sum_c[1]->SetLineWidth(2.);
            histo_sum_c[1]->Draw("hist same");

            //histo_sum_c[2]= new TH1F(histos[var_number][0] + histos[var_number][1] + histos[var_number][2] + histos[var_number][3] + histos[var_number][4] + histos[var_number][5]);

            legend->AddEntry(histo_sum_c[2],"SM + INT1 + QUAD1 + INT2 + QUAD2 + INT","l");
          	histo_sum_c[2]->SetLineColor(kWhite+1+2);
          	histo_sum_c[2]->SetFillColor(kWhite);
            histo_sum_c[2]->SetLineStyle(10);
          	histo_sum_c[2]->SetLineWidth(2.);
            histo_sum_c[2]->Draw("hist same");

            /*
            int coeff[]={1,0,0,0,0};
            string single_titles[]={"SM","INT1","QUAD1","INT2","QUAD2","INT12"};

            for(int i=0;i<number;i++){

                histo_sum_c[i]= new TH1F(histos[var_number][0] + coeff[0]*histos[var_number][1] + coeff[1]*histos[var_number][2]+ coeff[2]*histos[var_number][3]+ coeff[3]*histos[var_number][4]+ coeff[4]*histos[var_number][5]);
                title = single_titles[0];
                for(int i=0;i<number;i++){
                    if(coeff[i]!=0){
                        title+=" + "+single_titles[i+1];
                    }
                }
                legend->AddEntry(histo_sum_c[i],title.c_str(),"l");
              	histo_sum_c[i]->SetLineColor(kWhite+1+i);
              	histo_sum_c[i]->SetFillColor(kWhite);
                histo_sum_c[i]->SetLineStyle(10);
              	histo_sum_c[i]->SetLineWidth(2.);
                histo_sum_c[i]->Draw("hist same");
                coeff[i+1]=1;
            }
            */

            legend->Draw("same");

            /*
          	h_stack->Add(&histos[var_number][0]);
          	h_stack->Add(&histos[var_number][1]);
          	h_stack->Add(&histos[var_number][2]);
            h_stack->Add(&histos[var_number][3]);
            h_stack->Add(&histos[var_number][4]);
            h_stack->Add(&histos[var_number][5]);

          	TH1F* histo_sum = new TH1F(histos[var_number][0] + histos[var_number][1] + histos[var_number][2] + histos[var_number][3]+ histos[var_number][4]+ histos[var_number][5]);
          	histo_sum->SetTitle("SM + BSM_1 + INT_1 + BSM_2 + INT_2 + INT_12");
          	histo_sum->SetLineColor(kRed);
          	histo_sum->SetFillColor(kWhite);
          	histo_sum->SetLineWidth(2.);

            //p->cd(1);
            p1->cd();
            // Upper plot will be in pad1
           TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1.0);
           pad1->SetBottomMargin(0); // Upper and lower plot are joined
           pad1->SetGrid();         // Vertical grid
           pad1->Draw();             // Draw the upper pad: pad1
           pad1->cd();               // pad1 becomes the current pad



         	stringstream ss_title;
         	ss_title << wilson_coeff;
         	string title = variables[var_number] + string(" (OPERATOR = ") + ss_title.str() + string(")");
         	h_stack->SetTitle(title.c_str());
         	string xlabel = string(variables[var_number]);
            xlabel += string(" (Gev)");
         	string ylabel = "N events";


            h_stack->Draw("HIST");
         	histo_sum->Draw("same hist");

            TString nameLabel = Form ("L = %.1f fb^{-1}   (13 TeV)", luminosity);
            auto tex3 = new TLatex(0.62,0.905,nameLabel.Data());
            tex3->SetNDC();
            tex3->SetTextFont(52);
            tex3->SetTextSize(0.035);
            tex3->SetLineWidth(2);
            tex3->Draw("same");

         	if (variables[var_number]=="deltaetajj" ) xlabel = string("#eta units");
            else if (variables[var_number]=="deltaphijj") xlabel = string("#phi (rad)");
         	//else if (var_number_plot == 9 || var_number_plot == 10 || var_number_plot == 12) ylabel += string("#phi unit");

         	h_stack->GetXaxis()->SetTitle(xlabel.c_str());
         	h_stack->GetXaxis()->SetTitleSize(.05);
         	h_stack->GetXaxis()->SetTitleOffset(.9);
         	h_stack->GetYaxis()->SetTitle(ylabel.c_str());
         	h_stack->GetYaxis()->SetTitleSize(.05);
         	h_stack->GetYaxis()->SetTitleOffset(.9);

            TLegend * legend;
            if (variables[var_number] !="deltaphijj") legend = new TLegend(0.60,0.70,0.90,0.90);
            else legend= new TLegend(0.1,0.7,0.48,0.9,"");
            legend->AddEntry(&histos[var_number][0],"SM","f");
            legend->AddEntry(&histos[var_number][1],"INT1","f");
            legend->AddEntry(&histos[var_number][2],"BSM1","f");
            legend->AddEntry(&histos[var_number][3],"INT2","f");
            legend->AddEntry(&histos[var_number][4],"BSM2","f");
            legend->AddEntry(&histos[var_number][5],"INT12","f");
            legend->AddEntry(histo_sum,"EFT = SM + INT1 + BSM1 + INT2 + BSM2 + INT12","l");
            legend->Draw("same");
            //if (variables[var_number] !="deltaphijj") gPad->BuildLegend(0.60,0.70,0.90,0.90,"");
            //else gPad->BuildLegend(0.1,0.7,0.48,0.9,"");
         	/* if (var_number_plot < 7 || var_number_plot == 11) gPad->BuildLegend(0.60,0.70,0.90,0.90,"");
         	else if (var_number_plot != 12) gPad->BuildLegend(0.55,0.14,0.85,0.34,"");
         	else gPad->BuildLegend(0.15,0.44,0.45,0.64,"");*/




           // Do not draw the Y axis label on the upper plot and redraw a small
           // axis instead, in order to avoid the first label (0) to be clipped.
           /*h1->GetYaxis()->SetLabelSize(0.);
           TGaxis *axis = new TGaxis( -5, 20, -5, 220, 20,220,510,"");
           axis->SetLabelFont(43); // Absolute font size in pixel (precision 3)
           axis->SetLabelSize(15);
           axis->Draw();

           // lower plot will be in pad

           c->cd();          // Go back to the main canvas before defining pad2*/
           //p->cd(1);
           p1->cd();
           TPad *pad2 = new TPad("pad2", "pad2", 0, 0.05, 1, 0.3);
           pad2->SetTopMargin(0);
           pad2->SetBottomMargin(0.2);
           pad2->SetGridx(); // vertical grid
           pad2->Draw();
           pad2->cd();       // pad2 becomes the current pad


           /*
           // Define the ratio plot
           TH1F *h3 = (TH1F*)histos[var_number][0].Clone("h3");
           h3->SetLineColor(kBlack);
           h3->SetMinimum(0);  // Define Y ..
           h3->SetMaximum(2); // .. range

           h3->Sumw2();
           h3->SetStats(0);      // No statistics on lower plot
           h3->Divide(histo_sum);
           h3->SetMarkerStyle(21);
           h3->GetYaxis()->SetTitle("SM/EFT");
           h3->Draw("ep");       // Draw the ratio plot

           TLine *line1 = new TLine( h3->GetXaxis()->GetXmin(),1,h3->GetXaxis()->GetXmax(),1);
           line1->SetLineWidth(2);
           line1->SetLineStyle(2);
           line1->SetLineColor(kRed);
           line1->Draw();
           // h1 settings
           h1->SetLineColor(kBlue+1);
           h1->SetLineWidth(2);

           // Y axis h1 plot settings
           h1->GetYaxis()->SetTitleSize(20);
           h1->GetYaxis()->SetTitleFont(43);
           h1->GetYaxis()->SetTitleOffset(1.55);

           // h2 settings
           h2->SetLineColor(kRed);
           h2->SetLineWidth(2);

           // Ratio plot (h3) settings
           h3->SetTitle(""); // Remove the ratio title

           // Y axis ratio plot settings
           //h3->GetYaxis()->SetTitle("ratio SM/QUAD ");
           h3->GetYaxis()->SetNdivisions(505);
           h3->GetYaxis()->SetTitleSize(20);
           h3->GetYaxis()->SetTitleFont(43);
           h3->GetYaxis()->SetTitleOffset(0.9);
           h3->GetYaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
           h3->GetYaxis()->SetLabelSize(15);

           //h3->GetYaxis()->SetTitleOffset(1.55);

           // X axis ratio plot settings
           h3->GetXaxis()->SetTitle(xlabel.c_str());
           //h3->GetXaxis()->SetTitleSize(.05);
           h3->GetXaxis()->SetTitleSize(20);
           h3->GetXaxis()->SetTitleFont(43);
           h3->GetXaxis()->SetTitleOffset(3);
           h3->GetXaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
           h3->GetXaxis()->SetLabelSize(15);
           */
           for(int i=0;i<number;i++){
               ratio[i]->SetLineColor(kWhite+1+i);
               ratio[i]->SetMarkerColor(kWhite+1+i);
               ratio[i]->SetMinimum(0);  // Define Y ..
               ratio[i]->SetMaximum(2); // .. range

               ratio[i]->Sumw2();
               ratio[i]->SetStats(0);      // No statistics on lower plot
               ratio[i]->Divide(histo_sum_c[i]);
               ratio[i]->SetMarkerStyle(21);
               ratio[i]->Draw("ep same");       // Draw the ratio plot

               // Ratio plot (h3) settings
               ratio[i]->SetTitle(""); // Remove the ratio title

               // Y axis ratio plot settings
               ratio[i]->GetYaxis()->SetTitle("SM/EFT");
               ratio[i]->GetYaxis()->SetNdivisions(505);
               ratio[i]->GetYaxis()->SetTitleSize(20);
               ratio[i]->GetYaxis()->SetTitleFont(43);
               ratio[i]->GetYaxis()->SetTitleOffset(0.9);
               ratio[i]->GetYaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
               ratio[i]->GetYaxis()->SetLabelSize(15);

               //ratio[i]->GetYaxis()->SetTitleOffset(1.55);

               // X axis ratio plot settings
               ratio[i]->GetXaxis()->SetTitle(xlabel.c_str());
               //ratio[i]->GetXaxis()->SetTitleSize(.05);
               ratio[i]->GetXaxis()->SetTitleSize(20);
               ratio[i]->GetXaxis()->SetTitleFont(43);
               ratio[i]->GetXaxis()->SetTitleOffset(3);
               ratio[i]->GetXaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
               ratio[i]->GetXaxis()->SetLabelSize(15);

           }


           //h3->GetXaxis()->SetTitleOffset(.9);
           string plot_name;
           if(g_syntax=="true"){
               plot_name = variables[var_number]+"_"+input_files[0];
           }
           else{
               plot_name = variables[var_number];
           }
           //p->Print(plot_name.c_str(), "png");
           p1->Print(("dist_"+plot_name+"_qq3_qq31_twoOp.png").c_str(), "png");
           p1->SaveAs(("dist_"+plot_name+"_qq3_qq31_twoOp.root").c_str(), "root");

          	//p->cd(2);
            /*p2->cd();
          	//LOGARITHMIC PLOT
          	gPad->SetLogy();
          	h_stack->Draw("HIST");
          	histo_sum->Draw("same hist");
          	string title_log = title + " (log scale)";
          	h_stack->SetTitle(title_log.c_str());
          	if (variables[var_number] !="deltaphijj") gPad->BuildLegend(0.60,0.70,0.90,0.90,"");
            else gPad->BuildLegend(0.1,0.7,0.48,0.9,"");
            /*else if (var_number_plot != 12) gPad->BuildLegend(0.55,0.14,0.85,0.34,"");



            p2->Print((plot_name+"_log.png").c_str(), "png");

            //p->cd(3);
            p3->cd();
            Double_t norm = 1;
            Double_t scale = norm/(histos[var_number][0].Integral());
            Double_t scale1 = norm/(histos[var_number][1].Integral());
            Double_t scale2 = norm/(histos[var_number][2].Integral());
            Double_t scale3 = norm/(histos[var_number][3].Integral());
            Double_t scale4 = norm/(histos[var_number][4].Integral());
            Double_t scale5 = norm/(histos[var_number][5].Integral());

            histos[var_number][5].Scale(scale5); // quadratic scaling relation
            histos[var_number][4].Scale(scale4); // quadratic scaling relation
            histos[var_number][3].Scale(scale3);	 // linear scaling relation
            histos[var_number][2].Scale(scale2); // quadratic scaling relation
            histos[var_number][1].Scale(scale1);	 // linear scaling relation
            histos[var_number][0].Scale(scale);	 // linear scaling relation

            histos[var_number][0].SetFillStyle(3690);
            histos[var_number][1].SetFillStyle(3690);
            histos[var_number][2].SetFillStyle(3690);
            histos[var_number][3].SetFillStyle(3690);
            histos[var_number][4].SetFillStyle(3690);
            histos[var_number][5].SetFillStyle(3690);

            histos[var_number][0].SetFillColor(0);
            histos[var_number][2].SetFillColor(0);
            histos[var_number][1].SetFillColor(0);
            histos[var_number][3].SetFillColor(0);
            histos[var_number][4].SetFillColor(0);
            histos[var_number][5].SetFillColor(0);

            histos[var_number][0].SetLineColor(kBlack);
            histos[var_number][1].SetLineColor(kRed);
            histos[var_number][2].SetLineColor(kBlue);
            histos[var_number][3].SetLineColor(kGreen);
            histos[var_number][4].SetLineColor(kOrange);
            histos[var_number][5].SetLineColor(kYellow);


            histos[var_number][0].SetLineWidth(2.);
            histos[var_number][1].SetLineWidth(2.);
            histos[var_number][2].SetLineWidth(2.);
            histos[var_number][3].SetLineWidth(2.);
            histos[var_number][4].SetLineWidth(2.);
            histos[var_number][5].SetLineWidth(2.);





            histos[var_number][0].Draw("HIST");
            auto m1= histos[var_number][0].GetMaximum();
            auto m2= histos[var_number][1].GetMaximum();
            auto m3= histos[var_number][2].GetMaximum();
            auto m4= histos[var_number][3].GetMaximum();
            auto m5= histos[var_number][4].GetMaximum();
            auto m6= histos[var_number][5].GetMaximum();

            histos[var_number][0].GetYaxis()->SetRangeUser(0.,max({m1,m2,m3,m4,m5,m6})+0.05);
            histos[var_number][1].Draw("hist same");
            histos[var_number][2].Draw("hist same");
            histos[var_number][3].Draw("hist same");
            histos[var_number][4].Draw("hist same");
            histos[var_number][5].Draw("hist same");
            histos[var_number][0].SetTitle(("Shapes "+variables[var_number]).c_str());
            histos[var_number][0].SetStats(0);
            histos[var_number][0].GetXaxis()->SetTitle(xlabel.c_str());

            gPad->BuildLegend(0.60,0.70,0.90,0.90,"");



          	//p->Modified();
          	//p->Update();

          	//To save the plots

            p3->Print((plot_name+"_shape.png").c_str(), "png");
*/
        }


      	//myapp->Run();
    }

  return 0 ;
}
