#include "dcutils.h"

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>

#include <TTreeReader.h>

#include "TFile.h"
#include "TH1F.h"

using namespace std ;


// for debugging purposes
void printvector (vector<string> & v)
{
  for (int i = 0 ; i < v.size () ; ++i) cout << "\t" << v.at (i) ;
  cout << endl ;	
  return ;
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


bool replace (std::string& str, const std::string& from, const std::string& to) 
{
    size_t start_pos = str.find (from) ;
    if (start_pos == std::string::npos) return false;
    str.replace (start_pos, from.length (), to) ;
    return true ;
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


// produces vector of bin edges for uniform binning
vector<float> splitaxis (float min, float max, int nbins)
{
  vector<float> dummy ;
  float delta = (max - min) / nbins ;
  for (int i = 0 ; i <= nbins ; ++i)
    {
      dummy.push_back (min + delta * i) ;
    }
  return dummy ;
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


// produce the vector of bin edges for the histograms,
// to that binning may be variable and tails do not have empty bins
vector<float> calcBinEdges (string varname, float min, float max, int nbins) 
{
  if (   varname == "deltaetajj" 
      || varname == "deltaphijj"
      || varname == "noshape")
    {
      return splitaxis (min, max, nbins) ;
    }  
  vector<float> dummy ;
  float delta = (max - min) / (3. * nbins) ;
  int changes = 0 ;
  float limit = min ;
  while (limit < max)
    {
      dummy.push_back (limit) ;
      if (changes == 2) continue ;
      if (limit >= 4./5. * (max-min)) 
        {
          delta = delta * 3. / 2. ;
          changes = 2 ;
        }  
      if (changes == 1) continue ;
      if (limit >= 3./4. * (max-min)) 
        {
          delta = delta * 2. ;
          changes = 1 ; 
        } 
      limit += delta ;
    }
  dummy.push_back (max) ;
  return dummy ;
}  


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


// loop on the ntuple saved in a root file
// and fill the histograms to be used for the creation of the datacards.
// Each variable in the cfg file will have an histogram created.
map<string, TH1F *> 
readNtupleFile (string rootFileName, string ntupleName,
                string hTag, string hTitle,
                CfgParser * gConfigParser)
{
  // extract from the config parser the relevant information
  // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 

  // variables to be studied
  vector<string> variables = gConfigParser->readStringListOpt ("general::variables") ;
  sort (variables.begin (), variables.end ()) ;

  // create the output histograms
  map<string, TH1F *> output_histos ;
  for (int i = 0 ; i < variables.size () ; ++i)
    {
      vector<float> edges = calcBinEdges (
          variables.at (i),
          gConfigParser->readFloatOpt (variables.at (i) + "::min"),
          gConfigParser->readFloatOpt (variables.at (i) + "::max"),
          gConfigParser->readFloatOpt (variables.at (i) + "::bins")
       ) ;

      output_histos[variables.at (i)] = new TH1F 
        (
          (hTag + variables.at (i)).c_str (), hTitle.c_str (),
          edges.size () - 1,
          edges.data ()
        ) ;
    }

  // read cuts
  map <string, float> cuts ;
  vector<string> cutslist = gConfigParser->readListOfOpts ("cuts") ;
  for (int iCut = 0 ; iCut < cutslist.size () ; ++iCut)
    {
      cuts[cutslist[iCut]] = gConfigParser->readFloatOpt ("cuts::" + cutslist[iCut]) ;
    } 

  map <string, float> uppercuts ;
  vector<string> uppercutslist = gConfigParser->readListOfOpts ("uppercuts") ;
  for (int iCut = 0 ; iCut < uppercutslist.size () ; ++iCut)
    {
      uppercuts[uppercutslist[iCut]] = gConfigParser->readFloatOpt ("cuts::" + uppercutslist[iCut]) ;
    } 

  // open the input file and extract general info concerning the sample 
  // ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- 

  TFile rootFile (rootFileName.c_str ()) ;
  string histoName = ntupleName + "_nums" ;
  TH1F * global_numbers = (TH1F *) rootFile.Get (histoName.c_str ()) ;

  float cross_section        = global_numbers->GetBinContent (1) ;
  float sum_weights_total    = global_numbers->GetBinContent (2) ;
  float sum_weights_selected = global_numbers->GetBinContent (3) ;
  
  float luminosity = gConfigParser->readFloatOpt ("general::luminosity") ;
  //NB luminosity in fb, cross-section expected in pb in the config files
  float normalization = cross_section * 1000. * luminosity / (sum_weights_total) ;

  // open the TTree for reading
  TTreeReader reader (ntupleName.c_str (), &rootFile) ;

  // prepare the raeders for each variable
  bool savenoshape = false ;
  map<string, TTreeReaderValue<float> > treeReaderValues ;

  // all variables to be saved and to be cut on have to be read
  vector<string> allvars = variables ;
  allvars.insert (allvars.end (), cutslist.begin (), cutslist.end ()) ;
  allvars.insert (allvars.end (), uppercutslist.begin (), uppercutslist.end ()) ;
  sort (allvars.begin (), allvars.end ()) ;
  vector<string>::iterator it = unique (allvars.begin (), allvars.end ()) ;
  allvars.resize (distance (allvars.begin (), it)) ;
 
  printvector (allvars) ;

  for (int iVar = 0 ; iVar < allvars.size () ; ++iVar)
    {	
      if (allvars[iVar] == "noshape") 
        {
          savenoshape = true ;
          continue ;
        }
      treeReaderValues.insert (
        pair<string, TTreeReaderValue<float> > (
          allvars[iVar], 
          TTreeReaderValue<float> (reader, allvars[iVar].c_str ())
        ) 
      ) ; 
//      treeReaderValues[ variables[iVar]] = TTreeReaderValue<float> (reader, variables[iVar].c_str ()) ;
    } 
  TTreeReaderValue<float> weight (reader, "w") ;

  // loop over the events
  while (reader.Next ())
    {
      // apply selections
      bool save = true ;
      for (map<string, float>::const_iterator iCut = cuts.begin () ;
           iCut != cuts.end () ;
           ++iCut)
        {
          if (*(treeReaderValues.at (iCut->first)) < iCut->second) 
            {
              save = false ;
              break ;
            } 
        } 
      for (map<string, float>::const_iterator iCut = uppercuts.begin () ;
           iCut != uppercuts.end () ;
           ++iCut)
        {
          if (*(treeReaderValues.at (iCut->first)) > iCut->second) 
            {
              save = false ;
              break ;
            } 
        } 
      if (save == false) continue ;

      // fill histograms
      for (map<string, TH1F*>::iterator iHisto = output_histos.begin () ;
           iHisto != output_histos.end () ;
           ++iHisto)
        {
          iHisto->second->Fill ( *(treeReaderValues.at (iHisto->first)), *weight) ;
        }
      if (savenoshape) output_histos["noshape"]->Fill (1., *weight) ;

    } // loop over the events

  for (map<string, TH1F* >::const_iterator iHisto = output_histos.begin () ;
       iHisto != output_histos.end () ;
       ++iHisto)
    {
      iHisto->second->Scale (normalization) ;
      //overflow bin events moved in the last bin
      //last bin content=last bin content + content of overflow bin
      //FIXME underflow missing
      if (iHisto->first != "noshape")
        {
          iHisto->second->SetBinContent 
            (
          	  iHisto->second->GetNbinsX (), 
              iHisto->second->GetBinContent (iHisto->second->GetNbinsX ())
                + iHisto->second->GetBinContent (iHisto->second->GetNbinsX () + 1)
            ) ;
          iHisto->second->SetBinContent (iHisto->second->GetNbinsX () + 1, 0.) ;
        }
    } 
  checkEmptyBins (output_histos) ;

  return output_histos ;
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


void 
scaleAllHistos (std::map<std::string, TH1F *> & hMap, float value) 
{
  for (map<string, TH1F* >::const_iterator iHisto = hMap.begin () ;
       iHisto != hMap.end () ;
       ++iHisto)
    {
      iHisto->second->Scale (value) ;
    } 
  return ;
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


void 
checkEmptyBins (std::map<std::string, TH1F *> & hMap)
{
  cout << "Empty bins check:\n" ;
  for (map<string, TH1F* >::const_iterator iHisto = hMap.begin () ;
       iHisto != hMap.end () ;
       ++iHisto)
    {
      cout << " > histo: " << iHisto->second->GetName () << "\n" ; 	
      for (int iBin = 0 ; iBin < iHisto->second->GetNbinsX () + 2 ; ++iBin) 
        {
          if (iHisto->second->GetBinContent (iBin) < 1)
            {
              cout << "   bin " << iBin << " = " << iHisto->second->GetBinContent (iBin) << "\n" ;
            }  
        }
    } 
  return ;
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


int 
createDataCard (TH1F * h_SM, TH1F * h_LI, TH1F * h_QU, 
                string prefix, string varname) 
{
  // create the root file containing the three histograms
  string rootfilename = prefix + "_" + varname + ".root" ;
  TFile outf (rootfilename.c_str (), "recreate") ;
  h_SM->Write ("histo_sm") ;
  h_LI->Write ("histo_linear") ;
  h_QU->Write ("histo_quadratic") ;
  outf.Close () ;

  // create the root file containing the three histograms
  string txtfilename = prefix + "_" + varname + ".txt" ;
  ofstream output_datacard (txtfilename.c_str ()) ;

  string separator= "-------------------------------------------------------\n" ;
  output_datacard << separator ;
  output_datacard << "imax *" <<endl ;
  output_datacard << "jmax *" <<endl ;
  output_datacard << "kmax *" <<endl ;
  output_datacard << separator ;

  output_datacard << "shapes *\t* " + rootfilename + " histo_$PROCESS $PROCESS_$SYSTEMATIC\n" ;
  output_datacard << "shapes data_obs\t* " + rootfilename + " " + "histo_sm" << endl ;
  output_datacard << separator ;
  output_datacard << "bin\t test\n" ;
  output_datacard << "observation\t" << h_SM->Integral () << endl ;
  output_datacard <<separator ;

  output_datacard << "bin\ttest\ttest\ttest\n";
  output_datacard << "process\tsm\tlinear\tquadratic\n" ;
  output_datacard << "process\0\t1\t2\n" ;
  output_datacard << "rate\t" << h_SM->Integral () << "\t" 
                  << h_LI->Integral () << "\t"
                  << h_QU->Integral () << "\n" ;
  output_datacard <<separator ;
  output_datacard <<"lumi\tlnN\t1.02\t1.02\t1.02\n";
  output_datacard <<"bla\tlnN\t1-\t-\t1.05\n";

  return 0 ;
}


