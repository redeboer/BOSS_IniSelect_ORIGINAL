source "${BOSS_IniSelect}/setup/FunctionsPrint.sh"
source "${BOSS_IniSelect}/setup/FunctionsRoot.sh"
source "${BOSS_IniSelect}/setup/Functions.sh"

# cmtmake && \
# boss.exe test/TestJob.txt && \
rtcompile test/CompareRootFiles.C && \
./test/CompareRootFiles.o test/TestJob.root test/TestJob.old.root && \
# mv test/TestJob.root test/TestJob.old.root && \
true