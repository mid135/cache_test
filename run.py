from os import system
from time import time
from itertools import chain

def run(cmd):
       
    for n in [128]:#payload size 
        for l in chain(range(0,20,1),range(20,200,10),range(200,800,50),range(800,3000,200),range(3000,4000,500)):
            start = time()
            system(cmd.format(l, n))
            print n, l, "time = ", time() - start

run("./cache_test -l {} -n {} -r>> output/out_2.ods")

