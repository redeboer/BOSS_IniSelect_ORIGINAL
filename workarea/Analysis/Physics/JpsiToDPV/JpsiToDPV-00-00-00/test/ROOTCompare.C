#include "ObjComparer.h"
#include "TFile.h"
#include "TSystem.h"
#include <iostream>

#include "TFile.h"
#include "TROOT.h"
#include "TSystem.h"
#include <string>
#include <cstdlib>

// Wrap command line stuff in this namespace
// Command line argument explanation:
// 1. Path to a ROOT file to be produced.
// 2. Name of object in the ROOT file. This is optional and if it's left blank
//    the ROOT file itself is assumed to be the object.
// Positional arguments have to follow the ones above:
// -d | --diff The ROOT file with object to compare to. Again, the object name
//             can be laft out to use the ROOT file itself.
// -l | --libraries Pass paths to libraries to be loaded by ROOT. This is mostly
//                  useful/required by ROOT version < 6.
// -i | --ignoreROOTVersion Use if ROOT version < 6 and don't want to load
//                          don't want/need to load libraries.
// -h | --help Print a brief help message.

// Namespace wrapping the command line processing together with related error
// handling.
namespace cmd
{

  // File and object the first file/object should be compared to.
  const char* fileDiffParam[3] = {"-d", "--diff", "<diff_filepath> [<object name>]"};
  // Library paths to be loaded
  const char* libParam[3] = {"-l", "--libraries",
                             "<list of additional libraries to be loaded (ROOT version < 6)>"};
  // Ignore no libraries present in case of ROOT version < 6
  const char* ignoreROOTVersionParam[3] = {"-i", "--ignoreROOTVersion",
                                           "ignore lib warning ROOT version < 6"};
  // Help
  const char* helpParam[3] = {"-h", "--help", "Show this message and exit"};

  // Print brief help message
  void printHelp()
  {
    std::cerr << "Arguments:\n"
              << "\t<filepath> [<object_name>]\n"
              << "\t[" << fileDiffParam[0] << " | " << fileDiffParam[1] << " : " << fileDiffParam[2]
              << " ]\n"
              << "\t[ " << libParam[0] << " | " << libParam[1] << " : " << libParam[2] << " ]\n"
              << "\t[ " << ignoreROOTVersionParam[0] << " | " << ignoreROOTVersionParam[1] << " : "
              << ignoreROOTVersionParam[2] << " ]\n"
              << "\t" << helpParam[0] << " | " << helpParam[1] << " : " << helpParam[2] << "\n";
  }

  // Parse passed command line arguments
  int parseArgs(int argc, char* argv[], std::string& fileName1, std::string& objectPath1,
                std::string& fileName2, std::string& objectPath2,
                std::vector<std::string>& libraryNames, bool& ignoreROOTVersion)
  {
    // First, check for help
    for(int i = 0; i < argc; i++)
    {
      if(strcmp(helpParam[0], argv[i]) == 0 || strcmp(helpParam[1], argv[i]) == 0)
      {
        printHelp();
        return 0;
      }
    }

    // Check whether there is a trailing parameter without arg
    if(argv[argc - 1][0] == '-')
    {
      std::cerr << "Trailing parameter " << argv[argc - 1] << "\n";
      return -1;
    }

    // Collect paths to libraries.
    libraryNames.clear();
    // Flag for ignoring the ROOT version < 6.
    ignoreROOTVersion = false;
    // At least one file path is required.
    if(argc < 2)
    {
      std::cerr << "No arguments found\n";
      return -1;
    }
    // First argument must not be a positional but the ROOT file path.
    if(argv[1][0] == '-')
    {
      std::cerr << "Positional arg found as first argument\n";
      return -1;
    }
    // Where to start positional argument extraction
    int argCountStart = 3;
    // Count number of object packs. At least 1 will be there because otherwise
    // extraction will be aborded.
    int objectPacks = 1;
    // Now pass the first argument as filename and extract optional object name
    // therein.
    fileName1 = argv[1];
    if(argc > 2)
    {
      if(argv[2][0] != '-')
      {
        objectPath1 = argv[2];
        argCountStart++;
      }
      else
      {
        objectPath1 = "";
      }
    }

    // Extract trailing positional arguments.
    for(int i = argCountStart; i < argc; i += 2)
    {
      // First check flags without values
      if(strcmp(ignoreROOTVersionParam[0], argv[i - 1]) == 0 ||
         strcmp(ignoreROOTVersionParam[1], argv[i - 1]) == 0)
      {
        ignoreROOTVersion = true;
        // Decrement because only skipping one and not two arguments.
        i--;
        continue;
      }
      // Check for positional argument
      if(argv[i - 1][0] != '-')
      {
        std::cerr << "Not a positional argument: " << argv[i - 1] << "\n";
        return -1;
      }

      // Check for missing value of positional argument
      if(argv[i][0] == '-')
      {
        std::cerr << "Positional argument missing for " << argv[i - 1] << "\n";
        return -1;
      }

      // Check for libraries to be loaded and append to list.
      if(strcmp(libParam[0], argv[i - 1]) == 0 || strcmp(libParam[1], argv[i - 1]) == 0)
      {
        // Load all libs until next parameter is found indicated by '-' as
        // first character
        for(int j = i; j < argc; j++)
        {
          if(argv[j][0] == '-') { break; }
          // Increment i since we moved one step
          i++;
          libraryNames.push_back(argv[j]);
        }
        // Want to get the argument following immediately after that so
        // decrement i by 1.
        i--;
        continue;
        // Extract the diff file file object name (if any)
      }
      else if(strcmp(fileDiffParam[0], argv[i - 1]) == 0 ||
              strcmp(fileDiffParam[1], argv[i - 1]) == 0)
      {
        if(objectPacks > 1)
        {
          std::cerr << "Found too many diff files. 1 is maximum\n";
          return -1;
        }
        objectPacks++;
        fileName2 = argv[i];
        // If next is a parameter, leave the object name empty, otherwise set
        // object path.
        if(i + 1 < argc)
        {
          if(argv[i + 1][0] == '-') { objectPath2 = ""; }
          else
          {
            objectPath2 = argv[i + 1];
            // Move i by 1 since the object name was found.
            i++;
          }
        }
        // No point in comparing the same files and objects.
        if(fileName1.compare(fileName2) == 0 && objectPath1.compare(objectPath2) == 0)
        {
          std::cerr << "File paths and object names are identical\n";
          return -1;
        }
        continue;
        // Skip unknowns.
      }
      else
      {
        std::cerr << "Skip unknown parameter " << argv[i - 1] << "\n";
        // Skip this so decrement i since it's incremented by 2 next time the
        // loop is executed.
        i--;
      }
    }

    return objectPacks;
  } // end parseArgs
} // end namespace cmd

