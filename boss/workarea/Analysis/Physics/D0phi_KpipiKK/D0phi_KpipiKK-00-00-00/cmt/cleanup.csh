# echo "cleanup D0phi_KpipiKK D0phi_KpipiKK-00-00-00 in /besfs/users/deboer/BOSS_Afterburner/boss/workarea/Analysis/Physics"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /afs/ihep.ac.cn/bes3/offline/ExternalLib/SLC6/contrib/CMT/v1r25
endif
source ${CMTROOT}/mgr/setup.csh
set cmtD0phi_KpipiKKtempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if $status != 0 then
  set cmtD0phi_KpipiKKtempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt cleanup -csh -pack=D0phi_KpipiKK -version=D0phi_KpipiKK-00-00-00 -path=/besfs/users/deboer/BOSS_Afterburner/boss/workarea/Analysis/Physics  $* >${cmtD0phi_KpipiKKtempfile}
if ( $status != 0 ) then
  echo "${CMTROOT}/mgr/cmt cleanup -csh -pack=D0phi_KpipiKK -version=D0phi_KpipiKK-00-00-00 -path=/besfs/users/deboer/BOSS_Afterburner/boss/workarea/Analysis/Physics  $* >${cmtD0phi_KpipiKKtempfile}"
  set cmtcleanupstatus=2
  /bin/rm -f ${cmtD0phi_KpipiKKtempfile}
  unset cmtD0phi_KpipiKKtempfile
  exit $cmtcleanupstatus
endif
set cmtcleanupstatus=0
source ${cmtD0phi_KpipiKKtempfile}
if ( $status != 0 ) then
  set cmtcleanupstatus=2
endif
/bin/rm -f ${cmtD0phi_KpipiKKtempfile}
unset cmtD0phi_KpipiKKtempfile
exit $cmtcleanupstatus

