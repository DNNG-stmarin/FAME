#!/usr/bin/env python
import ROOT as r

def find_rough_coinc(ifname,coincgate=150,large_offset=15367.):
    ifile = r.TFile(ifname)
    itree = ifile.Get("pt")
    allevtlists = []
    # create the event lists
    for i in range(itree.GetEntries()):
	itree.GetEntry(i)
	ts = itree.GetLeaf("ts").GetValue()
	ch = itree.GetLeaf("bnum").GetValue()
	allevtlists.append([ts,ch])
    # find the rough coincs
    coincidences = []
    evtlist = allevtlists
    evtlist.sort()
    coinc_events = []
    temp_evts = [evtlist[0]]
    n_coinc = 0
    # build our event structures
    for event in evtlist[1:]:
        tdiff = event[0]-temp_evts[0][0]-large_offset
        print tdiff
        if tdiff<coincgate:
            temp_evts.append(event)
            n_coinc += 1
        else:
            coinc_events.append(temp_evts)
            temp_evts = [event]
    coinc_events.append(temp_evts)
    # cut out all but the mult > 1
    tempcoincidences = []
    for coinclist in coinc_events:
        if len(coinclist)>1:
            tempcoincidences.append(coinclist)
    coincidences.append(tempcoincidences)
    return tempcoincidences

def make_coinc_spectra(ofname,coinclist,large_offset):
    ofile = r.TFile(ofname,'recreate')
    hist_tdiff = r.TH1D("tdiff","tdiff",20,-10,10)
    hist_bnums = r.TH2D("bnums","bnums",5,0,5,5,0,5)
    for coinc in coinclist:
        tdiff = coinc[0][0]-coinc[1][0]+large_offset
        hist_tdiff.Fill(tdiff)
        hist_bnums.Fill(coinc[0][1],coinc[1][1])
    hist_tdiff.Write()
    hist_bnums.Write()
    ofile.Close()

if __name__=='__main__':
    ifname = 'processed_data/run92_wfproc.root'
    coincgate = 100.
    #large_offset = 15367.
    large_offset = 15367.-6428
    coincidences = find_rough_coinc(ifname,coincgate,large_offset)
    print "i found",len(coincidences),"coincidences"

    ofname = './hists/run92_coinc.root'
    make_coinc_spectra(ofname,coincidences,large_offset)
