#include "dcutils.h"

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>

#include <TTreeReader.h>

#include "TFile.h"
#include "TH1F.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "THStack.h"
#include "TLegend.h"
#include "TString.h"

using namespace std ;

void setTDRStyle () 
{
  TStyle *tdrStyle = new TStyle("tdrStyle","Style for P-TDR");

  // For the canvas:
  tdrStyle->SetCanvasBorderMode(0);
  tdrStyle->SetCanvasColor(kWhite);
  tdrStyle->SetCanvasDefH(600); //Height of canvas
  tdrStyle->SetCanvasDefW(600); //Width of canvas
  tdrStyle->SetCanvasDefX(0);   //POsition on screen
  tdrStyle->SetCanvasDefY(0);

  // For the Pad:
  tdrStyle->SetPadBorderMode(0);
  // tdrStyle->SetPadBorderSize(Width_t size = 1);
  tdrStyle->SetPadColor(kWhite);
  tdrStyle->SetPadGridX(false);
  tdrStyle->SetPadGridY(false);
  tdrStyle->SetGridColor(0);
  tdrStyle->SetGridStyle(3);
  tdrStyle->SetGridWidth(1);

  // For the frame:
  tdrStyle->SetFrameBorderMode(0);
  tdrStyle->SetFrameBorderSize(1);
  tdrStyle->SetFrameFillColor(0);
  tdrStyle->SetFrameFillStyle(0);
  tdrStyle->SetFrameLineColor(1);
  tdrStyle->SetFrameLineStyle(1);
  tdrStyle->SetFrameLineWidth(1);

  // For the histo:
  // tdrStyle->SetHistFillColor(1);
  // tdrStyle->SetHistFillStyle(0);
  tdrStyle->SetHistLineColor(1);
  tdrStyle->SetHistLineStyle(0);
  tdrStyle->SetHistLineWidth(1);
  // tdrStyle->SetLegoInnerR(Float_t rad = 0.5);
  // tdrStyle->SetNumberContours(Int_t number = 20);

  tdrStyle->SetEndErrorSize(2);
  //tdrStyle->SetErrorMarker(20);
  tdrStyle->SetErrorX(0.);
  
  tdrStyle->SetMarkerStyle(20);

  //For the fit/function:
  tdrStyle->SetOptFit(1);
  tdrStyle->SetFitFormat("5.4g");
  tdrStyle->SetFuncColor(2);
  tdrStyle->SetFuncStyle(1);
  tdrStyle->SetFuncWidth(1);

  //For the date:
  tdrStyle->SetOptDate(0);
  // tdrStyle->SetDateX(Float_t x = 0.01);
  // tdrStyle->SetDateY(Float_t y = 0.01);

  // For the statistics box:
  tdrStyle->SetOptFile(0);
  tdrStyle->SetOptStat(0); // To display the mean and RMS:   SetOptStat("mr");
  tdrStyle->SetStatColor(kWhite);
  tdrStyle->SetStatFont(42);
  tdrStyle->SetStatFontSize(0.025);
  tdrStyle->SetStatTextColor(1);
  tdrStyle->SetStatFormat("6.4g");
  tdrStyle->SetStatBorderSize(1);
  tdrStyle->SetStatH(0.1);
  tdrStyle->SetStatW(0.15);
  // tdrStyle->SetStatStyle(Style_t style = 1001);
  // tdrStyle->SetStatX(Float_t x = 0);
  // tdrStyle->SetStatY(Float_t y = 0);

  // Margins:
  tdrStyle->SetPadTopMargin(0.05);
  tdrStyle->SetPadBottomMargin(0.13);
  tdrStyle->SetPadLeftMargin(0.16);
  tdrStyle->SetPadRightMargin(0.05);

  // For the Global title:

  //  tdrStyle->SetOptTitle(0);
  tdrStyle->SetTitleFont(42);
  tdrStyle->SetTitleColor(1);
  tdrStyle->SetTitleTextColor(1);
  tdrStyle->SetTitleFillColor(10);
  tdrStyle->SetTitleFontSize(0.05);
  // tdrStyle->SetTitleH(0); // Set the height of the title box
  // tdrStyle->SetTitleW(0); // Set the width of the title box
  // tdrStyle->SetTitleX(0); // Set the position of the title box
  // tdrStyle->SetTitleY(0.985); // Set the position of the title box
  // tdrStyle->SetTitleStyle(Style_t style = 1001);
  // tdrStyle->SetTitleBorderSize(2);

  // For the axis titles:

  tdrStyle->SetTitleColor(1, "XYZ");
  tdrStyle->SetTitleFont(42, "XYZ");
  tdrStyle->SetTitleSize(0.06, "XYZ");
  // tdrStyle->SetTitleXSize(Float_t size = 0.02); // Another way to set the size?
  // tdrStyle->SetTitleYSize(Float_t size = 0.02);
  tdrStyle->SetTitleXOffset(0.9);
  tdrStyle->SetTitleYOffset(1.25);
  // tdrStyle->SetTitleOffset(1.1, "Y"); // Another way to set the Offset

  // For the axis labels:

  tdrStyle->SetLabelColor(1, "XYZ");
  tdrStyle->SetLabelFont(42, "XYZ");
  tdrStyle->SetLabelOffset(0.007, "XYZ");
  tdrStyle->SetLabelSize(0.04, "XYZ");

  // For the axis:

  tdrStyle->SetAxisColor(1, "XYZ");
  tdrStyle->SetStripDecimals(kTRUE);
  tdrStyle->SetTickLength(0.03, "XYZ");
  tdrStyle->SetNdivisions(510, "XYZ");
  tdrStyle->SetPadTickX(1);  // To get tick marks on the opposite side of the frame
  tdrStyle->SetPadTickY(1);

  // Change for log plots:
  tdrStyle->SetOptLogx(0);
  tdrStyle->SetOptLogy(0);
  tdrStyle->SetOptLogz(0);

  // Postscript options:
  // tdrStyle->SetPaperSize(15.,15.);
  // tdrStyle->SetLineScalePS(Float_t scale = 3);
  // tdrStyle->SetLineStyleString(Int_t i, const char* text);
  // tdrStyle->SetHeaderPS(const char* header);
  // tdrStyle->SetTitlePS(const char* pstitle);

  // tdrStyle->SetBarOffset(Float_t baroff = 0.5);
  // tdrStyle->SetBarWidth(Float_t barwidth = 0.5);
  // tdrStyle->SetPaintTextFormat(const char* format = "g");
  // tdrStyle->SetPalette(Int_t ncolors = 0, Int_t* colors = 0);
  // tdrStyle->SetTimeOffset(Double_t toffset);
  // tdrStyle->SetHistMinimumZero(kTRUE);

  tdrStyle->cd();
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


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
          if (iHisto->first == "noshape") continue ; 
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
                string destinationfolder, string prefix, string varname) 
{
  // create the root file containing the three histograms
  string rootfilename = destinationfolder + "/" + prefix + "_" + varname + ".root" ;
  TFile outf (rootfilename.c_str (), "recreate") ;
  h_SM->Write ("histo_sm") ;
  h_LI->Write ("histo_linear") ;
  h_QU->Write ("histo_quadratic") ;
  outf.Close () ;

  // create the root file containing the three histograms
  string txtfilename = destinationfolder + "/" + prefix + "_" + varname + ".txt" ;
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
  output_datacard << "bin\t\ttest\n" ;
  output_datacard << "observation\t" << h_SM->Integral () << endl ;
  output_datacard <<separator ;

  output_datacard << "bin\t\ttest\ttest\ttest\n";
  output_datacard << "process\t\tsm\tlinear\tquadratic\n" ;
  output_datacard << "process\t\t0\t1\t2\n" ;
  output_datacard << "rate\t\t" << h_SM->Integral () << "\t" 
                  << h_LI->Integral () << "\t"
                  << h_QU->Integral () << "\n" ;
  output_datacard <<separator ;
  output_datacard <<"lumi\t\tlnN\t1.02\t1.02\t1.02\n";
  output_datacard <<"bla\t\tlnN\t-\t-\t1.05\n";

  return 0 ;
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----


int 
plotHistos (TH1F * h_SM, TH1F * h_LI, TH1F * h_QU, 
            string destinationfolder, string prefix, string varname, 
            float wilson_gen, float wilson_plot, bool log) 
{
  setTDRStyle () ;

  TH1F * h_LI_loc = (TH1F *) h_LI->Clone (TString (h_LI->GetName ()) + "_loc") ;
  TH1F * h_QU_loc = (TH1F *) h_QU->Clone (TString (h_QU->GetName ()) + "_loc") ;

  h_LI_loc->Scale (wilson_plot / wilson_gen) ;
  h_QU_loc->Scale (wilson_plot * wilson_plot / (wilson_gen * wilson_gen)) ;

  TH1F * h_tot = (TH1F *) h_SM->Clone (TString (h_SM->GetName ()) + "_TOT") ;
  h_tot->Add (h_LI_loc) ;
  h_tot->Add (h_QU_loc) ;
  h_tot->SetLineColor (kOrange + 8) ;
  h_tot->SetFillColor (kOrange + 8) ;
  h_tot->SetTitle ("") ;
  h_tot->GetXaxis ()->SetTitle (varname.c_str ()) ;

  h_SM->SetLineColor (1) ;
  h_SM->SetFillColor (kYellow-9) ;

  TLegend legend (0.70, 0.80, 0.90, 0.90) ;
  legend.AddEntry (h_SM    , "SM"    , "f") ;
  legend.AddEntry (h_tot   , "total" , "f") ;
  legend.SetBorderSize (0) ;
  legend.SetFillStyle (0) ;

  TCanvas c1 ("c1", "", 600, 600) ;
  if (log) c1.SetLogy () ;
  h_tot->DrawCopy ("HIST") ;
  h_tot->SetFillColor (kWhite) ;
  h_tot->Draw ("HIST same") ;
  h_SM->Draw ("HIST same") ;
  c1.RedrawAxis () ;
  legend.Draw () ;

  // create the root file containing the three histograms
  string outfilename = destinationfolder + "/" + prefix + "_plots_" + varname ;
  if (log) outfilename += "_log" ;
  outfilename += ".gif" ;
  c1.Print (outfilename.c_str (), "gif") ;

  return 0 ;
}


