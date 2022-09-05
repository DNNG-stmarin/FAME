#!/usr/bin/env python
import ROOT as r

def process_file(ifname,ofname,ofoption,lnum,ofhname):
    count = 0
    for line in open(ifname):
        if count == lnum:
            temp = line.split()
            numbers = []
            for i in temp:
                numbers.append(int(i))
            nbins = len(numbers)
            hist = r.TH1D(ofhname,ofhname,nbins,0,nbins)
            for i in range(nbins):
                hist.SetBinContent(i+1,numbers[i])
            ofile = r.TFile(ofname,ofoption)
            hist.Write()
            ofile.Close()
            break
        else:
            count += 1
            continue

if __name__=='__main__':
    ifname = 'tempwf2.txt'
    ofname = 'tempwf.root'
    ofoption = 'recreate'
    lnum = 0
    ofhname = 'tempwf2hist'
    process_file(ifname,ofname,ofoption,lnum,ofhname)
    ifname = 'tempwf.txt'
    ofname = 'tempwf.root'
    ofoption = 'update'
    lnum = 0
    ofhname = 'tempwfhist'
    process_file(ifname,ofname,ofoption,lnum,ofhname)
