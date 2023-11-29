
#include <TAxis.h>
#include <TCanvas.h>
#include <TDirectory.h>
#include <TH2.h>
#include <TLine.h>
#include <TPad.h>
#include <TSystem.h>
#include <TFile.h>
#include <TObject.h>




#include <iostream>


/*
JE 31.05.22
Example usage of EnergyCal, must be preloaded with .L libEnergyCal.so (which must be precompiled)
*/

void docal(){
    EnergyCal * p = new EnergyCal("Co_source.root","peaks0410.data");
    p->Start();
    p->Draw();
    
}