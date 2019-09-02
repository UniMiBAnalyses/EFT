// c++  -g -o Terzo Terzo.cpp `root-config --libs --glibs --cflags`
//./Terzo      unweighted_events.lhe      unweighted_events_cqq3.lhe         unweighted_events_cW.lhe
#include "LHEF.h"
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>

#include "TStyle.h"
#include "TLorentzVector.h"
#include "TLegend.h"
#include "TH1F.h"
#include "TApplication.h"
#include "TF1.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TMath.h"
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


//PG ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


using namespace std ;

int main (int argc, char ** argv)
{
  LHEreader reader (argv[1]) ;
  int maxNum = -1 ;
  string outFileName = "output_SM" ;
  string outFileName_1 = "output_cqq3";
  string outFileName_2 = "output_cW";

  //AT Primo file--------------------------------------------------------------------------------------------------------------------------------------------
  TNtuple mll ("mll", "mll", "el:ve:mu:vm:mLep:mQuark:angLep:PsRap:MET:ze:zm") ; /*AT momento trasverso (4), massa inviariante leptoni, 
    massa invariante quark, angolo tra i leptoni, differenza della pseudorapidità dei quark*/
  TNtuple qll ("qll", "qll", "qll"); //AT momento trasverso quarks

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

      if(dilepton.M() > 20 && 
         diquark.M() > 500 && 
         abs(DiffPsRap) > 2.5 && 
         reader.m_qu.at (0).second.Perp() > 30 && 
         reader.m_qu.at (1).second.Perp() > 30 && 
         MET >40 &&
         abs(z_e) < 0.75 &&
         abs(z_m) < 0.75){
            mll.Fill (reader.m_el.at(0).second.Perp(), reader.m_ve.at(0).second.Perp(), reader.m_mu.at(0).second.Perp(), 
              reader.m_vm.at(0).second.Perp(), dilepton.M (), diquark.M (), reader.m_le.at(0).second.Angle(reader.m_le.at(1).second.Vect()), abs(DiffPsRap), MET
              , z_e, z_m);
            qll.Fill(reader.m_qu.at(0).second.Perp());
            qll.Fill(reader.m_qu.at(1).second.Perp());
          }
    } //PG loop over events

 






  LHEreader reader_1 (argv[2]) ;

  //AT Secondo file------------------------------------------------------------------------------------------------------------------------------------------
  TNtuple mll_1 ("mll", "mll", "el:ve:mu:vm:mLep:mQuark:angLep:PsRap:MET:ze:zm") ; /*AT momento trasverso (4), massa inviariante leptoni, 
    massa invariante quark, angolo tra i leptoni, differenza della pseudorapidità dei quark, energia mancante*/
  TNtuple qll_1 ("qll", "qll", "qll"); //AT momento trasverso quarks

  //PG loop over events
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

      if(dilepton.M() > 20 && 
         diquark.M() > 500 && 
         abs(DiffPsRap) > 2.5 && 
         reader_1.m_qu.at (0).second.Perp() > 30 && 
         reader_1.m_qu.at (1).second.Perp() > 30 && 
         MET >40 &&
         abs(z_e) < 0.75 &&
         abs(z_m) < 0.75){
              mll_1.Fill (reader_1.m_el.at(0).second.Perp(), reader_1.m_ve.at(0).second.Perp(), reader_1.m_mu.at(0).second.Perp(), 
                reader_1.m_vm.at(0).second.Perp(), dilepton.M (), diquark.M (), reader_1.m_le.at(0).second.Angle(reader_1.m_le.at(1).second.Vect()),
                abs(DiffPsRap), MET, z_e, z_m);
              qll_1.Fill(reader_1.m_qu.at(0).second.Perp());
              qll_1.Fill(reader_1.m_qu.at(1).second.Perp());
            }
    } //PG loop over events







    LHEreader reader_2 (argv[3]) ;

  //AT Terzo file------------------------------------------------------------------------------------------------------------------------------------------
  TNtuple mll_2 ("mll", "mll", "el:ve:mu:vm:mLep:mQuark:angLep:PsRap:MET:ze:zm") ; /*AT momento trasverso (4), massa inviariante leptoni, 
    massa invariante quark, angolo tra i leptoni, differenza della pseudorapidità dei quark, energia mancante*/
  TNtuple qll_2 ("qll", "qll", "qll"); //AT momento trasverso quarks

  //PG loop over events
  while (reader_2.readEvent ())
    {
      if (maxNum > 0 && reader_2.m_evtNum > maxNum) break;
      if (reader_2.m_el.size () != 1) continue; 
      if (reader_2.m_ve.size () != 1) continue;
      if (reader_2.m_mu.size () != 1) continue;
      if (reader_2.m_vm.size () != 1) continue;
      if (reader_2.m_qu.size () != 2) continue;
      if (reader_2.m_le.size () != 2) continue ;
      TLorentzVector dilepton = reader_2.m_le.at (0).second + reader_2.m_le.at (1).second;
      TLorentzVector diquark = reader_2.m_qu.at (0).second + reader_2.m_qu.at (1).second;
      double DiffPsRap = reader_2.m_qu.at (0).second.PseudoRapidity() - reader_2.m_qu.at (1).second.PseudoRapidity(); /*AT Differenza tra le psedudorapidità 
        dei quark*/
      double AddPsRap = reader_2.m_qu.at (0).second.PseudoRapidity() + reader_2.m_qu.at (1).second.PseudoRapidity(); /*AT Somma tra le psedudorapidità 
        dei quark*/
      double MET = sqrt(reader_2.m_ve.at(0).second.Perp()*reader_2.m_ve.at(0).second.Perp() + reader_2.m_vm.at(0).second.Perp()
        *reader_1.m_vm.at(0).second.Perp());
      double z_e = (reader_2.m_el.at (0).second.PseudoRapidity() - (AddPsRap/2))/abs(DiffPsRap);
      double z_m = (reader_2.m_mu.at (0).second.PseudoRapidity() - (AddPsRap/2))/abs(DiffPsRap);

      if(dilepton.M() > 20 && 
         diquark.M() > 500 && 
         abs(DiffPsRap) > 2.5 && 
         reader_2.m_qu.at (0).second.Perp() > 30 && 
         reader_2.m_qu.at (1).second.Perp() > 30 && 
         MET >40 &&
         abs(z_e) < 0.75 &&
         abs(z_m) < 0.75){
              mll_2.Fill (reader_2.m_el.at(0).second.Perp(), reader_2.m_ve.at(0).second.Perp(), reader_2.m_mu.at(0).second.Perp(), 
                reader_2.m_vm.at(0).second.Perp(), dilepton.M (), diquark.M (), reader_2.m_le.at(0).second.Angle(reader_2.m_le.at(1).second.Vect()), 
                abs(DiffPsRap), MET, z_e, z_m);
              qll_2.Fill(reader_2.m_qu.at(0).second.Perp());
              qll_2.Fill(reader_2.m_qu.at(1).second.Perp());
            }
    } //PG loop over events




    TH1F el ("h1","",7,0,700);
    TH1F ve ("h2","",5,0,500);
    TH1F mu ("h3","",5,0,500);
    TH1F vm ("h4","",7,0,700);
    TH1F mLep ("h5","",5,0,1000);
    TH1F mQuark ("h6","",10,500,5000);
    TH1F angLep ("h7","",10,0,3.5);
    TH1F PsRap ("h8","",7,2.5,9);
    TH1F qu ("h9", "", 7,0,1200);

    TH1F el_1 ("h01","",7,0,700);
    TH1F ve_1 ("h02","",5,0,500);
    TH1F mu_1 ("h03","",5,0,500);
    TH1F vm_1 ("h04","",7,0,700);
    TH1F mLep_1 ("h05","",5,0,1000);
    TH1F mQuark_1 ("h06","",10,500,5000);
    TH1F angLep_1 ("h07","",10,0,3.5);
    TH1F PsRap_1 ("h08","",7,2.5,9);
    TH1F qu_1 ("h09", "", 7,0,1200);

    TH1F el_2 ("h001","",7,0,700);
    TH1F ve_2 ("h002","",5,0,500);
    TH1F mu_2 ("h003","",5,0,500);
    TH1F vm_2 ("h004","",7,0,700);
    TH1F mLep_2 ("h005","",5,0,1000);
    TH1F mQuark_2 ("h006","",10,500,5000);
    TH1F angLep_2 ("h007","",10,0,3.5);
    TH1F PsRap_2 ("h008","",7,2.5,9);
    TH1F qu_2 ("h009", "", 7,0,1200);


    el_1.SetLineStyle(9);
    ve_1.SetLineStyle(9);
    mu_1.SetLineStyle(9);
    vm_1.SetLineStyle(9);
    mLep_1.SetLineStyle(9);
    mQuark_1.SetLineStyle(9);
    angLep_1.SetLineStyle(9);
    PsRap_1.SetLineStyle(9);
    qu_1.SetLineStyle(9);

    el_2.SetLineStyle(9);
    ve_2.SetLineStyle(9);
    mu_2.SetLineStyle(9);
    vm_2.SetLineStyle(9);
    mLep_2.SetLineStyle(9);
    mQuark_2.SetLineStyle(9);
    angLep_2.SetLineStyle(9);
    PsRap_2.SetLineStyle(9);
    qu_2.SetLineStyle(9);


    el.SetLineWidth(2);
    ve.SetLineWidth(2);
    mu.SetLineWidth(2);
    vm.SetLineWidth(2);
    mLep.SetLineWidth(2);
    mQuark.SetLineWidth(2);
    angLep.SetLineWidth(2);
    PsRap.SetLineWidth(2);
    qu.SetLineWidth(2);

    el_1.SetLineWidth(2);
    ve_1.SetLineWidth(2);
    mu_1.SetLineWidth(2);
    vm_1.SetLineWidth(2);
    mLep_1.SetLineWidth(2);
    mQuark_1.SetLineWidth(2);
    angLep_1.SetLineWidth(2);
    PsRap_1.SetLineWidth(2);
    qu_1.SetLineWidth(2);

    el_2.SetLineWidth(2);
    ve_2.SetLineWidth(2);
    mu_2.SetLineWidth(2);
    vm_2.SetLineWidth(2);
    mLep_2.SetLineWidth(2);
    mQuark_2.SetLineWidth(2);
    angLep_2.SetLineWidth(2);
    PsRap_2.SetLineWidth(2);
    qu_2.SetLineWidth(2);

    el_1.GetXaxis()-> SetTitle("P_{T} [e^{+}] (GeV/c^{2})");
    el_1.GetXaxis() -> CenterTitle();
    el_1.GetXaxis() -> SetTitleOffset(1.25);
    el_1.GetXaxis() -> SetTitleSize(0.03);
    el_1.GetXaxis() -> SetTitleFont(52);
    el_1.GetYaxis() -> SetTitle("Events (L_{int} = 150 fb^{-1})");
    el_1.GetYaxis() -> SetTitleFont(52);
    el_1.GetYaxis() -> SetTitleSize(0.03);

    ve_1.GetXaxis()-> SetTitle("P_{T} [#nu_{e}] (GeV/c^{2})");
    ve_1.GetXaxis() -> CenterTitle();
    ve_1.GetXaxis() -> SetTitleOffset(1.25);
    ve_1.GetXaxis() -> SetTitleSize(0.03);
    ve_1.GetXaxis() -> SetTitleFont(52);
    ve_1.GetYaxis() -> SetTitle("Events (L_{int} = 150 fb^{-1})");
    ve_1.GetYaxis() -> SetTitleFont(52);
    ve_1.GetYaxis() -> SetTitleSize(0.03);

    mu_1.GetXaxis()-> SetTitle("P_{T} [#mu^{+}] (GeV/c^{2})");
    mu_1.GetXaxis() -> CenterTitle();
    mu_1.GetXaxis() -> SetTitleOffset(1.25);
    mu_1.GetXaxis() -> SetTitleSize(0.03);
    mu_1.GetXaxis() -> SetTitleFont(52);
    mu_1.GetYaxis() -> SetTitle("Events (L_{int} = 150 fb^{-1})");
    mu_1.GetYaxis() -> SetTitleFont(52);
    mu_1.GetYaxis() -> SetTitleSize(0.03);

    vm_1.GetXaxis()-> SetTitle("P_{T} [#nu_{mu}] (GeV/c^{2})");
    vm_1.GetXaxis() -> CenterTitle();
    vm_1.GetXaxis() -> SetTitleOffset(1.25);
    vm_1.GetXaxis() -> SetTitleSize(0.03);
    vm_1.GetXaxis() -> SetTitleFont(52);
    vm_1.GetYaxis() -> SetTitle("Events (L_{int} = 150 fb^{-1})");
    vm_1.GetYaxis() -> SetTitleFont(52);
    vm_1.GetYaxis() -> SetTitleSize(0.03);

    mLep_1.GetXaxis()-> SetTitle("M [leptons] (GeV/c^{2})");
    mLep_1.GetXaxis() -> CenterTitle();
    mLep_1.GetXaxis() -> SetTitleOffset(1.25);
    mLep_1.GetXaxis() -> SetTitleSize(0.03);
    mLep_1.GetXaxis() -> SetTitleFont(52);
    mLep_1.GetYaxis() -> SetTitle("Events (L_{int} = 150 fb^{-1})");
    mLep_1.GetYaxis() -> SetTitleFont(52);
    mLep_1.GetYaxis() -> SetTitleSize(0.03);

    mQuark_1.GetXaxis()-> SetTitle("M [quark] (GeV/c^{2})");
    mQuark_1.GetXaxis() -> CenterTitle();
    mQuark_1.GetXaxis() -> SetTitleOffset(1.25);
    mQuark_1.GetXaxis() -> SetTitleSize(0.03);
    mQuark_1.GetXaxis() -> SetTitleFont(52);
    mQuark_1.GetYaxis() -> SetTitle("Events (L_{int} = 150 fb^{-1})");
    mQuark_1.GetYaxis() -> SetTitleFont(52);
    mQuark_1.GetYaxis() -> SetTitleSize(0.03);

    angLep_1.GetXaxis()-> SetTitle("#theta [leptons]");
    angLep_1.GetXaxis() -> CenterTitle();
    angLep_1.GetXaxis() -> SetTitleOffset(1.25);
    angLep_1.GetXaxis() -> SetTitleSize(0.03);
    angLep_1.GetXaxis() -> SetTitleFont(52);
    angLep_1.GetYaxis() -> SetTitle("Events (L_{int} = 150 fb^{-1})");
    angLep_1.GetYaxis() -> SetTitleFont(52);
    angLep_1.GetYaxis() -> SetTitleSize(0.03);

    PsRap_1.GetXaxis()-> SetTitle("#Delta#eta [quark]");
    PsRap_1.GetXaxis() -> CenterTitle();
    PsRap_1.GetXaxis() -> SetTitleOffset(1.25);
    PsRap_1.GetXaxis() -> SetTitleSize(0.03);
    PsRap_1.GetXaxis() -> SetTitleFont(52);
    PsRap_1.GetYaxis() -> SetTitle("Events (L_{int} = 150 fb^{-1})");
    PsRap_1.GetYaxis() -> SetTitleFont(52);
    PsRap_1.GetYaxis() -> SetTitleSize(0.03);

    qu_2.GetXaxis()-> SetTitle("P_{T} [quark] (GeV/c^{2})");
    qu_2.GetXaxis() -> CenterTitle();
    qu_2.GetXaxis() -> SetTitleOffset(1.25);
    qu_2.GetXaxis() -> SetTitleSize(0.03);
    qu_2.GetXaxis() -> SetTitleFont(52);
    qu_2.GetYaxis() -> SetTitle("Events (L_{int} = 150 fb^{-1})");
    qu_2.GetYaxis() -> SetTitleFont(52);
    qu_2.GetYaxis() -> SetTitleSize(0.03);

    gStyle->SetOptStat(0);





	TCanvas c1 ("c1", "c1");
    el_1.SetLineColor(kRed);
    el_2.SetLineColor(kGreen);
    mll.Draw("el >> h1", "0.5694");
    mll_1.Draw("el >> h01", "4.0035");
    mll_2.Draw("el >> h001", "1.0353");
    el_1.Draw("HIST");
    el.Draw("HIST, SAME");
    el_2.Draw("HIST, SAME");
    TLegend leg1 (0.69,0.75,0.89,0.89);
    leg1.AddEntry(&el, "SM", "l");
    leg1.AddEntry(&el_1, "cqq3", "l");
    leg1.AddEntry(&el_2, "cW", "l");
    leg1.SetMargin(0.75);
    leg1.Draw();
    c1.Print("c1_confronto.png");

    TCanvas c2 ("c2", "c2");
    ve_1.SetLineColor(kRed);
    ve_2.SetLineColor(kGreen);
    mll.Draw("ve >> h2", "0.5694");
    mll_1.Draw("ve >> h02", "4.0035");
    mll_2.Draw("ve >> h002", "1.0353");
    ve_1.Draw("HIST");
    ve.Draw("HIST, SAME");
    ve_2.Draw("HIST, SAME");
    TLegend leg2 (0.69,0.75,0.89,0.89);
    leg2.AddEntry(&ve, "SM", "l");
    leg2.AddEntry(&ve_1, "cqq3", "l");
    leg2.AddEntry(&ve_2, "cW", "l");
    leg2.SetMargin(0.75);
    leg2.Draw();
    c2.Print("c2_confronto.png");

    TCanvas c3 ("c3", "c3");
    mu_1.SetLineColor(kRed);
    mu_2.SetLineColor(kGreen);
    mll.Draw("mu >> h3", "0.5694");
    mll_1.Draw("mu >> h03", "4.0035");
    mll_2.Draw("mu >> h003", "1.0353");
    mu_1.Draw("HIST");
    mu.Draw("HIST, SAME");
    mu_2.Draw("HIST, SAME");
    TLegend leg3 (0.69,0.75,0.89,0.89);
    leg3.AddEntry(&mu, "SM", "l");
    leg3.AddEntry(&mu_1, "cqq3", "l");
    leg3.AddEntry(&mu_2, "cW", "l");
    leg3.SetMargin(0.75);
    leg3.Draw();
    c3.Print("c3_confronto.png");

    TCanvas c4 ("c4", "c4");
    vm_1.SetLineColor(kRed);
    vm_2.SetLineColor(kGreen);
    mll.Draw("vm >> h4", "0.5694");
    mll_1.Draw("vm >> h04", "4.0035");
    mll_2.Draw("vm >> h004", "1.0353");
    vm_1.Draw("HIST");
    vm.Draw("HIST, SAME");
    vm_2.Draw("HIST, SAME");
    TLegend leg4 (0.69,0.75,0.89,0.89);
    leg4.AddEntry(&vm, "SM", "l");
    leg4.AddEntry(&vm_1, "cqq3", "l");
    leg4.AddEntry(&vm_2, "cW", "l");
    leg4.SetMargin(0.75);
    leg4.Draw();
    c4.Print("c4_confronto.png");

    TCanvas c5 ("c5", "c5");
    mLep_1.SetLineColor(kRed);
    mLep_2.SetLineColor(kGreen);
    mll.Draw("mLep >> h5", "0.5694");
    mll_1.Draw("mLep >> h05", "4.0035");
    mll_2.Draw("mLep >> h005", "1.0353");
    mLep_1.Draw("HIST");
    mLep.Draw("HIST, SAME");
    mLep_2.Draw("HIST, SAME");
    TLegend leg5 (0.69,0.75,0.89,0.89);
    leg5.AddEntry(&mLep, "SM", "l");
    leg5.AddEntry(&mLep_1, "cqq3", "l");
    leg5.AddEntry(&mLep_2, "cW", "l");
    leg5.SetMargin(0.75);
    leg5.Draw();
    c5.Print("c5_confronto.png");

	TCanvas c6 ("c6", "c6");
    mQuark_1.SetLineColor(kRed);
    mQuark_2.SetLineColor(kGreen);
    mll.Draw("mQuark >> h6", "0.5694");
    mll_1.Draw("mQuark >> h06", "4.0035");
    mll_2.Draw("mQuark >> h006", "1.0353");
    mQuark_1.Draw("HIST");
    mQuark.Draw("HIST, SAME");
    mQuark_2.Draw("HIST, SAME");
    TLegend leg6 (0.69,0.75,0.89,0.89);
    leg6.AddEntry(&mQuark, "SM", "l");
    leg6.AddEntry(&mQuark_1, "cqq3", "l");
    leg6.AddEntry(&mQuark_2, "cW", "l");
    leg6.SetMargin(0.75);
    leg6.Draw();
    c6.Print("c6_confronto.png");

    TCanvas c7 ("c7", "c7");
    angLep_1.SetLineColor(kRed);
    angLep_2.SetLineColor(kGreen);
    mll.Draw("angLep >> h7", "0.5694");
    mll_1.Draw("angLep >> h07", "4.0035");
    mll_2.Draw("angLep >> h007", "1.0353");
    angLep_1.Draw("HIST");
    angLep.Draw("HIST, SAME");
    angLep_2.Draw("HIST, SAME");
    TLegend leg7 (0.69,0.75,0.89,0.89);
    leg7.AddEntry(&angLep, "SM", "l");
    leg7.AddEntry(&angLep_1, "cqq3", "l");
    leg7.AddEntry(&angLep_2, "cW", "l");
    leg7.SetMargin(0.75);
    leg7.Draw();
    c7.Print("c7_confronto.png");

    TCanvas c8 ("c8", "c8");
    PsRap_1.SetLineColor(kRed);
    PsRap_2.SetLineColor(kGreen);
    mll.Draw("PsRap >> h8", "0.5694");
    mll_1.Draw("PsRap >> h08", "4.0035");
    mll_2.Draw("PsRap >> h008", "1.0353");
    PsRap_1.Draw("HIST");
    PsRap.Draw("HIST, SAME");
    PsRap_2.Draw("HIST, SAME");
    TLegend leg8 (0.69,0.75,0.89,0.89);
    leg8.AddEntry(&PsRap, "SM", "l");
    leg8.AddEntry(&PsRap_1, "cqq3", "l");
    leg8.AddEntry(&PsRap_2, "cW", "l");
    leg8.SetMargin(0.75);
    leg8.Draw();
    c8.Print("c8_confronto.png");

    TCanvas c9 ("c9", "c9");
    qu_1.SetLineColor(kRed);
    qu_2.SetLineColor(kGreen);
    qll.Draw("qll >> h9", "0.5694");
    qll_1.Draw("qll >> h09", "4.0035");
    qll_2.Draw("qll >> h009", "1.0353");
    qu_2.Draw("HIST");
    qu.Draw("HIST, SAME");
    qu_1.Draw("HIST, SAME");
    TLegend leg9 (0.69,0.75,0.89,0.89);
    leg9.AddEntry(&qu, "SM", "l");
    leg9.AddEntry(&qu_1, "cqq3", "l");
    leg9.AddEntry(&qu_2, "cW", "l");
    leg9.SetMargin(0.75);
    leg9.Draw();
    c9.Print("c9_confronto.png");





    outFileName += ".root" ; 
  	TFile outFile (outFileName.c_str (), "recreate") ;
  	mll.Write () ;
  	qll.Write () ;
  	outFile.Close () ;

  	
  	outFileName_1 += ".root" ; 
  	TFile outFile_1 (outFileName_1.c_str (), "recreate") ;
  	mll_1.Write () ;
  	qll_1.Write () ;
  	outFile_1.Close () ;

  	outFileName_2 += ".root" ; 
  	TFile outFile_2 (outFileName_2.c_str (), "recreate") ;
  	mll_2.Write () ;
  	qll_2.Write () ;
  	outFile_2.Close () ;




  return 0 ;
}