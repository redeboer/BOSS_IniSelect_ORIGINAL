# echo "setup D0omega_Kenu3pi D0omega_Kenu3pi-00-00-00 in /besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/Physics"

if test "${CMTROOT}" = ""; then
  CMTROOT=/afs/ihep.ac.cn/bes3/offline/ExternalLib/SLC6/contrib/CMT/v1r25; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
cmtD0omega_Kenu3pitempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if test ! $? = 0 ; then cmtD0omega_Kenu3pitempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt setup -sh -pack=D0omega_Kenu3pi -version=D0omega_Kenu3pi-00-00-00 -path=/besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/Physics  -no_cleanup $* >${cmtD0omega_Kenu3pitempfile}
if test $? != 0 ; then
  echo >&2 "${CMTROOT}/mgr/cmt setup -sh -pack=D0omega_Kenu3pi -version=D0omega_Kenu3pi-00-00-00 -path=/besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/Physics  -no_cleanup $* >${cmtD0omega_Kenu3pitempfile}"
  cmtsetupstatus=2
  /bin/rm -f ${cmtD0omega_Kenu3pitempfile}
  unset cmtD0omega_Kenu3pitempfile
  return $cmtsetupstatus
fi
cmtsetupstatus=0
. ${cmtD0omega_Kenu3pitempfile}
if test $? != 0 ; then
  cmtsetupstatus=2
fi
/bin/rm -f ${cmtD0omega_Kenu3pitempfile}
unset cmtD0omega_Kenu3pitempfile
return $cmtsetupstatus

