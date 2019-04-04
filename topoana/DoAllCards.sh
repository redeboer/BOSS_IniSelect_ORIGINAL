# * Import script arguments * #
	if [ $# == 0 ]; then
		echo "ERROR: Script needs a directory as parameter"
	fi
	folderToRun="${1}"

# * Main function * #
	cd "${folderToRun}"
	if [ $(ls | grep .card | wc -l) == 0 ]; then
		echo "ERROR: Path \"${folderToRun}\" does not contain topoana cards"
		cd - > /dev/null
		exit
	fi
	mkdir -p pdf
	mkdir -p tex
	for i in $(ls | grep .card); do
		topoana.exe "${i}"
		mv -f "${i/card/pdf}" pdf
		mv -f "${i/card/tex}" tex
		rm -f "${i/card/root}"
		rm -f "${i/card/txt}"
	done
	cd - > /dev/null