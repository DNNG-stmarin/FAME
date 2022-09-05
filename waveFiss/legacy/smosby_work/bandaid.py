#!/usr/bin/env python
import ROOT as r
import sys
r.gROOT.ProcessLine(".x ./macros/startana.C")
r.processFile(sys.argv[1],sys.argv[2])
