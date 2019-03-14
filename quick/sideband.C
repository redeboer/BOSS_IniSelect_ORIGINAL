// * ========================= * //
// * ------- LIBRARIES ------- * //
// * ========================= * //
	#include "TCanvas.h"
	#include "TChain.h"
	#include "TFile.h"
	#include "TH1.h"
	#include "TList.h"
	#include "TString.h"
	#include "TSystem.h"
	#include "TSystemDirectory.h"
	#include <fstream>
	#include <iomanip>
	#include <list>
	#include <iostream>
	#include <sstream>
	#include <map>
	using namespace std;



// * ============================ * //
// * ------- DECLARATIONS ------- * //
// * ============================ * //


	class ParameterBase {
	public:
		ParameterBase(const char* name);
		~ParameterBase();
		// static size_t GetNParameters();
		// static void PrintParameters();
	protected:
		const string fName;
	private:
		list<string> fValuesStr;
		static map<string, ParameterBase*> fInstances;
	};


	template<class TYPE>
	class Parameter : public ParameterBase {
	public:
		Parameter(const char *name);
		Parameter(const char *name, const TYPE val);
		void operator=(const TYPE val);
		operator TYPE() const { return fValue; }
		const char* Data() const;
		// operator const char*() const;
		bool Set(const std::string &name, const std::string &value);
	private:
		TYPE fValue;
		void SetValue(const string &value);
	};


	template<> const char* Parameter<TString>::Data() const;


	void TrimString(string &line);
	void LoadSettings(const char* filename);
	template<class TYPE>
	TYPE Draw(const char *varexp, const char *selection, Option_t *option="", const char *binning=nullptr, const char *xTitle=nullptr, const char *yTitle=nullptr, const char *outputFilename=nullptr);



// * ======================= * //
// * ------- GLOBALS ------- * //
// * ======================= * //


	// * File names * //
		Parameter<TString> gDirName  {"gDirName",  "/mnt/d/IHEP/root/D0phi_KpiKK/excl/1e6"};
		Parameter<TString> gTreeName {"gTreeName", "fit4c_best"};
		TString gOutputDir = "/mnt/c/IHEP/besfs/users/deboer/BOSS_Afterburner/plots/quick/";

	// * Switch settings (booleans) * //
		Parameter<bool> gSetLogY {"gSetLogY"};


	// * Particle decay labels * //
		const char* gTex_D0     {"#it{D}^{^{0}}"};
		const char* gTex_D0dec  {"#it{D}^{^{0}} #rightarrow #it{K}^{-} #pi^{+}"};
		const char* gTex_phi    {"#phi"};
		const char* gTex_phidec {"#phi #rightarrow #it{K}^{-} #it{K}^{+}"};
		const char* gMassUnit   {"MeV/#it{c}^{2}"};


	// * Binnings * //
		Parameter<double> gBinsMin_phi1  {"gBinsMin_phi",   .95};
		Parameter<double> gBinsMin_phi   {"gBinsMin_phi",   .95};
		Parameter<double> gBinsMax_phi   {"gBinsMax_phi",   1.2};
		Parameter<double> gBinsWidth_phi {"gBinsWidth_phi", .01};
		Parameter<double> gBinsMin_D0    {"gBinsMin_D0",    1.6};
		Parameter<double> gBinsMax_D0    {"gBinsMax_D0",    2.0};
		Parameter<double> gBinsWidth_D0  {"gBinsWidth_D0",  .01};
		int gNBins_phi;
		int gNBins_D0;
		TString gBins_phi {"100"};
		TString gBins_D0  {"100"};


	TChain gChain(gTreeName.Data());



