// c++ -o datacard_creator_2 `root-config --glibs --cflags` CfgParser.cc dcutils.cc -lm datacard_creator_2.cpp
/*
one should pass the program a config file like file.cfg
to run: ./datacard_creator_2 file.cfg
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

#include "CfgParser.h"
#include "dcutils.h"

using namespace std ;


int main (int argc, char ** argv)
{
  TH1::SetDefaultSumw2 () ;
  if (argc < 2)
    {
      cerr << "Forgot to put the cfg file --> exit " << endl ;
      return 1 ;
    }

  CfgParser * gConfigParser = new CfgParser (argv[1]) ;

 // reading generic parameters of the generation
  // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 

  //wilson_coeff should be a float like 0.3, it will be processed after
  //FIXME TO BE RATIONALISED
  float wilson_coeff = gConfigParser->readFloatOpt ("general::wilson_coeff_generation_value") ;
  string EFT_operator = gConfigParser->readStringOpt ("general::operator") ;

  //otherwise if you don't want to use my syntax(which I highly reccomend not to do)
  //string infile = gConfigParser->readStringListOpt ("general::infile") ;

  // reading input files information
  // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 

  //name of ntuples in this order: sm, linear(interference), quadratic(BSM)
  //also the evtNum_nisto_names should be in the format name_ntuple_nums(which also Vittorio used)
  vector<string> input_files = gConfigParser->readStringListOpt ("general::input_files") ;
  vector<string> name_ntuples = gConfigParser->readStringListOpt ("general::name_ntuples") ;

  // reading the physics from the input files
  // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 

  //this is for output names only
  string histo_names[] = {"histo_sm", "histo_linear", "histo_quadratic"};
  string dist_names[] = {"sm","linear","quadratic"};

  std::map<std::string, TH1F *> hmap_SM = readNtupleFile (input_files[0], name_ntuples[0], "SM_", "sm",        gConfigParser) ;
  std::map<std::string, TH1F *> hmap_LI = readNtupleFile (input_files[1], name_ntuples[1], "LI_", "linear",    gConfigParser) ;
  scaleAllHistos (hmap_LI, 1./wilson_coeff) ;
  std::map<std::string, TH1F *> hmap_QU = readNtupleFile (input_files[2], name_ntuples[2], "QU_", "quadratic", gConfigParser) ;
  scaleAllHistos (hmap_QU, 1./(wilson_coeff * wilson_coeff)) ;

  // creating datacards and rootfile for each variable
  // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 

  string outfilesprefix = gConfigParser->readStringOpt ("general::outfilesprefix") ;
  outfilesprefix += ("_" + EFT_operator) ;
  //loop on variables 
  for (map<string, TH1F* >::const_iterator iHisto = hmap_SM.begin () ;
       iHisto != hmap_SM.end () ;
       ++iHisto)
    {
      // get the three histograms 
      TH1F * h_SM = iHisto->second ;
      TH1F * h_LI = hmap_LI.at (iHisto->first) ;
      TH1F * h_QU = hmap_QU.at (iHisto->first) ;
      createDataCard (h_SM, h_LI, h_QU, outfilesprefix, iHisto->first) ;
    } 
  return 0 ;
}
