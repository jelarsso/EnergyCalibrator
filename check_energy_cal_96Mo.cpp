#include<list> 

/*
JE 31.05
makes awesome figures with energy calibraiton
*/



float fit_epeaks(float energy, float deltaE, TH2D * h_LaBr3_energy){
    TF1 * gaus = new TF1("gaus","gaus",energy-deltaE,energy+deltaE);
    TH1D * h_LaBr3_energy_det;
        for (int i = 1; i<31; i++){
            h_LaBr3_energy_det = h_LaBr3_energy->ProjectionX("",i,i,"");
            h_LaBr3_energy_det->Rebin(4);
            h_LaBr3_energy_det->Fit(gaus,"QR");
            cout << "Detector ID = " << i << " E_g = " << gaus->GetParameter(1) << " keV. Difference : " << energy - gaus->GetParameter(1) << " keV , Fit dmu = " << gaus->GetParError(1) << " keV"  << endl;
        }
        h_LaBr3_energy_det = h_LaBr3_energy->ProjectionX("",1,1,"");
        h_LaBr3_energy_det->Fit(gaus,"QR");
        cout << "Detector all " << " E_g = " << gaus->GetParameter(1) << " keV. Difference : " << energy - gaus->GetParameter(1) << " keV , Fit dmu = " << gaus->GetParError(1) << " keV" << endl; 
        cout << "FWHM = " << gaus->GetParameter(2)*2.355 << "  resolution = " << gaus->GetParameter(2)*2.355/gaus->GetParameter(1)*100  << "%\n\n\n"; 
        return gaus->GetParameter(1); //return the fit to all detectors
}       



void check_energy_cal_96Mo(){

    TFile * infile = TFile::Open("96Mo_all.root");

    TH2D * h_LaBr3_energy;

    //list<float> peaks_to_find = {535.61, 1063.78, 1372.1, 1750.8, 4438.94, 6128.63, 7115.15};
    list<float> peaks_to_find = {778.223, 847.689, 1091.344, 1317.43};
    list<float> fits = {};


    infile->GetObject("h_LaBr3_energy_pp",h_LaBr3_energy);
    //fits.push_back(fit_epeaks(535.61, 20, h_LaBr3_energy));
    //fits.push_back(fit_epeaks(1063.78, 20, h_LaBr3_energy));
    //fits.push_back(fit_epeaks(1372.1, 30, h_LaBr3_energy));
    //fits.push_back(fit_epeaks(1750.8, 30, h_LaBr3_energy));
    //fits.push_back(fit_epeaks(4438.94, 30, h_LaBr3_energy));
    //fits.push_back(fit_epeaks(6128.63, 40, h_LaBr3_energy));
    //fits.push_back(fit_epeaks(7115.15, 40, h_LaBr3_energy));
    
    
    
    fits.push_back(fit_epeaks(778.223, 20, h_LaBr3_energy));
    fits.push_back(fit_epeaks(847.689, 20, h_LaBr3_energy));
    fits.push_back(fit_epeaks(1091.344, 30, h_LaBr3_energy));
    fits.push_back(fit_epeaks(1317.43, 30, h_LaBr3_energy));
    

    

    


    TH1D * energy_spect = h_LaBr3_energy->ProjectionX("",1,1,"");
    Double_t binwidth = energy_spect->GetBinWidth(10);
    energy_spect->Draw();
    energy_spect->SetTitle("Gamma Energy spectrum 96Mo detector 1;Energy (keV);Counts");
    energy_spect->GetXaxis()->SetRangeUser(0,3000);
    energy_spect->GetYaxis()->SetRangeUser(3*1e2,1*1e6);


    gPad->SetLogy();
    list<float>::iterator t2 = peaks_to_find.begin();
    for (list<float>::iterator t = fits.begin(); t!=fits.end(); ++t){
        float y_val = energy_spect->GetBinContent(energy_spect->FindBin(*t));
        TArrow * arrow  = new TArrow(*t,y_val,*t,y_val+10000,0.003,"<|");
        arrow->Draw();
        TText * text = new TText(*t,y_val+10000,Form("%5.1lf %5.1lf keV",*t,*t-*t2));
        text->SetTextFont(11);
        text->SetTextAngle(45);
        text->Draw();
        ++t2;
    }

    gPad->SaveAs("/home/johan/Documents/UiO/master/analysis/figs/96Mo_calibration_annotated_peaks.pdf");
    gPad->WaitPrimitive();

    h_LaBr3_energy->Draw("COLZ");
    gPad->SetLogz();
    gPad->SetLogy(0);
    h_LaBr3_energy->SetTitle("Gamma Energy Spectrum 96Mo all detectors;;Energy (keV);Detector ID");
    h_LaBr3_energy->GetXaxis()->SetRangeUser(0,10000);
    gPad->SaveAs("/home/johan/Documents/UiO/master/analysis/figs/96Mo_calibration_detectors_color.pdf");
    gPad->WaitPrimitive();
    


}