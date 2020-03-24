// utilities for the datacard creator

#ifndef dcutils_h
#define dcutils_h

#include "TH1F.h"
#include "TStyle.h"
#include "TGraph.h"

#include <vector>
#include <map>
#include <string>

#include "CfgParser.h"

const std::vector<std::string> gAllCoeff 
  {
    "cG",
    "cGtil",
    "cH",
    "cHB",
    "cHBtil",
    "cHDD",
    "cHG",
    "cHGtil",
    "cHW",
    "cHWB",
    "cHWBtil",
    "cHWtil",
    "cHbox",
    "cHd",
    "cHe",
    "cHl1",
    "cHl3",
    "cHq1",
    "cHq3",
    "cHu",
    "cHudAbs",
    "cHudPh",
    "cW",
    "cWtil",
    "cdBAbs",
    "cdBPh",
    "cdGAbs",
    "cdGPh",
    "cdHAbs",
    "cdHPh",
    "cdWAbs",
    "cdWPh",
    "cdd",
    "cdd1",
    "ceBAbs",
    "ceBPh",
    "ceHAbs",
    "ceHPh",
    "ceWAbs",
    "ceWPh",
    "ced",
    "cee",
    "ceu",
    "cld",
    "cle",
    "cledqAbs",
    "cledqPh",
    "clequ1Abs",
    "clequ1Ph",
    "clequ3Abs",
    "clequ3Ph",
    "cll",
    "cll1",
    "clq1",
    "clq3",
    "clu",
    "cqd1",
    "cqd8",
    "cqe",
    "cqq1",
    "cqq11",
    "cqq3",
    "cqq31",
    "cqu1",
    "cqu8",
    "cquqd1Abs",
    "cquqd1Ph",
    "cquqd8Abs",
    "cquqd8Ph",
    "cuBAbs",
    "cuBPh",
    "cuGAbs",
    "cuGPh",
    "cuHAbs",
    "cuHPh",
    "cuWAbs",
    "cuWPh",
    "cud1",
    "cud8",
    "cuu",
    "cuu1"
  } ;

const std::map<std::string, std::string> gvarNames 
  {
    std::pair<std::string, std::string> ("met"       , "MET"            ),
    std::pair<std::string, std::string> ("mjj"       , "m_{jj}"         ),
    std::pair<std::string, std::string> ("mll"       , "m_{#ell #ell}"  ),
    std::pair<std::string, std::string> ("ptl1"      , "p_{T}^{#ell_{1}}"   ),
    std::pair<std::string, std::string> ("ptl2"      , "p_{T}^{#ell_{2}}"   ),
    std::pair<std::string, std::string> ("ptll"      , "p_{T}^{#ell#ell}" ),
    std::pair<std::string, std::string> ("ptj1"      , "p_{T}^{j_{1}}"      ),
    std::pair<std::string, std::string> ("ptj2"      , "p_{T}^{j_{2}}"      ),
    std::pair<std::string, std::string> ("deltaetajj", "#Delta#eta_{jj}"),
    std::pair<std::string, std::string> ("deltaphijj", "#Delta#phi_{jj}"),
    std::pair<std::string, std::string> ("noshape"   , "XS"             )
  } ;


void setTDRStyle () ;

bool 
replace (std::string& str, const std::string& from, const std::string& to) ;

std::vector<float> 
calcBinEdges (std::string varname, 
              float min, float max, int nbins) ;

std::map<std::string, TH1F *> 
readNtupleFile (
    std::string rootFileName, 
    std::string ntupleName,
    std::string hTag, 
    std::string hTitle,
    CfgParser * gConfigParser) ;

void 
scaleAllHistos (std::map<std::string, TH1F *> & hMap, float value) ;

void 
checkEmptyBins (std::map<std::string, TH1F *> & hMap) ;

std::pair <std::string, std::string>
prepareFreeze (std::vector<std::string> activeCoeff) ;

std::string
merge (std::vector<std::string> list, const std::string & joint) ;

std::pair <std::string, std::string>
createDataCard (TH1F * h_SM, TH1F * h_LI, TH1F * h_QU, 
                std::string destinationfolder, std::string prefix, std::string varname,
                CfgParser * gConfigParser) ;

int 
plotHistos (TH1F * h_SM, TH1F * h_LI, TH1F * h_QU, 
            std::string destinationfolder, std::string prefix, std::string varname, 
            float wilson_gen, float wilson_plot = 1., bool log = false) ;

// set the likelihood scan aestetics
void setLSaspect (TGraph * graphScan, std::string variable) ;

std::pair<float, float> 
getXrange (TGraph * graphScan) ;

// find the minimum of a likelihood scan
float 
getLSminimum (TGraph * graphScan) ;

// get the list of crossings between a horizontal line and a tgraph
// points should be dense enough
std::vector <float>
getRawIntersections (TGraph * graphScan, float val, float resol = 0.001) ;

// get the list of crossings between a horizontal line and a tgraph
// points should be dense enough
std::vector <float>
getLSintersections (TGraph * graphScan, float val, float resol = 0.001) ;

bool 
sortBySensitivity (const std::pair<std::string, std::vector<float> > & a ,
                   const std::pair<std::string, std::vector<float> > & b) ;

TH1F * 
setDummyHisto (float xmin, float ymin, float xmax, float ymax, 
               std::vector<std::string> labels) ;

void 
drawSensitivities (std::string op, 
                   std::vector <std::pair<std::string, std::vector<float> > > limits, 
                   std::string basefilename) ;


#endif