# echo "cleanup D0V D0V-00-00-00 in /besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/Physics"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /afs/ihep.ac.cn/bes3/offline/ExternalLib/SLC6/contrib/CMT/v1r25
endif
source ${CMTROOT}/mgr/setup.csh
set cmtD0Vtempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if $status != 0 then
  set cmtD0Vtempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt cleanup -csh -pack=D0V -version=D0V-00-00-00 -path=/besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/Physics  $* >${cmtD0Vtempfile}
if ( $status != 0 ) then
  echo "${CMTROOT}/mgr/cmt cleanup -csh -pack=D0V -version=D0V-00-00-00 -path=/besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/Physics  $* >${cmtD0Vtempfile}"
  set cmtcleanupstatus=2
  /bin/rm -f ${cmtD0Vtempfile}
  unset cmtD0Vtempfile
  exit $cmtcleanupstatus
endif
set cmtcleanupstatus=0
source ${cmtD0Vtempfile}
if ( $status != 0 ) then
  set cmtcleanupstatus=2
endif
/bin/rm -f ${cmtD0Vtempfile}
unset cmtD0Vtempfile
exit $cmtcleanupstatus

