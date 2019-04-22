TestName="${1}"
IniSelectObjects="${BOSS_IniSelect}/workarea/Analysis/IniSelect/IniSelect/IniSelect-00-00-00"

if [[ ${#} != 1 ]]; then
  echo "ERROR: This script needs 1 argument"
  exit 1
fi

jobFile="UnitTests/jobs/job_${TestName}.txt"
if [[ ! -f "${IniSelectObjects}/${jobFile}" ]]; then
  echo "ERROR: File \"${jobFile}\" does not exist"
  exit 1
fi

clear && \
cd "${IniSelectObjects}" && \
cd cmt && \
# cmt config && \
make && \
# source setup.sh && \
cd "${IniSelectObjects}" && \
boss.exe "UnitTests/jobs/job_${TestName}.txt"