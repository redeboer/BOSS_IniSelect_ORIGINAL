#!/bin/bash -
# echo "Loading \"${BASH_SOURCE[0]/$(dirname ${BOSS_IniSelect})\/}\""

export BOSS_IniSelect_OutputDir="${SCRATCHFS}/data"

source "${BOSS_IniSelect}/workarea/Analysis/IniSelect/IniSelect-00-00-00/cmt/setup.sh"
source "${BOSS_IniSelect}/workarea/Analysis/Physics/D0omega_K4pi/D0omega_K4pi-00-00-00/cmt/setup.sh"
source "${BOSS_IniSelect}/workarea/Analysis/Physics/D0omega_Kenu3pi/D0omega_Kenu3pi-00-00-00/cmt/setup.sh"
source "${BOSS_IniSelect}/workarea/Analysis/Physics/D0phi_3Kpi/D0phi_3Kpi-00-00-00/cmt/setup.sh"
source "${BOSS_IniSelect}/workarea/Analysis/Physics/JpsiToDPV/JpsiToDPV-00-00-00/cmt/setup.sh"
source "${BOSS_IniSelect}/workarea/Analysis/Physics/PsiPrime/PipiJpsiAlg/PipiJpsiAlg-01-00-00/cmt/setup.sh"
source "${BOSS_IniSelect}/workarea/Analysis/Physics/RhopiAlg/RhopiAlg-01-00-00/cmt/setup.sh"
source "${BOSS_IniSelect}/workarea/Analysis/TopoAna/MctruthForTopoAnaAlg/MctruthForTopoAnaAlg-00-00-03/cmt/setup.sh"
source "${BOSS_IniSelect}/workarea/TestRelease/TestRelease-00-00-00/cmt/setup.sh"