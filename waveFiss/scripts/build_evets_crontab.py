#!/usr/bin/env python
import os
import subprocess
import time

def process_singles(ipath,opath,exepath,runlolim):
    for fname in os.listdir(ipath):
        if fname.endswith('.root'):
            if fname.find('_sp')!=-1:
                pass
            else:
                continue
            # look at modification times
            modtime = os.path.getmtime(ipath+'/'+fname)
            nowtime = time.time()
            if (nowtime - modtime)<60:
                print 'time since mod too short - skip'
                continue
            loindex = fname.find('n')+1
            hiindex = fname.find('_')
            fnumber = int(fname[loindex:hiindex])
            ofname = 'run%i_evt.root'%(fnumber)
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
    ipath = '/home/smosby/tke_analysis/tke_rootfiles/'
    opath = '/home/smosby/tke_analysis/tke_rootfiles/'
    exepath = '/home/smosby/tke_analysis/eventbuild'
    process_singles(ipath,opath,exepath,runlolim)
