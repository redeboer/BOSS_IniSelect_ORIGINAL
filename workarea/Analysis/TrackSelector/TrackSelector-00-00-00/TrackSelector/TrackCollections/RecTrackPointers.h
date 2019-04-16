#ifndef Analysis_RecTrackPointers_H
#define Analysis_RecTrackPointers_H

#include "EmcRecEventModel/RecEmcShower.h"
#include "ExtEvent/RecExtTrack.h"
#include "MdcRecEvent/RecMdcDedx.h"
#include "MdcRecEvent/RecMdcKalTrack.h"
#include "MdcRecEvent/RecMdcTrack.h"
#include "MucRecEvent/RecMucTrack.h"
#include "TofRecEvent/RecTofTrack.h"

/// @addtogroup BOSS_objects
/// @{

/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     April 16th, 2018
class RecTrackPointers
{
protected:
  RecEmcShower* fTrackEMC;
  ///< Pointer to reconstructed data from the EMC. See more info in [`RecEmcShower`](http://bes3.to.infn.it/Boss/7.0.2/html/classRecEmcShower.html).
  RecExtTrack* fTrackExt;
  ///< Pointer to reconstructed data from the extension through all detectors. See more info in [`RecExtTrack`](http://bes3.to.infn.it/Boss/7.0.2/html/classRecExtTrack.html).
  RecMdcDedx* fTrackDedx;
  ///< Pointer to reconstructed \f$dE/dx\f$ data from the MDC. See more info in [`RecMdcDedx`](http://bes3.to.infn.it/Boss/7.0.2/html/classRecMdcDedx.html).
  RecMdcKalTrack* fTrackKal;
  ///< Pointer to reconstructed Kalman fit data from the MDC. See more info in [`RecMdcKalTrack`](http://bes3.to.infn.it/Boss/7.0.2/html/classRecMdcKalTrack.html).
  RecMdcTrack* fTrackMDC;
  ///< Pointer to reconstructed track data from the MDC. See more info in [`RecMdcTrack`](http://bes3.to.infn.it/Boss/7.0.2/html/classRecMdcTrack.html).
  RecMucTrack* fTrackMUC;
  ///< Pointer to reconstructed data from the MUC. See more info in [`RecMucTrack`](http://bes3.to.infn.it/Boss/7.0.2/html/classRecMucTrack.html).
  RecTofTrack* fTrackTOF;
  ///< Pointer to reconstructed data from the TOF. See more info in [`RecTofTrack`](http://bes3.to.infn.it/Boss/7.0.2/html/classRecTofTrack.html).
};

/// @}
#endif