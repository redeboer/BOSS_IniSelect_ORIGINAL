# Run this script if you failed to build Geant,
# but still want to have an include path

geantDir="extLibs/geant4"
if [[ ! -d "${geantDir}" ]]; then
  echo "ERROR: Geant directory \"${geantDir}\" does not exist"
  exit 1
fi
extensions=(
  hh
  icc
)
cd "${geantDir}"
mkdir -p include
rm -rf include/*
for ext in ${extensions[@]}; do
  echo "Copying files with extention \"${ext}\""
  { cp -f $(find -type f -iname "*.${ext}") include/; } &> /dev/null
done