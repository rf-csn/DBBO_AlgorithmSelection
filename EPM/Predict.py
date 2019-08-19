#This file perform a random forest regression to predict which algorithm is the best for an important list of problem
#R.C.
import csv
import numpy as np
from random import random
from sklearn.ensemble import RandomForestRegressor
from sklearn.ensemble import RandomForestClassifier
from sklearn.multioutput import MultiOutputRegressor
import argparse
import myplot
import math

__do__ =["performances-info","simple-prediction-test"][0]
 

parser = argparse.ArgumentParser()
parser.add_argument("-P","--path",dest="path",help="path to csv Data files",type=str,default="../DATA_PERFORMANCES/")
parser.add_argument("--Feature-path",dest="feat_path",help="path to csv Data files",type=str,default="../DATA_PERFORMANCES/feature.csv")
parser.add_argument("--numberOfFiles",dest="nbf",help="number of csv files",type=int,default=1)
parser.add_argument("--numberOfAlgo",dest="nbA",help="number of algorithm",type=int,default=9)
parser.add_argument("-f","--ProblemFeatures",dest="nbfeat",help="number of parameter of the W-Model (if you had an extension)",type=int,default=29)
parser.add_argument("-r","--numberOfRun",dest="nbR",help="number of run of each algorithm",type=int,default=20)
parser.add_argument("-l","--limit",dest="lim",help="limit the number of problem to the given value",type=int,default=float('inf'))
parser.add_argument("-t","--target",dest="target",help="target for which the model learn to predict algorithm performances",type=float,default=1.0)
parser.add_argument("-p","--probability",dest="proba",help="probability that a problem will be part of the testing set",type=float,default=0.85)
parser.add_argument("--offset_a",dest="offset_a",help="to allow small error from the testing part",type=float,default=0.05)
parser.add_argument("--offset_b",dest="offset_b",help="to allow small error from the testing part",type=float,default=5)

args = parser.parse_args()
#chemin du dossier contenant les donnees
csv_folder = args.path
features_file=args.feat_path
#nombre de fichier de donnees
nbfile=args.nbf
#nombre d'algo dans les donnees
nb_algo=args.nbA
#nombre de parametre des problemes 
WModel_features=args.nbfeat

#targets of data files
target=[0.5,0.75,0.90,0.95,1.0]
target_order="increasing"
#nombre d'execution de chaque algorithmes (dans les donnees)
nb_run=args.nbR
#nom des fichiers
#files=["DATA"+str(i)+".csv" for i in range(1,nbfile+1)]
files=[AgorithmsPerfomances.csv] #this file doesn't have infinity
infini=999999999
#nombre maximal de probleme a extraire
Max_number_of_problem=args.lim

#objectif : pourcentage de l'optimum a viser (doit appartenir a target)
Objectif_target=args.target

#probabilite d'un problem d'etre dans l'echantillon de test
probability=args.proba

#marge d'erreur autorisee
# if opt <= trouve*(1-offset_a)+offset_b
offset_a=args.offset_a
offset_b=args.offset_b

#value are "ALL" or "ONE". -> if the model should learn for a given problem
#to predict all algorithm performances or if it should only predict the best one
#prediction="One"
prediction="ALL"

alg_names=["(1+(lda_lda))","(1+1)EA","(1+10)EA1","(1+10)EA3","(1+10)FGA","(1+2)EA","1hill","2hill","RandomSearch"]


def filter_features(feat,filt):
  return [i for ind,i in enumerate(feat) if ind in filt]

class Problem:
  def __init__(self,param,algs,tar):
    self.param=param
    self.best=min(algs ,key=lambda x:x[1])
    self.algs=sorted(algs ,key=lambda x:x[0])
    self.tar=tar

  def link_features(self,feat):
#    self.features=feat
    self.features=filter_features(feat,[0,1,3,6,8,10])

  def __str__(self):
    res="\n--------------------------\nProblem : "+str(self.param)
    res+="\n"
    for i,j in self.algs:
      res+=str(i)+"  "+str(j)+"\n"
    res+="\nbest:  +"
    res+=str(self.best)
    res+="\n--------------------------\n"
    return res

 
#-------------------
#some data selection..
#-------------------
def load_data():
  line_per_problem=nb_algo*nb_run
  obj=target.index(Objectif_target)
  x=0
  DATA=[]
  for i,fi in enumerate(files):
    with open(csv_folder+fi, newline='') as f:
      reader = csv.reader(f, delimiter=',', quoting=csv.QUOTE_NONE)
      #skip first line csv DATA1
      #if i==0: row=reader.__next__()
      #for all algo while there is still some lines get the ert time at the specific target 
      row=reader.__next__()
      while (row!=None) :
        try:
          probl=(int(row[0]),int(row[1]),int(row[2]),int(row[3]),int(row[4]))
          tar=int(row[5])*Objectif_target
          x+=1
          if x>=Max_number_of_problem:break
          algs=[]
          for j in range(0,nb_algo):
            for targ in range(0,len(target)):
              if targ==obj:
