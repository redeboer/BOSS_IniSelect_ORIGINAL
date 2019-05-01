# * Parameters * #
  BOSS_IniSelect="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
	BOSSVERSION="7.0.4"
	BOSSWORKAREA="${BOSS_IniSelect}"
	CMTHOME="/afs/ihep.ac.cn/bes3/offline/Boss/cmthome/cmthome-${BOSSVERSION}"
	CMTHOMENAME="cmthome"

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
	CheckDirectory "${BOSSWORKAREA}"
	# CheckDirectory "${CMTHOME}"