{
    gROOT->ProcessLine(".L macros/loopOnTree.C");
    TChain *ch = makeBigEventChainLujanGammas();
    // Lujan specific parameters
    ch->SetAlias("tof","dt+500");
    ch->SetAlias("v","(23.2*1.e9)/tof");
    ch->SetAlias("vc","v/(3.e8)");
    ch->SetAlias("gamma","1./sqrt(1.-pow(vc,2))");
    ch->SetAlias("nke","(gamma-1.)*939.56*1e6");
    ch->SetAlias("gtof","gt[0]+19.71");
    ch->SetAlias("geraw","-1*ge[0]");
}
