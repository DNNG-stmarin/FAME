#!/usr/bin/env python
import ROOT as r

def locate_rollover(ifname,ofname):
    ifile = r.TFile(ifname)
    itree = ifile.Get("t")
    nevt = itree.GetEntries()
    last_ts = [0,0,0]
    rollovers = [[],[],[]]
    ofile = r.TFile(ofname,'recreate')
    #entrylists = [[r.TEntryList('ch0list0','ch0list0')],[r.TEntryList('ch1list0','ch1list0')],[r.TEntryList('ch2list0','ch2list0')]] # entrylists[chnum][which]
    entrylists = [[r.TEntryList('ch0list0','ch0list0')],[r.TEntryList('ch1list0','ch1list0')]] # entrylists[chnum][which]
    for i in range(nevt):
        itree.GetEntry(i)
        ts = itree.GetLeaf("ts").GetValue()
        chnum = int(itree.GetLeaf("chnum").GetValue())
        if ts < last_ts[chnum]:
            print 'sense rollover ch',chnum,'entry',i
            titlestring = 'ch%ilist%i'%(chnum,len(entrylists[chnum]))
            entrylists[chnum].append(r.TEntryList(titlestring,titlestring))
            entrylists[chnum][-1].Enter(i)
        else:
            entrylists[chnum][-1].Enter(i)
        last_ts[chnum] = ts
    # check that all lists are same length
    reflist = len(entrylists[0])
    for chlist in entrylists:
        if reflist == len(chlist):
	    for fraglist in chlist:
		fraglist.Write()
        else:
	    print 'not all lists are same length'
    ofile.Close()

def find_rough_coinc(ifname,iename,ntimeloops,chlist,coincgate=150):
    ifile = r.TFile(ifname)
    itree = ifile.Get("t")
    iefile = r.TFile(iename)
    allevtlists = []
    # create the event lists
    for i in range(ntimeloops):
        evtlist = []
        for ch in chlist:
      	    titlestring = 'ch%ilist%i'%(ch,i)
    	    entrylist = iefile.Get(titlestring)
	    itree.SetEntryList(entrylist)
	    nevt = entrylist.GetN()
	    for evt in range(nevt):
	        treeev = entrylist.GetEntry(evt)
	        itree.GetEntry(treeev)
	        ts = itree.GetLeaf("ts").GetValue()
                evtlist.append([ts,ch,evt])
        allevtlists.append(evtlist)
    # find the rough coincs
    coincidences = []
    for i in range(ntimeloops):
        evtlist = allevtlists[i]
        evtlist.sort()
        coinc_events = []
        temp_evts = [evtlist[0]]
        # build our event structures
        for event in evtlist[1:]:
            if event[0]-temp_evts[0][0]<coincgate:
                temp_evts.append(event)
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
    return coincidences
    
                
    
        

def parse_file(ifname):
    ifile = r.TFile(ifname)
    itree = ifile.Get("t")
    nevt = itree.GetEntries()
    evtlist = []
    for i in range(nevt):
        itree.GetEntry(i)
        ts = itree.GetLeaf("ts").GetValue()
        chnum = itree.GetLeaf("chnum").GetValue()
        evtlist.append([ts,chnum,i])
    ifile.Close()
    return evtlist

def assemble_events(evtlist,coincgate):
    evtlist.sort()
    coinc_events = []
    temp_evts = [evtlist[0]]
    # build our event structures
    for event in evtlist[1:]:
        if event[0]-temp_evts[0][0]<coincgate:
            temp_evts.append(event)
        else:
            coinc_events.append(temp_evts)
            temp_evts = [event]
    coinc_events.append(temp_evts)
    return coinc_events

def write_coinc_file(ofname,coinclist):
    ofile = open(ofname,'w')
    for coinc in coinclist:
        ostring = ''
        for icoinc in coinc:
            ostring += '%i %i %i '%(icoinc[0],icoinc[1],icoinc[2])
        ostring = ostring[:-1] + '\n'
        ofile.write(ostring)

