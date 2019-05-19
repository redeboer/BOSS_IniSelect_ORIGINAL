#!/bin/bash -
# echo "Loading \"${BASH_SOURCE[0]/$(dirname ${BOSS_IniSelect})\/}\""

# * ========================= * #
# * ------- FUNCTIONS ------- * #
# * ========================= * #

	function cdiniselect()
	{
		local subfolder="${1}"
		if [ $# == 0 ] ; then
			subfolder=""
		fi
		cd "${BOSS_IniSelect}/${subfolder}"
	}
	export -f cdiniselect
	function cdbesfs()
	{
		local subfolder="${1}"
		if [ $# == 0 ] ; then
			subfolder=""
		fi
		cd "${BESFS}/${subfolder}"
	}
	export -f cdbesfs
	function cdihepbatch()
	{
		local subfolder="${1}"
		if [ $# == 0 ] ; then
			subfolder=""
		fi
		cd "${IHEPBATCH}/${subfolder}"
	}
	export -f cdihepbatch
	function cdscratchfs()
	{
		local subfolder="${1}"
		if [ $# == 0 ] ; then
			subfolder=""
		fi
		cd "${SCRATCHFS}/${subfolder}"
	}
	export -f cdscratchfs
	function cdworkarea()
	{
		local subfolder="${1}"
		if [ $# == 0 ] ; then
			subfolder=""
		fi
		cd "${BOSSWORKAREA}/workarea/${subfolder}"
	}
	export -f cdworkarea
	function cdmctruth()
	{
		local number="${1}"
		if [ $# == 0 ] ; then
			number=""
		fi
		cdworkarea Analysis/TopoAna/MctruthForTopoAnaAlg/MctruthForTopoAnaAlg-00-00-0${number}
	}
	export -f cdmctruth



# * ======================= * #
# * ------- ALIASES ------- * #
# * ======================= * #

	alias ihep6="ssh -Y ${USER}@lxslc6.ihep.ac.cn"
	alias ihep7="ssh -Y ${USER}@lxslc7.ihep.ac.cn"
	alias ihep="ihep7"

	alias cdroot="cd ${IHEPROOT}"

	alias myjobs="hep_q -u ${USER}"
	alias alljobs="hep_q | less"

	alias cdjobs="cdiniselect jobs"

	alias cdomega="cdworkarea Analysis/Physics/D0omega_K4pi/D0omega_K4pi-00-00-00"
	alias cdPsiPrime="cdworkarea Analysis/Physics/PsiPrime/PipiJpsiAlg/PipiJpsiAlg-01-00-00"
	alias cdRhopiAlg="cdworkarea Analysis/Physics/RhopiAlg/RhopiAlg-01-00-00"
	alias cdalgorithms="cdworkarea Analysis/IniSelect/IniSelect-00-00-00"
	alias cddata="cdscratchfs data"

	alias cdtopoana="cdworkarea Analysis/TopoAna/v1.6.9.1"
	alias cdmctruth1="cdmctruth 1"
	alias cdmctruth2="cdmctruth 2"
	alias cdmctruth3="cdmctruth 3"
	alias cdmctruth4="cdmctruth 4"
	alias cdmctruth5="cdmctruth 5"
	alias cdmctruth6="cdmctruth 6"

	alias gitreset="git fetch --all; git reset --hard origin/master; git pull"
	alias myjobs="hep_q -u $USER"

	alias cmtomega="clear && cdomega && cmtmake && cdjobs && boss.exe ana/D0omega_K4pi/100,000_events/ana_D0omega_K4pi_0.txt"
	alias cmtomega_all="clear && cdtracksel && cmtmake && cdomega && cmtmake && cdjobs && boss.exe ana/D0omega_K4pi/100,000_events/ana_D0omega_K4pi_0.txt"