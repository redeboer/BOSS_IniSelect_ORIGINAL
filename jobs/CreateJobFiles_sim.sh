#!/bin/bash -
# * ===============================================================================
# *   DESCRIPTION: Batch create jobOption files based on a template
# *        AUTHOR: Remco de Boer <remco.de.boer@ihep.ac.cn>
# *  ORGANIZATION: IHEP, CAS (Beijing, CHINA)
# *       CREATED: 22 October 2018
# *         USAGE: bash CreateJobFiles.sh <package name> <number of jobs> <number of events> <output level>
# *     ARGUMENTS:
# *       1) package name (default is "D0phi_KpiKK")
# *       2) number of job files to be created (default is 25)
# *       3) number of events per job (default is 10,000)
# *       4) terminal output level (default is 4)
# *          2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL
# * ===============================================================================

set -e # exit if a command or function exits with a non-zero status

# * ===================================================== * #
# * ------- Attempt to load script with functions ------- * #
# * ===================================================== * #
	commonFunctionsScriptName="CommonFunctions.sh"
	if [ -s "${commonFunctionsScriptName}" ]; then
		source "${commonFunctionsScriptName}"
	else
		echo -e "\e[91mFATAL ERROR: Source script \"${commonFunctionsScriptName}\" does not exist\e[0m"
		exit
	fi


