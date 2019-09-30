This module implement a Generic enpirical performance prediction model 
(EPM).

It can be use to solve the Algorithm Selection/configuration problem/
the genericity allow also to solve Per instance Algorithm Selection

Given data about algorithms performances on a list of problem (or a list 
of problem instance), and given features (Landscape characterization 
of the problem) EPM learn to predict algorithms performances given 
features.


Files:
-Main.py : contain an example of how to use the EPM
-Load_Data.py : contains code to load data of algorithms performances 
and features
-Problem.py : contain a structure Problem to store data
-Selector.py : sorting methods to select which problems to learn from 
and which to use as a cross-validation procedure, can be extended
-Learning_method.py : regroup different learning method usable by the 
EPM. Can be extended
-epm : generic EPM, using selectors and learning methods from the 
previous files to learn and predict.
