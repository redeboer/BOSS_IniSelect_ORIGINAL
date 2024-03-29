# echo "cleanup JpsiToDPV JpsiToDPV-00-00-00 in /besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/Physics"

if test "${CMTROOT}" = ""; then
  CMTROOT=/afs/ihep.ac.cn/bes3/offline/ExternalLib/SLC6/contrib/CMT/v1r25; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
cmtJpsiToDPVtempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if test ! $? = 0 ; then cmtJpsiToDPVtempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt cleanup -sh -pack=JpsiToDPV -version=JpsiToDPV-00-00-00 -path=/besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/Physics  $* >${cmtJpsiToDPVtempfile}
if test $? != 0 ; then
  echo >&2 "${CMTROOT}/mgr/cmt cleanup -sh -pack=JpsiToDPV -version=JpsiToDPV-00-00-00 -path=/besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/Physics  $* >${cmtJpsiToDPVtempfile}"
  cmtcleanupstatus=2
  /bin/rm -f ${cmtJpsiToDPVtempfile}
  unset cmtJpsiToDPVtempfile
  return $cmtcleanupstatus
fi
cmtcleanupstatus=0
. ${cmtJpsiToDPVtempfile}
if test $? != 0 ; then
  cmtcleanupstatus=2
fi
/bin/rm -f ${cmtJpsiToDPVtempfile}
unset cmtJpsiToDPVtempfile
return $cmtcleanupstatus

