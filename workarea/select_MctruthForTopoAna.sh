targetPath="Analysis/TopoAna/MctruthForTopoAnaAlg"

echo
echo "Which version do you want to activate? You can choose:"
ls "${targetPath}"
read mctruthVersion
if [ "${mctruthVersion}" == "" ]; then
	mctruthVersion="MctruthForTopoAnaAlg-00-00-03"
fi

cd "${targetPath}/${mctruthVersion}/cmt"
cmt config && make && source setup.sh