#                alg=(row[6],math.log(float(row[29])))
                alg=(row[6],float(row[29]))
                row=reader.__next__()
              else:row=reader.__next__()
            algs.append(alg)
          pr=Problem(probl,algs,tar)
          DATA.append(pr)
        except StopIteration:row=None

  #---------------
  # Sort data by dim, dummy, neutrality,epistasis, ruggedness
  #---------------
  DATA=sorted(DATA,key=lambda val: val.param[4])
  DATA=sorted(DATA,key=lambda val: val.param[3])
  DATA=sorted(DATA,key=lambda val: val.param[2])
  DATA=sorted(DATA,key=lambda val: val.param[1])
  DATA=sorted(DATA,key=lambda val: val.param[0])
  return DATA


def read_ELA_features(row):
  meta_start=7
  meta_end=14
  distr_start=17
  distr_end=19
  level_start=22
  level_end=39
  dim=int(row[1])
  meta=[float(row[i]) if row[i]!= 'NA' else 0 for i in range(meta_start,meta_end+1)]
  distr=[float(row[i]) if row[i]!= 'NA' else 0 for i in range(distr_start,distr_end+1)]
  level=[float(row[i]) if row[i]!= 'NA' else 0 for i in range(level_start,level_end+1)]
  return meta+distr+level

def load_features(DATA):
  FEATURES=[]
  with open(features_file, newline='') as f:
    reader = csv.reader(f, delimiter=',', quoting=csv.QUOTE_NONE)
    #read column names
    names=reader.__next__()
    row=reader.__next__()
    while (row!=None) :
      try:
        probl=(int(row[1]),int(row[2]),int(row[3]),int(row[4]),int(row[5]))
        feats=read_ELA_features(row)
#        feats=filter_features(feats,[0,1,3,6,8,10])
        FEATURES.append((probl,feats))
        row=reader.__next__()
      except StopIteration:row=None
  #---------------
  # Sort features by dim, dummy, neutrality,epistasis, ruggedness
  #---------------
  FEATURES=sorted(FEATURES,key=lambda val: val[0][4])
  FEATURES=sorted(FEATURES,key=lambda val: val[0][3])
  FEATURES=sorted(FEATURES,key=lambda val: val[0][2])
  FEATURES=sorted(FEATURES,key=lambda val: val[0][1])
  FEATURES=sorted(FEATURES,key=lambda val: val[0][0])

  #---------------
  # Link features to problems
  #---------------
  for i,pr in enumerate(DATA):
    pr.link_features(FEATURES[i][1])
    

DATA=load_data()
load_features(DATA)

def SingleBestSolver(data,algs=alg_names):
  pr=[0 for i in alg_names]
  nb_pr=0
  for p in data:
    for i,_ in enumerate(algs):
      pr[i]+=p.algs[i][1]
    nb_pr+=1
  res=[(alg,pr[i]/nb_pr) for i,alg in enumerate(alg_names)]
  best,ert=min(res ,key=lambda x:x[1])
  return (best,int(ert))

def VirtualBestSolver(data,algs=alg_names):
  nb_pr=0
  ert=0
  for p in data:
    ert+=p.best[1]
    nb_pr+=1
  x=ert/nb_pr
  return ('VBS',int(x))

def average_ert_prediction(data):
  X,y,Xt,yt=get_data(prediction,data,probability)
  regr = RandomForestRegressor(max_depth=20, random_state=0, n_estimators=100)
  regr.fit(X, y)
  s=0
  nb=len(Xt)
  for i in range(0,len(Xt)):
    d=np.array(Xt[i])
    d.shape=(1,WModel_features)
    pred=regr.predict(d)
    id_pred=np.argmin(pred)
    res=str(Xt[i])
    s+=yt[i][id_pred]
  ms=s/nb
  return (int(ms),regr.feature_importances_)

def satisf_SBS_VBS(sbs,vbs,ms):
  a=ms-vbs
  b=sbs-vbs
  res=a/b
  return res

#--------------------------
#Prediction Model
#--------------------------

#just modify the data to get the good shape
def get_data(prediction,lstPr,proba):
  if prediction=="ALL":
    return get_training_and_testing_data_ALL(lstPr,proba)
  if prediction=="ONE":
    return get_training_and_testing_data_ONE(lstPr,proba)

