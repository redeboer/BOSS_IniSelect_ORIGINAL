# echo "cleanup D0phi_3Kpi D0phi_3Kpi-00-00-00 in /besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/Physics"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /afs/ihep.ac.cn/bes3/offline/ExternalLib/SLC6/contrib/CMT/v1r25
endif
source ${CMTROOT}/mgr/setup.csh
set cmtD0phi_3Kpitempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if $status != 0 then
  set cmtD0phi_3Kpitempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt cleanup -csh -pack=D0phi_3Kpi -version=D0phi_3Kpi-00-00-00 -path=/besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/Physics  $* >${cmtD0phi_3Kpitempfile}
if ( $status != 0 ) then
  echo "${CMTROOT}/mgr/cmt cleanup -csh -pack=D0phi_3Kpi -version=D0phi_3Kpi-00-00-00 -path=/besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/Physics  $* >${cmtD0phi_3Kpitempfile}"
  set cmtcleanupstatus=2
  /bin/rm -f ${cmtD0phi_3Kpitempfile}
  unset cmtD0phi_3Kpitempfile
  exit $cmtcleanupstatus
endif
set cmtcleanupstatus=0
source ${cmtD0phi_3Kpitempfile}
if ( $status != 0 ) then
  set cmtcleanupstatus=2
endif
/bin/rm -f ${cmtD0phi_3Kpitempfile}
unset cmtD0phi_3Kpitempfile
exit $cmtcleanupstatus

