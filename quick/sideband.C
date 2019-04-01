// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //

	#include "TCanvas.h"
	#include "TChain.h"
	#include "TMath.h"
	#include "TFile.h"
	#include "TH1.h"
	#include "TLine.h"
	#include "TLegend.h"
	#include "TList.h"
	#include "TString.h"
	#include "TStyle.h"
	#include "TSystem.h"
	#include "TSystemDirectory.h"
	#include "TTimeStamp.h"
	#include <fstream>
	#include <iomanip>
	#include <iostream>
	#include <map>
	#include <sstream>
	using namespace std;



// * ============================ * //
// * ------- DECLARATIONS ------- * //
// * ============================ * //


	template<class TYPE>
	class Parameter {
	public:
		Parameter(const char *name);
		Parameter(const char *name, const TYPE val);
		void operator=(const TYPE val);
		operator TYPE() const { return fValue; }
		const char* Data() const;
		// operator const char*() const;
		bool Set(const std::string &name, const std::string &value);
	private:
		const string fName;
		TYPE fValue;
		void SetValue(const string &value);
	};


	template<> const char* Parameter<TString>::Data() const;


	void TrimString(string &line);
	void LoadSettings(const char* filename);
	template<class TYPE>
	TYPE Draw(const char *varexp, const char *selection, Option_t *option="", const char *binning=nullptr, const char *xTitle=nullptr, const char *yTitle=nullptr, const Color_t linecolor=0, const char *outputFile=nullptr, const bool buildlegend=false, const double mass=DBL_MAX, const double sigma=DBL_MAX);



// * ======================= * //
// * ------- GLOBALS ------- * //
// * ======================= * //


	// * File names * //
		Parameter<TString> gDirName  {"gDirName",   "/mnt/d/IHEP/root/D0phi_KpiKK/excl/1e6"};
		Parameter<TString> gTreeName {"gTreeName",  "fit4c_best"};
		Parameter<TString> gOutputDir{"gOutputDir", "/mnt/c/IHEP/besfs/users/deboer/BOSS_Afterburner/plots/quick/"};


	// * Switch settings (booleans) * //
		Parameter<bool> gSetLogY   {"gSetLogY",   false};
		Parameter<bool> gSetLogZ   {"gSetLogZ",   false};
		Parameter<bool> gPlotStats {"gPlotStats", false};


	// * Particle decay labels * //
		const char* gTex_D0     {"#it{D}^{0}"};
		const char* gTex_D0dec  {"#it{D}^{0} #rightarrow #it{K}^{-} #pi^{+}"};
		const char* gTex_phi    {"#phi"};
		const char* gTex_phidec {"#phi #rightarrow #it{K}^{-} #it{K}^{+}"};
		const char* gMassUnit   {"GeV/#it{c}^{2}"};


	// * Binnings full data * //
		Parameter<double> gBinsMin_phi_full   {"gBinsMin_phi_full",   .95};
		Parameter<double> gBinsMax_phi_full   {"gBinsMax_phi_full",   1.2};
		Parameter<double> gBinsWidth_phi_full {"gBinsWidth_phi_full", .01};
		Parameter<double> gBinsMin_D0_full    {"gBinsMin_D0_full",    1.6};
		Parameter<double> gBinsMax_D0_full    {"gBinsMax_D0_full",    2.0};
		Parameter<double> gBinsWidth_D0_full  {"gBinsWidth_D0_full",  .01};
		int gNBins_phi_full;
		int gNBins_D0_full;
		TString gBins_phi_full {"100"};
		TString gBins_D0_full  {"100"};
	// * Binnings 3 sigma cut * //
		Parameter<double> gBinsMin_phi_3sig   {"gBinsMin_phi_3sig",   .95};
		Parameter<double> gBinsMax_phi_3sig   {"gBinsMax_phi_3sig",   1.2};
		Parameter<double> gBinsWidth_phi_3sig {"gBinsWidth_phi_3sig", .01};
		Parameter<double> gBinsMin_D0_3sig    {"gBinsMin_D0_3sig",    1.6};
		Parameter<double> gBinsMax_D0_3sig    {"gBinsMax_D0_3sig",    2.0};
		Parameter<double> gBinsWidth_D0_3sig  {"gBinsWidth_D0_3sig",  .01};
		int gNBins_phi_3sig;
		int gNBins_D0_3sig;
		TString gBins_phi_3sig {"100"};
		TString gBins_D0_3sig  {"100"};


	TChain gChain(gTreeName.Data());
	// * determined statistically from distribution
		double gM_D0 {1.872};
		double gM_phi{1.021};
		double gSigma_D0 {0.00426};
		double gSigma_phi{0.00837};
	// * determined from fit
		// double gM_D0 {1.8719134};
		// double gM_phi{1.0192461};
		// double gSigma_D0 {0.00779};
		// double gSigma_phi{0.00989};



