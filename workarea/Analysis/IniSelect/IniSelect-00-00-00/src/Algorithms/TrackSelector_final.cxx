#include "IniSelect/Algorithms/TrackSelector.h"

#include "IniSelect/Exceptions/AlgorithmResult.h"
using namespace std;

/// Is called at the end *of the entire process*.
/// Writes total cut flow to terminal and to the output file.
StatusCode TrackSelector::finalize()
{
  try
  {
    PrintFunctionName("TrackSelector", __func__);
    NTupleContainer::PrintFilledTuples();
    AddAndWriteCuts();
    CutObject::PrintAll();
  }
  catch(const AlgorithmFailure& e)
  {
    e.Print();
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

/// Write all cuts to a branch called "_cutvalues".
/// There will only be one entry per output file: a `C`-style array of size 3. Entry 0 of that array is the `min` value, entry 1 is the `max` value, and entry 2 is the recorded. As opposed to earlier versions of the `TrackSelector` (where the `"_cutvalues"` `TTree` contained 3 entries), we now use a `C`-style array (written through `NTuple::addIndexedItem`), because it allows to add up the number of counts when you have split up your jobs, which makes your output contain more than ROOT file. If you load these files as a `TChain`, you can get the total count of the `CutObject` by adding up all '2nd entries' (the counts) of each branch.
/// @see BOSSOutputLoader::PrintCuts
void TrackSelector::AddAndWriteCuts()
{
  list<CutObject*>::iterator cut   = CutObject::gCutObjects.begin();
  NTuple::Item<int>&         index = *fNTuple_cuts.AddItem<int>("index", 0, 3);
  for(; cut != CutObject::gCutObjects.end(); ++cut)
  {
    NTuple::Array<double>& array = *fNTuple_cuts.AddIndexedItem<double>((*cut)->Name(), index);

    index        = 0;
    array[index] = (*cut)->min;
    index        = 1;
    array[index] = (*cut)->max;
    index        = 2;
    array[index] = (int)((*cut)->counter);
  }
  ++index; // to make the array size 3
  fNTuple_cuts.Write();
}

void TrackSelector::PutParticleInCorrectVector(Event::McParticle* mcParticle)
{
  int pdgCode = mcParticle->particleProperty();

  CandidateTracks<Event::McParticle>* coll = fFinalState.GetParticleSelectionMC().FirstParticle();
  while(coll)
  {
    if(coll->GetPdgCode() == pdgCode)
    {
      coll->AddTrack(mcParticle);
      return;
    }
    coll = fFinalState.GetParticleSelectionMC().NextCharged();
  }
  fLog << MSG::DEBUG << "PDG code " << pdgCode << " does not exist in ParticleSelectionMC" << endmsg;
}