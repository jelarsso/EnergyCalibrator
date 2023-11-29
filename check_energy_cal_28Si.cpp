#include<list> 


/*
JE 31.05
This also makes awesome figures
*/

float fit_epeaks(float energy, float deltaE, TH2D * h_LaBr3_energy){
    TF1 * gaus = new TF1("gaus","gaus",energy-deltaE,energy+deltaE);
    TH1D * h_LaBr3_energy_det;
        for (int i = 1; i<31; i++){
            h_LaBr3_energy_det = h_LaBr3_energy->ProjectionX("",i,i,"");
            h_LaBr3_energy_det->Fit(gaus,"QR");
            cout << "Detector ID = " << i << " E_g = " << gaus->GetParameter(1) << " keV. Difference : " << energy - gaus->GetParameter(1) << " keV , Fit dmu = " << gaus->GetParError(1) << " keV"  << endl;
        }
        h_LaBr3_energy_det = h_LaBr3_energy->ProjectionX("",1,29,"");
        h_LaBr3_energy_det->Fit(gaus,"QR");
        cout << "Detector all " << " E_g = " << gaus->GetParameter(1) << " keV. Difference : " << energy - gaus->GetParameter(1) << " keV , Fit dmu = " << gaus->GetParError(1) << " keV" << endl; 
        cout << "FWHM = " << gaus->GetParameter(2)*2.355 << "  resolution = " << gaus->GetParameter(2)*2.355/gaus->GetParameter(1)*100  << "%\n\n\n"; 
        return gaus->GetParameter(1); //return the fit to all detectors
}



void check_energy_cal_28Si(){

    TFile * infile = TFile::Open("28Si_cal_2403.root");

    TH2D * h_LaBr3_energy;

    list<float> peaks_to_find = {1778.969,7414.2,5600.4,9476.1,6877.0,2838.29,5107.6};
    list<float> fits = {};


    infile->GetObject("h_LaBr3_gated_1779",h_LaBr3_energy);
    
    fits.push_back(fit_epeaks(1778.969, 60, h_LaBr3_energy));
    //gPad->WaitPrimitive();
    infile->GetObject("h_LaBr3_gated_7416",h_LaBr3_energy);
    
    fits.push_back(fit_epeaks(7414.2, 60, h_LaBr3_energy));
    //gPad->WaitPrimitive();
    infile->GetObject("h_LaBr3_energy_pp",h_LaBr3_energy);
    
    fits.push_back(fit_epeaks(5600.4, 60, h_LaBr3_energy));
    //gPad->WaitPrimitive();
    fits.push_back(fit_epeaks(9476.1, 60, h_LaBr3_energy));
    //gPad->WaitPrimitive();
    fits.push_back(fit_epeaks(6877.0, 60, h_LaBr3_energy));
    fits.push_back(fit_epeaks(2838.29, 60, h_LaBr3_energy));
    fits.push_back(fit_epeaks(5107.6, 60, h_LaBr3_energy));
    //gPad->WaitPrimitive();

    TH1D * energy_spect = h_LaBr3_energy->ProjectionX("",1,29,"");
    energy_spect->Draw();
    energy_spect->SetTitle(";Energy (keV);Counts");
    energy_spect->GetXaxis()->SetRangeUser(0,10000);
    energy_spect->GetYaxis()->SetRangeUser(1*1e2,5.6*1e6);
    energy_spect->SetStats(0);

    gPad->SetLogy();
    list<float>::iterator t2 = peaks_to_find.begin();
    for (list<float>::iterator t = fits.begin(); t!=fits.end(); ++t){
        float y_val = energy_spect->GetBinContent(energy_spect->FindBin(*t));
        TArrow * arrow  = new TArrow(*t,y_val,*t,y_val+2000,0.003,"<|");
        arrow->Draw();
        TText * text = new TText(*t,y_val+2000,Form("%5.1lf %5.1lf keV",*t,*t-*t2));
        text->SetTextFont(11);
        text->SetTextAngle(75);
        text->SetTextSize(0.04);
        text->Draw();
        ++t2;
    }

    gPad->SaveAs("/home/johan/Documents/UiO/master/analysis/figs/28Si_peaks.pdf");
    gPad->WaitPrimitive();

    h_LaBr3_energy->Draw("COL");
    h_LaBr3_energy->SetStats(0);

    gPad->SetLogz();
    gPad->SetLogy(0);
    h_LaBr3_energy->SetTitle(";Energy (keV);Detector ID");
    h_LaBr3_energy->GetXaxis()->SetRangeUser(0,10000);
    gPad->SaveAs("/home/johan/Documents/UiO/master/analysis/figs/28Si_energy.pdf");
    


}