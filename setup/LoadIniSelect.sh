#!/bin/bash -
# echo "Loading \"${BASH_SOURCE[0]/$(dirname ${BOSS_IniSelect})\/}\""

export BOSSWORKAREA="${BOSS_IniSelect}"

source "${BOSSWORKAREA}/${WORKAREANAME}/Analysis/IniSelect/IniSelect-00-00-00/cmt/setup.sh"
source "${BOSSWORKAREA}/${WORKAREANAME}/Analysis/TopoAna/MctruthForTopoAnaAlg/MctruthForTopoAnaAlg-00-00-03/cmt/setup.sh"
source "${BOSSWORKAREA}/${WORKAREANAME}/TestRelease/TestRelease-00-00-00/cmt/setup.sh"
source "${BOSSWORKAREA}/${WORKAREANAME}/Analysis/Physics/PsiPrime/PipiJpsiAlg/PipiJpsiAlg-01-00-00/cmt/setup.sh"
source "${BOSSWORKAREA}/${WORKAREANAME}/Analysis/Physics/RhopiAlg/RhopiAlg-01-00-00/cmt/setup.sh"
source "${BOSSWORKAREA}/${WORKAREANAME}/Analysis/Physics/D0omega_K4pi/D0omega_K4pi-00-00-00/cmt/setup.sh"