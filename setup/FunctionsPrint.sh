#!/bin/bash -
# echo "Loading \"${BASH_SOURCE[0]/$(dirname ${BOSS_IniSelect})\/}\""

# * ============================================================================= * #
# *   DESCRIPTION: Shell script that contains definitions of functions that are   * #
# *                used in the bash scripts this folder.                          * #
# *        AUTHOR: Remco de Boer (@IHEP), EMAIL: remco.de.boer@ihep.ac.cn         * #
# *  ORGANIZATION: IHEP, CAS (Beijing, CHINA)                                     * #
# *       CREATED: 12 May 2018                                                    * #
# *         USAGE: include in another bash script with this line:                 * #
# *                source FunctionsPrint.sh                                       * #
# * ============================================================================= * #


export gColorCodeError="\e[91m"
export gColorCodeSuccess="\e[92m"
export gColorCodeInput="\e[93m"
export gColorCodeEnd="\e[0m"


function PrintErrorMessage()
{
	echo -e "${gColorCodeError}ERROR: ${1}${gColorCodeEnd}"
}
export -f PrintErrorMessage


function PrintSuccessMessage()
{
	echo -e "${gColorCodeSuccess}${1}${gColorCodeEnd}"
}
export -f PrintSuccessMessage


function AskForInput()
{
	echo -e "${gColorCodeInput}${1}${gColorCodeEnd}"
	echo -e "${gColorCodeInput}Press ENTER to continue or break with Ctrl+C or Ctrl+Z${gColorCodeEnd}"
	read -p ""
}
export -f AskForInput