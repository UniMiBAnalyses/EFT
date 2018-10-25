// c++  -g -o Discrepanza     Discrepanza.cpp `root-config --libs --glibs --cflags`
//./Discrepanza
#include "LHEF.h"
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <algorithm>
#include <vector>

#include "TStyle.h"
#include "TLorentzVector.h"
#include "TH1F.h"
#include "TLegend.h"
#include "TApplication.h"
#include "TF1.h"
#include "TGraph.h"
#include "TFile.h"
#include "TLatex.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TROOT.h"
#include "TNtuple.h"


class LHEreader
{
  public:
    LHEreader (std::string inputFileFile, bool makeClasses = true) ;
    bool readEvent () ;
    ~LHEreader () {} ;

    std::vector<std::pair<int, TLorentzVector> > m_particles ;
    std::vector<std::pair<int, TLorentzVector&> > m_el ; // electrons
    std::vector<std::pair<int, TLorentzVector&> > m_mu ; // muons
    std::vector<std::pair<int, TLorentzVector&> > m_ta ; // taus
    std::vector<std::pair<int, TLorentzVector&> > m_le ; // leptons
    std::vector<std::pair<int, TLorentzVector&> > m_ve ; // electron neutrinos
    std::vector<std::pair<int, TLorentzVector&> > m_vm ; // muon neutrinos
    std::vector<std::pair<int, TLorentzVector&> > m_vT ; // tau neutrinos
    std::vector<std::pair<int, TLorentzVector&> > m_vs ; // all neutrinos
    std::vector<std::pair<int, TLorentzVector&> > m_qu ; // quarks
    std::vector<std::pair<int, TLorentzVector&> > m_gl ; // gluons
    std::vector<std::pair<int, TLorentzVector&> > m_je ; // partons (q+g)

  private:
    std::ifstream m_ifs ;  
    LHEF::Reader  m_reader ;
    bool          m_makeClasses ;

  public:
    int           m_evtNum ;


} ;

LHEreader::LHEreader (std::string inputFileName, bool makeClasses):
  m_ifs (inputFileName.c_str ()),
  m_reader (m_ifs),
  m_evtNum (0),
  m_makeClasses (makeClasses)
{
  std::cout << "[LHEreader] reading file " << inputFileName << "\n" ;
  std::cout << "[LHEreader] preparing sub-vectors " << makeClasses << "\n" ;
}

