# This script fetches the header files of BOSS so you
# can use them for auto-complete in your favourite IDE

# * Check input arguments * #
  if [ ${#} != 1 ]; then
    echo "ERROR: This script needs the BOSS version number as argument"
    exit
  fi

# * Check BOSS version * #
  BossPath="/afs/ihep.ac.cn/bes3/offline/Boss"
  BossVersion="${1}"
  if [ ! -d "${BossPath}/${BossVersion}" ]; then
    echo "ERROR: BOSS ${BossVersion} does not exist"
    echo "  Available versions:"
    echo
    ls "${BossPath}" | grep "^[0-9].*[0-9]$"
    echo
    exit 1
  fi

# * Check input and output paths * #
  HeaderSubPath="InstallArea/include"
  inputPath="${BossPath}/${BossVersion}/${HeaderSubPath}"
  outputPath="BOSS"
  # outputPath="/scratchfs/bes/${USER}/BOSS/include"
  if [ ! -d "${inputPath}" ]; then
    echo "ERROR: Folder"
    echo "  \"${inputPath}\""
    echo "does not exist!"
    exit 1
  fi

# * Ask for confirmation * #
  echo "This script will COPY all files in:"
  echo "  \"${inputPath}\""
  echo "following symbolic links, to path:"
  echo "  \"${outputPath}\""
  echo "All old files will be OVERWRITTEN."
  echo
  read -p "Continue? (y/n) " -n 1 -r
  echo
  if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    exit 1
  fi
  echo

# * Perform COPY and OVERWRITE task * #
  echo "Copying files..."
  mkdir -p "${outputPath}"
  rm -rf "${outputPath}"
  cp -Lr "${inputPath}" "${outputPath}/"

# * Perform COPY and OVERWRITE task * #
  cd "${outputPath}"
  toBeRemoved=(
    CVS
    *.cmtref
  )
  for i in ${toBeRemoved[@]}; do
    echo "Removing files \"${i}\""
    rm -rf $(find -iname "${i}")
  done

# * Put headers one folder up and then into an include folder * #
  echo "Moving all files one folder up..."
  for i in $(ls); do
    mv $i/$i/* $i/
  done
  mkdir -p "include"
  mv * "include/"