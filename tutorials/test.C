{
//header 


    int numBins = 100;
    int xMin = 0;
    int xMax = 100;
    int numParticles = 10500;


        //- Create a TFile to store the output


    //	- Define a TF1 for photon PSD (single gaussian)
    TF1* photonGauss = new TF1("fphoton", "[0]*exp(-0.5*((x-[1])/[2])^2)", xMin, xMax);
    //	- Define a TF1 for neutron PSD (single gaussian)
    TF1* neutronGauss = new TF1("fneutron", "[0]*exp(-0.5*((x-[1])/[2])^2)", xMin, xMax);
    //	- Define a TF1 for combined (in terms of the other 2)
    TF1* doubleGauss = new TF1 ("fdouble", "fphoton + fneutron", xMin, xMax);
    //	- Define a TF1 for the difference between neutrons and photons
    TF1* differenceGauss = new TF1 ("fdifference", "fphoton - fneutron", xMin, xMax);

}