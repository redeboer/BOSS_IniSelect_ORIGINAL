# echo "cleanup RhopiAlg RhopiAlg-01-00-00 in ${BOSSWORKAREA}/workarea/Analysis/Physics"

if test "${CMTROOT}" = ""; then
  CMTROOT=/afs/ihep.ac.cn/bes3/offline/ExternalLib/SLC6/contrib/CMT/v1r25; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
cmtRhopiAlgtempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if test ! $? = 0 ; then cmtRhopiAlgtempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt cleanup -sh -pack=RhopiAlg -version=RhopiAlg-01-00-00 -path=${BOSSWORKAREA}/workarea/Analysis/Physics  $* >${cmtRhopiAlgtempfile}
if test $? != 0 ; then
  echo >&2 "${CMTROOT}/mgr/cmt cleanup -sh -pack=RhopiAlg -version=RhopiAlg-01-00-00 -path=${BOSSWORKAREA}/workarea/Analysis/Physics  $* >${cmtRhopiAlgtempfile}"
  cmtcleanupstatus=2
  /bin/rm -f ${cmtRhopiAlgtempfile}
  unset cmtRhopiAlgtempfile
  return $cmtcleanupstatus
fi
cmtcleanupstatus=0
. ${cmtRhopiAlgtempfile}
if test $? != 0 ; then
  cmtcleanupstatus=2
fi
/bin/rm -f ${cmtRhopiAlgtempfile}
unset cmtRhopiAlgtempfile
return $cmtcleanupstatus

