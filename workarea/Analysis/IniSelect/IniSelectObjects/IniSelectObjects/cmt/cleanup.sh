# echo "cleanup IniSelectObjects IniSelectObjects-00-00-00 in /besfs/users/deboer/BOSS_IniSelect/workarea/Analysis"

if test "${CMTROOT}" = ""; then
  CMTROOT=/afs/ihep.ac.cn/bes3/offline/ExternalLib/SLC6/contrib/CMT/v1r25; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
cmtIniSelectObjectstempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if test ! $? = 0 ; then cmtIniSelectObjectstempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt cleanup -sh -pack=IniSelectObjects -version=IniSelectObjects-00-00-00 -path=/besfs/users/deboer/BOSS_IniSelect/workarea/Analysis  $* >${cmtIniSelectObjectstempfile}
if test $? != 0 ; then
  echo >&2 "${CMTROOT}/mgr/cmt cleanup -sh -pack=IniSelectObjects -version=IniSelectObjects-00-00-00 -path=/besfs/users/deboer/BOSS_IniSelect/workarea/Analysis  $* >${cmtIniSelectObjectstempfile}"
  cmtcleanupstatus=2
  /bin/rm -f ${cmtIniSelectObjectstempfile}
  unset cmtIniSelectObjectstempfile
  return $cmtcleanupstatus
fi
cmtcleanupstatus=0
. ${cmtIniSelectObjectstempfile}
if test $? != 0 ; then
  cmtcleanupstatus=2
fi
/bin/rm -f ${cmtIniSelectObjectstempfile}
unset cmtIniSelectObjectstempfile
return $cmtcleanupstatus

