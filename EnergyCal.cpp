#include <TAxis.h>
#include <TCanvas.h>
#include <TDirectory.h>
#include<TString.h>
#include <TH2.h>
#include <TH1.h>
#include <TF1.h>
#include <TLine.h>
#include <TPad.h>
#include <TSystem.h>
#include <TFile.h>
#include <TObject.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include<string>

#include "EnergyCal.h"

using namespace std;


ClassImp(EnergyCal);

EnergyCal::EnergyCal(string source_filename, string out_filename) 
: TObject(){
    
    
    outfilename = out_filename;

    Double_t pe[number_of_peaks] = {511, 1173.2, 1332.5};
    histogram_name = "h_LaBr3_energy";
    histo_index = 0;

    for (int i=0; i<number_of_peaks;i++){
        peak_energies[i] = pe[i];
        for (int j = 0; j<number_of_detectors; j++){
            peak_clicks[i][j] = 0;
        }
    }



    source_file = TFile::Open(source_filename.c_str(),"READ");
    if(!source_file){
        cout << "Error on file open!" << endl;
        exit(-1);
    }

    fitwidth = 50;


    current_detector = 1;
    current_peak_id = 1;

    source_file->GetObject(histogram_name.c_str(),current_energies);
    current_slice = current_energies->ProjectionX("",current_detector,current_detector,"");

}

void EnergyCal::ChangeHistogram(){
    histo_index ++;
    string created_histos[3] = {"h_LaBr3_energy","h_LaBr3_energy_pp"};
    if (histo_index >= 3) histo_index = 0;
    string current_histo_name = created_histos[histo_index];
    cout << "CHANGED HISTOGRAM TO " << current_histo_name << endl;
    source_file->GetObject(current_histo_name.c_str(),current_energies);
    current_slice = current_energies->ProjectionX("",current_detector,current_detector,"");
    DrawHisto();
}

void EnergyCal::UnZoom(){
    current_slice->GetXaxis()->UnZoom();

}

void EnergyCal::Zoom(int xval){
    TAxis *xax = current_slice->GetXaxis();
    float widthx = xax->GetBinUpEdge(xax->GetLast()) - xax->GetBinLowEdge(xax->GetFirst());
    xax->SetRangeUser(xval-widthx/4,   xval+widthx/4);
    DrawHisto();
}


void EnergyCal::RebinCurrentHistogram(){
    current_slice->Rebin(2);
    DrawHisto();
}

void EnergyCal::FitPeak(float xval){
    fitfunc = new TF1("fitfunc","gaus(0)",xval-fitwidth,xval+fitwidth);
    current_slice->Fit(fitfunc, "R");
    peak_clicks[current_peak_id-1][current_detector-1] = fitfunc->GetParameter(1);
}

void EnergyCal::SetGateLow(float xval){
    tempgate_low = xval;
}
void EnergyCal::SetGateHighFit(float xval){
    tempgate_high = xval;
    if (tempgate_high < tempgate_low) return;

    fitwidth = (tempgate_high - tempgate_low)*0.5;

    FitPeak(xval-fitwidth);
}

void EnergyCal::NextDet(){
    if (current_peak_id == number_of_peaks && current_detector == number_of_detectors){
        gPad->DeleteExec(EC_EXECNAME);
        Finish();
        cout << "Done!" << endl;
        return;
    }
    if(current_detector == number_of_detectors){
        current_peak_id ++;
        current_detector = 1;
        source_file->GetObject(histogram_name.c_str(),current_energies);
        current_slice = current_energies->ProjectionX("",current_detector,current_detector,"");
    }else{
    current_detector ++;
    current_slice = current_energies->ProjectionX("",current_detector,current_detector,"");
    }
    DrawHisto();
    return;
}
void EnergyCal::LastDet(){
    if (current_peak_id == 1 && current_detector == 1){
        cout << "You are already at the first one!" << endl;
    }
    if(current_detector == 1){
        current_peak_id --;
        current_detector = 30;
        source_file->GetObject(histogram_name.c_str(),current_energies);
        current_slice = current_energies->ProjectionX("",current_detector,current_detector,"");
    }else{
    current_detector --;
    current_slice = current_energies->ProjectionX("",current_detector,current_detector,"");
    }
    DrawHisto();
    return;
}

void EnergyCal::DrawHisto(){
    current_slice->Draw();
    current_slice->SetTitle(Form("Detector %d , Peak %d, E = %f keV",current_detector,current_peak_id,peak_energies[current_peak_id-1]));
}

void EnergyCal::Start(){
    DrawHisto();
    gPad->AddExec(EC_EXECNAME,"EnergyCal_exec()");

}

EnergyCal::~EnergyCal(){
    source_file->Close();
    
}

void EnergyCal::Finish(){
    ofstream outfile(outfilename);
    outfile << "#first entry is the peak value, the rest are the centroids 1-30" << endl;
    for (int i=0; i<number_of_peaks;i++){
        outfile << Form("%f, ",peak_energies[i]);
        for (int j = 0; j<number_of_detectors; j++){
            outfile << Form("%f", peak_clicks[i][j]);
            if (j<number_of_detectors-1) outfile << ", ";
        }
        outfile << "\n";
    }
    outfile.close();
}

void EnergyCal_exec(){
    TObject* o = gPad->GetPrimitive(EC_OBJECT);
    EnergyCal* p = (EnergyCal*)o;
    
    const int event  = gPad->GetEvent();
    const int eventx = gPad->GetEventX();
    const int eventy = gPad->GetEventY();

    // some magic to get the coordinates...
    double xd = gPad->AbsPixeltoX(eventx);
    double yd = gPad->AbsPixeltoY(eventy);
    float x = gPad->PadtoX(xd);
    float y = gPad->PadtoY(yd);
    
    static float last_x;
    static float last_y;
 
    if(event!=kKeyPress)
    {
        last_x=x;
        last_y=y;
        return;
    }

    if (event==kKeyPress){
    
    if (eventx == 'f'){
        cout << "Fitting: " << endl;
        p->FitPeak(last_x);
        p->Draw();
    }
    if (eventx == 'n'){
        p->NextDet();
        p->Draw();
    }if (eventx == 'z'){
        p->Zoom((int)last_x);
        p->Draw();
    }if (eventx == 'u'){
        p->UnZoom();
        p->Draw();
    }if (eventx == 's'){
        p->SetGateLow(last_x);
        p->Draw();
    }if (eventx == 'd'){
        p->SetGateHighFit(last_x);
        p->Draw();
    }if (eventx == 'b'){
        p->LastDet();
        p->Draw();
    }if (eventx == 'm'){
        p->ChangeHistogram();
        p->Draw();
    }if (eventx == 'r'){
        p->RebinCurrentHistogram();
        p->Draw();
    }
    }
    gPad->Modified();
    gPad->Update();
    
}


