# echo "cleanup D0phi_3Kpi D0phi_3Kpi-00-00-00 in /besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/Physics"

if test "${CMTROOT}" = ""; then
  CMTROOT=/afs/ihep.ac.cn/bes3/offline/ExternalLib/SLC6/contrib/CMT/v1r25; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
cmtD0phi_3Kpitempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if test ! $? = 0 ; then cmtD0phi_3Kpitempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt cleanup -sh -pack=D0phi_3Kpi -version=D0phi_3Kpi-00-00-00 -path=/besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/Physics  $* >${cmtD0phi_3Kpitempfile}
if test $? != 0 ; then
  echo >&2 "${CMTROOT}/mgr/cmt cleanup -sh -pack=D0phi_3Kpi -version=D0phi_3Kpi-00-00-00 -path=/besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/Physics  $* >${cmtD0phi_3Kpitempfile}"
  cmtcleanupstatus=2
  /bin/rm -f ${cmtD0phi_3Kpitempfile}
  unset cmtD0phi_3Kpitempfile
  return $cmtcleanupstatus
fi
cmtcleanupstatus=0
. ${cmtD0phi_3Kpitempfile}
if test $? != 0 ; then
  cmtcleanupstatus=2
fi
/bin/rm -f ${cmtD0phi_3Kpitempfile}
unset cmtD0phi_3Kpitempfile
return $cmtcleanupstatus

