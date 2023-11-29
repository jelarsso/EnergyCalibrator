
#include <TAxis.h>
#include <TCanvas.h>
#include <TDirectory.h>
#include <TH2.h>
#include <TF1.h>
#include<TString.h>
#include <TLine.h>
#include <TPad.h>
#include <TSystem.h>
#include <TFile.h>
#include <TObject.h>
#include <TList.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;


//You must set how many detectors there are, and how many peaks you want to fit. 
//The number of peaks should correspond to the entries in the array of peaks (not strictly necessary but it means that the labels will be correct.)
//


#define EC_OBJECT  "EnergyCal"
#define EC_EXECNAME "EnergyCal_exec"

class EnergyCal : public TObject{
    public:
    const static int number_of_detectors = 30;
    const static int number_of_peaks = 3;
    int histo_index;
    Int_t current_detector;
    Int_t current_peak_id;
    Double_t tempgate_low;
    Double_t tempgate_high;
    Double_t fitwidth;

    string outfilename;

    Double_t peak_energies[number_of_peaks];
    Double_t peak_clicks[number_of_peaks][number_of_detectors];
    string histogram_name;

    TList * list_of_histogram_fits;
    
    TF1 * fitfunc;
    TH2D * current_energies;
    TH1D * current_slice;
    TFile * source_file;


    TF1 * coeff_func;


    EnergyCal(string, string);
    ~EnergyCal();
    void Start();
    void FitPeak(float);
    void NextDet();
    void DrawHisto();
    void Finish();
    void Zoom(int);
    void UnZoom();
    void SetGateLow(float);
    void SetGateHighFit(float);
    void LastDet();
    void ChangeHistogram();
    void RebinCurrentHistogram();


    ClassDef(EnergyCal,1);
};


void EnergyCal_exec();
void energy_cal2();