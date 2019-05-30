# echo "setup JpsiToDPV JpsiToDPV-00-00-00 in /besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/Physics"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /afs/ihep.ac.cn/bes3/offline/ExternalLib/SLC6/contrib/CMT/v1r25
endif
source ${CMTROOT}/mgr/setup.csh
set cmtJpsiToDPVtempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if $status != 0 then
  set cmtJpsiToDPVtempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt setup -csh -pack=JpsiToDPV -version=JpsiToDPV-00-00-00 -path=/besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/Physics  -no_cleanup $* >${cmtJpsiToDPVtempfile}
if ( $status != 0 ) then
  echo "${CMTROOT}/mgr/cmt setup -csh -pack=JpsiToDPV -version=JpsiToDPV-00-00-00 -path=/besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/Physics  -no_cleanup $* >${cmtJpsiToDPVtempfile}"
  set cmtsetupstatus=2
  /bin/rm -f ${cmtJpsiToDPVtempfile}
  unset cmtJpsiToDPVtempfile
  exit $cmtsetupstatus
endif
set cmtsetupstatus=0
source ${cmtJpsiToDPVtempfile}
if ( $status != 0 ) then
  set cmtsetupstatus=2
endif
/bin/rm -f ${cmtJpsiToDPVtempfile}
unset cmtJpsiToDPVtempfile
exit $cmtsetupstatus

