echo "This script helps you 'install' BOSS:"

read -p "  - Which version of BOSS do you want to load? (default is 7.0.4) " BOSSVERSION
export BOSSVERSION=${BOSSVERSION:-7.0.4}

read -e -p "  - What do you want to be your workarea? (default \"/besfs/users/${USER}\") " BOSSWORKAREA
export BOSSWORKAREA=${BOSSWORKAREA:-/besfs/users/${USER}}

CMTHOME="/afs/ihep.ac.cn/bes3/offline/Boss/cmthome/cmthome-${BOSSVERSION}"
CMTHOMENAME="cmthome"
WORKAREANAME="workarea"

# * Check paramteres * #
	if [ ! -d "${BOSSWORKAREA}" ]; then
		echo "ERROR: Folder \"${BOSSWORKAREA}\" (BOSSWORKAREA) does not exist"
		exit 1
	fi
	if [ ! -d "${CMTHOME}" ]; then
		echo "ERROR: Folder \"${CMTHOME}\" (CMTHOME) does not exist. Check chosen BOSS version..."
		exit 1
	fi
	cmtPath="${BOSSWORKAREA}/${CMTHOMENAME}"
	waPath="${BOSSWORKAREA}/${WORKAREANAME}"
	echo
	echo "Loading BOSS version ${BOSSVERSION} to path \"${cmtPath}\""

# * Main script * #
	# * Copy BOSS CMT files
	if [[ -d "${cmtPath}" ]]; then
		read -p "  CMT home folder \""${cmtPath}"\" already exists. Remove content ($(find ${cmtPath} -type f | wc -l) files)? (y/n)" -n 1 -r
		echo
		if [[ $REPLY =~ ^[Yy]$ ]]; then
			rm -rf "${cmtPath}"
		fi
		mkdir -p "${cmtPath}"
	fi
	if [[ -d "${waPath}" ]]; then
		read -p "  Workarea home folder \""${waPath}"\" already exists. Remove content ($(find ${waPath} -type f | wc -l) files)? (y/n)" -n 1 -r
		echo
		if [[ $REPLY =~ ^[Yy]$ ]]; then
			rm -rf "${waPath}"
		fi
		mkdir -p "${waPath}"
	fi
	cp -R "${CMTHOME}/"* "${cmtPath}"
	cd "${cmtPath}"

	# * Replace lines in `requirements`
	awk '{flag = 1}
		{sub(/#macro WorkArea "\/ihepbatch\/bes\/maqm\/workarea"/,"macro WorkArea \"'${BOSSWORKAREA}'/workarea\"")}
		{sub(/#path_remove CMTPATH  "\$\{WorkArea\}"/,"path_remove  CMTPATH \"${WorkArea}\"")}
		{sub(/#path_prepend CMTPATH "\$\{WorkArea\}"/,"path_prepend CMTPATH \"${WorkArea}\"")}
		{if(flag == 1) {print $0} else {next} }' \
	"${cmtPath}/requirements" > "${cmtPath}/requirements_new"
	mv  "${cmtPath}/requirements_new" "${cmtPath}/requirements"

	# * Setup BOSS environment
	echo
	echo "__________________"
	echo "source setupCMT.sh"
	source setupCMT.sh  # starts connection to the CMT
	echo
	echo "__________"
	echo "cmt config"
	cmt config          # initiates configuration
	echo
	echo "_______________"
	echo "source setup.sh"
	source setup.sh     # sets path variables