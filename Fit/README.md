# Fits with combine

Instructions on how to installa and use combine for likelihood scan.

Use this as reference:

    https://github.com/amassiro/AnalyticAnomalousCoupling
    
Step by step Instructions. You need to be either on MiB cluster/virgilio (read the message after logging in on what to source to be able to use cmssw) or on lxplus.


Combine tutorial:

    https://indico.cern.ch/event/859454/

Install "combine":

    https://github.com/cms-analysis/HiggsAnalysis-CombinedLimit/tree/master

    https://cms-analysis.github.io/HiggsAnalysis-CombinedLimit/



1) install Combine

    cmsrel CMSSW_10_2_13
    cd CMSSW_10_2_13/src
    cmsenv
    git clone https://github.com/cms-analysis/HiggsAnalysis-CombinedLimit.git HiggsAnalysis/CombinedLimit
    cd HiggsAnalysis/
    
    cd $CMSSW_BASE/src/HiggsAnalysis/CombinedLimit
    git fetch origin
    git checkout v8.0.1
    scramv1 b clean; scramv1 b
    
    git clone git@github.com:amassiro/AnalyticAnomalousCoupling.git
    
    
NB:  Move between a folder and the other. The last one where you run

    cmsenv
    
is the one that is used.


2) install model

    cd $CMSSW_BASE/src/HiggsAnalysis
    
    git clone git@github.com:amassiro/AnalyticAnomalousCoupling.git

3) compile

    scramv1 b -j 20
    

    
Now fit and get results:

1) text2workspace

                                                             folder                    file.py             object defined in the file.py
    text2workspace.py        datacard.txt     -P HiggsAnalysis.AnalyticAnomalousCoupling.AnomalousCoupling:analiticAnomalousCoupling                 --PO=k_my,r  -o      model_test.root   

2) fit

Simulate with k_my set to 1 and r=1

    combine -M MultiDimFit model_test.root  --algo=grid --points 120  -m 125   -t -1 --expectSignal=1     \
        --redefineSignalPOIs k_my --freezeParameters r --setParameters r=1    --setParameterRanges k_my=-20,20     \
        --verbose -1

To simulate "sm" part, k_my == 0 (this is how we simulate the expected result with "sm") :
        
    combine -M MultiDimFit model_test.root  --algo=grid --points 120  -m 125   -t -1 --expectSignal=1     \
        --redefineSignalPOIs k_my --freezeParameters r --setParameters r=1,k_my=0   --setParameterRanges k_my=-20,20   
    

Plot:

    cd $CMSSW_BASE/src/HiggsAnalysis/AnalyticAnomalousCoupling/test/

    root -l     higgsCombineTest.MultiDimFit.mH125.root  higgsCombineTest.MultiDimFit.mH125.root   draw.cxx


    
    