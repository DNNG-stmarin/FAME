{
    gROOT->ProcessLine(".L macros/loopOnTree.C");

    //TChain *ch = makeBigEventChainLujanFriends();
    //TChain *ch = makePu239PulseChain();
    //TChain *ch = makeEventChainWNRPu9();
    //TChain *ch = makeBigEventChain();
    //TChain *ch = makeNpEventChain();
    TChain *ch = makeNpEventChainAll();
    //TChain *ch = makeBigEventChainFriends();
    //TChain *ch = makeBigEventChainLujanFriends();
    //TChain *ch = makeBigEventChainLujan();
    ch->SetAlias("aph0","-aph[0]");
    ch->SetAlias("aph1","-aph[1]");
    ch->SetAlias("xdcos0","-gph[0]/aph[0]");
    ch->SetAlias("xdcos1","-gph[1]/aph[1]");
    /*
    // Lujan specific parameters
    ch->SetAlias("tof","dt+500");
    ch->SetAlias("v","(23.2*1.e9)/tof");
    ch->SetAlias("vc","v/(3.e8)");
    ch->SetAlias("gamma","1./sqrt(1.-pow(vc,2))");
    ch->SetAlias("nke","(gamma-1.)*939.56*1e6");
    */
    // WNR specific parameters
    //ch->SetAlias("tof","((100000.*(dt-466)%(100000*1788.819875776))/100000.)");
    //ch->SetAlias("v","(8.9*1.e9)/tof");
    ch->SetAlias("tof","((100000.*(dt-1200.0)%(100000*1788.819875776))/100000.)");
    ch->SetAlias("v","(10.6*1.e9)/tof");
    ch->SetAlias("vc","v/(3.e8)");
    ch->SetAlias("gamma","1./sqrt(1.-pow(vc,2))");
    ch->SetAlias("nke","(gamma-1.)*939.56");



    ch->SetAlias("tke_nocal","aph0+aph1");
    ch->SetAlias("ke0","6.75 + 0.192*aph0");
    ch->SetAlias("ke1","15.75 + 0.188*aph1");
    ch->SetAlias("tke_roughcal","ke0+ke1");

    // gates
    //ch->SetAlias("goodevent","aph[0]<-50&&cph<-5000&&vc>0&&vc<1&&xdcos0>20");
    ch->SetAlias("goodevent","aph[0]<-50&&cph<-5000&&dt<150e3");


    // Friend related stuff
    ch->SetAlias("cos0","at.costheta[0]");
    ch->SetAlias("cos1","at.costheta[1]");
}
