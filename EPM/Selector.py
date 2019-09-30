#This file implement functions to split data into training and testing instances for the empirical performance model
import random as rd

# Random selector choisit les problemes qui appartiennent selon un probabilité p
class Random_selector:
  def __init__(self, probability):
    self.probability=probability

  def is_in_training_set(self,item):
    return rd.random()<self.probability

#Full_selector utilise tout les problemes pour l'apprentissage
class Full_selector:
  def is_in_training_set(self,item):
    return 1

