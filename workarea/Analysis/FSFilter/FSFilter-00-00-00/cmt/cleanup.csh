# echo "cleanup FSFilter FSFilter-00-00-00 in /afs/ihep.ac.cn/users/r/remitche/Algorithms"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /afs/ihep.ac.cn/bes3/offline/ExternalLib/SLC6/contrib/CMT/v1r25
endif
source ${CMTROOT}/mgr/setup.csh
set cmtFSFiltertempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if $status != 0 then
  set cmtFSFiltertempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt cleanup -csh -pack=FSFilter -version=FSFilter-00-00-00 -path=/afs/ihep.ac.cn/users/r/remitche/Algorithms  $* >${cmtFSFiltertempfile}
if ( $status != 0 ) then
  echo "${CMTROOT}/mgr/cmt cleanup -csh -pack=FSFilter -version=FSFilter-00-00-00 -path=/afs/ihep.ac.cn/users/r/remitche/Algorithms  $* >${cmtFSFiltertempfile}"
  set cmtcleanupstatus=2
  /bin/rm -f ${cmtFSFiltertempfile}
  unset cmtFSFiltertempfile
  exit $cmtcleanupstatus
endif
set cmtcleanupstatus=0
source ${cmtFSFiltertempfile}
if ( $status != 0 ) then
  set cmtcleanupstatus=2
endif
/bin/rm -f ${cmtFSFiltertempfile}
unset cmtFSFiltertempfile
exit $cmtcleanupstatus