/** 
read the event with the LHEF class and produces collections of paris (pdgid, TLorentzVector)
*/
bool 
LHEreader::readEvent ()
{
  bool reading = m_reader.readEvent () ;
  if (!reading) return reading ;
  m_particles.clear ();
  if (m_makeClasses) 
    {
      m_el.clear () ;
      m_mu.clear () ;
      m_ta.clear () ;
      m_le.clear () ;
      m_ve.clear () ;
      m_vm.clear () ;
      m_vT.clear () ;
      m_vs.clear () ;
      m_qu.clear () ;
      m_gl.clear () ;
      m_je.clear () ;
    }
  if (m_evtNum % 10000 == 0) std::cerr << "event " << m_evtNum << "\n" ;



  //PG loop over particles in the event
  for (int iPart = 0 ; iPart < m_reader.hepeup.IDUP.size (); ++iPart)
    {
      // std::cerr << "\t part type [" << iPart << "] " << m_reader.hepeup.IDUP.at (iPart)
      //           << "\t status " << m_reader.hepeup.ISTUP.at (iPart)
      //           << "\n" ;

      //PG outgoing particles only
      if (m_reader.hepeup.ISTUP.at (iPart) != 1) continue ; //AT Le particelle uscenti sono contrassegante da ISTUP = 1

      int partType = m_reader.hepeup.IDUP.at (iPart) ;
      TLorentzVector particle 
        (
          m_reader.hepeup.PUP.at (iPart).at (0), //PG px
          m_reader.hepeup.PUP.at (iPart).at (1), //PG py
          m_reader.hepeup.PUP.at (iPart).at (2), //PG pz
          m_reader.hepeup.PUP.at (iPart).at (3) //PG E
        ) ;
      m_particles.push_back (std::pair<int, TLorentzVector> (partType, particle)) ;
      if (m_makeClasses)
        {
          if (abs (partType) < 6) 
            {
              m_qu.push_back (std::pair<int, TLorentzVector&> (partType, m_particles.back ().second)) ;
              m_je.push_back (std::pair<int, TLorentzVector&> (partType, m_particles.back ().second)) ;
            }
          else if (abs (partType) == 11 || abs (partType) == 13 || abs (partType) == 15)
            {
              m_le.push_back (std::pair<int, TLorentzVector&> (partType, m_particles.back ().second)) ;
              switch(abs (partType)) 
                {
                  case 11 :
                    m_el.push_back (std::pair<int, TLorentzVector&> (partType, m_particles.back ().second)) ;
                    break ;
                  case 13 : 
                    m_mu.push_back (std::pair<int, TLorentzVector&> (partType, m_particles.back ().second)) ;
                    break ;
                  case 15 : 
                    m_ta.push_back (std::pair<int, TLorentzVector&> (partType, m_particles.back ().second)) ;
                    break ;
                }
            }
          else if (abs (partType) == 12 || abs (partType) == 14 || abs (partType) == 16)
            {
              m_vs.push_back (std::pair<int, TLorentzVector&> (partType, m_particles.back ().second)) ;
              switch(abs (partType)) 
                {
                  case 12 :
                    m_ve.push_back (std::pair<int, TLorentzVector&> (partType, m_particles.back ().second)) ;
                    break ;
                  case 14 : 
                    m_vm.push_back (std::pair<int, TLorentzVector&> (partType, m_particles.back ().second)) ;
                    break ;
                  case 16 : 
                    m_vT.push_back (std::pair<int, TLorentzVector&> (partType, m_particles.back ().second)) ;
                    break ;
                }
            }
          else if (abs (partType) == 21 || abs (partType) == 9)
            {
              m_gl.push_back (std::pair<int, TLorentzVector&> (partType, m_particles.back ().second)) ;
              m_je.push_back (std::pair<int, TLorentzVector&> (partType, m_particles.back ().second)) ;
            }
        } // if (makeClasses)
    } //PG loop over particles in the event
  ++m_evtNum ;
  return true ;
}















using namespace std ;