# ! ================================ ! #
# ! ------- Script arguments ------- ! #
# ! ================================ ! #
	# * (1) Package name
	packageName="D0phi_KpiKK" # default argument
	if [ $# -ge 1 ]; then packageName="${1}"; fi
	# * (2) number of jobOption files and submit scripts that need to be generated
	nJobs=25 # default argument
	if [ $# -ge 2 ]; then nJobs=${2}; fi
	# * (3) number of events per job
	nEventsPerJob=10000 # default argument
	if [ $# -ge 3 ]; then nEventsPerJob=${3}; fi
	# * (4) Terminal message output level
	outputLevel=4 # default argument: 4 (MSG::WARNING)
	if [ $# -ge 4 ]; then outputLevel=${4}; fi
	# * (5) Output subdirectory
	outputSubdir="${packageName}" # default argument
	if [ $# -ge 5 ]; then outputSubdir="${5}"; fi


# * ================================= * #
# * ------- Script parameters ------- * #
# * ================================= * #
	afterburnerPath="${PWD/${PWD/*BOSS_Afterburner}}" # get path of BOSS Afterburner
	scriptFolder="${afterburnerPath}/jobs" # contains templates and will write scripts to its subfolders
	decayCardDir="${scriptFolder}/dec"
	templateFile_sim="${scriptFolder}/templates/simulation.txt"
	templateFile_rec="${scriptFolder}/templates/reconstruction.txt"


# * =============================================== * #
# * ------- Check arguments and parameters -------  * #
# * =============================================== * #
	CheckIfFolderExists "${scriptFolder}"
	CheckIfFolderExists "${gDataOutputDir}"
	CheckIfFolderExists "${decayCardDir}"
	CheckIfFileExists "${templateFile_sim}"
	CheckIfFileExists "${templateFile_rec}"


# * ============================= * #
# * ------- Main function ------- * #
# * ============================= * #

	# * Output directories
		outputDir_sim="${scriptFolder}/sim/${outputSubdir}"
		outputDir_rec="${scriptFolder}/rec/${outputSubdir}"
		outputDir_sub="${scriptFolder}/sub/${outputSubdir}_mc"
		outputDir_raw="${gDataOutputDir}/raw/${outputSubdir}"
		outputDir_dst="${gDataOutputDir}/dst/${outputSubdir}"
		outputDir_log="${gDataOutputDir}/log/${outputSubdir}"

	# * User input
		echo "This will create ${nJobs} \"${packageName}\" simulation and reconstruction job option files with ${nEventsPerJob} events each in job."
		echo "These files will be written to folder:"
		echo "   \"${outputDir_sim}\""
		echo "   \"${outputDir_rec}\""
		echo "   \"${outputDir_sub}\""
		echo
		echo "  --> Total number of events: $(printf "%'d" $((${nJobs} * ${nEventsPerJob})))"
		echo
		AskForInput "Write ${nJobs} \"${packageName}\" simulation and reconstruction job files?"

	# * Create and EMPTY output directories
		CreateOrEmptyDirectory "${outputDir_sim}"
		CreateOrEmptyDirectory "${outputDir_rec}"
		CreateOrEmptyDirectory "${outputDir_sub}"
		CreateOrEmptyDirectory "${outputDir_raw}"
		CreateOrEmptyDirectory "${outputDir_dst}"
		CreateOrEmptyDirectory "${outputDir_log}"

	# * Loop over jobs
	for jobNo in $(seq 0 $((${nJobs} - 1))); do

		echo -en "\e[0K\rCreating files for job $(($jobNo+1))/${nJobs}..." # overwrite previous line

		# * Generate the simulation files (sim)
		randomSeed=$(($(date +%s%N) % 1000000000)) # random seed based on system time
		outputFile="${outputDir_sim}/sim_${packageName}_${jobNo}.txt"
		awk '{flag = 1}
			{sub(/__RANDSEED__/,'${randomSeed}')}
			{sub(/__OUTPUTLEVEL__/,'${outputLevel}')}
			{sub(/__DECAYCARD__/,"'${decayCardDir}'/'${packageName}'.dec")}
			{sub(/__OUTPUTFILE__/,"'${outputDir_raw}'/'${packageName}'_'${jobNo}'.rtraw")}
			{sub(/__NEVENTS__/,'${nEventsPerJob}')}
			{if(flag == 1) {print $0} else {next} }' \
		${templateFile_sim} > "${outputFile}"
		ChangeLineEndingsFromWindowsToUnix "${outputFile}"
		chmod +x "${outputFile}"

		# * Generate the reconstruction files (rec)
		outputFile="${outputDir_rec}/rec_${packageName}_${jobNo}.txt"
		awk '{flag = 1}
			{sub(/__RANDSEED__/,'${randomSeed}')}
			{sub(/__OUTPUTLEVEL__/,'${outputLevel}')}
			{sub(/__INPUTFILE__/,"'${outputDir_raw}'/'${packageName}'_'${jobNo}'.rtraw")}
			{sub(/__OUTPUTFILE__/,"'${outputDir_dst}'/'${packageName}'_'${jobNo}'.dst")}
			{sub(/__NEVENTS__/,'${nEventsPerJob}')}
			{if(flag == 1) {print $0} else {next} }' \
		"${templateFile_rec}" > "${outputFile}"
		ChangeLineEndingsFromWindowsToUnix "${outputFile}"
		chmod +x "${outputFile}"

		# * Generate the submit files (sub)
		outputFile="${outputDir_sub}/sub_${packageName}_mc_${jobNo}.sh"
		echo "#!/bin/bash" > "${outputFile}" # empty file and write first line
		echo "{ boss.exe \"${outputDir_sim}/sim_${packageName}_${jobNo}.txt\"; } &> \"${outputDir_log}/sim__${packageName}_${jobNo}.log\"" >> "${outputFile}"
		echo "{ boss.exe \"${outputDir_rec}/rec_${packageName}_${jobNo}.txt\"; } &> \"${outputDir_log}/rec_${packageName}_${jobNo}.log\"" >> "${outputFile}"
		ChangeLineEndingsFromWindowsToUnix "${outputFile}"
		chmod +x "${outputFile}"

	done
	echo


# * ===================================== * #
# * ------- Final terminal output ------- * #
# * ===================================== * #
	PrintSuccessMessage "Succesfully created ${nJobs} \"${packageName}\" job files with ${nEventsPerJob} events each\n"

set +e # exit if a command or function exits with a non-zero status