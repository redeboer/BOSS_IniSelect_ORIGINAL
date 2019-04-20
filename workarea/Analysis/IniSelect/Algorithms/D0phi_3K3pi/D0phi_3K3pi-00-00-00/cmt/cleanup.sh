# echo "cleanup D0phi_3K3pi D0phi_3K3pi-00-00-00 in /besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/IniSelect/Algorithms"

if test "${CMTROOT}" = ""; then
  CMTROOT=/afs/ihep.ac.cn/bes3/offline/ExternalLib/SLC6/contrib/CMT/v1r25; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
cmtD0phi_3K3pitempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if test ! $? = 0 ; then cmtD0phi_3K3pitempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt cleanup -sh -pack=D0phi_3K3pi -version=D0phi_3K3pi-00-00-00 -path=/besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/IniSelect/Algorithms  $* >${cmtD0phi_3K3pitempfile}
if test $? != 0 ; then
  echo >&2 "${CMTROOT}/mgr/cmt cleanup -sh -pack=D0phi_3K3pi -version=D0phi_3K3pi-00-00-00 -path=/besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/IniSelect/Algorithms  $* >${cmtD0phi_3K3pitempfile}"
  cmtcleanupstatus=2
  /bin/rm -f ${cmtD0phi_3K3pitempfile}
  unset cmtD0phi_3K3pitempfile
  return $cmtcleanupstatus
fi
cmtcleanupstatus=0
. ${cmtD0phi_3K3pitempfile}
if test $? != 0 ; then
  cmtcleanupstatus=2
fi
/bin/rm -f ${cmtD0phi_3K3pitempfile}
unset cmtD0phi_3K3pitempfile
return $cmtcleanupstatus

