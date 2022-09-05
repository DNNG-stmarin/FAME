# TKE analysis software
- Shea Mosby
- Devin Connolly 
  + [dconnolly@lanl.gov](mailto:dconnolly@lanl.gov)

## TKE analysis

This software package is intended to analyze raw total kinetic energy (TKE) data collected with the Twin Frisch-Gridded Ionization Chamber (TFGIC) using a CAEN 720 or 730 digitizer with the [Waveform Recording Firmware](https://www.caen.it/products/d-wave/). Compiling installs a shared library and three executables to convert raw MIDAS files into root files for further post-processing analysis as follows:

- `lib`
  + `libWaveformProcessing.so`
- `bin`
  + `unpacker`
  + `singlesprocessing`
  + `eventbuild`

To compile, simply type `make`. The executables are run as follows in the order specified:

`analyzer <ifile>.mid <ofile1>.root`

`singlesprocessing <ofile1>.root <ofile2>.root`

`eventbuild <ofile2>.root <ofile3>.root` 

Optionally, one may pass sets of fitting parameters to singlesprocessing for the extraction of pulse-heights from the anode and grid signals in the form of a config files as follows:

`singlesprocessing <ofile1>.root <ofile2>.root <config-file>.cfg`

An example config file is provided in [analysisParams.cfg](./analysisParams.cfg)

See [USAGE.md](doc/USAGE.md) for more details on running the analysis code.
