# echo "setup IniSelectObjects IniSelectObjects-00-00-00 in /besfs/users/deboer/BOSS_IniSelect/workarea/Analysis"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /afs/ihep.ac.cn/bes3/offline/ExternalLib/SLC6/contrib/CMT/v1r25
endif
source ${CMTROOT}/mgr/setup.csh
set cmtIniSelectObjectstempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if $status != 0 then
  set cmtIniSelectObjectstempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt setup -csh -pack=IniSelectObjects -version=IniSelectObjects-00-00-00 -path=/besfs/users/deboer/BOSS_IniSelect/workarea/Analysis  -no_cleanup $* >${cmtIniSelectObjectstempfile}
if ( $status != 0 ) then
  echo "${CMTROOT}/mgr/cmt setup -csh -pack=IniSelectObjects -version=IniSelectObjects-00-00-00 -path=/besfs/users/deboer/BOSS_IniSelect/workarea/Analysis  -no_cleanup $* >${cmtIniSelectObjectstempfile}"
  set cmtsetupstatus=2
  /bin/rm -f ${cmtIniSelectObjectstempfile}
  unset cmtIniSelectObjectstempfile
  exit $cmtsetupstatus
endif
set cmtsetupstatus=0
source ${cmtIniSelectObjectstempfile}
if ( $status != 0 ) then
  set cmtsetupstatus=2
endif
/bin/rm -f ${cmtIniSelectObjectstempfile}
unset cmtIniSelectObjectstempfile
exit $cmtsetupstatus

