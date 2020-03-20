# Fits with Combine

Instructions on how to install and use combine for likelihood scan.
  * The reference for the building of anomalous coupling analytic fitting 
    are [here](https://github.com/amassiro/AnalyticAnomalousCoupling).
  * A Combine tutorial can be found [here](https://indico.cern.ch/event/859454/)

    
## Step by step Instructions. 

You need to be either on MiB cluster/virgilio 
(read the message after logging in on what to source to be able to use cmssw) 
or on lxplus.

  * The Combine repository and some instruction are [here](https://github.com/cms-analysis/HiggsAnalysis-CombinedLimit/tree/master)
  * The main Combine documentation is [here](https://cms-analysis.github.io/HiggsAnalysis-CombinedLimit/)

### A) install Combine

Prepare a CMSSW release where to install combine, 
which sets the proper compiling environment with a consistent set of libraries
to be linked in the compilation process
(the command ```cmsenv``` sets all the shell environment variables needed for compiling and running
once and for all):

    cmsrel CMSSW_10_2_13
    cd CMSSW_10_2_13/src
    cmsenv

Download the Combine tool inside the CMSSW release environment and enter the subfolder:

    git clone https://github.com/cms-analysis/HiggsAnalysis-CombinedLimit.git HiggsAnalysis/CombinedLimit

Go to the Combine subfolder, 
and get the right stable version of Combine
compatible with our setup:
    
    cd $CMSSW_BASE/src/HiggsAnalysis/CombinedLimit
    git fetch origin
    git checkout v8.0.1

Compile combine with the ```scramv1``` compiling tool of the CMS software

    scramv1 b clean; scramv1 b


### B) install the fitting model

Download our fitting model and compile it

    cd $CMSSW_BASE/src/HiggsAnalysis
    git clone git@github.com:amassiro/AnalyticAnomalousCoupling.git
    scramv1 b -j 20
    
## Fit and get results:

### A) text2workspace

                                                             folder                    file.py             object defined in the file.py
    text2workspace.py        datacard.txt     -P HiggsAnalysis.AnalyticAnomalousCoupling.AnomalousCoupling:analiticAnomalousCoupling                 --PO=k_my,r  -o      model_test.root   

### B) fit

Simulate with k_my set to 1 and r=1

    combine -M MultiDimFit model_test.root  --algo=grid --points 120  -m 125   -t -1 --expectSignal=1     \
        --redefineSignalPOIs k_my --freezeParameters r --setParameters r=1    --setParameterRanges k_my=-20,20     \
        --verbose -1

To simulate "sm" part, k_my == 0 (this is how we simulate the expected result with "sm") :
        
    combine -M MultiDimFit model_test.root  --algo=grid --points 120  -m 125   -t -1 --expectSignal=1     \
        --redefineSignalPOIs k_my --freezeParameters r --setParameters r=1,k_my=0   --setParameterRanges k_my=-20,20   
    

### C) plot:

    cd $CMSSW_BASE/src/HiggsAnalysis/AnalyticAnomalousCoupling/test/

    root -l     higgsCombineTest.MultiDimFit.mH125.root  higgsCombineTest.MultiDimFit.mH125.root   draw.cxx

