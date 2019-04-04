bossVersion="6.6.5"
inputPath="/besfs/users/zhouxy/workarea/workarea-${bossVersion}/Analysis/Physics/MctruthForTopoAnaAlg"
targetPath="Analysis/TopoAna/MctruthForTopoAnaAlg"

if [ -d "${targetPath}" ]; then
	echo
	echo "This script will import and OVERWRITE all versions of MctruthForTopoAna to the BOSS Afterburner"
	read -p "Press ENTER to continue..."
	echo "Removing old MctruthForTopoAna from \"${targetPath}\"..."
	rm -rf "${targetPath}"
else
	mkdir -p "${targetPath}"
fi
cp -R "${inputPath}" "$(dirname ${targetPath})"