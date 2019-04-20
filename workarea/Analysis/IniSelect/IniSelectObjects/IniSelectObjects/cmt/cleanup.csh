# echo "cleanup IniSelectObjects IniSelectObjects-00-00-00 in /besfs/users/deboer/BOSS_IniSelect/workarea/Analysis"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /afs/ihep.ac.cn/bes3/offline/ExternalLib/SLC6/contrib/CMT/v1r25
endif
source ${CMTROOT}/mgr/setup.csh
set cmtIniSelectObjectstempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if $status != 0 then
  set cmtIniSelectObjectstempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt cleanup -csh -pack=IniSelectObjects -version=IniSelectObjects-00-00-00 -path=/besfs/users/deboer/BOSS_IniSelect/workarea/Analysis  $* >${cmtIniSelectObjectstempfile}
if ( $status != 0 ) then
  echo "${CMTROOT}/mgr/cmt cleanup -csh -pack=IniSelectObjects -version=IniSelectObjects-00-00-00 -path=/besfs/users/deboer/BOSS_IniSelect/workarea/Analysis  $* >${cmtIniSelectObjectstempfile}"
  set cmtcleanupstatus=2
  /bin/rm -f ${cmtIniSelectObjectstempfile}
  unset cmtIniSelectObjectstempfile
  exit $cmtcleanupstatus
endif
set cmtcleanupstatus=0
source ${cmtIniSelectObjectstempfile}
if ( $status != 0 ) then
  set cmtcleanupstatus=2
endif
/bin/rm -f ${cmtIniSelectObjectstempfile}
unset cmtIniSelectObjectstempfile
exit $cmtcleanupstatus

