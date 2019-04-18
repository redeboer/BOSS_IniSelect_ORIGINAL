# * Check number of input parameters * #
  if [[ $# -lt 1 ]]; then
    echo "ERROR: This script needs one parameter"
    exit
  fi

# * Import repository name from input argument * #
  repoChoice="${1}"
  repoChoice="$(echo "${repoChoice}" | tr '[:upper:]' '[:lower:]')"
  if [ "${repoChoice}" == "clhep" ]; then
    repoURL="https://gitlab.cern.ch/CLHEP/CLHEP.git"
  elif [ "${repoChoice}" == "gaudi" ]; then
    repoURL="https://gitlab.cern.ch/gaudi/Gaudi.git"
  elif [ "${repoChoice}" == "geant4" ]; then
    repoURL="https://gitlab.cern.ch/geant4/geant4.git"
  elif [ "${repoChoice}" == "root" ]; then
    repoURL="http://github.com/root-project/root.git"
    repoVersion="v6-16-00"
    # Best to get the Pro release.
    # See latest Pro release here:
    # https://root.cern.ch/releases
  else
    echo "ERROR: No repository URL defined for \"${repoChoice}\""
    exit
  fi

# * Import version number if given parameters * #
  if [[ $# -gt 1 ]]; then
    repoVersion="${1}"
  fi

# * Parameter names * #
  outputPath="include"
  buildDir="build"

# * Get repository name * #
  repoName="${repoURL/*\/}"
  repoName="${repoName/.git*}"

# * Ask confirmation from user * #
  echo "This script will clone or pull repository"
  echo "  \"${repoURL}\""
  if [[ "${repoVersion}" != "" ]]; then
    echo "version \"${repoVersion}\""
  fi
  echo "into folder"
  echo "  \"${outputPath}/${repoName}\""
  echo "--> Press ENTER to continue..."
  read -p ""


# * Clone repository * #
  cd "${outputPath}"
  if [ ! -d "${repoName}/.git" ]; then
    echo "Repo does not yet exist locally. Cloning..."
    git clone "${repoURL}"
  else
    echo "Pulling latest version from \"${repoURL}\""...
  fi
  cd "${repoName}"
  git pull
  if [[ $? != 0 ]]; then
    echo
    echo
    echo "ERROR: Failed to pull repository"
    echo "Read the above output..."
  fi

# * Set repo version * #
  if [[ "${repoVersion}" != "" ]]; then
    echo "Setting version \"${repoVersion}\""
    git checkout -b "${repoVersion}" "${repoVersion}"
    if [[ $? != 0 ]]; then
      echo "ERROR: version \"${repoVersion}\" does not exist"
      read -p "List available versions? (y/n) " -n 1 -r
      if [[ $REPLY =~ ^[Yy]$ ]]; then
        echo
        git tag -l | less
      fi
      exit 1
    fi
  fi

# * Ask user whether to compile * #
  echo
  read -p "Compile source code for repository \"${repoName}\"? (y/n) " -n 1 -r
  if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    echo
    exit 1
  fi

# * Compile * #
  mkdir -p "${buildDir}"
  cd "${buildDir}"
  if [[ "$(ls | wc -l)" != "0" ]]; then
    echo "Build directory \"${outputPath}/${buildDir}\" is not empty"
    read -p "--> Remove files? (y/n) " -n 1 -r
    if [[ $REPLY =~ ^[Yy]$ ]]; then
      rm -rf *
    fi
  fi
  cmake ..

  if [[ $? != 0 ]]; then
    echo
    echo "ERROR: Failed to run cmake"
    echo "Perhaps you miss some dependencies?"
    echo "Read the above output..."
  fi