# echo "setup D0phi_KpipiKK D0phi_KpipiKK-00-00-00 in /besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/Physics"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /afs/ihep.ac.cn/bes3/offline/ExternalLib/SLC6/contrib/CMT/v1r25
endif
source ${CMTROOT}/mgr/setup.csh
set cmtD0phi_KpipiKKtempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if $status != 0 then
  set cmtD0phi_KpipiKKtempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt setup -csh -pack=D0phi_KpipiKK -version=D0phi_KpipiKK-00-00-00 -path=/besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/Physics  -no_cleanup $* >${cmtD0phi_KpipiKKtempfile}
if ( $status != 0 ) then
  echo "${CMTROOT}/mgr/cmt setup -csh -pack=D0phi_KpipiKK -version=D0phi_KpipiKK-00-00-00 -path=/besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/Physics  -no_cleanup $* >${cmtD0phi_KpipiKKtempfile}"
  set cmtsetupstatus=2
  /bin/rm -f ${cmtD0phi_KpipiKKtempfile}
  unset cmtD0phi_KpipiKKtempfile
  exit $cmtsetupstatus
endif
set cmtsetupstatus=0
source ${cmtD0phi_KpipiKKtempfile}
if ( $status != 0 ) then
  set cmtsetupstatus=2
endif
/bin/rm -f ${cmtD0phi_KpipiKKtempfile}
unset cmtD0phi_KpipiKKtempfile
exit $cmtsetupstatus

