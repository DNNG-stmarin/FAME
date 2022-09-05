#!/usr/bin/env python

def parse_file(ifname):
    """Retrieve events from text file

    Keyword Arguments:
    ifname - the name of the input file
    """
    #initialize the event list
    events = []
    for line in open(ifname):
        # check for commented lines
        if line[0]=='#':
            continue
        # initialize the event list for current channel
        temp_events = []
        # split the line, remove the text channel tag
        for event in line.split()[1:]:
            temp_events.append(float(event))
        events.append(temp_events)
    return events

def find_tof_ch(evtlist,chnum):
    """Find the tof for all events relative to t0

    Keyword Arguments:
    evtlist - 2d list of channels and event times
    chnum - the channel to consider
    """
    tofs = []
    # obtain the referene t0 (ch0)
    t0_list = evtlist[0]
    # reverse the list to be clever with our simple algorithm
    t0_list.reverse()
    # loop over the events in the chosen channel
    for event_t in evtlist[chnum]:
        for t0_t in t0_list:
            # the first t0 we are greater than in the t0 list defines our time
            if event_t>t0_t:
                tofs.append(event_t-t0_t)
                break
    return tofs

def find_tof_coinc(evtlist,coincgate):
    """Find the tof for all events regardless of channels. 

    Keyword Arguments:
    evtlist - 2d list of channels and event times
    coincgate - width of coincidence window
    """
    collated_events = []
    # merge all events from all detector channels into one list
    for channel_events in evtlist[1:]:
        for event in channel_events:
            collated_events.append(event)
    collated_events.sort()
    coinc_events = []
    temp_evts = [collated_events[0]]
    # build our event structures
    for event in collated_events[1:]:
        if event-temp_evts[0]<coincgate:
            temp_evts.append(event)
        else:
            coinc_events.append(temp_evts)
            temp_evts = [event]
    coinc_events.append(temp_evts)
    # now build up the average tof list
    average_tofs = []
    for tevt in coinc_events:
        num = 0.
        denom = 0.
        for evt in tevt:
            num += evt
            denom += 1
        average_tofs.append(num/denom)
    # make sure things are sorted right
    t0list = evtlist[0]
    t0list.sort()
    tofs = find_tof_ch([t0list,average_tofs],1)
    return tofs



if __name__=='__main__':
    ifname = './practice_events.txt'
    events = parse_file(ifname)
    for channel in events:
        print channel
    chnum = 1
    tofs = find_tof_ch(events,chnum)
    print tofs
    coincwindow = 20.
    tofs = find_tof_coinc(events,coincwindow)
    print tofs
    print len(tofs)
