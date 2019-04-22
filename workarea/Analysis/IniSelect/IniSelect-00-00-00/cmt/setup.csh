# echo "setup IniSelect IniSelect-00-00-00 in /besfs/users/deboer/BOSS_IniSelect/workarea/Analysis"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /afs/ihep.ac.cn/bes3/offline/ExternalLib/SLC6/contrib/CMT/v1r25
endif
source ${CMTROOT}/mgr/setup.csh
set cmtIniSelecttempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if $status != 0 then
  set cmtIniSelecttempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt setup -csh -pack=IniSelect -version=IniSelect-00-00-00 -path=/besfs/users/deboer/BOSS_IniSelect/workarea/Analysis  -no_cleanup $* >${cmtIniSelecttempfile}
if ( $status != 0 ) then
  echo "${CMTROOT}/mgr/cmt setup -csh -pack=IniSelect -version=IniSelect-00-00-00 -path=/besfs/users/deboer/BOSS_IniSelect/workarea/Analysis  -no_cleanup $* >${cmtIniSelecttempfile}"
  set cmtsetupstatus=2
  /bin/rm -f ${cmtIniSelecttempfile}
  unset cmtIniSelecttempfile
  exit $cmtsetupstatus
endif
set cmtsetupstatus=0
source ${cmtIniSelecttempfile}
if ( $status != 0 ) then
  set cmtsetupstatus=2
endif
/bin/rm -f ${cmtIniSelecttempfile}
unset cmtIniSelecttempfile
exit $cmtsetupstatus