def generate_evtlist(ifname):
    evtlist = []
    for line in open(ifname):
        temp = line.split()
        mult = len(temp)/3
        coincevt = []
        for i in range(mult):
            ts = int(temp[i*3+0])
            ch = int(temp[i*3+1])
            evt = int(temp[i*3+2])
            crysevt = [ts,ch,evt]
            coincevt.append(crysevt)
        evtlist.append(coincevt)
    return evtlist

def write_basic_hists(masterlist):
    ofile = r.TFile("basic_hists.root","recreate")
    chnum_hist = r.TH1D("chnum","chnum",10,0,10)
    tdiff_hist = r.TH1D("tdiff","tdiff",140,-140,0)
    counter2 = 0
    counter3 = 0
    for coinc_evts in masterlist:
        for events in coinc_evts:
            if len(events)==2:
                counter2 += 1
                chnum_hist.Fill(events[0][1])
                chnum_hist.Fill(events[1][1])
                tdiff_hist.Fill(events[0][0]-events[1][0])
            if len(events)==3:
                counter3 += 1
    print "Found",counter2,"multiplicity 2 events"
    print "Found",counter3,"multiplicity 3 events"
    print "Found",counter2+counter3,"multiplicity 2/3 events"
    chnum_hist.Write()
    tdiff_hist.Write()
    ofile.Close()

def hist2time(hist,bindelta,frac=0.3,decimation=4.):
    baseline = 0.
    num = 0.
    denom = 0.
    for i in range(1,60/int(decimation)):
        num += hist.GetBinContent(i)
        denom += 1.
    baseline = num/denom
    #print 'baseline',baseline
    peakloc = hist.GetMaximumBin()
    #print 'max bin',peakloc
    max = hist.GetBinContent(peakloc)
    #print 'max',max
    fracpoint = baseline + frac*(max-baseline)
    #print 'fracpoint',fracpoint
    time = 0.
    index_greater = 0.
    for i in range(peakloc-bindelta/int(decimation),peakloc+1):
        #print 'index',i
        if hist.GetBinContent(i)>fracpoint:
            index_greater = i
            break
    # interpolate the time based on linear interpolation of samples
    #print index_greater
    v0 = hist.GetBinContent(index_greater-1)-baseline
    deltav = hist.GetBinContent(index_greater)-v0-baseline
    try:
        frac = (fracpoint-baseline-v0)/deltav
    except ZeroDivisionError:
        return -1.
    t0 = index_greater-1
    time = t0+frac
    return time
    
        

