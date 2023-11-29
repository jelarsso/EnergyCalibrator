/*
These are the first attempt at linear coefficients, this script will try to automatically find more peaks :)
It works quite well, however, only fit one energy at a time, and the fit range + shift might have to be adjusted 
for each energy. 


*/
double labrCal [30][3] = { 
{ -31.053890808793927 , 0.4845147805707581 , 0 },
{ -21.283854581954106 , 0.4959714184037152 , 0 },
{ -70.31800678212831 , 0.6455194189614629 , 0 },
{ -50.897695817848515 , 0.5925490430909609 , 0 },
{ -62.974750301953314 , 1.2697551722167404 , 0 },
{ -36.377699679014505 , 0.5165799991407178 , 0 },
{ -57.22665391540198 , 0.6553134117023743 , 0 },
{ -48.206650815544165 , 0.5772966429526195 , 0 },
{ -32.413678111946446 , 0.6386507034315937 , 0 },
{ -48.97137965446016 , 0.5632004810348619 , 0 },
{ -55.57760169982152 , 0.5227237922214631 , 0 },
{ -10.233257504788417 , 0.5216654653780285 , 0 },
{ -29.33995547059745 , 0.6452711539294602 , 0 },
{ -28.713838093305885 , 0.5642895219761855 , 0 },
{ -18.152788900627364 , 0.49838820397405575 , 0 },
{ -38.22628522536721 , 0.4210228334812437 , 0 },
{ -56.55229908079133 , 0.5044549280519446 , 0 },
{ -18.873074120371683 , 0.6536710656378323 , 0 },
{ -14.010637629605226 , 0.49492227015980245 , 0 },
{ -23.849747414244145 , 0.45026508305690666 , 0 },
{ -28.59956489408795 , 0.6205992369749415 , 0 },
{ -53.24267700466761 , 0.4751162623041934 , 0 },
{ -16.60779285027285 , 1.0580698674565476 , 0 },
{ -50.285957834379886 , 0.4681737954391461 , 0 },
{ -40.36002997613827 , 0.6244444467358188 , 0 },
{ -22.578996649296187 , 1.063758874136117 , 0 },
{ -34.05310987783062 , 0.5269720898233018 , 0 },
{ -25.249691279051373 , 1.0827777092832744 , 0 },
{ -70.0269698946015 , 0.570839906790359 , 0 },
{ -31.714758162569048 , 0.5825053951362708 , 0 }
};

double get_uncalibrated_energy(double calibrated_energy, int detectorID){
    return (calibrated_energy-labrCal[detectorID-1][0])/labrCal[detectorID-1][1];
}


void EnergyGuesser(){

    static int number_of_detectors = 30;

    TFile * uncalibrated_file = TFile::Open("12C_t_siri.root");
    TH2D * h_labr_energy;
    TH1D * single_spectrum;
    TF1 * fitfunc;

    double uncal_centroids[number_of_detectors+1];
    double uncal_energy;
    
    
    //change these guys
    uncalibrated_file->GetObject("h_LaBr3_energy_pp",h_labr_energy);
    double desired_energy = 4438.94-510.998;//{1063.78, 6128.63-510.998, 7115.15-510.998};
    double fit_width = desired_energy*0.02;
    double small_shift = 0;


    uncal_centroids[0] = desired_energy;

    for (int ID = 1; ID<=number_of_detectors; ID++){
        single_spectrum = h_labr_energy->ProjectionX("",ID,ID,"");
        single_spectrum->Rebin(8);
        uncal_energy = get_uncalibrated_energy(desired_energy,ID);
        if (ID == 5) single_spectrum->GetXaxis()->SetRangeUser(uncal_energy-100,uncal_energy+100); // also change me
        else single_spectrum->GetXaxis()->SetRangeUser(uncal_energy-200,uncal_energy+200); // and me!

        uncal_energy = single_spectrum->GetBinCenter(single_spectrum->GetMaximumBin());

        single_spectrum->Draw("COL");
        fitfunc = new TF1("fit","gaus", uncal_energy-fit_width+small_shift,uncal_energy+fit_width+small_shift);

        single_spectrum->Fit(fitfunc,"QR");
        cout << Form("LaBr3 id = %i, Found = %f, Attempt = %f",ID,fitfunc->GetParameter(1),uncal_energy) << endl;
        TAxis *xax = single_spectrum->GetXaxis();
        xax->SetRangeUser(fitfunc->GetParameter(1)-1500,   fitfunc->GetParameter(1)+1500); // This is to visualize, makes no matter for the fit.
        uncal_centroids[ID] = fitfunc->GetParameter(1);
        gPad->WaitPrimitive();
    }
    

    for (int ID = 0; ID <= number_of_detectors; ID ++){
        cout << uncal_centroids[ID] << ", ";
    }
    cout << endl;


    return;
}