// * ============================= * //
// * ------- MAIN FUNCTION ------- * //
// * ============================= * //
	void sideband(const char* settingsFile="quick/D0phi_KpiKK_excl.set")
	{
		LoadSettings(settingsFile);
		// * Load directory of ROOT files
			TString dirname((TString)gDirName);
			if(!dirname.EndsWith("/")) dirname += "/";
			TSystemDirectory dir(dirname.Data(), dirname.Data());
			auto listOfFiles = dir.GetListOfFiles();
			if(!listOfFiles) {
				return;
			}

		// * Add files to a TChain
			TIter next(listOfFiles);
			while(auto obj = next()) {
				TString filename(obj->GetName());
				if(!filename.EndsWith(".root")) continue;
				TFile test((dirname+filename).Data());
				if(test.IsZombie()) continue;
				gChain.AddFile((dirname+filename).Data());
			}

		// * Check chain
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


		// * Create output directory
			TString subDir { __FILE__ };
			subDir = subDir(subDir.Last('/')+1, subDir.Last('.')-subDir.Last('/')-1);
			subDir += "/";
			gOutputDir += subDir.Data();
			gSystem->mkdir(gOutputDir.Data());

		// * Plot branches
			TCanvas c;
			Draw<TH1F*>("mD0",  "", "e1", gBins_D0 , Form("%s (%s)", gTex_D0dec , gMassUnit), Form("counts per %g %s", (double)gBinsWidth_D0 , gMassUnit), "mD0.pdf");
			Draw<TH1F*>("mphi", "", "e1", gBins_phi, Form("%s (%s)", gTex_phidec, gMassUnit), Form("counts per %g %s", (double)gBinsWidth_phi, gMassUnit), "mphi.pdf");
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
				// Set binnings
				if(gBinsMin_phi  .Set(name, val)) continue;
				if(gBinsMax_phi  .Set(name, val)) continue;
				if(gBinsWidth_phi.Set(name, val)) continue;
				if(gBinsMin_D0   .Set(name, val)) continue;
				if(gBinsMax_D0   .Set(name, val)) continue;
				if(gBinsWidth_D0 .Set(name, val)) continue;
				// Set binnings
				if(gSetLogY.Set(name, val)) continue;
				// Default
				cout << "WARNING: Parameter \"" << name << "\" not defined" << endl;
			}
			// Set new nbins
			gNBins_phi = ((gBinsMax_phi-gBinsMin_phi)/gBinsWidth_phi);
			gNBins_D0  = ((gBinsMax_D0 -gBinsMin_D0)/gBinsWidth_D0);
			gBins_phi = Form("%d,%f,%f", gNBins_phi, (double)gBinsMin_phi, (double)gBinsMax_phi);
			gBins_D0  = Form("%d,%f,%f", gNBins_D0 , (double)gBinsMin_D0 , (double)gBinsMax_D0 );
	}

	template<class TYPE>
	TYPE Draw(const char *varexp, const char *selection, Option_t *option, const char *binning, const char *xTitle, const char *yTitle, const char *outputFilename)
	{
		// * Add binning to varexp
			TString varExp;
			if(binning) varExp = Form("%s>>output(%s)", varexp, binning);
			else        varExp = varexp;
		// * Draw branch(es) with TTree::Draw method
			gChain.Draw(Form("%s>>output(%s)", varexp, binning), selection, option);
		// * Get histogram from TPad
			TH1* hist = dynamic_cast<TH1*>(gPad->GetPrimitive("output"));
			if(!dynamic_cast<TYPE>(hist)){
				return nullptr;
			}
		// * Modify histogram and canvas layout
			if(gSetLogY) gPad->SetLogy();
			hist->GetXaxis()->SetTitle(xTitle);
			hist->GetYaxis()->SetTitle(yTitle);
			gPad->Update();
		// * Write output if required
			if(outputFilename) gPad->SaveAs((gOutputDir+outputFilename));
		return dynamic_cast<TYPE>(hist);
	}


	ParameterBase::ParameterBase(const char *name) : fName(name)
	{
		if(!(fInstances.find(fName) == fInstances.end())) {
			cout << "FATAL ERROR: Parameter \"" << fName << "\" has already been defined" << endl;
			terminate();
		}
		fInstances.emplace(make_pair(fName, this));
	}


	ParameterBase::~ParameterBase()
	{
		fInstances.erase(fName);
	}


	// size_t ParameterBase::GetNParameters()
	// {
	// 	return fInstances.size();
	// }


	template<class TYPE>
	Parameter<TYPE>::Parameter(const char *name) : ParameterBase(name) {}


	template<class TYPE>
	Parameter<TYPE>::Parameter(const char *name, const TYPE val) : ParameterBase(name), fValue(val) {}


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


	template<> void Parameter<bool>::SetValue(const string &value)
	{
		if(!value.compare("false") || !value.compare("0"))
			fValue = false;
		else fValue = true;
	}