int main (int argc, char ** argv){
  int maxNum = -1;
  double a[19], b[19], c[19], d[19], e[19], f[19], g[19], h[19], l[19], m[19], n[19], o[19], p[19], q[19], r[19], s[19], t[19], u[19], v[19], z[19];
  int x = 0;
  int y = 0;


  //AT Primo file--------------------------------------------------------------------------------------------------------------------------------------------
  TNtuple mll ("mll", "mll", "el:ve:mu:vm:mLep:mQuark:angLep:PsRap:MET:ze:zm:lep");
  TNtuple qll ("qll", "qll", "qll");

  string Name1 = "unweighted_events_SM.lhe";
  LHEreader reader (Name1) ;

  //PG loop over events
  while (reader.readEvent ())
    {
      if (maxNum > 0 && reader.m_evtNum > maxNum) break;
      if (reader.m_el.size () != 1) continue; 
      if (reader.m_ve.size () != 1) continue;
      if (reader.m_mu.size () != 1) continue;
      if (reader.m_vm.size () != 1) continue;
      if (reader.m_qu.size () != 2) continue;
      if (reader.m_le.size () != 2) continue ;
      TLorentzVector dilepton = reader.m_le.at (0).second + reader.m_le.at (1).second;
      TLorentzVector diquark = reader.m_qu.at (0).second + reader.m_qu.at (1).second;
      double DiffPsRap = reader.m_qu.at (0).second.PseudoRapidity() - reader.m_qu.at (1).second.PseudoRapidity(); /*AT Differenza tra le psedudorapidità 
        dei quark*/
      double AddPsRap = reader.m_qu.at (0).second.PseudoRapidity() + reader.m_qu.at (1).second.PseudoRapidity(); /*AT Somma tra le psedudorapidità 
        dei quark*/
      double MET = sqrt(reader.m_ve.at(0).second.Perp()*reader.m_ve.at(0).second.Perp()  +  reader.m_vm.at(0).second.Perp()*reader.m_vm.at(0).second.Perp());
      double z_e = (reader.m_el.at (0).second.PseudoRapidity() - (AddPsRap/2))/abs(DiffPsRap);
      double z_m = (reader.m_mu.at (0).second.PseudoRapidity() - (AddPsRap/2))/abs(DiffPsRap);
      double somma_lep = sqrt(reader.m_el.at(0).second.Perp()*reader.m_el.at(0).second.Perp()  +  reader.m_mu.at(0).second.Perp()*reader.m_mu.at(0).second.Perp());


      if(dilepton.M() > 20 && 
         diquark.M() > 500 && 
         abs(DiffPsRap) > 2.5 && 
         reader.m_qu.at (0).second.Perp() > 30 && 
         reader.m_qu.at (1).second.Perp() > 30 && 
         MET>40 &&
         abs(z_e) < 0.75 &&
         abs(z_m) < 0.75){
            mll.Fill (reader.m_el.at(0).second.Perp(), reader.m_ve.at(0).second.Perp(), reader.m_mu.at(0).second.Perp(), 
              reader.m_vm.at(0).second.Perp(), dilepton.M (), diquark.M (), reader.m_le.at(0).second.Angle(reader.m_le.at(1).second.Vect()), abs(DiffPsRap)
              ,MET,z_e, z_m, somma_lep);
            qll.Fill(reader.m_qu.at(0).second.Perp());
            qll.Fill(reader.m_qu.at(1).second.Perp());
            x++;
      }
    } //PG loop over events

    //AT Secondo file------------------------------------------------------------------------------------------------------------------------------------------
    TNtuple mll_1 ("mll", "mll", "el:ve:mu:vm:mLep:mQuark:angLep:PsRap:MET:ze:zm:lep") ;
    TNtuple qll_1 ("qll", "qll", "qll");

    Name1 = "unweighted_events_cW_0_3.lhe";
    LHEreader reader_1 (Name1) ;


    //PG loop over event
    while (reader_1.readEvent ())
    {
      if (maxNum > 0 && reader_1.m_evtNum > maxNum) break;
      if (reader_1.m_el.size () != 1) continue; 
      if (reader_1.m_ve.size () != 1) continue;
      if (reader_1.m_mu.size () != 1) continue;
      if (reader_1.m_vm.size () != 1) continue;
      if (reader_1.m_qu.size () != 2) continue;
      if (reader_1.m_le.size () != 2) continue ;
      TLorentzVector dilepton = reader_1.m_le.at (0).second + reader_1.m_le.at (1).second;
      TLorentzVector diquark = reader_1.m_qu.at (0).second + reader_1.m_qu.at (1).second;
      double DiffPsRap = reader_1.m_qu.at (0).second.PseudoRapidity() - reader_1.m_qu.at (1).second.PseudoRapidity(); /*AT Differenza tra le psedudorapidità 
        dei quark*/
      double AddPsRap = reader_1.m_qu.at (0).second.PseudoRapidity() + reader_1.m_qu.at (1).second.PseudoRapidity(); /*AT Somma tra le psedudorapidità 
        dei quark*/
      double MET = sqrt(reader_1.m_ve.at(0).second.Perp()*reader_1.m_ve.at(0).second.Perp() + reader_1.m_vm.at(0).second.Perp()
        *reader_1.m_vm.at(0).second.Perp());
      double z_e = (reader_1.m_el.at (0).second.PseudoRapidity() - (AddPsRap/2))/abs(DiffPsRap);
      double z_m = (reader_1.m_mu.at (0).second.PseudoRapidity() - (AddPsRap/2))/abs(DiffPsRap);
      double somma_lep = sqrt(reader_1.m_el.at(0).second.Perp()*reader_1.m_el.at(0).second.Perp()  +  reader_1.m_mu.at(0).second.Perp()*reader_1.m_mu.at(0).second.Perp());

      if(dilepton.M() > 20 && 
         diquark.M() > 500 && 
         abs(DiffPsRap) > 2.5 && 
         reader_1.m_qu.at (0).second.Perp() > 30 && 
         reader_1.m_qu.at (1).second.Perp() > 30 && 
         MET>40 &&
         abs(z_e) < 0.75 &&
         abs(z_m) < 0.75){
            mll_1.Fill (reader_1.m_el.at(0).second.Perp(), reader_1.m_ve.at(0).second.Perp(), reader_1.m_mu.at(0).second.Perp(), 
              reader_1.m_vm.at(0).second.Perp(), dilepton.M (), diquark.M (), reader_1.m_le.at(0).second.Angle(reader_1.m_le.at(1).second.Vect()), 
              abs(DiffPsRap),MET, z_e, z_m, somma_lep);
            qll_1.Fill(reader_1.m_qu.at(0).second.Perp());
            qll_1.Fill(reader_1.m_qu.at(1).second.Perp());
            y++;
          }
    } //PG loop over events

    gStyle->SetOptStat(0);



    TH1F posit_SM_prova ("h1_SM_prova","",100,0,1300);

    TH1F posit_EFT_prova ("h1_EFT_prova", "", 100,0,1300);

    TCanvas c2("c2", "c2");
    c2.SetLogy();
    mll.Draw("lep >> h1_SM_prova", "0.0005958");
    mll_1.Draw("lep >> h1_EFT_prova", "0.0006458");
    posit_SM_prova.GetXaxis()-> SetTitle("P_{T} [leptons] (GeV/c^{2})");
    posit_SM_prova.GetXaxis() -> CenterTitle();
    posit_SM_prova.GetXaxis() -> SetTitleOffset(1.25);
    posit_SM_prova.GetXaxis() -> SetTitleSize(0.03);
    posit_SM_prova.GetXaxis() -> SetTitleFont(52);
    posit_SM_prova.GetYaxis() -> SetTitle("Events (L_{int} = 150 fb^{-1})");
    posit_SM_prova.GetYaxis() -> SetTitleFont(52);
    posit_SM_prova.GetYaxis() -> SetTitleSize(0.03);
    posit_EFT_prova.GetXaxis()-> SetTitle("P_{T} [leptons] (GeV/c^{2})");
    posit_EFT_prova.GetXaxis() -> CenterTitle();
    posit_EFT_prova.GetXaxis() -> SetTitleOffset(1.25);
    posit_EFT_prova.GetXaxis() -> SetTitleSize(0.03);
    posit_EFT_prova.GetXaxis() -> SetTitleFont(52);
    posit_EFT_prova.GetYaxis() -> SetTitle("Events (L_{int} = 150 fb^{-1})");
    posit_EFT_prova.GetYaxis() -> SetTitleFont(52);
    posit_EFT_prova.GetYaxis() -> SetTitleSize(0.03);
    posit_SM_prova.SetLineColor(kBlue);
    posit_EFT_prova.SetLineColor(kRed);
    posit_SM_prova.SetFillColor(kBlue);
    posit_EFT_prova.SetFillColor(kRed);
    posit_EFT_prova.Draw("HIST");
    posit_SM_prova.Draw("HIST, SAME");
    c2.Print("posit_somma.png");

	int min = posit_SM_prova.GetXaxis() -> FindBin(0.);
	cout << "bin_min: " << min << endl;
    int max = posit_SM_prova.GetXaxis() -> FindBin(1300);
	cout << "bin_max: " << max << endl;


    TCanvas c3("c3", "c3");
    double sum[max-min];
    double bin[max-min];
    int k = 0;
    int max_gr = 0;


    for (int i = max - 1; i >= min; i--){
    	double sum_SM = 0;
    	double sum_EFT = 0;
    	double err_SM = 0;
    	double err_EFT = 0;
    	double grande = 0;

    	sum_SM = posit_SM_prova.Integral(i, max);
    	sum_EFT = posit_EFT_prova.Integral(i, max);
    	err_SM = sqrt(sum_SM);
    	err_EFT = sqrt(sum_EFT);
    	if(err_SM > err_EFT) grande = err_SM;
    	if(err_EFT > err_SM) grande = err_EFT;
    	sum[k] = abs(sum_SM - sum_EFT) / grande;
    	cout << sum[k] << "   " << sum[k-1] << endl;
    	if (sum[k] > sum [k-1]) max_gr = k;
    	cout << max_gr << endl;
      cout << "k" << k << endl;
    	cout << endl;
    	bin[k] = posit_SM_prova.GetXaxis() -> GetBinCenter(i);
    	k++;
    }

    TGraph maxx(k, bin ,sum);  
    maxx.SetMarkerStyle(20);
    maxx.SetMarkerSize(0.8);
    maxx.SetMarkerColor(kBlack);
    maxx.SetTitle();
    maxx.GetYaxis() -> SetTitle("#sigma distance");
    maxx.GetYaxis() -> SetTitleSize(0.04);
    maxx.GetXaxis()-> SetTitle("P_{T} [leptons] (GeV/c^{2})");
    maxx.GetXaxis() -> CenterTitle();
    maxx.GetXaxis() -> SetTitleOffset(1.25); 
    maxx.GetXaxis() -> SetRangeUser(0,1300);       
    maxx.Draw("AP");
    c3.Print("massimo.png");  





    for(int j = 1; j <= 20; j++){
      double sigma_SM = 3.968924;
      double sigma_EFT = 4.294595; 
      double lum = 150; 
       
      double w_SM = (sigma_SM * j* lum) / 1000000;
      string ww_SM;
      ostringstream convert_SM;
      convert_SM << w_SM;
      ww_SM = convert_SM.str();
      const char * peso_SM = ww_SM.c_str();

      double w_EFT = (sigma_EFT * j* lum) / 1000000;
      string ww_EFT;
      ostringstream convert_EFT;
      convert_EFT << w_EFT;
      ww_EFT = convert_EFT.str();
      const char * peso_EFT = ww_EFT.c_str();

      TH1F posit_SM ("h1_SM","",100,0,1300);

      TH1F posit_EFT ("h1_EFT", "", 100,0,1300);


      TCanvas c1("c1", "c1");
      mll.Draw("lep >> h1_SM", peso_SM);
      mll_1.Draw("lep >> h1_EFT", peso_EFT);

      //Selezioni

     
      cout << "valore taglio: " << posit_SM.GetBinCenter(max - max_gr) << endl;
      double integrale_SM_selezioni = posit_SM.Integral(max - max_gr,max);
      double integrale_EFT_selezioni = posit_EFT.Integral(max - max_gr,max);

      double Err_SM_selezioni = sqrt(integrale_SM_selezioni); //Errori poisson selezioni
      double Err_EFT_selezioni = sqrt(integrale_EFT_selezioni); //Errori poisson selezioni

      double Sistematica_5_SM_selezioni = 0.05 * integrale_SM_selezioni;
      double Sistematica_5_EFT_selezioni = 0.05 * integrale_EFT_selezioni;
      double Sistematica_75_SM_selezioni = 0.075 * integrale_SM_selezioni;
      double Sistematica_75_EFT_selezioni = 0.075 * integrale_EFT_selezioni;
      double Sistematica_10_SM_selezioni = 0.1 * integrale_SM_selezioni;
      double Sistematica_10_EFT_selezioni = 0.1 * integrale_EFT_selezioni;
      double Sistematica_20_SM_selezioni = 0.2 * integrale_SM_selezioni;
      double Sistematica_20_EFT_selezioni = 0.2 * integrale_EFT_selezioni;

      double Totale_5_SM_selezioni = sqrt((Err_SM_selezioni * Err_SM_selezioni) + (Sistematica_5_SM_selezioni * Sistematica_5_SM_selezioni));
      double Totale_5_EFT_selezioni = sqrt((Err_EFT_selezioni * Err_EFT_selezioni) + (Sistematica_5_EFT_selezioni * Sistematica_5_EFT_selezioni));
      double Totale_75_SM_selezioni = sqrt((Err_SM_selezioni * Err_SM_selezioni) + (Sistematica_75_SM_selezioni * Sistematica_75_SM_selezioni));
      double Totale_75_EFT_selezioni = sqrt((Err_EFT_selezioni * Err_EFT_selezioni) + (Sistematica_75_EFT_selezioni * Sistematica_75_EFT_selezioni));
      double Totale_10_SM_selezioni = sqrt((Err_SM_selezioni * Err_SM_selezioni) + (Sistematica_10_SM_selezioni * Sistematica_10_SM_selezioni));
      double Totale_10_EFT_selezioni = sqrt((Err_EFT_selezioni * Err_EFT_selezioni) + (Sistematica_10_EFT_selezioni * Sistematica_10_EFT_selezioni));
      double Totale_20_SM_selezioni = sqrt((Err_SM_selezioni * Err_SM_selezioni) + (Sistematica_20_SM_selezioni * Sistematica_20_SM_selezioni));
      double Totale_20_EFT_selezioni = sqrt((Err_EFT_selezioni * Err_EFT_selezioni) + (Sistematica_20_EFT_selezioni * Sistematica_20_EFT_selezioni));

      //Tutto

      double integrale_SM = posit_SM.Integral(1,100);
      double integrale_EFT = posit_EFT.Integral(1,100);

      double Err_SM = sqrt(integrale_SM); //Errore poisson
      double Err_EFT = sqrt(integrale_EFT); //Errore poisson


      double Sistematica_5_SM = 0.05 * integrale_SM;
      double Sistematica_5_EFT = 0.05 * integrale_EFT;
      double Sistematica_75_SM = 0.075 * integrale_SM;
      double Sistematica_75_EFT = 0.075 * integrale_EFT;
      double Sistematica_10_SM = 0.1 * integrale_SM;
      double Sistematica_10_EFT = 0.1 * integrale_EFT;
      double Sistematica_20_SM = 0.2 * integrale_SM;
      double Sistematica_20_EFT = 0.2 * integrale_EFT;

      double Totale_5_SM = sqrt((Err_SM * Err_SM) + (Sistematica_5_SM * Sistematica_5_SM));
      double Totale_5_EFT = sqrt((Err_EFT * Err_EFT) + (Sistematica_5_EFT * Sistematica_5_EFT));
      double Totale_75_SM = sqrt((Err_SM * Err_SM) + (Sistematica_75_SM * Sistematica_75_SM));
      double Totale_75_EFT = sqrt((Err_EFT * Err_EFT) + (Sistematica_75_EFT * Sistematica_75_EFT));
      double Totale_10_SM = sqrt((Err_SM * Err_SM) + (Sistematica_10_SM * Sistematica_10_SM));
      double Totale_10_EFT = sqrt((Err_EFT * Err_EFT) + (Sistematica_10_EFT * Sistematica_10_EFT));
      double Totale_20_SM = sqrt((Err_SM * Err_SM) + (Sistematica_20_SM * Sistematica_20_SM));
      double Totale_20_EFT = sqrt((Err_EFT * Err_EFT) + (Sistematica_20_EFT * Sistematica_20_EFT)); 



      double grande_poisson_selezioni;
      double grande_poisson;


      if(Err_SM_selezioni > Err_EFT_selezioni) grande_poisson_selezioni = Err_SM_selezioni;
      if(Err_SM_selezioni < Err_EFT_selezioni) grande_poisson_selezioni = Err_EFT_selezioni;

      if(Err_SM > Err_EFT) grande_poisson = Err_SM;
      if(Err_SM < Err_EFT) grande_poisson = Err_EFT;

      //Grafico errore poisson selezioni
      a[j-1] = abs(integrale_SM_selezioni - integrale_EFT_selezioni) / grande_poisson_selezioni;
      cout << a[j-1] << endl;
      b[j-1] = j * lum; 

      //Grafico errore poisson senza selezione
      c[j-1] = abs(integrale_SM - integrale_EFT) / grande_poisson;
      d[j-1] = j * lum; 



      double grande_totale_5;
      double grande_totale_75;
      double grande_totale_10;
      double grande_totale_20;

      if(Totale_5_SM > Totale_5_EFT) grande_totale_5 = Totale_5_SM;
      if(Totale_5_SM < Totale_5_EFT) grande_totale_5 = Totale_5_EFT;
      if(Totale_75_SM > Totale_75_EFT) grande_totale_75 = Totale_75_SM;
      if(Totale_75_SM < Totale_75_EFT) grande_totale_75 = Totale_75_EFT;
      if(Totale_10_SM > Totale_10_EFT) grande_totale_10 = Totale_10_SM;
      if(Totale_10_SM < Totale_10_EFT) grande_totale_10 = Totale_10_EFT;
      if(Totale_20_SM > Totale_20_EFT) grande_totale_20 = Totale_20_SM;
      if(Totale_20_SM < Totale_20_EFT) grande_totale_20 = Totale_20_EFT;

      //Grafico errore con sistematica 5 
      e[j-1] = abs(integrale_SM - integrale_EFT) / grande_totale_5;
      f[j-1] = j * lum; 


      //Grafico errore con sistematica 7.5 
      g[j-1] = abs(integrale_SM - integrale_EFT) / grande_totale_75;
      h[j-1] = j * lum;
 

      //Grafico errore con sistematica 10
      l[j-1] = abs(integrale_SM - integrale_EFT) / grande_totale_10;
      m[j-1] = j * lum; 


      //Grafico errore con sistematica 20
      n[j-1] = abs(integrale_SM - integrale_EFT) / grande_totale_20;
      o[j-1] = j * lum; 



      //Selezioni

      double grande_totale_5_selezioni;
      double grande_totale_75_selezioni;
      double grande_totale_10_selezioni;
      double grande_totale_20_selezioni;

      if(Totale_5_SM_selezioni > Totale_5_EFT_selezioni) grande_totale_5_selezioni = Totale_5_SM_selezioni;
      if(Totale_5_SM_selezioni < Totale_5_EFT_selezioni) grande_totale_5_selezioni = Totale_5_EFT_selezioni;
      if(Totale_75_SM_selezioni > Totale_75_EFT_selezioni) grande_totale_75_selezioni = Totale_75_SM_selezioni;
      if(Totale_75_SM_selezioni < Totale_75_EFT_selezioni) grande_totale_75_selezioni = Totale_75_EFT_selezioni;
      if(Totale_10_SM_selezioni > Totale_10_EFT_selezioni) grande_totale_10_selezioni = Totale_10_SM_selezioni;
      if(Totale_10_SM_selezioni < Totale_10_EFT_selezioni) grande_totale_10_selezioni = Totale_10_EFT_selezioni;
      if(Totale_20_SM_selezioni > Totale_20_EFT_selezioni) grande_totale_20_selezioni = Totale_20_SM_selezioni;
      if(Totale_20_SM_selezioni < Totale_20_EFT_selezioni) grande_totale_20_selezioni = Totale_20_EFT_selezioni;

      //Grafico errore con sistematica 5 e selezioni
      p[j-1] = abs(integrale_SM_selezioni - integrale_EFT_selezioni) / grande_totale_5_selezioni;
      q[j-1] = j * lum; 


      //Grafico errore con sistematica 7.5 e selezioni
      r[j-1] = abs(integrale_SM_selezioni - integrale_EFT_selezioni) / grande_totale_75_selezioni;
      s[j-1] = j * lum; 

      //Grafico errore con sistematica 10 e selezioni
      t[j-1] = abs(integrale_SM_selezioni - integrale_EFT_selezioni) / grande_totale_10_selezioni;
      u[j-1] = j * lum; 

      //Grafico errore con sistematica 20 e selezioni
      v[j-1] = abs(integrale_SM_selezioni - integrale_EFT_selezioni) / grande_totale_20_selezioni;
      z[j-1] = j * lum; 
    }


    TCanvas canv1 ("canv1", "canv1");
    TGraph gr_poisson(20, d, c);
    TGraph gr_poisson_selezioni (20, b, a);

    gr_poisson_selezioni.SetTitle("");
    gr_poisson_selezioni.GetXaxis()-> SetTitle("Integrated luminosity (/fb)");
    gr_poisson_selezioni.GetYaxis()-> SetTitle("#sigma distance");
    gr_poisson_selezioni.GetYaxis() -> SetRangeUser(0,12);

    gr_poisson.SetMarkerStyle(20);
    gr_poisson.SetMarkerSize(0.5);
    gr_poisson.SetMarkerColor(kRed);
    gr_poisson_selezioni.SetMarkerStyle(20);
    gr_poisson_selezioni.SetMarkerSize(0.5);
    gr_poisson_selezioni.SetMarkerColor(kBlue);

    gr_poisson_selezioni.Draw("AP");
    gr_poisson.Draw("P");

    TLegend leg1 (0.15,0.7,0.40,0.85);
    leg1.AddEntry(&gr_poisson, "Total", "p"); 
    leg1.AddEntry(&gr_poisson_selezioni, "Selections", "p");
    leg1.SetMargin(0.2);
    leg1.Draw();

    canv1.Print("Poisson.png");


    TCanvas canv2 ("canv2", "canv2");
    TGraph gr_5(20, f, e);
    TGraph gr_5_selezioni (20, q, p);
    TGraph gr_75 (20, h, g);
    TGraph gr_75_selezioni (20, s, r);
    TGraph gr_10 (20, m, l);
    TGraph gr_10_selezioni (20, u, t);

    gr_5_selezioni.SetTitle("");
    gr_5_selezioni.GetXaxis()-> SetTitle("Integrated luminosity (/fb)");
    gr_5_selezioni.GetYaxis()-> SetTitle("#sigma distance");
    gr_5_selezioni.GetYaxis()-> SetRangeUser(0.,10.);

    gr_5_selezioni.SetMarkerStyle(20);
    gr_5_selezioni.SetMarkerSize(0.8);
    gr_5_selezioni.SetMarkerColor(kBlue);
    gr_5.SetMarkerStyle(22);
    gr_5.SetMarkerSize(1);
    gr_5.SetMarkerColor(kBlue);
    gr_75_selezioni.SetMarkerStyle(20);
    gr_75_selezioni.SetMarkerSize(0.8);
    gr_75_selezioni.SetMarkerColor(kRed);
    gr_75.SetMarkerStyle(22);
    gr_75.SetMarkerSize(1);
    gr_75.SetMarkerColor(kRed);
    gr_10.SetMarkerStyle(22);
    gr_10.SetMarkerSize(1);
    gr_10.SetMarkerColor(kBlack);
    gr_10_selezioni.SetMarkerStyle(20);
    gr_10_selezioni.SetMarkerSize(0.8);
    gr_10_selezioni.SetMarkerColor(kBlack);


    gr_5_selezioni.Draw("AP");
    gr_5.Draw("P");
    gr_75_selezioni.Draw("P");
    gr_75.Draw("P");
    gr_10.Draw("P");
    gr_10_selezioni.Draw("P");

    TLegend leg2 (0.15,0.65,0.40,0.88);
    leg2.AddEntry(&gr_5, "Total: 5%", "p"); 
    leg2.AddEntry(&gr_5_selezioni, "Selections: 5%", "p");
    leg2.AddEntry(&gr_75, "Total: 7,5%", "p");
    leg2.AddEntry(&gr_75_selezioni, "Selections: 7,5%", "p");
    leg2.AddEntry(&gr_10, "Total: 10%", "p");
    leg2.AddEntry(&gr_10_selezioni, "Selections: 10%", "p");
    leg2.SetMargin(0.2);
    leg2.Draw();

    canv2.Print("Sistematica_5_75_10.png");

    return 0;

}