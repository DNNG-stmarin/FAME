#!/usr/bin/env python
import ROOT as r
import numpy as np

def wf_integrate(int_lims,baseline,wf):
    num = 0.
    denom = 0.
    for i in range(baseline):
        num += wf[i]
        denom += 1.
    baseline_val = num/denom
    peakpoint = np.argmin(wf)
    intval = 0
    for i in range(peakpoint + int_lims[0],peakpoint + int_lims[1]):
        intval += wf[i]-baseline
    return intval

def generate_singles_energy_tree(ifname,ofname,int_lims,baseline):
    # set up input file
    ifile = r.TFile(ifname)
    itree = ifile.Get("t")
    chnum = np.zeros(1,dtype=int)
    wf = np.zeros(1000,dtype=int)
    itree.SetBranchAddress("chnum",chnum)
    itree.SetBranchAddress("wf",wf)
    nentries = itree.GetEntries()
    # set up output file
    ofile = r.TFile(ofname)
    otree = ifile.Get("t")
    for i in range(nentries):
        itree.GetEntry(i)
        wfproc = wf[:900]
        if chnum < 2:
            intval = wf_integrate(int_lims[chnum],baseline,wf)

    

if __name__=='__main__':
    ifname = '../processed_data/run120.root'
    ofname = '../simple_trees/run120.root'
    int_lims = [[-14,90],[16,700]]
    baseline = 50
    generate_singles_energy_tree(ifname,ofname,int_lims,baseline)
