#This file allow to perform a learning task from problems loaded in
#Load_Data, using the Problem class with the epm model.

import Problem
import Load_Data
import epm
import Selector
ALGORITHMS = ["(1+(lda_lda))","(1+1)EA","(1+10)EA1","(1+10)EA3","(1+10)FGA","(1+2)EA","1hill","2hill","RandomSearch"]
filename="../DATA_PERFORMANCES/Algorithms_Performances_compact.csv"
features_file="../DATA_PERFORMANCES/features_ordered.csv"
#number_of_features = 4 parameters + 1 dimension + 1 target
number_of_features=6
filter_target=[0.5,0.75,0.90,0.95]


def Main(filename,features_file,Algorithms,number_of_features,filter_target):
  print("\n---------------------------------\n")
  print("Load Data",end="")
  DATA=Load_Data.load_data(filename,ALGORITHMS,filter_target=filter_target)
  print("  [OK]\nLoad ELA Features",end="")
  P,D,F=Load_Data.load_ELA_features(features_file)
  print("  [OK]\nLink ELA Features to problems",end="")
  Problem.link_all_features(DATA,P,D,F)
  print("  [OK]\nInitialize Empirical Performance Model",end="")
  model=epm.EmpiricalPerformanceModel(6, len(ALGORITHMS),input_type="parameters",selector=Selector.Random_selector(probability=0.7))
  print("  [OK]\nFix Training and Testing sets",end="")
  model.build_training_and_testing_sets(DATA)
  print("  [OK]\nTrain EPM",end="")
  model.train_model()
  print("  [OK]\nTest EPM",end="")

  model.test_model()
  print("  [OK]\n")

  for i in model.get_results():
    print(i)
  print("\nNumber of problems : "+str(len(model.get_results()))+"\n")

Main(filename,features_file,ALGORITHMS,number_of_features,filter_target)