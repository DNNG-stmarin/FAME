{
    gSystem->Load("/home/smosby/tke_analysis/trunk/smosby_work/libraries/waveform_processing_C.so");
    gROOT->ProcessLine(".L macros/loopOnTree.C");


    TChain *ch = makeBigPulseChain();
    ch->SetAlias("aph0","-aph[0]");
    ch->SetAlias("aph1","-aph[1]");
    ch->SetAlias("xdcos0","-gph[0]/aph[0]");
    ch->SetAlias("xdcos1","-gph[1]/aph[1]");
}
