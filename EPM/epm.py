#This file contains the empirical performance prediction model.
#Given a list of problem (class Problem), it randomly select (given a tunable probability) which are training instance and which are test instance
#Then it perform the training/testing and return the result


import numpy as np
import Selector
import Learning_method

#		learning_method=RandomForestRegressor(max_depth=2, random_state=0, n_estimators=100),

class EmpiricalPerformanceModel:
  def __init__(self, numberOfFeatures, numberOfAlgorithms, input_type="features",learning_method=Learning_method.classical_forestRegression(),selector=Selector.Random_selector(probability=0.2)):
    if input_type not in ["parameters","features"]:
      raise Exception('EmpiricalPerformanceModel, bad input_type', str(input_type))
    self.numberOfFeatures=numberOfFeatures
    self.numberOfAlgorithms=numberOfAlgorithms
    self.input_type=input_type
    self.selector=selector
    self.learning_method=learning_method

  def reset_model(learning_method=Learning_method.classical_forestRegression()):
    self.learning_method=learning_method

  #this function use the selector to split data into two sets
  #the first one is the training set, used to train the model
  #the second is the testing set, used to test the model
  def build_training_and_testing_sets(self,data):
    Training_set=[]
    Testing_set=[]
    for item in data:
      if self.selector.is_in_training_set(item):
        Training_set.append(item)
      else:
        Testing_set.append(item)
    self.training_set=Training_set
    self.testing_set=Testing_set

  #train the model to predict.
  #use all training_set instance to learn from them
  def train_model(self):
    X=[]
    Y=[]
    for problem in self.training_set:
      a,b=self.learning_method.__transform_item__(self.input_type,problem)
      X+=a
      Y+=b
    self.learning_method.train(X,Y,self.numberOfFeatures,self.numberOfAlgorithms)

  #this function take a problem and for all configuration it computes predictions from "features"
  def predict(self, features):
   result=self.learning_method.predict(features,self.numberOfFeatures)
   return result
  #this function use all testing instance to test the EPM and keep the result in the problem class
  def test_model(self):
    for problem in self.testing_set:
      a,b=self.learning_method.__transform_item__(self.input_type,problem)
      problem.predictions=self.learning_method.test(a,self.numberOfFeatures,len(problem.dimensions),len(problem.targets))
  def get_results(self):
    return self.testing_set

  def set_input_type(self,input_type):
    if input_type not in ["parameters","features"]:
      raise Exception('EmpiricalPerformanceModel, bad input_type', str(input_type))
    self.input_type=input_type
