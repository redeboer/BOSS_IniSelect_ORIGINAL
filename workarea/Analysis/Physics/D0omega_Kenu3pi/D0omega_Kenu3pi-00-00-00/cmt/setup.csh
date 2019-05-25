# echo "setup D0omega_Kenu3pi D0omega_Kenu3pi-00-00-00 in /besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/Physics"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /afs/ihep.ac.cn/bes3/offline/ExternalLib/SLC6/contrib/CMT/v1r25
endif
source ${CMTROOT}/mgr/setup.csh
set cmtD0omega_Kenu3pitempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if $status != 0 then
  set cmtD0omega_Kenu3pitempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt setup -csh -pack=D0omega_Kenu3pi -version=D0omega_Kenu3pi-00-00-00 -path=/besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/Physics  -no_cleanup $* >${cmtD0omega_Kenu3pitempfile}
if ( $status != 0 ) then
  echo "${CMTROOT}/mgr/cmt setup -csh -pack=D0omega_Kenu3pi -version=D0omega_Kenu3pi-00-00-00 -path=/besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/Physics  -no_cleanup $* >${cmtD0omega_Kenu3pitempfile}"
  set cmtsetupstatus=2
  /bin/rm -f ${cmtD0omega_Kenu3pitempfile}
  unset cmtD0omega_Kenu3pitempfile
  exit $cmtsetupstatus
endif
set cmtsetupstatus=0
source ${cmtD0omega_Kenu3pitempfile}
if ( $status != 0 ) then
  set cmtsetupstatus=2
endif
/bin/rm -f ${cmtD0omega_Kenu3pitempfile}
unset cmtD0omega_Kenu3pitempfile
exit $cmtsetupstatus

