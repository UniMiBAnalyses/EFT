// utilities for the datacard creator

#ifndef dcutils_h
#define dcutils_h

#include "TH1F.h"
#include <vector>
#include <map>
#include <string>

#include "CfgParser.h"

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

int 
createDataCard (TH1F * h_SM, TH1F * h_LI, TH1F * h_QU, 
                std::string prefix, std::string varname) ;



#endif