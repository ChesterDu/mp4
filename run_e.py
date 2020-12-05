from ctypes import *
import tqdm
import torch
import argparse
# from struct import *

class results_t(Structure):
	_fields_ = [('util',c_float),('idle',c_float),('collision',c_float)]

csma = cdll.LoadLibrary("./libcsma.so")
csma.csma_api.restype = (results_t)

parser = argparse.ArgumentParser()
parser.add_argument("--L",type=int,default=20)
args = parser.parse_args()

L = args.L
N_list = range(5,501,1)
R = 8
M = 6
T = 50000

utilization_list = []
idle_list = []
collision_list = []
for N in tqdm.tqdm(N_list):
    retval = csma.csma_api(N,L,R,M,T)
    utilization_list.append(retval.util)
    idle_list.append(retval.idle)
    collision_list.append(retval.collision)

torch.save({"utilization":utilization_list, "idle":idle_list, "collision":collision_list}, "e_results_{}.pkl".format(L))
