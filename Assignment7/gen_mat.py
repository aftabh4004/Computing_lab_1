#!/usr/bin/python3

import argparse
import numpy as np
import random
import sys

def gen_mat(auto=False):
    if not auto:
        print(":: Generates input.txt and output.txt, be sure you don't want to lose data!!!")
        N=int(input("Enter the Length of Matrix Array[10]: "))
        X,Y=[int (x) for x in input("Enter The Range of Sizes of Matrix Array[4 10]: ").split(' ')] 
        rval=int(input("Enter Value Range[5]: "))
    else:
        N=200
        X,Y=(30,100)
        rval=5
    cols=[random.randint(X,Y) for _ in range(N+1)]
    file=open("input.txt","w")
    index=0
    file.write("{}\n".format(N))
    prev=None;cur=None;ok=False
    if not auto:
        print(":: Matrix Dimensions: ",cols)
    while(index+1<len(cols)):
        file.write("{} {}\n".format(cols[index],cols[index+1]))
        cur=[]
        for i in range(cols[index]):
            cur.append([])
            for _ in range(cols[index+1]-1):
                val=random.randint(-rval,rval)
                file.write("{} ".format(val))
                cur[i].append(val)
            val=random.randint(-rval,rval)
            cur[i].append(val)
            file.write("{}\n".format(val))
        cur=np.array(cur)
        if ok==False:
            prev=cur
            ok=True
        else:
            prev=np.matmul(prev,cur)
        index+=1
    out=open("output.txt","w")
    out.write("{} {}\n".format(*prev.shape))
    arr=prev.tolist()
    for row in arr:
        for ele in row:
            out.write("{} ".format(ele," "))
        out.write("\n")
    print(":: Matrix Generation Complete")

def verify(file1,file2):
    import re
    res=open(file1,"r")
    out=open(file2,"r")
    print(":: Reading {}, {}".format(file1,file2))

    res=list(filter(None,re.split(' |\n',res.read())))
    out=list(filter(None,re.split(' |\n',out.read())))
    if len(res)!=len(out):
        print(":: Sizes Differ")
        print(":: Done")
        return;
    for val1,val2 in zip(res,out):
        if(val1!=val2):
            print(":: Mismatch: ",val1,val2)

    print(":: Done")
    sys.exit(1)

if __name__=="__main__":
    parser=argparse.ArgumentParser(description="A Python Program to Generate and Verify Matrices")
    sub=parser.add_subparsers(title="Sub Commands")

    gen=sub.add_parser("gen", help="Generates Matrices that uses matrix length, range of sizes and values...")
    gen.add_argument("-a", "--auto",dest="auto",action="store_true",help="Auto Generate Matrices")
    gen.set_defaults(func=lambda args: gen_mat(auto=args.auto))

    ver=sub.add_parser("verify", help="Sub Command For Verification")
    ver.add_argument(dest='file',default="2",help='Verifies Two Files', nargs=2)
    ver.set_defaults(func=lambda args: verify(*args.file))

    args=parser.parse_args()
    args.func(args)