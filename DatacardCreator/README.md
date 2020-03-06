# datacard\_creator
It reads .cfg file, file\_W.cfg is an example. From the .cfg it reads which .root file contains the SM, interference and BSM terms, cuts and also which variable to analyse.

It creates a datacard.txt for each variable specified and also a .root which will be passed to Combine.

It also creates different type of plots(stack, log and shape), putting the three distributions together.

# datacard\_twoOp\_creator
It is used to study two operators together creating, as datacard\_creator both datacard files and plots.

# range_vs_kineticvar
Reads a .txt containing the confidence interval's bounds found with Combine. It sorts the list of variables and plots the width of the C.I.