// Wrapping file opening and object extraction
TObject* loadObject(const std::string& fileName, TFile*& file, const std::string& entry)
{
  file = TFile::Open(fileName.c_str());
  if(!file)
  {
    std::cerr << "Cannot open file " << fileName << "\n";
    exit(1);
  }
  TObject* obj;
  // If entry is empty treat file itself as object
  if(entry.compare("") == 0)
  {
    obj = (TObject*)file;
    std::cout << "Taking file " << fileName << " as object to be processed\n";
  }
  else
  {
    obj = (TObject*)file->Get(entry.c_str());
    if(!obj)
    {
      std::cerr << "File " << fileName << " does not contain an object for entry " << entry << "\n";
      exit(1);
    }
    std::cout << "Processing entry " << entry << " in file " << fileName << "\n";
  }
  return obj;
}

// Print an object in a TFile
void printEntry(const std::string& fileName, const std::string& entry)
{
  TFile* file = TFile::Open(fileName.c_str());
  if(!file)
  {
    std::cerr << "Cannot open file " << fileName << "\n";
    return;
  }
  TObject* obj = loadObject(fileName, file, entry);

  ROOTObjComparer cmp;
  cmp.SetDebug(false);
  cmp.Print(obj);

  // for debugging purposes, can dump XML here via ROOT serialization
  // dump them to XML just to have a text comparison also
  TString outxml(fileName.c_str());
  outxml += ".xml";
  TFile* file1xml = TFile::Open(outxml.Data(), "RECREATE");
  if(file1xml)
  {
    std::cout << "Writing XML for comparision\n";
    file1xml->WriteTObject(obj);
    file1xml->Close();
  }
  file->Close();
}

// Compare two objects in a TFile
void compareEntry(const std::string& fileName1, const std::string& objectName1,
                  const std::string& fileName2, const std::string& objectName2)
{

  TFile*   file1;
  TObject* obj1 = loadObject(fileName1, file1, objectName1);
  TFile*   file2;
  TObject* obj2 = loadObject(fileName2, file2, objectName2);

  ROOTObjComparer cmp;
  cmp.SetVerbose(false);
  cmp.SetDebug(false);
  cmp.Diff(obj1, obj2);

  file1->Close();
  file2->Close();
}

int main(int argc, char* argv[])
{

  // Prepare file and object names as well as vector and flag for additional
  // libraries.
  std::string              fileName1;
  std::string              fileName2;
  std::string              objectPath1;
  std::string              objectPath2;
  bool                     ignoreROOTVersion = false;
  std::vector<std::string> libraryNames;

  int nPacks = cmd::parseArgs(argc, argv, fileName1, objectPath1, fileName2, objectPath2,
                              libraryNames, ignoreROOTVersion);
  // Leave if errors occurred.
  if(nPacks < 0)
  {
    std::cerr << "Errors occurred\n";
    cmd::printHelp();
    return 1;
  }

  // Check if ROOT version < 6 and warn or ignore depending on value of
  // ignoreROOTVersion
  std::string ROOTVersion(gROOT->GetVersion());
  bool        isLowerROOTVersion = (ROOTVersion.find("6.") == std::string::npos);
  if(libraryNames.size() == 0 && isLowerROOTVersion && !ignoreROOTVersion)
  {
    std::cerr << "WARNING: No libs loaded although you are using "
              << "ROOT version < 6. Pass '-i' flag to ignore this.\n";
    cmd::printHelp();
  }

  // Load libraries.
  std::vector<std::string>::iterator it = libraryNames.begin();
  for(; it != libraryNames.end(); ++it)
  {
    if(gSystem->Load(it->c_str()) < 0)
    {
      std::cerr << "Cannot load library " << *it << "\n";
      return 1;
    }
  }

  // Print if only one ROOT file/object encountered.
  if(nPacks == 1)
  {
    std::cout << "#########\n"
              << "# Print #\n"
              << "#########\n";
    printEntry(fileName1, objectPath1);
    // Diff otherwise
  }
  else if(nPacks == 2)
  {
    std::cout << "###########\n"
              << "# Compare #\n"
              << "###########\n";
    compareEntry(fileName1, objectPath1, fileName2, objectPath2);
  }

  // Just return in any other case.
  return 0;
}
