#include "IniSelect/Globals.h"


/// Test whether an MC truth particle is `91` (the PDG code of a 'cluster') or `92` (the PDG code of a string). This function is used to characterised the initial cluster, e.g. \f$J/\psi\f$
bool IniSelect::McTruth::IsInitialCluster(Event::McParticle* mcparticle)
{
  return (mcparticle->particleProperty() == 91 || mcparticle->particleProperty() == 92);
}

/// Test whether an MC truth particle is \f$J/\psi\f$ (PDG code `91`).
bool IniSelect::McTruth::IsJPsi(Event::McParticle* mcparticle)
{
  return (mcparticle->particleProperty() == 443);
}

/// Test whether an MC truth particle is \f$J/\psi\f$ (PDG code `91`).
bool IniSelect::McTruth::IsFromJPsi(Event::McParticle* mcparticle)
{
  return (mcparticle->mother().particleProperty() == 443);
}

void IniSelect::TerminalIO::ColourPrint(const char code, const TString& message,
                                        const TString& qualifier)
{
  if(qualifier.Length()) printf("\033[1;%dm%s: \033[0m", code, qualifier.Data());
  printf("\033[%dm%s\033[0m\n", code, message.Data());
}

void IniSelect::TerminalIO::ColourPrintBold(const char code, const TString& message)
{
  printf("\033[1;%dm%s\033[0m\n", code, message.Data());
}

void IniSelect::TerminalIO::PrintFatalError(const TString& message)
{
  ColourPrint(Red, message, "FATAL ERROR");
}

void IniSelect::TerminalIO::PrintSuccess(const TString& message)
{
  ColourPrint(Green, message, "SUCCESS");
}

void IniSelect::TerminalIO::PrintWarning(const TString& message)
{
  ColourPrint(Yellow, message, "WARNING");
}

void IniSelect::TerminalIO::PrintRed(const TString& message)
{
  ColourPrintBold(Red, message);
}

void IniSelect::TerminalIO::PrintGreen(const TString& message)
{
  ColourPrintBold(Green, message);
}

void IniSelect::TerminalIO::PrintYellow(const TString& message)
{
  ColourPrintBold(Yellow, message);
}

void IniSelect::TerminalIO::PrintBold(const TString& message)
{
  printf("\033[1m%s\033[0m\n", message.Data());
}