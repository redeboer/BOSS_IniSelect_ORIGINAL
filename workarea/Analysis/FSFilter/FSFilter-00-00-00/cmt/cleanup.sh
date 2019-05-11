# echo "cleanup FSFilter FSFilter-00-00-00 in /afs/ihep.ac.cn/users/r/remitche/Algorithms"

if test "${CMTROOT}" = ""; then
  CMTROOT=/afs/ihep.ac.cn/bes3/offline/ExternalLib/SLC6/contrib/CMT/v1r25; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
cmtFSFiltertempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if test ! $? = 0 ; then cmtFSFiltertempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt cleanup -sh -pack=FSFilter -version=FSFilter-00-00-00 -path=/afs/ihep.ac.cn/users/r/remitche/Algorithms  $* >${cmtFSFiltertempfile}
if test $? != 0 ; then
  echo >&2 "${CMTROOT}/mgr/cmt cleanup -sh -pack=FSFilter -version=FSFilter-00-00-00 -path=/afs/ihep.ac.cn/users/r/remitche/Algorithms  $* >${cmtFSFiltertempfile}"
  cmtcleanupstatus=2
  /bin/rm -f ${cmtFSFiltertempfile}
  unset cmtFSFiltertempfile
  return $cmtcleanupstatus
fi
cmtcleanupstatus=0
. ${cmtFSFiltertempfile}
if test $? != 0 ; then
  cmtcleanupstatus=2
fi
/bin/rm -f ${cmtFSFiltertempfile}
unset cmtFSFiltertempfile
return $cmtcleanupstatus