// * ============================= * //
// * ------- MAIN FUNCTION ------- * //
// * ============================= * //
	void sideband(const char* settingsFile="quick/D0phi_KpiKK_excl.set")
	{
		LoadSettings(settingsFile);
		// * Load directory of ROOT files * //
			TString dirname((TString)gDirName);
			if(!dirname.EndsWith("/")) dirname += "/";
			TSystemDirectory dir(dirname.Data(), dirname.Data());
			auto listOfFiles = dir.GetListOfFiles();
			if(!listOfFiles) {
				return;
			}

		// * Add files to a TChain * //
			TIter next(listOfFiles);
			while(auto obj = next()) {
				TString filename(obj->GetName());
				if(!filename.EndsWith(".root")) continue;
				TFile test((dirname+filename).Data());
				if(test.IsZombie()) continue;
				gChain.AddFile((dirname+filename).Data());
			}

		// * Check chain * //
			auto listOfAddedFiles = gChain.GetListOfFiles();
			if(!listOfAddedFiles || !listOfAddedFiles->GetEntries()) {
				return;
			}
			auto listOfBranches = gChain.GetListOfBranches();
			if(!listOfBranches || !listOfBranches->GetEntries()) {
				return;
			}
			if(!gChain.GetTree()) {
				((TFile*)listOfAddedFiles->First())->ls();
				return;
			}
			TIter next2(listOfBranches);
			while(auto obj = next2()) {
			}


		// * Create output directory * //
			gSystem->mkdir(gOutputDir.Data());

		// * Plot branches * //
			cout << endl;
			TCanvas c;

			// * D0
				Draw<TH1F*>(
					"mD0", "", "e1", gBins_D0_full,
					Form("%s (%s)", gTex_D0dec, gMassUnit),
					Form("counts per %g %s", (double)gBinsWidth_D0_full, gMassUnit),
					0, "mD0_full.pdf", false, gM_D0, gSigma_D0);
				Draw<TH1F*>(
					"mD0", Form("mphi>%f && mphi<%f", gM_phi-3*gSigma_phi, gM_phi+3*gSigma_phi), "e1", gBins_D0_3sig,
					Form("%s (%s)", gTex_D0dec, gMassUnit),
					Form("counts per %g %s", (double)gBinsWidth_D0_3sig, gMassUnit),
					kRed, "mD0_3sig.pdf", false, gM_D0, gSigma_D0);

				Draw<TH1F*>(
					"mD0", "", "e1", gBins_D0_3sig,
					Form("%s (%s)", gTex_D0dec, gMassUnit),
					Form("counts per %g %s", (double)gBinsWidth_D0_3sig, gMassUnit));
				Draw<TH1F*>( // SIDEBAND
					"mD0", "mphi>1.1 && mphi<1.5", "e1 same", gBins_D0_3sig,
					Form("%s (%s)", gTex_D0dec, gMassUnit),
					Form("counts per %g %s", (double)gBinsWidth_D0_3sig, gMassUnit),
					kOrange-2);
				Draw<TH1F*>(
					"mD0", Form("mphi>%f && mphi<%f", gM_phi-3*gSigma_phi, gM_phi+3*gSigma_phi), "e1 same", gBins_D0_3sig,
					Form("%s (%s)", gTex_D0dec, gMassUnit),
					Form("counts per %g %s", (double)gBinsWidth_D0_3sig, gMassUnit),
					kRed, "mD0.pdf", true, gM_D0, gSigma_D0);

			// * phi
				Draw<TH1F*>(
					"mphi", "", "e1", gBins_phi_full,
					Form("%s (%s)", gTex_phidec, gMassUnit),
					Form("counts per %g %s", (double)gBinsWidth_phi_full, gMassUnit),
					0, "mphi_full.pdf", false, gM_phi, gSigma_phi);
				Draw<TH1F*>(
					"mphi", Form("mD0>%f && mD0<%f", gM_D0-3*gSigma_D0, gM_D0+3*gSigma_D0), "e1", gBins_phi_3sig,
					Form("%s (%s)", gTex_phidec, gMassUnit),
					Form("counts per %g %s", (double)gBinsWidth_phi_3sig, gMassUnit),
					kRed, "mphi_3sig.pdf", false, gM_phi, gSigma_phi);

				Draw<TH1F*>(
					"mphi", "", "e1", gBins_phi_3sig,
					Form("%s (%s)", gTex_phidec, gMassUnit),
					Form("counts per %g %s", (double)gBinsWidth_phi_3sig, gMassUnit));
				Draw<TH1F*>(
					"mphi", "mD0>1.7 && mD0<2.0", "e1 same", gBins_phi_3sig,
					Form("%s (%s)", gTex_phidec, gMassUnit),
					Form("counts per %g %s", (double)gBinsWidth_phi_3sig, gMassUnit),
					kGreen+2);
				Draw<TH1F*>( // SIDEBAND
					"mphi", "mD0>1.2 && mD0<1.8", "e1 same", gBins_phi_3sig,
					Form("%s (%s)", gTex_phidec, gMassUnit),
					Form("counts per %g %s", (double)gBinsWidth_phi_3sig, gMassUnit),
					kOrange-2);
				Draw<TH1F*>(
					"mphi", Form("mD0>%f && mD0<%f", gM_D0-3*gSigma_D0, gM_D0+3*gSigma_D0), "e1 same", gBins_phi_3sig,
					Form("%s (%s)", gTex_phidec, gMassUnit),
					Form("counts per %g %s", (double)gBinsWidth_phi_3sig, gMassUnit),
					kRed, "mphi.pdf", true, gM_phi, gSigma_phi);

				Draw<TH1F*>(
					"mD0", "mphi>1.1 && mphi<1.5", "e1", gBins_D0_3sig,
					Form("%s (%s)", gTex_D0dec, gMassUnit),
					Form("counts per %g %s", (double)gBinsWidth_D0_3sig, gMassUnit),
					0, "mD0_sideband.pdf");

				Draw<TH1F*>(
					"mphi", "mD0>1.2 && mD0<1.8", "e1", gBins_phi_3sig,
					Form("%s (%s)", gTex_phidec, gMassUnit),
					Form("counts per %g %s", (double)gBinsWidth_phi_3sig, gMassUnit),
					0, "mphi_sideband.pdf");

			cout << endl;
			exit(1);
	}



