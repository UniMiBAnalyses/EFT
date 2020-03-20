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
once and for all).
Since the CMSSW and Combine code is taken from a Git repository, it's suggested to install it
*outside* of the EFT folder.

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

### A) generate a RooFit workspace starting from the datacards

The workspace is the RooFit container that holds all the information needed by Combine di perform fits.
THe following script creates the workspace starting from the txt and root files 
produced by datacard_creator and datacard_creator_2
of EFT/DatacardCreator.

    text2workspace.py datacard.txt \
                      -P HiggsAnalysis.AnalyticAnomalousCoupling.AnomalousCoupling:analiticAnomalousCoupling \
                      --PO=k_my,r \
                      -o  model_test.root   
where:

| option                                        | meaning                                                                    |
| --------------------------------------------- | -------------------------------------------------------------------------- |
| ```-P PHYSMODEL```                            | Physics model to use. It should be in the form (module name):(object name) |
| ```HiggsAnalysis.AnalyticAnomalousCoupling``` | module name of the physics model, it's a folder                            |
| ```analiticAnomalousCoupling```               | object name of the physics model                                           |
| ```--PO = k_my,r```                           | define the physics observables to be ```k_my``` and ```r```                |
| ```-o model_test.root```                      | filename of the workspace created                                          |

The module ```HiggsAnalysis.AnalyticAnomalousCoupling``` is actually implemented in 
```HiggsAnalysis/AnalyticAnomalousCoupling/python/AnomalousCoupling.py```,
```HiggsAnalysis/AnalyticAnomalousCoupling/python/AnomalousCouplingTwoOp.py``` and
```HiggsAnalysis/AnalyticAnomalousCoupling/python/AnomalousCouplingOneOp.py```.
**NB** the root file is searched for in the subfolder indicated in the txt file, 
       by the ```text2workspace.py``` script.
The workspace created can be printed for inspection:

    root -b model_test.root
    w->Print ()

When using datacard_creator_2, a shell script is also created,
that contains the commands to be issued to actually create the workspaces.

### B) fit

Simulate with k_my set to 1 and r=1

    combine -M MultiDimFit model_test.root             \
            --algo=grid --points 120  -m 125           \
            -t -1 --expectSignal=1                     \
            --redefineSignalPOIs k_my                  \
            --freezeParameters r --setParameters r=1   \ 
            --setParameterRanges k_my=-20,20           \
            --verbose -1

Where:

| option                                 | meaning                                                                                          |
| -------------------------------------- | ------------------------------------------------------------------------------------------------ |
| ```-M MultiDimFit```                   | use the method ```MultiDimFit``` to extract upper limits                                         |
| ```--algo=grid```                      | choose the algorithm ```grid``` to compute uncertainties                                         |
| ```--points 120```                     | set to 120 the number of points in *DOING SOMETHING*                                             |
| ```-m 125```                           | set the Higgs boson mass to 125 GeV                                                              |
| ```-t -1```                            | number of Toy MC extractions                                                                     |
| ```--expectSignal=1```                 | generate *signal* toys instead of background ones, with the specified signal strength.           |
| ```--redefineSignalPOIs k_my```        | set ```k_my``` as the only physics observable in the fit                                         |
| ```--freezeParameters r```             | the parameter ```r``` has a value which will not change in the fit                               |
| ```--setParameters r=1```              | the value to which ```r``` is frozen                                                             |
| ```--setParameterRanges k_my=-20,20``` | range of variability of the free parameter considered in the fit                                 |
| ```--verbose -1```                     | verbosity set to minimum                                                                         |

To simulate "sm" part, k_my == 0 (this is how we simulate the expected result with "sm") :
        
    combine -M MultiDimFit model_test.root                  \           
            --algo=grid --points 120  -m 125                \              
            -t -1 --expectSignal=1                          \      
            --redefineSignalPOIs k_my                       \     
            --freezeParameters r --setParameters r=1,k_my=0 \                             
            --setParameterRanges k_my=-20,20
    
### C) plot the profile likelihood obtained

    cd $CMSSW_BASE/src/HiggsAnalysis/AnalyticAnomalousCoupling/test/
    root -l higgsCombineTest.MultiDimFit.mH125.root  \
            higgsCombineTest.MultiDimFit.mH125.root draw.cxx

