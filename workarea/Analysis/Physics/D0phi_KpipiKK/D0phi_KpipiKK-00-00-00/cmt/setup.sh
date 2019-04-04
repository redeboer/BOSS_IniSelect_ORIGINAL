# echo "setup D0phi_KpipiKK D0phi_KpipiKK-00-00-00 in /besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/Physics"

if test "${CMTROOT}" = ""; then
  CMTROOT=/afs/ihep.ac.cn/bes3/offline/ExternalLib/SLC6/contrib/CMT/v1r25; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
cmtD0phi_KpipiKKtempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if test ! $? = 0 ; then cmtD0phi_KpipiKKtempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt setup -sh -pack=D0phi_KpipiKK -version=D0phi_KpipiKK-00-00-00 -path=/besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/Physics  -no_cleanup $* >${cmtD0phi_KpipiKKtempfile}
if test $? != 0 ; then
  echo >&2 "${CMTROOT}/mgr/cmt setup -sh -pack=D0phi_KpipiKK -version=D0phi_KpipiKK-00-00-00 -path=/besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/Physics  -no_cleanup $* >${cmtD0phi_KpipiKKtempfile}"
  cmtsetupstatus=2
  /bin/rm -f ${cmtD0phi_KpipiKKtempfile}
  unset cmtD0phi_KpipiKKtempfile
  return $cmtsetupstatus
fi
cmtsetupstatus=0
. ${cmtD0phi_KpipiKKtempfile}
if test $? != 0 ; then
  cmtsetupstatus=2
fi
/bin/rm -f ${cmtD0phi_KpipiKKtempfile}
unset cmtD0phi_KpipiKKtempfile
return $cmtsetupstatus

