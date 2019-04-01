#!/bin/bash -
# * ===============================================================================
# *   DESCRIPTION: Batch submit jobOption files created with CreateJobFiles.sh
# *        AUTHOR: Remco de Boer (@IHEP), EMAIL: remco.de.boer@ihep.ac.cn
# *  ORGANIZATION: IHEP, CAS (Beijing, CHINA)
# *       CREATED: 22 October 2018
# *         USAGE: bash SubmitAll.sh <analysis name>
# *     ARGUMENTS: $1 analysis name (e.g. "RhopiAlg")
# * ===============================================================================

# * ===================================================== * #
# * ------- Attempt to load script with functions ------- * #
# * ===================================================== * #

	commonFunctionsScriptName="CommonFunctions.sh"
	if [ -s "${commonFunctionsScriptName}" ]; then
		source "${commonFunctionsScriptName}"
	else
		PrintErrorMessage "ERROR: Source script \"${commonFunctionsScriptName}\" does not exist"
		exit
	fi



# ! ================================= ! #
# ! ------- Script parameters ------- ! #
# ! ================================= ! #

	outputDir="${1}"
	jobIdentifier="${2}"
	afterburnerPath="${PWD/${PWD/*BOSS_Afterburner}}" # get path of BOSS Afterburner
	scriptFolder="${afterburnerPath}/jobs/sub"



# * ========================= * #
# * ------- FUNCTIONS ------- * #
# * ========================= * #

	function CheckFolder()
	{
		folderToCheck="${1}"
		if [ ! -d ${folderToCheck} ]; then
			PrintErrorMessage "Folder \"${folderToCheck}\" does not exist. Check this script..."
			exit
		fi
	}



# * ================================ * #
# * ------- Check parameters ------- * #
# * ================================ * #

	CheckFolder ${scriptFolder}
	nJobs=$(ls ${scriptFolder}/${outputDir}/* | grep -E sub_${jobIdentifier}_[0-9]+.sh$ | wc -l)
	if [ ${nJobs} == 0 ]; then
		PrintErrorMessage "No jobs of type \"${jobIdentifier}\" available in \"${scriptFolder}/${outputDir}\""
		exit
	fi


# * ====================================== * #
# * ------- Run over all job files ------- * #
# * ====================================== * #

	AskForInput "Submit ${nJobs} jobs for \"${jobIdentifier}\"?"
	tempFilename="${temp.sh}"
	echo > ${tempFilename} # temporary fix due to submit error: "Failed to create new proc id"
	for job in $(ls ${scriptFolder}/${outputDir}/* | grep -E sub_${jobIdentifier}_[0-9]+.sh$); do
		echo "hep_sub -g physics \"${job}\"" >> ${tempFilename}
		# hep_sub -g physics "${job}"
		# if [ $? != 0 ]; then
		# 	PrintErrorMessage "Aborted submitting jobs"
		# 	exit
		# fi
	done
	bash ${tempFilename}
echo "Now run:"
echo "  bash ${tempFilename}"
echo "and use:"
echo "  hep_q -u $USER"
echo "to see which jobs you have running."
echo "Yes, it's a temporary solution..."
exit
	# rm temp.sh


# * ===================================== * #
# * ------- Final terminal output ------- * #
# * ===================================== * #

	PrintSuccessMessage "Succesfully submitted ${nJobs} \"${jobIdentifier}\" jobs"
	echo
	echo "These are your jobs:"
	hep_q -u $USER