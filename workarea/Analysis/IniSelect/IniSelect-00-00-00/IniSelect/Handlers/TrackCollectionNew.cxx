#include "IniSelect/Handlers/PID.h"
#include "IniSelect/Handlers/TrackCollectionNew.h"
using namespace std;

void TrackCollectionNew::PushBack(EvtRecTrack* trk)
{
  auto pdgCode = PID::FindMostProbable(trk);
  switch(pdgCode)
  {
    case 11: e.neg.push_back(trk); break;
    case -11: e.pos.push_back(trk); break;
    case 211: pi.pos.push_back(trk); break;
    case -211: pi.neg.push_back(trk); break;
    case 321: K.pos.push_back(trk); break;
    case -321: K.neg.push_back(trk); break;
    case 13: mu.neg.push_back(trk); break;
    case -13: mu.pos.push_back(trk); break;
    case 2212: p.pos.push_back(trk); break;
    case -2212: p.neg.push_back(trk); break;
    default: throw Exception("Undefined PID case: PDG code %s", pdgCode);
  }
}