// * =========================== * //
// * ------- DEFINITIONS ------- * //
// * =========================== * //


	void TrimString(string &line)
	{
		while(line.front() == ' ' || line.front() == '\t' || line.front() == '\"') line.erase(0, 1);
		while(line.back()  == ' ' || line.back()  == '\t' || line.back()  == '\"') line.pop_back();
	}


	void LoadSettings(const char* filename)
	{
		// * Attempt to open input file
			ifstream file(filename);
			if(!file.is_open()) {
				cout << "FATAL ERROR: Settings file \"" << filename << "\" cannot me open" << endl;
				terminate();
			}
		// * Check each line
			string line;
			while(getline(file, line)) {
				// Remove weird characters like EOF.
				if(line.back()<' ') line.pop_back();
				// Skip line if it does <i>not</i> contain spaces or an equal sign.
				if(line.find('=') == std::string::npos && line.find(' ') == std::string::npos) continue;
				// Remove leading and trailing spaces and tabs.
				TrimString(line);
				// Skip line if it is a comment.
				std::string beginning = line.substr(0,2);
				if(!beginning.compare("//")) continue; // C++ line comment
				if(!beginning.compare("/*")) continue; // C++ begin block comment
				if(beginning.front() == '*') continue; // C++ continue/end block comment
				if(beginning.front() == '#') continue; // bash comment
				if(beginning.front() == '%') continue; // LaTeX comment
				// Skip if line does not contain an equal sign.
				if(line.find('=') == string::npos) continue;
				// Get paramter name and value and trim.
				std::string name { line.substr(0, line.find_first_of('=')) };
				std::string val  { line.substr(line.find_first_of('=')+1) };
				TrimString(name);
				TrimString(val);
				// Set file and directory names
				if(gDirName  .Set(name, val)) continue;
				if(gTreeName .Set(name, val)) continue;
				if(gOutputDir.Set(name, val)) continue;
				// Set binnings full data
				if(gBinsMin_phi_full  .Set(name, val)) continue;
				if(gBinsMax_phi_full  .Set(name, val)) continue;
				if(gBinsWidth_phi_full.Set(name, val)) continue;
				if(gBinsMin_D0_full   .Set(name, val)) continue;
				if(gBinsMax_D0_full   .Set(name, val)) continue;
				if(gBinsWidth_D0_full .Set(name, val)) continue;
				// Set binnings 3 sigma cut
				if(gBinsMin_phi_3sig  .Set(name, val)) continue;
				if(gBinsMax_phi_3sig  .Set(name, val)) continue;
				if(gBinsWidth_phi_3sig.Set(name, val)) continue;
				if(gBinsMin_D0_3sig   .Set(name, val)) continue;
				if(gBinsMax_D0_3sig   .Set(name, val)) continue;
				if(gBinsWidth_D0_3sig .Set(name, val)) continue;
				// Set switch settings
				if(gSetLogY  .Set(name, val)) continue;
				if(gSetLogZ  .Set(name, val)) continue;
				if(gPlotStats.Set(name, val)) continue;
				// Default
				cout << "WARNING: Parameter \"" << name << "\" not defined" << endl;
			}
		// * Set new nbins full range
			gNBins_phi_full = ((gBinsMax_phi_full - gBinsMin_phi_full)/gBinsWidth_phi_full);
			gNBins_D0_full  = ((gBinsMax_D0_full  - gBinsMin_D0_full)/gBinsWidth_D0_full);
			gBins_phi_full = Form("%d,%f,%f", gNBins_phi_full, (double)gBinsMin_phi_full, (double)gBinsMax_phi_full);
			gBins_D0_full  = Form("%d,%f,%f", gNBins_D0_full , (double)gBinsMin_D0_full , (double)gBinsMax_D0_full );
		// * Set new nbins full range
			gNBins_phi_3sig = ((gBinsMax_phi_3sig - gBinsMin_phi_3sig)/gBinsWidth_phi_3sig);
			gNBins_D0_3sig  = ((gBinsMax_D0_3sig  - gBinsMin_D0_3sig)/gBinsWidth_D0_3sig);
			gBins_phi_3sig = Form("%d,%f,%f", gNBins_phi_3sig, (double)gBinsMin_phi_3sig, (double)gBinsMax_phi_3sig);
			gBins_D0_3sig  = Form("%d,%f,%f", gNBins_D0_3sig , (double)gBinsMin_D0_3sig , (double)gBinsMax_D0_3sig );
		// * Implement switch settings
			if(!gPlotStats) gStyle->SetOptStat(0);
	}

	template<class TYPE>
	TYPE Draw(const char *varexp, const char *selection, Option_t *option, const char *binning, const char *xTitle, const char *yTitle, const Color_t linecolor, const char *outputFile, const bool buildlegend, const double mass, const double sigma)
	{
		// * Add binning to varexp
			TString varExp;
			if(binning) varExp = Form("%s>>output(%s)", varexp, binning);
			else        varExp = varexp;
		// * Draw branch(es) with TTree::Draw method
			TTimeStamp stamp;
			gChain.Draw(Form("%s>>hist%d(%s)", varexp, stamp.GetNanoSec(), binning), selection, option);
		// * Get histogram from TPad
			TH1* hist = dynamic_cast<TH1*>(gPad->GetPrimitive(Form("hist%d", stamp.GetNanoSec())));
			if(!dynamic_cast<TYPE>(hist)){
				return nullptr;
			}
		// * Modify histogram
			hist->GetXaxis()->SetTitle(xTitle);
			hist->GetYaxis()->SetTitle(yTitle);
			if(linecolor) hist->SetLineColor(linecolor);
			if(gSetLogY) gPad->SetLogy();
			if(gSetLogZ) gPad->SetLogz();
			gPad->Update();
			double x1, y1, x2, y2;
			gPad->GetRangeAxis(x1, y1, x2, y2);
			if(!gSetLogY) hist->GetYaxis()->SetRangeUser(0., y2);
			else y2 = DBL_MAX;
			if(buildlegend) gPad->BuildLegend();
		// * Draw mass lines
			if(mass<DBL_MAX) {
				auto line = new TLine(mass, 0, mass, y2);
				line->SetLineStyle(9);
				line->SetLineColor(kGray+2);
				line->Draw();
			}
			if(mass<DBL_MAX) {
				auto line1 = new TLine(mass+3*sigma, 0, mass+3*sigma, y2);
				auto line2 = new TLine(mass-3*sigma, 0, mass-3*sigma, y2);
				line1->SetLineColor(kGray+2);
				line2->SetLineColor(kGray+2);
				line1->SetLineStyle(3);
				line2->SetLineStyle(3);
				line1->Draw();
				line2->Draw();
			}
		// * Set output file name and save canvas
			if(outputFile) gPad->SaveAs((gOutputDir+outputFile).Data());
		return dynamic_cast<TYPE>(hist);
	}


	template<class TYPE>
	Parameter<TYPE>::Parameter(const char *name) : fName(name) {}


	template<class TYPE>
	Parameter<TYPE>::Parameter(const char *name, const TYPE val) : fName(name), fValue(val) {}


	template<class TYPE>
	void Parameter<TYPE>::operator=(const TYPE val)
	{
		fValue = val;
	}


	template<> const char* Parameter<TString>::Data() const
	{
		return fValue.Data();
	}


	template<class TYPE>
	bool Parameter<TYPE>::Set(const std::string &name, const std::string &value)
	{
		if(fName.compare(name)) return false;
		SetValue(value);
		return true;
	}


	template<class TYPE>
	void Parameter<TYPE>::SetValue(const string &value)
	{
		stringstream ss(value);
		ss >> fValue;
	}


	template<> void Parameter<TString>::SetValue(const string &value)
	{
		fValue = value;
	}