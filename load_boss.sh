# * Repository locations * #
	BESFS="/besfs/users/${USER}"
	BOSS_Afterburner="${BESFS}/BOSS_Afterburner"
	BOSS_IniSelect="${BESFS}/BOSS_IniSelect"
	BOSS_GitBook="${BESFS}/BOSS_GitBook"
	export BOSS_Afterburner
	export BOSS_IniSelect
	export BOSS_GitBook

# * BOSS variables * #
	BOSSVERSION="7.0.4"
	BOSSWORKAREA="${BOSS_IniSelect}"
	CMTHOME="/afs/ihep.ac.cn/bes3/offline/Boss/cmthome/cmthome-${BOSSVERSION}"
	CMTHOMENAME="cmthome"
	IHEPBATCH="/ihepbatch/bes/${USER}"
	SCRATCHFS="/scratchfs/bes/${USER}"
	WORKAREANAME="workarea"
	export BOSSVERSION
	export BESFS
	export BOSSWORKAREA
	export IHEPBATCH
	export SCRATCHFS
	# export CMTHOME # ! do not export, inconsistent with GaudiPolicy

# * Setup BOSS area * #
	source "${BOSSWORKAREA}/${CMTHOMENAME}/setupCMT.sh"
	source "${BOSSWORKAREA}/${CMTHOMENAME}/setup.sh"
	source "${BOSSWORKAREA}/${WORKAREANAME}/Analysis/IniSelect/Algorithms/D0omega_K4pi/D0omega_K4pi-00-00-00/cmt/setup.sh"
	source "${BOSSWORKAREA}/${WORKAREANAME}/Analysis/IniSelect/Algorithms/D0phi_3K3pi/D0phi_3K3pi-00-00-00/cmt/setup.sh"
	source "${BOSSWORKAREA}/${WORKAREANAME}/Analysis/IniSelect/Algorithms/D0phi_KpiKK/D0phi_KpiKK-00-00-00/cmt/setup.sh"
	source "${BOSSWORKAREA}/${WORKAREANAME}/Analysis/IniSelect/Algorithms/D0phi_KpipiKK/D0phi_KpipiKK-00-00-00/cmt/setup.sh"
	source "${BOSSWORKAREA}/${WORKAREANAME}/Analysis/IniSelect/Algorithms/TrackSelector/TrackSelector-00-00-00/cmt/setup.sh"
	source "${BOSSWORKAREA}/${WORKAREANAME}/Analysis/IniSelect/IniSelect/IniSelect-00-00-00/cmt/setup.sh"
	source "${BOSSWORKAREA}/${WORKAREANAME}/Analysis/TopoAna/MctruthForTopoAnaAlg/MctruthForTopoAnaAlg-00-00-03/cmt/setup.sh"
	source "${BOSSWORKAREA}/${WORKAREANAME}/TestRelease/TestRelease-00-00-00/cmt/setup.sh"
	source "${BOSSWORKAREA}/${WORKAREANAME}/Analysis/Physics/PsiPrime/PipiJpsiAlg/PipiJpsiAlg-01-00-00/cmt/setup.sh"
	export PATH=$PATH:/afs/ihep.ac.cn/soft/common/sysgroup/hep_job/bin/
	export PATH=$PATH:${BOSSWORKAREA}/${WORKAREANAME}/Analysis/TopoAna/v1.6.9/bin

