# TKE analysis software usage
### Shea Mosby
### Devin Connolly

### September 2020

The `tke_analysis` software is now located in this gitlab repo and `smm-sci0` in `/home/daq/tke/tke_analysis`, and MIDAS files are saved in `/data/tke` 

The tools currently in use are compiled, command line driven applications. As such they are faster than ROOT macros, but less flexible since all the analysis parameters are part of the executable. There are 3 applications which perform the major steps to MIDAS files to energies and times which we actually want to look at. To compile them, simply type `Make` (or `make -jn` where n is the number of processors<sup>[1](#footnote1)</sup> you wish to use when compiling). The analysis software also has the option to save the waveforms throughout the analysis or discard them after the first step. In order to turn on (off) the saving of waveforms, the preprocessor directive `SAVE_WAVEFORMS` must be uncommented (commented) in [`definitions.h`](./include/definitions.h) at compile time.

The first step is the `unpacker` which (as the name suggests) unpacks the MIDAS files and dumps the raw digitizer data into a root tree. One runs this as follows:

`unpacker <ifile>.mid.gz <unpack-file>.root`

where the arguments are the MIDAS file you want to read from and the root file you want to create respectively. The resulting root tree has board number, channel number, trigger timestamp, and waveform branches - this is the complete set of information which is read from the digitizers. The unpacker tree has the following branches:

- `bnum`     = board number
- `chnum`    = channel number
- `ts`       = trigger timestamp
- `wf[]`     = waveforms

The second step is `singlesprocessing` which extracts energies and times from the raw waveforms and places them into separate ROOT trees based on channel number. It can be run as follows:

`singlesprocessing <unpack-file>.root <sp-file>.root`

Optionally, one may pass fit parameters (used in the pulse height analysis for the anode and grid signals) to the `singlesprocessing` application as follows:

`singlesprocessing <unpack-file>.root <sp-file>.root <config-file>.cfg`

The naming scheme for the output trees in `<sp-file>.root`is `pt<bnum>_<chnum>`. It writes out the board and channel numbers, trigger timestamp, and the energy, time, and baseline for each waveform, Time within a waveform is where exactly within the waveform the peak was observed. One must add the trigger timestamp to the waveform time to get accurate time of flight for example. The branches in each singles tree are

-` bnum`     = board number
- `chnum`    = channel number
- `ts`       = trigger timestamp
- `peak`     = energy information whether it's a pulse height or charge integration
- `baseline` = the baseline of the signal
- (`wf[]`    = waveform for the given channel if the `SAVE_WAVEFORMS` flag is set)

The third step is to build physics events using the `eventbuild` application.

`eventbuild <sp-file>.root <eb-file>.root` 

This collects coincident events from the different channels and builds them into a new output tree. In the output tree, each entry is a physics event, and has an associated t0 timestamp, cathode timestamp, cathode pulse height, anode 0 & 1 pulse heights, and grid 0 & 1 pulse heights, as well as the anode waveforms, grid waveforms and cathode waveform. The event tree (`et`) has the following branches:

- `t0t`     = relevant t0 time
- `ct`      = cathode time
- `dt`      = time of flight between t0 and cathode
- `cph`     = cathode pulse height
- `aph`     = anode pulse heights
- `gph`     = grid pulse heights
- (`a0wf[]` = anode 0 waveform for the event if the `SAVE_WAVEFORMS` flag is set)
- (`a1wf[]` = anode 1 waveform for the event if the `SAVE_WAVEFORMS` flag is set)
- (`g0wf[]` = grid  0 waveform for the event if the `SAVE_WAVEFORMS` flag is set)
- (`g1wf[]` = grid  1 waveform for the event if the `SAVE_WAVEFORMS` flag is set)
- (`cwf[]`  = cathode waveform for the event if the `SAVE_WAVEFORMS` flag is set)


<a name="footnote1"><sup>[1]</sup></a> To find the number of available processors on linux, type `nproc --all`, or on mac, type `sysctl -n hw.ncpu`. Then, to use all available processors, one may type `make -j$(nproc --all)` on linux or `make -j$(sysctl -n hw.ncpu)` on mac.
