#ifndef Physics_JpsiToDPV_new_H
#define Physics_JpsiToDPV_new_H

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "IniSelect/IO/OutputFile.h"
#include "IniSelect/IO/DstFile.h"
#include "IniSelect/Containers/CutObject.h"
#include "JpsiToDPV/TreeCollectionSimple.h"
#include "JpsiToDPV/Types/D0omega.h"
#include "JpsiToDPV/Types/D0phi.h"
#include <string>

#define DECLAREWRITE(TREE) declareProperty(TREE.PropertyName(), TREE.write)

/// @addtogroup BOSS_packages
/// @{
/// @author   Remco de Boer 雷穆克 (r.e.deboer@students.uu.nl or remco.de.boer@ihep.ac.cn)
/// @date     May 29th, 2018
class JpsiToDPV_new : public Algorithm
{
public:
  JpsiToDPV_new(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  MsgStream  fLog;
  DstFile    fInputFile(eventSvc());
  OutputFile fOutputFile;

  D0omega::Package     fD0omega;
  D0phi::Package       fD0phi;
};
/// @}
#endif