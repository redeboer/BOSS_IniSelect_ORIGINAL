# Author: Remco de Boer
# Date: November 5th, 2018

# * Check if in administrator mode * #
if [ "$EUID" -ne 0 ]
  then echo "Please run as root. To do so, use sudo."
  exit
fi

# * Install prerequisites * #
{ apt-get install git dpkg-dev cmake g++ gcc binutils libx11-dev libxpm-dev libxft-dev libxext-dev; } \
&> /dev/null
# * Optional packages * #
{ apt-get install gfortran libssl-dev libpcre3-dev xlibmesa-glu-dev libglew1.5-dev libftgl-dev libmysqlclient-dev libfftw3-dev libcfitsio-dev graphviz-dev libavahi-compat-libdnssd-dev libldap2-dev python-dev libxml2-dev libkrb5-dev libgsl0-dev libqt4-dev; } \
&> /dev/null

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
    cmakeOptions="-Dminuit=ON -Dminuit2=ON"
    # Best to get the Pro release.
    # See latest Pro release here:
    # https://root.cern.ch/releases
  elif [ "${repoChoice}" == "yaml-cpp" ]; then
    repoURL="https://github.com/jbeder/yaml-cpp"
    cmakeOptions="-DBUILD_SHARED_LIBS=ON"
  else
    echo "ERROR: No repository URL defined for \"${repoChoice}\""
    exit
  fi

# * Import version number if given parameters * #
  if [[ $# -ge 2 ]]; then
    repoVersion="${2}"
  fi

# * Import CMAKE options * #
  if [[ $# -ge 3 ]]; then
    cmakeOptions="${3}"
  fi

# * Parameter names * #
  outputPath="/usr/local"
  buildDir="compile"

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
  echo "Switching to master..."
  git checkout master
  git pull
  if [[ $? != 0 ]]; then
    echo
    echo
    echo "ERROR: Failed to pull repository"
    echo "Read the above output..."
    exit 1
  fi

# * Set repo version * #
  if [[ "${repoVersion}" != "" ]]; then
    echo "Switching to version \"${repoVersion}\""
    git checkout ${repoVersion}
    if [[ $? != 0 ]]; then
      echo "ERROR: version \"${repoVersion}\" does not exist"
      read -p "List available versions? (y/n) " -n 1 -r
      if [[ $REPLY =~ ^[Yy]$ ]]; then
        git tag -l | less
      fi
      echo
      exit 1
    fi
  fi

# * Ask user whether to compile * #
  echo
  if [[ "${cmakeOptions}" == "" ]]; then
    echo "Will compile with default CMAKE options"
  else
    echo "Will compile with the following CMAKE options:"
    echo "  ${cmakeOptions}"
  fi
  if [ "${repoChoice}" == "root" ]; then
    echo "See here for more options: https://root.cern.ch/building-root"
  fi
  read -p "Continue with these options? (y/n) " -n 1 -r
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
    echo
  fi
  cmake ${cmakeOptions} .. && \
  make -j$(nproc) && \
  make install

  if [[ $? -ne 0 ]]; then
    echo
    echo "ERROR: Failed to run cmake"
    echo "Perhaps you miss some dependencies?"
    echo "Read the above output..."
  else
    echo "SUCCESSFULLY build ${repoName}"
    read -p "--> Remove build files? (y/n) " -n 1 -r
    if [[ $REPLY =~ ^[Yy]$ ]]; then
      cd - > /dev/null
      rm -rf "${buildDir}"
    fi

  fi

# * Install ROOT: tell Ubuntu how to run ROOT * #
  if [ "${repoChoice}" == "root" ]; then
    cd /usr/local/root
    if [[ $? -ne 0 ]]; then
      exit 1
    fi
    . bin/thisroot.sh
    source bin/thisroot.sh

    # * Information about adding ROOT to your bash.
    # ! BE CAREFUL IF YOU COPY FROM THIS FILE: replace \$ by $
    read -p "
    Installation of ROOT completed! Upon pressing ENTER, the path variables for ROOT will be set in your .bashrc file. Gedit will be opened so you can check the result."
    echo "
    # CERN ROOT
    export ROOTSYS=/usr/local/root
    export PATH=\$ROOTSYS/bin:\$PATH
    export PYTHONDIR=\$ROOTSYS
    export LD_LIBRARY_PATH=\$ROOTSYS/lib:\$PYTHONDIR/lib:\$ROOTSYS/bindings/pyroot:\$LD_LIBRARY_PATH
    export PYTHONPATH=\$ROOTSYS/lib:\$PYTHONPATH:\$ROOTSYS/bindings/pyroot" >> ~/.bashrc
    gedit ~/.bashrc
  else