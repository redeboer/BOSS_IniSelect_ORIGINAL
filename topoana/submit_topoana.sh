# * Input parameters * #
	inputCard="${1}"
	inputCard="${inputCard/.card}"
	echo "${inputCard}"

# * Script parameters * #
	inputCardFilename="$(basename ${inputCard})"
	echo "${inputCardFilename}"

# * Main function * #
	echo "topoana.exe \"${inputCard}.card\" > ${inputCardFilename}.log" > "${inputCardFilename}.sh"
	chmod +x "${inputCardFilename}.sh"
	read -p "Submit \"${inputCardFilename}.sh\" to queue? (press ENTER to continue...) "