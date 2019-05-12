#!/bin/bash -
# echo "Loading \"${BASH_SOURCE[0]/$(dirname ${BOSS_IniSelect})\/}\""

# * ============================================================================= * #
# *   DESCRIPTION: Shell script that contains definitions of functions that are   * #
# *                used in the bash scripts this folder.                          * #
# *        AUTHOR: Remco de Boer (@IHEP), EMAIL: remco.de.boer@ihep.ac.cn         * #
# *  ORGANIZATION: IHEP, CAS (Beijing, CHINA)                                     * #
# *       CREATED: 8 November 2018                                                * #
# *         USAGE: include in another bash script with this line:                 * #
# *                source Functions.sh                                            * #
# * ============================================================================= * #


# * ================================== * #
# * ------- SCRIPT INITIALISER ------- * #
# * ================================== * #


	# * Check if already being sourced * #
		if [ "${gFunctionsScriptIsSourced}" == true ]; then
			return
		fi


	# * Set identifier parameters for this script * #
		# * Parameter that blocks script from resourcing
		gFunctionsScriptIsSourced=true
		export gFunctionsScriptIsSourced
		# * Get absolute path to script
		if [[ "${BASH_SOURCE[0]}" == /* ]]; then # if absolute already
			PathToFunctionsScript="${BASH_SOURCE[0]}"
		else # if relative path
			cd "$(dirname "${BASH_SOURCE[0]}")" > /dev/null
			PathToFunctionsScript="$(pwd)/${BASH_SOURCE[0]}"
			cd - > /dev/null
		fi
		export PathToFunctionsScript



# * ============================= * #
# * ------- CMT FUNCTIONS ------- * #
# * ============================= * #


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
		local currentPath="$(pwd)"
		cdcmt
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
		local currentPath="$(pwd)"
		cdcmt
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
		local currentPath="$(pwd)"
		cdcmt
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



# * ==================================== * #
# * ------- NAVIGATION FUNCTIONS ------- * #
# * ==================================== * #


	function CheckDirectory()
	{
		local path="${1}"
		if [ ! -d "${path}" ]; then
			echo "ERROR: Folder \"${path}\" does not exist"
		fi
	}
	export -f CheckDirectory


	function CreateBaseDir()
	# Creates a base directory for a file to which you want to write.
	{
		mkdir -p "$(dirname "${1}")"
	}
	export -f CreateBaseDir


	function CdToBaseDir()
	# Creates a base directory for a file to which you want to write.
	{
		gStoredDirectory="$(pwd)"
		cd "$(dirname "${1}")"
	}
	export -f CdToBaseDir


	function CreateOrEmptyDirectory()
	# Create a directory if necessary. If it already exists, remove the already existing files (with a certain pattern).
	{
		# * Import function arguments
		local directory="${1}"
		# * Main function: empty or mkdir
		if [ -d "${directory}" ]; then
			numFiles=$(find ${directory} -type f | wc -l)
			if [ $numFiles != 0 ]; then
				AskForInput "Remove ${numFiles} files in \"${directory}\"?"
			fi
			rm -rf "${directory}"/*
		else
			mkdir -p "${directory}"
		fi
	}
	export -f CreateOrEmptyDirectory



# * ======================================= * #
# * ------- SCRIPT ACCESS FUNCTIONS ------- * #
# * ======================================= * #


	function ResourceCommonFunctions()
	# The parameter `gFunctionsScriptIsSourced` is set to `true` if this script is sourced. Use this function if you really want to bypass this safety measure and source again.
	{
		gFunctionsScriptIsSourced=false
		source "${PathToFunctionsScript}"
	}
	export -f ResourceCommonFunctions



# * =============================== * #
# * ------- CHECK FUNCTIONS ------- * #
# * =============================== * #


	function AffirmMkdir()
	# Check if the path to a folder exists. Exit the script if it doesn't.
	{
		local folderToMake="${1}"
		if [ ! -d "${folderToMake}" ]; then
			AskForInput "Create folder the following folder?\n\"${folderToMake}\""
			mkdir -p "${folderToMake}"
		fi
	}
	export -f AffirmMkdir


	function CheckIfFolderExists()
	# Check if a folder exists. Exit the script if it doesn't.
	{
		local folderToCheck="${1}"
		if [ ! -d "${folderToCheck}" ]; then
			PrintErrorMessage "Folder \"${folderToCheck}\" does not exist"
			exit
		fi
	}
	export -f CheckIfFolderExists


	function CheckIfBaseDirExists()
	# Check if the path to a folder exists. Exit the script if it doesn't.
	{
		CheckIfFolderExists "$(dirname "${1}")"
	}
	export -f CheckIfBaseDirExists


	function CheckIfFileExists()
	# Check if a file exists. Exit the script if it doesn't.
	{
		local fileToCheck="${1}"
		if [ ! -s "${fileToCheck}" ]; then
			PrintErrorMessage "File \"${fileToCheck}\" does not exist"
			exit
		fi
	}
	export -f CheckIfFileExists



# * ====================================== * #
# * ------- TEXT FILE MODIFICATION ------- * #
# * ====================================== * #

	function DeleteAllEmptyLines()
	# Delete all empty lines in a file, including files that only contain whitespace characters
	{
		# * Import function arguments
		local fileName=${1}
		# * Main function: delete all empty lines of the file
		sed -i '/^\s*$/d' ${fileName} # delete all empty lines of the file
	}
	export -f DeleteAllEmptyLines


	function FormatTextFileToCppVectorArguments()
	# Feed this function a text file, and it will prepend a `\t"` and append a `",` to each line. The comma is ommited for the last line.
	{
		# * Import function arguments
		local fileName=${1}
		# * Execute function if lines 
		local numCorrect=$(grep "^\s*\"[^\"]*\",\s*$" "${fileName}" | wc -l) # does not include last line
		local numLines=$(cat "${fileName}" | sed '/^\s*$/d' | wc -l)
		if [ $numLines != $(($numCorrect+1)) ]; then
			DeleteAllEmptyLines ${fileName}
			sed -i -e "s/.*/\t\"&\",/" ${fileName} # convert lines to C++ vector arguments
			sed -i "$ s/.$//"          ${fileName} # remove last comma
		fi
	}
	export -f FormatTextFileToCppVectorArguments


	function ChangeLineEndingsFromWindowsToUnix()
	# Windows sometimes stores files with a different type of line endings. To make the file compatible again with Unix/Linux, use this function.
	{
		# * Import function arguments
		local fileName=${1}
		# * Main function: remove Windows style newline characters
		sed -i 's/\r$//' "${fileName}"
	}
	export -f ChangeLineEndingsFromWindowsToUnix


	function SplitTextFile()
	# Feed this function a path to a text file ($1) and it will split up this file into separate files each with a maximum number of lines ($2 -- default value is 10).
	{
		# * Import function arguments
			local fileToSplit="${1}"
			local maxNLines=10 # default value
			if [ $# -ge 2 ]; then
				maxNLines=${2}
			fi
		# * Check arguments
			CheckIfFileExists "${fileToSplit}"
		# * Extract path, filename, and extension for prefix * #
			local path=$(dirname "${fileToSplit}")
			local filename=$(basename "${fileToSplit}")
			local extension="${filename/*.}"
			local filename="${filename%.*}"
			local prefix="${path}/${filename}_"
		# * Split input file * #
		rm -f "${prefix}"???".${extension}" #! remove existing files
		split -d -a3 -l${maxNLines} "${fileToSplit}" "${prefix}"
		# * Append extension again (if original file has one) * #
		# shopt -s extglob # for regex
		for file in $(ls ${prefix}???); do #! number of ? should match the -a3 argument above
			DeleteAllEmptyLines "${file}"
			if [ "${filename}" != "${extension}" ]; then
				mv -f "${file}" "${file}.${extension}"
			fi
		done
		# ! REMOVE ORIGINAL FILE ! #
			rm "${fileToSplit}"
	}
	export -f SplitTextFile



# * ====================================== * #
# * ------- LS INVENTORY FUNCTIONS ------- * #
# * ====================================== * #


	function CreateFilenameInventoryFromDirectory()
	# Feed this function a path to a directory ($1) and it will list all files within that directory including their absolute paths. This list will be written to text files ($2) with a maximum number of paths per file ($3 -- default is 0, namely no max). If you wish, you can only list files of a certain extension ($4).
	{
		# * Import function arguments
			local inputDirectory="${1}"
			local outputFile="${inputDirectory//\//_}"
				if [ "${outputFile:0:1}" == "_" ]; then
					outputFile="${outputFile:1}"
				fi
				outputFile="filenames/${outputFile}.txt"
			if [ $# -ge 2 ]; then outputFile="${2}"; fi
			local maxNLines=8 # default value
			if [ $# -ge 3 ]; then maxNLines=$3; fi
			local extension="*" # does not look for extensions by default
			if [ $# -ge 4 ]; then extension="${4}"; fi
		# * Check arguments
			CheckIfFolderExists "${inputDirectory}"
			CreateBaseDir "${outputFile}"
		# * Get absolute path of the input directory so that `find` lists absolute paths as well
			cd "${inputDirectory}"
			inputDirectory="$(pwd)"
			cd - > /dev/null
		# * Make an inventory and write to file
			find "${inputDirectory}" -type f | grep ".*.${extension}$" > "${outputFile}"
			DeleteAllEmptyLines "${outputFile}"
		# * Split the output file if required
			if [ $maxNLines -gt 0 ]; then
				SplitTextFile "${outputFile}" ${maxNLines}
			fi
	}
	export -f CreateFilenameInventoryFromDirectory


	function CreateFilenameInventoryFromFile()
	# Feed this function a path ($1) to a file containing directories and/or file names and it will list all files within those directories including their absolute paths.  This list will be written to text files ($2) with a maximum number of paths per file ($3 -- default is 0, namely no max). If you wish, you can only list files of a certain extension ($4).
	{
		# * Import function arguments * #
			local inputFile="${1}"
			local outputFile="${2}"
			local maxNLines=0 # default value
			if [ $# -ge 3 ]; then maxNLines=${3}; fi
			local extension="*" # does not look for extensions by default
			if [ $# -ge 4 ]; then extension=${4}; fi
		# * Check arguments * #
			CheckIfFileExists   "${inputFile}"
			DeleteAllEmptyLines "${inputFile}"
			CreateBaseDir "${outputFile}"
			local startPath="$(pwd)"
		# * Get absolute path of input file * #
			CdToBaseDir "${inputFile}"
			inputFile="$(pwd)/$(basename "${inputFile}")"
			cd - > /dev/null
		# * Get absolute path of output file * #
			CdToBaseDir "${outputFile}"
			outputFile="$(pwd)/$(basename "${outputFile}")"
			cd - > /dev/null
		# * Make an inventory and write to file * #
			CdToBaseDir "${inputFile}" # in case of relative paths
			echo > "${outputFile}" # empty output file
			{ cat "${inputFile}"; echo; } | while read line; do
				# * Check if empty line *
				if [ "${line}" != "" ]; then
					# * If line is a file, just add it to the output file
					if [ -f "${line}" ]; then
						echo "Added file \"$(basename ${line}\")"
						echo "${line}" >> "${outputFile}"
					# * Otherwise, presume line is a directory and add its contents
					elif [ -d "${line}" ]; then # check if directory exists
						CreateFilenameInventoryFromDirectory "${line}" "temp.txt" 0 "${extension}"
						cat "temp.txt" >> "${outputFile}"
						echo "Added directory \"$(basename ${line}\") ($(cat temp.txt | wc -l) files)"
						rm "temp.txt"
					# * Error moessage if nothing
					else
						PrintErrorMessage "WARNING: \"${line}\" does not exist"
					fi
				fi
			done
		# * Finalise output file * #
			DeleteAllEmptyLines "${outputFile}"
			PrintSuccessMessage "\nRead $(cat "${outputFile}" | wc -l) files and directories from file \"$(basename "${inputFile}")\""
			PrintSuccessMessage "--> output written to \"$(basename "${outputFile}")\"\n"
		# * Split the output file if required
			if [ $maxNLines -gt 0 ]; then
				echo "Splitting text file \"$(basename ${outputFile})\" to max $maxNLines lines each"
				SplitTextFile "${outputFile}" ${maxNLines}
			fi
		# * Go back to starting directory * #
			cd "${startPath}"
	}
	export -f CreateFilenameInventoryFromFile



# * =========================== * #
# * ------- UNIT TESTER ------- * #
# * =========================== * #


	function IniTest()
	{
		TestName="${1}"
		IniSelectObjects="${BOSS_IniSelect}/workarea/Analysis/IniSelect/IniSelect-00-00-00"

		if [[ ${#} != 1 ]]; then
			PrintErrorMessage "IniTest function needs 1 argument"
			return 1
		fi

		jobFile="UnitTests/jobs/job_${TestName}.txt"
		if [[ ! -f "${IniSelectObjects}/${jobFile}" ]]; then
			PrintErrorMessage "File \"${jobFile}\" does not exist"
			return 1
		fi

		clear && \
		cd "${IniSelectObjects}" && \
		cd cmt && \
		make && \
		cd "${IniSelectObjects}" && \
		boss.exe "UnitTests/jobs/job_${TestName}.txt"
	}
	export -f IniTest



# * ======================== * #
# * ------- EXTERNAL ------- * #
# * ======================== * #


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
			echo -e "\e[91mERROR: Failed to \"git commit -m ()\"\e[0m" # light red color code
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
	export -f gitsync


	function RunClang()
	{
		local pathToFormat="."
		local ExtensionsToFormat=( C cpp cxx h hpp )
		if [[ ${#} == 1 ]]; then
			pathToFormat="${1}"
			if [[ ! -d "${pathToFormat}" ]]; then
				echo "ERROR: Path \"${pathToFormat}\" does not exist"
				return 1
			fi
		fi
		cd "$(pwd)"
		for ext in ${ExtensionsToFormat[@]}; do
			clang-format -i $(find ${pathToFormat} -type f -iname "*.${ext}")
		done
		cd - > /dev/null
	}
	export -f RunClang


	function GenerateDoxygen()
	{
		# * SCRIPT PARAMETERS * #
			doxygenDir="doxygen"
			doxygenFile="doxygen.in"
			doxygenPath="${doxygenDir}/${doxygenFile}"
			htmlDir="docs"
			latexDir="latex"

		# * CHECK PARAMETERS * #
			if [ ! -d "${doxygenDir}" ]; then
				echo "\e[91mThis repository does not contain a Doxygen directory (\"${doxygenDir}\")\e[0m"
				exit
			fi
			if [ ! -f "${doxygenPath}" ]; then
				echo "\e[91mThis repository does not contain a Doxygen file \"${doxygenPath}\"\e[0m"
				exit
			fi

		# * PREPARE OUTPUT FOLDERS * #
			cd "${doxygenDir}"
			echo "Removed old Doxygen pages"
			rm -rf "../${htmlDir}"
			rm -rf "../${latexDir}"

		# * WRITE DOXYGEN PAGES * #
			echo "Writing Doxygen pages for repository $(basename $(pwd))"
			doxygen doxygen.in
			if [ $? == 0 ]; then
				echo -e "\e[92mSuccessfully created Doxygen documentation!\e[0m" # light green color code
			else
				echo -e "\e[91mFailed to create Doxygen documentation!\e[0m"     # light red color code
			fi

		cd - > /dev/null
	}
	export -f GenerateDoxygen