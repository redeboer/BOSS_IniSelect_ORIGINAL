# * Parameters * #
  BOSS_IniSelect="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
	BOSSVERSION="7.0.4"

	BESFS="${IHEPROOT}/besfs/users/${USER}"
	BOSSWORKAREA="${BOSS_IniSelect}"
	BesArea="${IHEPROOT}/afs/ihep.ac.cn/bes3/offline/Boss/${BOSSVERSION}"
	CMTHOME="${IHEPROOT}/afs/ihep.ac.cn/bes3/offline/Boss/cmthome/cmthome-${BOSSVERSION}"
	IHEPBATCH="${IHEPROOT}/ihepbatch/bes/${USER}"
	SCRATCHFS="${IHEPROOT}/scratchfs/bes/${USER}"

# * Functions * #
	function CheckDirectory()
	{
		local path="${1}"
		if [ ! -d "${path}" ]; then
			echo "ERROR: Folder \"${path}\" does not exist"
		fi
	}
	source "${BOSS_IniSelect}/jobs/CommonFunctions.sh"

# * Check paramteres * #
	CheckDirectory "${BOSS_IniSelect}"
	CheckDirectory "${BESFS}"
	CheckDirectory "${BOSSWORKAREA}"
	CheckDirectory "${BesArea}"
	CheckDirectory "${CMTHOME}"
	CheckDirectory "${IHEPBATCH}"
	CheckDirectory "${SCRATCHFS}"

# * BOSS aliases * #
	alias ihep6="ssh -Y ${USER}@lxslc6.ihep.ac.cn"
	alias ihep7="ssh -Y ${USER}@lxslc7.ihep.ac.cn"
	alias ihep="ihep7"
	alias cdroot="cd \"${IHEPROOT}\""
	alias cdworkarea="cd ${IHEPBATCH}/workarea-7.0.3"
	alias cdihepbatch="cd ${IHEPBATCH}"
	alias cdbesfs="cd ${BESFS}"
	alias cdafterburner="cd ${BOSS_Afterburner}"
	alias cdrun="cd ${IHEPBATCH}/workarea-7.0.3/TestRelease/TestRelease-00-00-86/run"
	alias myjobs="hep_q -u ${USER}"
	alias alljobs="hep_q | less"

# * Functions * #
	function gitsync()
	{
		# * Go to BOSS Afterburner main dir
		local mainDir="${BOSS_IniSelect}"
		cd ${mainDir}
		if [ $? != 0 ]; then
			echo -e "\e[91mERROR: Folder \"${mainDir}\" does not exist\"\e[0m" # light red color code
			cd - > /dev/null
			return
		fi
		# * Add all (!) files (can only be done from main folder)
		git add --all .
		if [ $? != 0 ]; then
			echo -e "\e[91mERROR: Failed to \"add -all .\"\e[0m" # light red color code
			cd - > /dev/null
			return
		fi
		# * Commit with a default description (randomiser to make unique)
		git commit -m "updated ($RANDOM)"
		if [ $? != 0 ]; then
			echo -e "\e[91mERROR: Failed to \"git commit -m (...)\"\e[0m" # light red color code
			cd - > /dev/null
			return
		else
			echo -e "\e[92mSuccessfully added and commited changes\e[0m" # light green color code
		fi
		# * Pull possible new changes and rebase
		git pull --rebase
		if [ $? != 0 ]; then
			echo -e "\e[91mERROR: Failed to \"git pull --rebase\"\e[0m" # light red color code
			cd - > /dev/null
			return
		else
			echo -e "\e[92mSuccessfully pulled from GitHub\e[0m" # light green color code
		fi
		# * Push to Git #
		git push
		if [ $? == 0 ]; then
			echo -e "\e[92mSuccessfully pushed changes to to GitHub!\e[0m" # light green color code
		fi
		cd - > /dev/null
	}
	export gitsync