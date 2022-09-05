### Tues. 10/1/2013

This is a very short intro on how to use the analysis tools I've written so far. In case you're looking at this somewhere random (like an email or our elog) the location of this software is `daq@emdaq:/home/daq/from_smosby/tke_analysis` and this document is located in the doc subdirectory off of that. You'll need to copy the tke_analysis directory in to your own analysis directory to use it. Midas files are currently located in `daq@emdaq:/data/1/tke/`

All the tools we're using right now are compiled, command line driven applications. As such they're faster than ROOT macros, but less flexible since all the analysis parameters are part of the executable. There are 3 applications which perform the major steps to get from midas files and waveforms to energies and times which we actually want to look at.

The first step is analyzer which unpacks the midas files and dumps them into a root tree. You run like: 

`./analyzer <ifile> <ofile>`

where the paths are the midas file you want to read from and the root file you want to create respectively. The resulting root tree has board number, channel number, trigger timestamp, and waveform branches - this is the complete set of information which is read from the digitizers. You build this application with the MakeAnalyzer script.

The second step is singlesprocessing which converts the waveforms into energies and times. You run like: 

`./singlesprocessing <ifile> <ofile>`

For "convenience" later down the road, it writes out a tree for each digitizer channel you're using. The naming scheme for the output trees is `pt<bnum>_<chnum>`. It writes out the board and channel numbers, trigger timestamp, and the energy, time, and baseline for each waveform. Time within a waveform is where exactly within the waveform the peak was observed. You must add the trigger timestamp to the waveform time to get accurate time of flight for example. Parameter map:

- bnum     = board number
- chnum    = channel number
- ts       = trigger timestamp
- peak     = energy information whether it's a pulse height analysis or charge integration
- baseline = the baseline of the signal

You build this application with the MakeSinglesProcessing script.

The third step is to build physics events. I have an application which does this for the ionization chamber called eventbuild. You run like: 

`./eventbuild <ifile> <ofile>` 

This collects coincident events from the different channels and builds them into a new output tree. In the output tree, each entry is a physics event. parameter map:

- t0t = relevant t0 time
- ct  = cathode time
- dt  = time of flight between t0 and cathode
- cph = cathode pulse height
- aph = anode pulse heights
- gph = grid pulse heights

The rest of the parameters are irrelevant. You build this application with MakeEventBuild script. I have an elog entry which shows what some of the spectra should look like. The other files in the directory are convenience scripts that I find helpful (now in [`legacy`](./legacy/smosby_work) - DSC). You can take them or leave them.

### January 2019
#### D. Connolly

All analysis software (`tke_analysis` and `offline_analyzer`) are now located in this gitlab repo, and have been slightly (but not functionally) modified from their original form on emdaq. Below are the locations of the original software on `emdaq`.

Midas files and unpacking code are in

`/home/daq/caen_daq/tke_fe/offline_analyzer` (`analyzer` in the current repo)

Physics analysis code is in `/home/daq/from_smosby/tke_analysis` (root dir of current repo)

`/home/daq/from_smosby/tke_analysis/smosby_work/macros` has convenience functions (`smosby_work/macros` in the current repo).

`startana.C` sets up run lists and convenience functions

In the current repo, the `Make*` scripts described below have been condensed into a single makefile. To compile all of the applications listed below, simply type `make`. All executables are then compiled and linked in the local `bin` directory.