# * Navigation functions * #
	function cdafterburner()
	{
		local subfolder="${1}"
		if [ $# == 0 ] ; then
			subfolder=""
		fi
		cd "${BOSS_Afterburner}/${subfolder}"
	}
	export cdafterburner # navigate to BOSS Afterburner (optional argument: subfolder)
	function cdiniselect()
	{
		local subfolder="${1}"
		if [ $# == 0 ] ; then
			subfolder=""
		fi
		cd "${BOSS_IniSelect}/${subfolder}"
	}
	export cdiniselect # navigate to BOSS IniSelect (optional argument: subfolder)
	function cdbesfs()
	{
		local subfolder="${1}"
		if [ $# == 0 ] ; then
			subfolder=""
		fi
		cd "${BESFS}/${subfolder}"
	}
	export cdbesfs # navigate to besfs folder (optional argument: subfolder)
	function cdihepbatch()
	{
		local subfolder="${1}"
		if [ $# == 0 ] ; then
			subfolder=""
		fi
		cd "${IHEPBATCH}/${subfolder}"
	}
	export cdihepbatch # navigate to ihepbatch folder (optional argument: subfolder)
	function cdscratchfs()
	{
		local subfolder="${1}"
		if [ $# == 0 ] ; then
			subfolder=""
		fi
		cd "${SCRATCHFS}/${subfolder}"
	}
	export cdscratchfs # navigate to scratchfs folder (optional argument: subfolder)
	function cdworkarea()
	{
		local subfolder="${1}"
		if [ $# == 0 ] ; then
			subfolder=""
		fi
		cd "${BOSSWORKAREA}/workarea/${subfolder}"
	}
	export cdworkarea # navigate to workarea (optional argument: subfolder)
	alias cdjobs="cdiniselect jobs"
	function cdmctruth()
	{
		local number="${1}"
		if [ $# == 0 ] ; then
			number=""
		fi
		cdworkarea Analysis/TopoAna/MctruthForTopoAnaAlg/MctruthForTopoAnaAlg-00-00-0${number}
	}
	export cdmctruth # navigate to BOSS Afterburner (optional argument: subfolder)

# * BOSS functions * #
	function AttemptToExecute()
	{
		local commandToExecute="${1}"
		echo -e "\n\n--== EXECUTING \"${commandToExecute}\" ==--"
		${commandToExecute}
		if [ $? != 0 ]; then
			echo "ERROR: failed to execute \"${commandToExecute}\""
			return 1
		fi
	}
	export AttemptToExecute
	function cdcmt()
	{
		# * Attempt to move to last cmt folder
		local currentPath="$(pwd)"
		if [ "$(basename "${currentPath}")" != "cmt" ]; then
			local cmtFolder="$(find -name cmt | head -1)"
			if [ "${cmtFolder}" == "" ]; then
				echo "ERROR: no cmt folder available!"
				return 1
			fi
			cd "${cmtFolder}"
		fi
	}
	export cdcmt
	function cmtbroadcast()
	{
		cdcmt
		local currentPath="$(pwd)"
		# * Print package and version name
		echo
		echo "=================================="
		echo "BROADCASTING PACKAGE \"$(basename $(dirname $(pwd)))\""
		echo "=================================="
		# * Connect your workarea to BOSS
		AttemptToExecute "cmt broadcast"
		if [ $? != 0 ]; then return 1; fi
		# * Perform setup and cleanup scripts
		AttemptToExecute "cmt config"
		if [ $? != 0 ]; then return 1; fi
		# * Build and connect executables to BOSS
		AttemptToExecute "cmt broadcast make"
		if [ $? != 0 ]; then return 1; fi
		# * Set bash variables
		AttemptToExecute "source setup.sh"
		if [ $? != 0 ]; then return 1; fi
		# * Move back to original path
		cd "${currentPath}"
	}
	export cmtbroadcast
	function cmtconfig()
	{
		cdcmt
		local currentPath="$(pwd)"
		# * Print package and version name
		echo; echo
		echo "====================================="
		echo "BUILDING PACKAGE \"$(basename $(dirname $(pwd)))\""
		echo "====================================="
		# * Create CMT scripts
		AttemptToExecute "cmt config"
		if [ $? != 0 ]; then return 1; fi
		# * Build executables
		AttemptToExecute "make"
		if [ $? != 0 ]; then return 1; fi
		# * Make package available to BOSS
		AttemptToExecute "source setup.sh"
		if [ $? != 0 ]; then return 1; fi
		# * Move back to original path
		cd "${currentPath}"
		return 0
	}
	export cmtconfig
	function cmtmake()
	{
		cdcmt
		local currentPath="$(pwd)"
		# * Print package and version name
		echo; echo
		echo "====================================="
		echo "BUILDING PACKAGE \"$(basename $(dirname $(pwd)))\""
		echo "====================================="
		# * Build executables
		AttemptToExecute "make"
		if [ $? != 0 ]; then return 1; fi
		# * Move back to original path
		cd "${currentPath}"
		return 0
	}
	export cmtmake