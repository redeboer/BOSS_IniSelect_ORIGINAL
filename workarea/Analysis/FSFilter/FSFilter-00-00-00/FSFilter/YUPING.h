#ifndef YUPING_H
#define YUPING_H

#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
#include "MdcRecEvent/RecMdcKalTrack.h"
#include "MdcRecEvent/RecMdcTrack.h"
#include "VertexFit/WTrackParameter.h"
#include <vector>

#include "TH1.h"

class YUPING
{

public:
  static void corgen(HepMatrix&, HepVector&, int);
  static void corset(HepSymMatrix&, HepMatrix&, int);
  static void calibration(RecMdcKalTrack*, HepVector&, int);
};

#endif
