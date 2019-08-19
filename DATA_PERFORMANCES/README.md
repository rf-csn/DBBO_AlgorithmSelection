This folder contains empirical data of 9 classical meta-heuristic over 
13230 configurations of the W-Model and features for all configurations
Experiments were made over 20 runs for each couple (algorithm, 
configuration) with a limited budget of 1000* dimension.

Algorithms performances:

Data contains algorithm mean cost to reach a specific fitness target 
(0.5,0.75,0.9,0.95,1.0). The cost is in number of function evaluation.

The W-Model differs from the original. It contains 4 layers :
-Random dummy Variables, with parameter k
-Neutrality, with parameter mu
-Epistasis, with parameter nu
-Ruggedness, with parameter gamma

Each layers parameter is a percentage of the dimension.

The file contains for all lines
Dimension, k, mu, nu, gamma, Maximal fitness, Algorithm name, Target, 
run.1, ... , run.20, number of succesful runs, mean cost.

Features :
the file contains for all lines

filename (unused) ,Dimension, k, mu, nu, gamma, sampleSize ,first set of features, cost for computing the first set, second set of features,  ...