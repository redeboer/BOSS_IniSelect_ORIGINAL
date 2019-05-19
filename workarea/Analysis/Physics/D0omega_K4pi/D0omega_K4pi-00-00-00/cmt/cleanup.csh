# echo "cleanup D0omega_K4pi D0omega_K4pi-00-00-00 in /besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/Physics"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /afs/ihep.ac.cn/bes3/offline/ExternalLib/SLC6/contrib/CMT/v1r25
endif
source ${CMTROOT}/mgr/setup.csh
set cmtD0omega_K4pitempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if $status != 0 then
  set cmtD0omega_K4pitempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt cleanup -csh -pack=D0omega_K4pi -version=D0omega_K4pi-00-00-00 -path=/besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/Physics  $* >${cmtD0omega_K4pitempfile}
if ( $status != 0 ) then
  echo "${CMTROOT}/mgr/cmt cleanup -csh -pack=D0omega_K4pi -version=D0omega_K4pi-00-00-00 -path=/besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/Physics  $* >${cmtD0omega_K4pitempfile}"
  set cmtcleanupstatus=2
  /bin/rm -f ${cmtD0omega_K4pitempfile}
  unset cmtD0omega_K4pitempfile
  exit $cmtcleanupstatus
endif
set cmtcleanupstatus=0
source ${cmtD0omega_K4pitempfile}
if ( $status != 0 ) then
  set cmtcleanupstatus=2
endif
/bin/rm -f ${cmtD0omega_K4pitempfile}
unset cmtD0omega_K4pitempfile
exit $cmtcleanupstatus

