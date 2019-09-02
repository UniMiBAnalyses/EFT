// c++  -g -o x     x.cpp `root-config --libs --glibs --cflags`
//./x     unweighted_events_SM.lhe      unweighted_events_cW_0_3.lhe 
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
  if (m_evtNum == 0) std::cerr << "xs " << m_reader.initComments << "\n";


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
  int maxNum = 2000;
  if (argc > 3) maxNum = atoi (argv[3]) ;
  string outFileName = "output_SM" ;
  string outFileName_1 = "output_cW";

  //AT Primo file--------------------------------------------------------------------------------------------------------------------------------------------
  TNtuple mll ("mll", "mll", "el:ve:mu:vm:mLep:mQuark:angLep:PsRap:MET:ze:zm") ; /*AT momento trasverso (4), massa inviariante leptoni, 
    massa invariante quark, angolo tra i leptoni, differenza della pseudorapidità dei quark, energia trasversa mancante*/
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
         MET>40 &&
         abs(z_e) < 0.75 &&
         abs(z_m) < 0.75){
            mll.Fill (reader.m_el.at(0).second.Perp(), reader.m_ve.at(0).second.Perp(), reader.m_mu.at(0).second.Perp(), 
              reader.m_vm.at(0).second.Perp(), dilepton.M (), diquark.M (), reader.m_le.at(0).second.Angle(reader.m_le.at(1).second.Vect()), abs(DiffPsRap)
              ,MET,z_e, z_m);
            qll.Fill(reader.m_qu.at(0).second.Perp());
            qll.Fill(reader.m_qu.at(1).second.Perp());
          }
    } //PG loop over events


  LHEreader reader_1 (argv[2]) ;


  //AT Secondo file------------------------------------------------------------------------------------------------------------------------------------------
  TNtuple mll_1 ("mll", "mll", "el:ve:mu:vm:mLep:mQuark:angLep:PsRap:MET:ze:zm") ; /*AT momento trasverso (4), massa inviariante leptoni, 
    massa invariante quark, angolo tra i leptoni, differenza della pseudorapidità dei quark*/
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
         MET>40 &&
         abs(z_e) < 0.75 &&
         abs(z_m) < 0.75){
            mll_1.Fill (reader_1.m_el.at(0).second.Perp(), reader_1.m_ve.at(0).second.Perp(), reader_1.m_mu.at(0).second.Perp(), 
              reader_1.m_vm.at(0).second.Perp(), dilepton.M (), diquark.M (), reader_1.m_le.at(0).second.Angle(reader_1.m_le.at(1).second.Vect()), 
              abs(DiffPsRap),MET, z_e, z_m);
            qll_1.Fill(reader_1.m_qu.at(0).second.Perp());
            qll_1.Fill(reader_1.m_qu.at(1).second.Perp());
          }
    } //PG loop over events






  //AT Istogrammi per controllo sulle selezioni
  TCanvas canvas ("canvas", "canvas");
  canvas.Divide(2,5);

  canvas.cd(1);
  TH1F histo ("histo","mLep_SM",5,0,50);
  mll.Draw("mLep >> histo");
  histo.Draw();

  canvas.cd(2);
  TH1F histo_1 ("histo_1","mLep_EFT",5,0,50);
  mll_1.Draw("mLep >> histo_1");
  histo_1.Draw();

  canvas.cd(3);
  TH1F histo1 ("histo1","mQuark_SM",10,0,1000);
  mll.Draw("mQuark >> histo1");
  histo1.Draw();

  canvas.cd(4);
  TH1F histo1_1 ("histo1_1","mQuark_EFT",10,0,1000);
  mll_1.Draw("mQuark >> histo1_1");
  histo1_1.Draw();

  canvas.cd(5);
  TH1F histo2 ("histo2","PsRap_SM",20,0,5);
  mll.Draw("PsRap >> histo2");
  histo2.Draw();

  canvas.cd(6);
  TH1F histo2_1 ("histo2_1","PsRap_EFT",20,0,5);
  mll_1.Draw("PsRap >> histo2_1");
  histo2_1.Draw();

  canvas.cd(7);
  TH1F histo3 ("histo3","qll_SM",10,0,50);
  qll.Draw("qll >> histo3");
  histo3.Draw();

  canvas.cd(8);
  TH1F histo3_1 ("histo3_1","ql_EFT",10,0,50);
  qll_1.Draw("qll >> histo3_1");
  histo3_1.Draw();

  canvas.cd(9);
  TH1F histo4 ("histo4","MET_SM",8,0,80);
  mll.Draw("MET >> histo4");
  histo4.Draw();

  canvas.cd(10);
  TH1F histo4_1 ("histo4_1","MET_EFT",8,0,80);
  mll_1.Draw("MET >> histo4_1");
  histo4_1.Draw();

  canvas.Print("Selezioni_eventi_1.pdf");
  
  TCanvas canvas1 ("canvas1", "canvas1");
  canvas1.Divide(2,2);

  canvas1.cd(1);
  TH1F histo5 ("histo5","z_e_SM",10,-1,1);
  mll.Draw("ze >> histo5");
  histo5.Draw();

  canvas1.cd(2);
  TH1F histo5_1 ("histo5_1", "z_e_EFT", 10, -1, 1);
  mll_1.Draw("ze >> histo5_1");
  histo5_1.Draw();

  canvas1.cd(3);
  TH1F histo6 ("histo6","z_m_SM",10,-1,1);
  mll.Draw("zm >> histo6");
  histo6.Draw();

  canvas1.cd(4);
  TH1F histo6_1 ("histo6_1", "z_m_EFT", 10, -1, 1);
  mll_1.Draw("zm >> histo6_1");
  histo6_1.Draw();

  canvas1.Print("Selezioni_eventi_2.pdf");






    TH1F el ("h1","",20,0,1000);
    TH1F ve ("h2","",20,0,1000);
    TH1F mu ("h3","",20,0,1000);
    TH1F vm ("h4","",20,0,1000);
    TH1F mLep ("h5","",20,0,1000);
    TH1F mQuark ("h6","",30,500,7000);
    TH1F angLep ("h7","",15,0,3.5);
    TH1F PsRap ("h8","",20,0,12);
    TH1F qu ("h9", "", 20,0,1200);

    TH1F el_1 ("h01","",20,0,1000);
    TH1F ve_1 ("h02","",20,0,1000);
    TH1F mu_1 ("h03","",20,0,1000);
    TH1F vm_1 ("h04","",20,0,1000);
    TH1F mLep_1 ("h05","",20,0,1000);
    TH1F mQuark_1 ("h06","",30,500,7000);
    TH1F angLep_1 ("h07","",15,0,3.5);
    TH1F PsRap_1 ("h08","",20,0,12);
    TH1F qu_1 ("h09", "", 20,0,1200);

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

	  qu_1.GetXaxis()-> SetTitle("P_{T} [quark] (GeV/c^{2})");
    qu_1.GetXaxis() -> CenterTitle();
    qu_1.GetXaxis() -> SetTitleOffset(1.25);
    qu_1.GetXaxis() -> SetTitleSize(0.03);
    qu_1.GetXaxis() -> SetTitleFont(52);
    qu_1.GetYaxis() -> SetTitle("Events (L_{int} = 150 fb^{-1})");
    qu_1.GetYaxis() -> SetTitleFont(52);
    qu_1.GetYaxis() -> SetTitleSize(0.03);

	gStyle->SetOptStat(0);
	gStyle->SetErrorX(0);

	  TCanvas c1 ("c1", "c1");
    c1.SetLogy();
    el_1.SetLineColor(kRed);
    mll.Draw("el >> h1", "0.0005958");
    mll_1.Draw("el >> h01", "0.0006458");
    for(int j = 1; j<=el.GetSize(); j++){
    	double errore = 0.0005958 * sqrt(el.GetBinContent(j) / 0.0005958);
    	el.SetBinError(j, errore);
    }
    for(int j = 1; j<=el_1.GetSize(); j++){
    	double errore = 0.0006458 * sqrt(el_1.GetBinContent(j) / 0.0006458);
    	el_1.SetBinError(j, errore);
    }
    el_1.SetFillColor(kRed);
    el_1.SetMarkerStyle(21);
    el_1.SetMarkerColor(kBlack);
    el.SetMarkerStyle(21);
    el.SetFillColor(kBlue);
    el.SetMarkerColor(kBlack);
    el_1.Draw("HIST, E1"); 
    el.Draw("HIST, SAME, E1");
    TLegend leg1 (0.69,0.75,0.89,0.89);
    leg1.AddEntry(&el_1, "cW = 0.3", "f");
    leg1.AddEntry(&el, "SM", "f");
    leg1.SetMargin(0.5);
    leg1.Draw();
    c1.Print("c1_cW_0_3_log.png");
    

    TCanvas c2 ("c2", "c2");
    c2.SetLogy();
    ve_1.SetLineColor(kRed);
    mll.Draw("ve >> h2", "0.0005958");
    mll_1.Draw("ve >> h02", "0.0006458");
    for(int j = 1; j<=ve.GetSize(); j++){
    	cout << "Bin " << j << ": " << ve.GetBinContent(j) << endl;
    	double errore = 0.0005958 * sqrt(ve.GetBinContent(j) / 0.0005958);
    	cout << "Errore " << j << ": " << errore << endl;
    	ve.SetBinError(j, errore);
    }
    for(int j = 1; j<=ve_1.GetSize(); j++){
    	double errore = 0.0006458 * sqrt(ve_1.GetBinContent(j) / 0.0006458);
    	ve_1.SetBinError(j, errore);
    }
    ve_1.SetFillColor(kRed);
    ve.SetFillColor(kBlue);
    ve_1.Draw("HIST, E1");
    ve.Draw("HIST, SAME, E1");
    TLegend leg2 (0.69,0.75,0.89,0.89);
    leg2.AddEntry(&el_1, "cW = 0.3", "f");
    leg2.AddEntry(&el, "SM", "f");
    leg2.SetMargin(0.5);
    leg2.Draw();
    c2.Print("c2_cW_0_3_log.png");

    TCanvas c3 ("c3", "c3");
    c3.SetLogy();
    mu_1.SetLineColor(kRed);
    mll.Draw("mu >> h3", "0.0005958");
    mll_1.Draw("mu >> h03", "0.0006458");
    for(int j = 1; j<=mu.GetSize(); j++){
    	cout << "Bin " << j << ": " << mu.GetBinContent(j) << endl;
    	double errore = 0.0005958 * sqrt(mu.GetBinContent(j) / 0.0005958);
    	cout << "Errore " << j << ": " << errore << endl;
    	mu.SetBinError(j, errore);
    }
    for(int j = 1; j<=mu_1.GetSize(); j++){
    	double errore = 0.0006458 * sqrt(mu_1.GetBinContent(j) / 0.0006458);
    	mu_1.SetBinError(j, errore);
    }
    mu_1.SetFillColor(kRed);
    mu.SetFillColor(kBlue);
    mu_1.Draw("HIST, E1");
    mu.Draw("HIST, SAME, E1");
    TLegend leg3 (0.69,0.75,0.89,0.89);
    leg3.AddEntry(&el_1, "cW = 0.3", "f");
    leg3.AddEntry(&el, "SM", "f");
    leg3.SetMargin(0.5);
    leg3.Draw();
    c3.Print("c3_cW_0_3_log.png");

    TCanvas c4 ("c4", "c4");
    c4.SetLogy();
    vm_1.SetLineColor(kRed);
    mll.Draw("vm >> h4", "0.0005958");
    mll_1.Draw("vm >> h04", "0.0006458");
    for(int j = 1; j<=vm.GetSize(); j++){
    	double errore = 0.0005958 * sqrt(vm.GetBinContent(j) / 0.0005958);
    	vm.SetBinError(j, errore);
    }
    for(int j = 1; j<=vm_1.GetSize(); j++){
    	double errore = 0.0006458 * sqrt(vm_1.GetBinContent(j) / 0.0006458);
    	vm_1.SetBinError(j, errore);
    }
    vm_1.SetFillColor(kRed);
    vm.SetFillColor(kBlue);
    vm_1.Draw("HIST, E1");
    vm.Draw("HIST, SAME, E1");
    TLegend leg4 (0.69,0.75,0.89,0.89);
    leg4.AddEntry(&el_1, "cW = 0.3", "f");
    leg4.AddEntry(&el, "SM", "f");
    leg4.SetMargin(0.5);
    leg4.Draw();
    c4.Print("c4_cW_0_3_log.png");

    TCanvas c5 ("c5", "c5");
    c5.SetLogy();
    mLep_1.SetLineColor(kRed);
    mll.Draw("mLep >> h5", "0.0005958");
    mll_1.Draw("mLep >> h05", "0.0006458");
    for(int j = 1; j<=mLep.GetSize(); j++){
    	double errore = 0.0005958 * sqrt(mLep.GetBinContent(j) / 0.0005958);
    	mLep.SetBinError(j, errore);
    }
    for(int j = 1; j<=mLep.GetSize(); j++){
    	double errore = 0.0006458 * sqrt(mLep_1.GetBinContent(j) / 0.0006458);
    	mLep_1.SetBinError(j, errore);
    }
    mLep_1.SetFillColor(kRed);
    mLep.SetFillColor(kBlue);
    mLep_1.Draw("HIST, E1");
    mLep.Draw("HIST, SAME, E1");
    TLegend leg5 (0.69,0.75,0.89,0.89);
    leg5.AddEntry(&el_1, "cW = 0.3", "f");
    leg5.AddEntry(&el, "SM", "f");
    leg5.SetMargin(0.5);
    leg5.Draw();
    c5.Print("c5_cW_0_3_log.png");

	  TCanvas c6 ("c6", "c6");
    c6.SetLogy();
    mQuark_1.SetLineColor(kRed);
    mll.Draw("mQuark >> h6", "0.0005958");
    mll_1.Draw("mQuark >> h06", "0.0006458");
    for(int j = 1; j<=mQuark.GetSize(); j++){
    	double errore = 0.0005958 * sqrt(mQuark.GetBinContent(j) / 0.0005958);
    	mQuark.SetBinError(j, errore);
    }
    for(int j = 1; j<=mQuark.GetSize(); j++){
    	double errore = 0.0006458 * sqrt(mQuark_1.GetBinContent(j) / 0.0006458);
    	mQuark_1.SetBinError(j, errore);
    }
    mQuark_1.SetFillColor(kRed);
    mQuark.SetFillColor(kBlue);
    mQuark_1.Draw("HIST, E1");
    mQuark.Draw("HIST, SAME, E1");
    TLegend leg6 (0.69,0.75,0.89,0.89);
    leg6.AddEntry(&el_1, "cW = 0.3", "f");
    leg6.AddEntry(&el, "SM", "f");
    leg6.SetMargin(0.5);
    leg6.Draw();
    c6.Print("c6_cW_0_3_log.png");

    TCanvas c7 ("c7", "c7");
    c7.SetLogy();
    angLep_1.SetLineColor(kRed);
    mll.Draw("angLep >> h7", "0.0005958");
    mll_1.Draw("angLep >> h07", "0.0006458");
    for(int j = 1; j<=angLep.GetSize(); j++){
    	double errore = 0.0005958 * sqrt(angLep.GetBinContent(j) / 0.0005958);
    	angLep.SetBinError(j, errore);
    }
    for(int j = 1; j<=angLep_1.GetSize(); j++){
    	double errore = 0.0006458 * sqrt(angLep_1.GetBinContent(j) / 0.0006458);
    	angLep_1.SetBinError(j, errore);
    }
    angLep_1.SetFillColor(kRed);
    angLep.SetFillColor(kBlue);
    angLep_1.Draw("HIST, E1");
    angLep.Draw("HIST, SAME, E1");
    TLegend leg7 (0.69,0.75,0.89,0.89);
    leg7.AddEntry(&el_1, "cW = 0.3", "f");
    leg7.AddEntry(&el, "SM", "f");
    leg7.SetMargin(0.5);
    leg7.Draw();
    c7.Print("c7_cW_0_3_log.png");

    TCanvas c8 ("c8", "c8");
    c8.SetLogy();
    PsRap_1.SetLineColor(kRed);
    mll.Draw("PsRap >> h8", "0.0005958");
    mll_1.Draw("PsRap >> h08", "0.0006458");
    for(int j = 1; j<=PsRap.GetSize(); j++){
    	double errore = 0.0005958 * sqrt(PsRap.GetBinContent(j) / 0.0005958);
    	PsRap.SetBinError(j, errore);
    }
    for(int j = 1; j<=PsRap_1.GetSize(); j++){
    	double errore = 0.0006458 * sqrt(PsRap_1.GetBinContent(j) / 0.0006458);
    	PsRap_1.SetBinError(j, errore);
    }
    PsRap_1.SetFillColor(kRed);
    PsRap.SetFillColor(kBlue);
    PsRap_1.Draw("HIST, E1");
    PsRap.Draw("HIST, SAME, E1");
    TLegend leg8 (0.72,0.75,0.89,0.89);
    leg8.AddEntry(&el_1, "cW = 0.3", "f");
    leg8.AddEntry(&el, "SM", "f");
    leg8.SetMargin(0.5);
    leg8.Draw();
    c8.Print("c8_cW_0_3_log.png");

    TCanvas c9 ("c9", "c9");
    c9.SetLogy();
    qu_1.SetLineColor(kRed);
    qll.Draw("qll >> h9", "0.0005958");
    qll_1.Draw("qll >> h09", "0.0006458");
    for(int j = 1; j<=qu.GetSize(); j++){
    	double errore = 0.0005958 * sqrt(qu.GetBinContent(j) / 0.0005958);
    	qu.SetBinError(j, errore);
    }
    for(int j = 1; j<=qu_1.GetSize(); j++){
    	double errore = 0.0006458 * sqrt(qu_1.GetBinContent(j) / 0.0006458);
    	qu_1.SetBinError(j, errore);
    }
    qu_1.SetFillColor(kRed);
    qu.SetFillColor(kBlue);
    qu_1.Draw("HIST, E1");
    qu.Draw("HIST, SAME, E1");
    TLegend leg9 (0.69,0.75,0.89,0.89);
    leg9.AddEntry(&el_1, "cW = 0.3", "f");
    leg9.AddEntry(&el, "SM", "f");
    leg9.SetMargin(0.5);
    leg9.Draw();
    c9.Print("c9_cW_0_3_log.png");




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

  return 0 ;
}