def process_times(ifname,iename,ofname,masterlist,frac=0.3,decimation=4.,diagdump=1):
    gc = 0
    ifile = r.TFile(ifname)
    itree = ifile.Get("t")
    iefile = r.TFile(iename)
    #tdiff_hist = r.TH1D("tdiff","tdiff",60,-125,-110) # good for ch0ch1 coincs
    tdiff_hist = r.TH1D("tdiff","tdiff",60,-65,-50) # good for filtered ch0ch1 coincs
    #tdiff_hist = r.TH1D("tdiff","tdiff",60,25,40) # good for ch1ch2 coincs
    #tdiff_hist = r.TH1D("tdiff","tdiff",60,-90,-75) # good for ch1ch2 coincs
    tdiff_coarse_hist = r.TH1D("tdiff_coarse","tdiff_coarse",200,-200,200) 
    # histogram for waveform analysis
    ofile = r.TFile(ofname,"recreate")
    wfhist = r.TH1D('wfhist','wfhist',200/int(decimation),0,200/int(decimation))
    tempofile = open('tempwf.txt','w')
    tempofile2 = open('tempwf2.txt','w')
    for clockindex,clocklist in enumerate(masterlist):
        for coinclist in clocklist:
            entrylist = []
            for i in range(3):
                estring = 'ch%ilist%i'%(i,clockindex)
                entrylist.append(iefile.Get(estring))
            if len(coinclist)==2:
                gc += 1
                if gc%100==0:
                    print 'on coinc event',gc
                if gc > 1000:
                   break
                if coinclist[0][1]+coinclist[1][1]==1: # gating on the combo of ch 0 and 1
                    #print 'whoo'
                    basetime = [0.,0.,0.]
                    deltatime = [0.,0.,0.]
                    for chindex,channel in enumerate([coinclist[0][1],coinclist[1][1]]):
			itree.SetEntryList(entrylist[channel])
			treeev = entrylist[channel].GetEntry(coinclist[chindex][2])
			itree.GetEntry(treeev)
			ts = itree.GetLeaf("ts").GetValue()
                        basetime[channel] = ts
                        for eye in range(200/int(decimation)):
                            temp = 0.
                            for jay in range(int(decimation)):
                                temp += -1.*itree.GetLeaf('wf').GetValue(int(decimation)*eye+jay)
                            wfhist.SetBinContent(eye+1,temp)
                            #print temp
                        if diagdump and channel == 1:
                            ostring = ''
                            ostring2 = ''
                            for eye in range(200):
                                ostring += "%i "%(int(itree.GetLeaf('wf').GetValue(eye)))
                            for eye in range(200/int(decimation)):
                                temp = 0.
                                for jay in range(int(decimation)):
                                    temp += itree.GetLeaf('wf').GetValue(int(decimation)*eye+jay)
                                temp /= decimation
                                ostring2 += "%i "%(int(temp))
                            tempofile.write(ostring[:-1]+'\n')
                            tempofile2.write(ostring2[:-1]+'\n')
                        deltatime[channel] = decimation*hist2time(wfhist,12,decimation=decimation)
                    timedelta = basetime[0] - basetime[1]
                    #print timedelta
                    tdiff_coarse_hist.Fill(timedelta)
                    timedelta = timedelta + deltatime[0] - deltatime[1]
                    tdiff_hist.Fill(timedelta)
    tdiff_hist.Write()
    tdiff_coarse_hist.Write()
    ofile.Close()
    tempofile.close()
    tempofile2.close()
                    

if __name__=='__main__':
    """
    ifname = '../processed_data/run102.root'
    evtlist = parse_file(ifname)
    coinc_evts = assemble_events(evtlist,2000)
    print 'length of event list',len(evtlist)
    print 'length of assembled events',len(coinc_evts)
    print 'delta ', len(evtlist)-len(coinc_evts)
    ofile = r.TFile("basic_hists.root","recreate")
    chnum_hist = r.TH1D("chnum","chnum",10,0,10)
    tdiff_hist = r.TH1D("tdiff","tdiff",140,-140,0)
    counter2 = 0
    counter3 = 0
    for events in coinc_evts:
        if len(events)==2:
            counter2 += 1
            chnum_hist.Fill(events[0][1])
            chnum_hist.Fill(events[1][1])
            tdiff_hist.Fill(events[0][0]-events[1][0])
        if len(events)==3:
            counter3 += 1
    print "Found",counter2,"multiplicity 2 events"
    print "Found",counter3,"multiplicity 3 events"
    print "Found",counter2+counter3,"multiplicity 2/3 events"
    chnum_hist.Write()
    tdiff_hist.Write()
    ofile.Close()
    """


    # find all the rollovers
    ifname = '../processed_data/run126.root'
    ofname = './run126_eventlists.root'
    #locate_rollover(ifname,ofname)
    ifname = '../processed_data/run126.root'
    iename = './run126_eventlists.root'
    ntimeloops = 4
    chlist = [0,1]
    """
    coinclist = find_rough_coinc(ifname,iename,ntimeloops,chlist)
    for i in range(ntimeloops):
       ofname = './filtered_coinc_%i.txt'%(i)
       write_coinc_file(ofname,coinclist[i])
    """
    masterlist = []
    for i in range(ntimeloops):
        ifname =  'filtered_coinc_%i.txt' % (i)
        masterlist.append(generate_evtlist(ifname))
    print 'writing the basic hists'
    write_basic_hists(masterlist)

    ifname = '../processed_data/run126.root'
    iename = './run126_eventlists.root'
    ofname = './filtered_timeproc_4dec.root'
    print 'processing the times'
    process_times(ifname,iename,ofname,masterlist,decimation=4.)
