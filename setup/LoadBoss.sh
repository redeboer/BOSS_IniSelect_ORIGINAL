#!/bin/bash -
# echo "Loading \"${BASH_SOURCE[0]/$(dirname ${BOSS_IniSelect})\/}\""

# * BOSS paths and parameters * #
export BOSSVERSION="7.0.4"
export CMTHOMENAME="cmthome"

export BESFS="/besfs/users/${USER}"
export BOSSWORKAREA="/besfs/users/deboer/BOSS_IniSelect"
export IHEPBATCH="/ihepbatch/bes/${USER}"
export SCRATCHFS="/scratchfs/bes/${USER}"
CMTHOME="/afs/ihep.ac.cn/bes3/offline/Boss/cmthome/cmthome-${BOSSVERSION}"


# * Load BOSS essentials * #
source "${BOSSWORKAREA}/${CMTHOMENAME}/setupCMT.sh"
source "${BOSSWORKAREA}/${CMTHOMENAME}/setup.sh"
export PATH=${PATH}:/afs/ihep.ac.cn/soft/common/sysgroup/hep_job/bin/
export PATH=${PATH}:/besfs/users/${USER}/topoana/bin