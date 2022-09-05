#!/usr/bin/env python
import os
import subprocess

def unpackfiles(ipath,opath,exepath,runlolim):
    for fname in os.listdir(ipath):
        if fname.endswith('.mid.gz'):
            fnumber = int(fname[3:8])
            ofname = 'run%i.root'%(fnumber)
            if (os.path.isfile(opath+ofname) or fnumber < runlolim):
                continue
            else:
                print 'processing file',fnumber
                cmdstr = 'source /home/smosby/.bashrc && %s %s/%s %s/%s'%(exepath,ipath,fname,opath,ofname)
                print 'about to call',cmdstr
                subprocess.call(cmdstr,shell=True)
                print 'done'


if __name__=='__main__':
    runlolim = 1518
    ipath = '/home/smosby/tke_analysis/tke_midasfiles/'
    opath = '/home/smosby/tke_analysis/tke_rootfiles/'
    exepath = '/home/smosby/tke_analysis/analyzer'
    unpackfiles(ipath,opath,exepath,runlolim)
