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
	echo "Writing Doxygen pages for repository $(basename $(pwd))..."
	doxygen doxygen.in
	if [ $? == 0 ]; then
		echo -e "\e[92mSuccessfully created Doxygen documentation!\e[0m" # light green color code
	else
		echo -e "\e[91mFailed to create Doxygen documentation!\e[0m"     # light red color code
	fi

cd - > /dev/null