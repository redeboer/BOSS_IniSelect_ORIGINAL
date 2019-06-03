source "${BOSS_IniSelect}/setup/FunctionsPrint.sh"
source "${BOSS_IniSelect}/setup/FunctionsRoot.sh"
source "${BOSS_IniSelect}/setup/Functions.sh"

currentPath="$(pwd)"
cd "${JPSITODPVROOT}" && \

cmtmake && \
boss.exe test/TestJob.txt && \
cd test && \
g++ ROOTCompare.C -I$(root-config --incdir) $(root-config --libs --evelibs --glibs) -lRooFit -lRooFitCore -lRooStats -lMinuit -o ROOTCompare && \
ls && \
./ROOTCompare "${1:-TestJob.root}" -d "${2:-TestJob.old.root}" && \

cd "${currentPath}"