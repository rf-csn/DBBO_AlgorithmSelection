#This file implement specific readers to load previously computed data into a list of problem

import Problem
import csv

#This 2 functions load data from Algorithms_Performances_compact.csv (W-Model data)
#data is stored on the form
#parameters1
#dim1
#targ1 Alg1, alg2,...  
#targ2 Alg1, alg2,...  
#...
#dim2
#targ1 Alg1, alg2,...  
#targ2 Alg1, alg2,...  
#...
#parameters2
#...
def read_block(reader_iterator,algorithms,filter_dim,filter_target):
  #get parameters of the problem
  params=next(reader_iterator)
  id1=-1
  id2=-1
  dim=[]
  tar=[]
  data=[[None for i in range(0,5-len(filter_target))] for j in range(0,5-len(filter_dim))]
  for i in range(0,5):
    temp_dim= next(reader_iterator)
    if temp_dim[0] not in filter_dim:
      dim+=temp_dim
      id1+=1
      id2=-1
    for j in range(0,5):
      row=next(reader_iterator)
      if row[0] not in filter_target and temp_dim not in filter_dim:
        id2+=1
        if i == 0:
          tar+=[row[0]]
        data[id1][id2]=row[1:]
  problem=Problem.Problem(algorithms,params,dim,tar, data)
  return problem

def load_data(filename,algorithms,filter_dim=[],filter_target=[]):
  data=[]
  with open(filename,'r', newline='') as f2:
    reader = csv.reader(f2,delimiter=',',quoting=csv.QUOTE_NONNUMERIC)
    it=iter(reader)
    try:
      while(1):
        problem=read_block(it,algorithms,filter_dim,filter_target)
        data.append(problem)
    except StopIteration:row=None
  #---------------
  # Sort features by dim, dummy, neutrality,epistasis, ruggedness
  #---------------
  data=sorted(data,key=lambda val: val.parameters[3])
  data=sorted(data,key=lambda val: val.parameters[2])
  data=sorted(data,key=lambda val: val.parameters[1])
  data=sorted(data,key=lambda val: val.parameters[0])
  return data

#----------------------------------------------------------------------------------
#this function load ELA_meta,y-distr and level set features
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
def read_feat_block(listfeat,ind,nbOfDim):
  row=listfeat[ind]
  params = [int(row[2]),int(row[3]),int(row[4]),int(row[5])]
  dim=[int(row[1])]
  data=[read_ELA_features(row)]
  for i in range(1,nbOfDim):
    row=listfeat[ind+i]
    if params!=[int(row[2]),int(row[3]),int(row[4]),int(row[5])]:
      print(params)
      print(ind)
      print((int(row[2]),int(row[3]),int(row[4]),int(row[5])))
    dim+=[int(row[1])]
    data+=[read_ELA_features(row)]
  return params,dim,data
def load_ELA_features(features_file):
  FEATURES=[]
  nbOfDim=5
  P=[]
  D=[]
  F=[]
  with open(features_file, newline='') as f:
    reader = csv.reader(f, delimiter=',', quoting=csv.QUOTE_NONE)
    rows=[i for i in reader]
    rows=rows[1:]
#    rows = sorted(rows[1:], key=lambda row:(row[2],row[3],row[4],row[5],row[1]), reverse=False)
    ind=0
    while(ind<len(rows)):
      p,d,f=read_feat_block(rows,ind,nbOfDim)
      ind+=nbOfDim
      P.append(p)
      D.append(d)
      F.append(f)

  return P,D,F
