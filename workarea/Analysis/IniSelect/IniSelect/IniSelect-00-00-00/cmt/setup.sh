# echo "setup IniSelect IniSelect-00-00-00 in /besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/IniSelect"

if test "${CMTROOT}" = ""; then
  CMTROOT=/afs/ihep.ac.cn/bes3/offline/ExternalLib/SLC6/contrib/CMT/v1r25; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
cmtIniSelecttempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if test ! $? = 0 ; then cmtIniSelecttempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt setup -sh -pack=IniSelect -version=IniSelect-00-00-00 -path=/besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/IniSelect  -no_cleanup $* >${cmtIniSelecttempfile}
if test $? != 0 ; then
  echo >&2 "${CMTROOT}/mgr/cmt setup -sh -pack=IniSelect -version=IniSelect-00-00-00 -path=/besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/IniSelect  -no_cleanup $* >${cmtIniSelecttempfile}"
  cmtsetupstatus=2
  /bin/rm -f ${cmtIniSelecttempfile}
  unset cmtIniSelecttempfile
  return $cmtsetupstatus
fi
cmtsetupstatus=0
. ${cmtIniSelecttempfile}
if test $? != 0 ; then
  cmtsetupstatus=2
fi
/bin/rm -f ${cmtIniSelecttempfile}
unset cmtIniSelecttempfile
return $cmtsetupstatus

