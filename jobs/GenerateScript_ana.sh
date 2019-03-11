#!/bin/bash -
# * ===============================================================================
# *   DESCRIPTION: This example script shows (1) how create inventory files
# *                listing dst files and (2) how to use those to create a job
# *                files for data analysis. It is desinged to illustrate the use
# *                of functions in the CommonFunctions.sh script.
# *        AUTHOR: Remco de Boer <remco.de.boer@ihep.ac.cn>
# *  ORGANIZATION: IHEP, CAS (Beijing, CHINA)
# *       CREATED: 23 November 2018
# *         USAGE: bash ExampleScript_CreateDataJoboptions.sh
# * ===============================================================================

source CommonFunctions.sh

# * Scripts parameters * #
packageName="D0phi_KpiKK"
nFilesPerJob=100
nEventsPerJob=-1
outputLevel=4
data_or_MC=2 # 1: exclusive MC, 2: inclusive MC, 3: data

# * Parameters in case of file/directory reading * #
gExampleFromFile=0

# * In case of analysing EXclusive Monte Carlo output * #
	if [ ${data_or_MC} == 1 ]; then 
		directoryToRead="/scratchfs/bes/deboer/data/dst/${packageName}_MC_1e5"
		identifier="${packageName}_excl"
# * In case of analysing INclusive Monte Carlo output * #
	elif [ ${data_or_MC} == 2 ]; then
		gExampleFromFile=1
		fileToRead="directories/incl/Jpsi2009+2012_mc_dst"
		nFilesPerJob=300
		identifier="${packageName}_incl"
# * In case of analysing Monte Carlo output * #
	elif [ ${data_or_MC} == 3 ]; then
		gExampleFromFile=1
		fileToRead="directories/data/Jpsi2009+2012_dst"
		nFilesPerJob=1000
		identifier="${packageName}_data"
# * If not defined properly * #
	else
		echo "Option \"data_or_MC = ${data_or_MC}\" cannot be handled"
		exit 1
	fi

# * Create job from template and submit * #
if [ ${gExampleFromFile} == 1 ]; then
	# * This will create your job files based on a file listing dst files and directories
	CreateFilenameInventoryFromFile "${fileToRead}" "filenames/${identifier}_fromfile.txt" ${nFilesPerJob} "dst"
	bash CreateJobFiles_ana.sh "${packageName}" "filenames/${identifier}_fromfile_???.txt" ${nEventsPerJob} ${outputLevel}
else
	# * This will create your job files based on a directory containing dst files
	CreateFilenameInventoryFromDirectory "${directoryToRead}" "filenames/${identifier}.txt" ${nFilesPerJob} "dst"
	bash CreateJobFiles_ana.sh "${packageName}" "filenames/${identifier}_???.txt" ${nEventsPerJob} ${outputLevel}
fi

# * SUBMIT * #
bash SubmitAll.sh "${packageName}_data"