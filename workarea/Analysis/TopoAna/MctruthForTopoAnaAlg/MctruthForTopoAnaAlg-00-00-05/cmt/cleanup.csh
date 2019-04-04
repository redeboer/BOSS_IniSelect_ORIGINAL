# echo "cleanup MctruthForTopoAnaAlg MctruthForTopoAnaAlg-00-00-05 in /besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/TopoAna"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /afs/ihep.ac.cn/bes3/offline/ExternalLib/SLC6/contrib/CMT/v1r25
endif
source ${CMTROOT}/mgr/setup.csh
set cmtMctruthForTopoAnaAlgtempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if $status != 0 then
  set cmtMctruthForTopoAnaAlgtempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt cleanup -csh -pack=MctruthForTopoAnaAlg -version=MctruthForTopoAnaAlg-00-00-05 -path=/besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/TopoAna  $* >${cmtMctruthForTopoAnaAlgtempfile}
if ( $status != 0 ) then
  echo "${CMTROOT}/mgr/cmt cleanup -csh -pack=MctruthForTopoAnaAlg -version=MctruthForTopoAnaAlg-00-00-05 -path=/besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/TopoAna  $* >${cmtMctruthForTopoAnaAlgtempfile}"
  set cmtcleanupstatus=2
  /bin/rm -f ${cmtMctruthForTopoAnaAlgtempfile}
  unset cmtMctruthForTopoAnaAlgtempfile
  exit $cmtcleanupstatus
endif
set cmtcleanupstatus=0
source ${cmtMctruthForTopoAnaAlgtempfile}
if ( $status != 0 ) then
  set cmtcleanupstatus=2
endif
/bin/rm -f ${cmtMctruthForTopoAnaAlgtempfile}
unset cmtMctruthForTopoAnaAlgtempfile
exit $cmtcleanupstatus