def get_training_and_testing_data_ALL(lstPr,proba):
  lpr=[]
  lt =[]
  for i in range(0,len(lstPr)):
    if random()<proba:
      lt.append(lstPr[i])
    else:
      lpr.append(lstPr[i])

  X=np.array([i.features for i in lpr])
  X.shape=(len(lpr),WModel_features)
  y=np.array([j[1] for i in lpr for j in i.algs])
  y.shape=(len(lpr),nb_algo)
  Xt=np.array([i.features for i in lt])
  Xt.shape=(len(lt),WModel_features)
  yt=np.array([j[1] for i in lt for j in i.algs])
  yt.shape=(len(lt),nb_algo)
  return X,y,Xt,yt

def get_training_and_testing_data_ONE(lstPr,proba):
  lpr=[]
  lt =[]
  for i in range(0,len(lstPr)):
    if random()<proba:
      lt.append(lstPr[i])
    else:
      lpr.append(lstPr[i])

  X=np.array([i.features for i in lpr])
  X.shape=(len(lpr),WModel_features)
  m=[i.algs.index(min(i.algs ,key=lambda x:x[1])) for i in lpr ]
  y=np.array(m)
  Xt=np.array([i.features for i in lt])
  Xt.shape=(len(lt),WModel_features)
  m=[i.algs.index(min(i.algs ,key=lambda x:x[1])) for i in lt ]
  yt=np.array(m)
  return X,y,Xt,yt


#for all problems in Xt and their solution yt, test the prediction model
def test_model(prediction,regr, Xt, yt,typ):
  if prediction=="ALL":
    return test_model_ALL(regr, Xt, yt,typ)
  if prediction=="ONE":
    return test_model_ONE(regr, Xt, yt)

def test_model_ALL(regr, Xt, yt,typ):
  count=0
  count_monumental_fail=0
  sum_diff=0
  for i in range(0,len(Xt)):
    d=np.array(Xt[i])
    d.shape=(1,WModel_features)
    pred=regr.predict(d)
    ind=np.argmin(yt[i])
    id_pred=np.argmin(pred)
    res=str(Xt[i])
    diff=abs((yt[i][ind]-yt[i][id_pred])/yt[i][ind])
    if(yt[i][ind]<infini and yt[i][id_pred]== infini):
      count_monumental_fail+=1
      diff=float('inf')
    else:
      sum_diff+=diff
    if yt[i][ind]<(1-offset_a)*yt[i][id_pred]-offset_b:
      res+="  opt -> "+alg_names[ind]+" "+str(round(yt[i][ind],2))+"  || pred -> "+alg_names[id_pred]+" "+str(round(yt[i][id_pred],2))
      count+=1
    else:
      res+=" [ok]"
    if typ=='verbose':
      print(res+"  error :"+str(round(diff,2)))
  if typ=='verbose':
    print("Nombre d'erreur : "+str(count)+"/"+str(len(Xt))+" = "+str(round(count/len(Xt),2))+"\n")
    print("monumental fails : "+str(count_monumental_fail)+"\n")
    print("Average diff in ERT : "+str(round(sum_diff/(len(Xt)-count_monumental_fail),2))+"\n")
  return (count/len(Xt), count_monumental_fail, sum_diff/(len(Xt)-count_monumental_fail) )

def test_model_ONE(regr, Xt, yt):
  count=0
  for i in range(0,len(Xt)):
    d=np.array(Xt[i])
    d.shape=(1,WModel_features)
    pred=int(round(regr.predict(d)[0]))
    res=str(Xt[i])
    if yt[i]!=pred:
      res+="  opt -> "+alg_names[yt[i]]+"  || pred -> "+alg_names[pred]
      count+=1
    else:
      res+=" [ok]"
    #print(res)
  #print("Nombre d'erreur : "+str(count)+"/"+str(len(Xt))+"\n")
  return (count/len(Xt))

 

if __do__ =="simple-prediction-test":
  X,y,Xt,yt=get_data(prediction,DATA,probability)
  regr = RandomForestRegressor(max_depth=20, random_state=0, n_estimators=100)
  regr.fit(X, y)
  qual=test_model(prediction,regr, Xt, yt,"verbose")
  print(regr.feature_importances_)


if __do__ == "Performances-info":
 
  print("\nnumber of problem : ",end="")
  print(len(DATA))
  sbs=SingleBestSolver(DATA,algs=alg_names)
  vbs=VirtualBestSolver(DATA,algs=alg_names)
  ms,feat=average_ert_prediction(DATA)
  print("Feature of importance : ")
  print(feat)
  print("Single best Solver : ",end="")
  print(sbs)
  print("Virtual best Solver : ",end="")
  print(vbs)
  print("Real solver ERT : ",end="")
  print(ms)
  print("Satisfaisability : ",end="")
  print(satisf_SBS_VBS(sbs[1],vbs[1],ms))








