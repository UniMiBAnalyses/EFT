Datacard creator and Likelihood scans
====

Datacard creator for likelihood scan

    1) no background, no cuts, likelihood scan
    2) no background, reasonable cuts, likelihood scan
    3) background, reasonable cuts, likelihood scan. Falling spectrum of background assumed and SM/background = 1

Programs:

- createFilesSelNew1.cpp: creates 13 root files for every kinetic variable contained in the HS ntuple "ntuple_RcW_0p3_HS_2.root"(link: https://cernbox.cern.ch/index.php/s/SIbEj8wsnpRu49S)), with preselections and variable width binning. Each root file is created with three histograms inside: histo_sm, histo_linear, histo_quadratic. The values of the integrals (to be inserted in datacard.txt to plot the likelihood scans) are print on terminal. The EFT 6th-order operator tested is Q_W.  

- createFilesSelNew2.cpp: same as createFilesSelNew1.cpp, but the EFT 6th-order operator tested is Q_HW (distributions are created with the HS ntuple "ntuple_RcHW_0p3_2.root" (link: https://cernbox.cern.ch/index.php/s/VKIkgTBeX2UfRtv)). 
