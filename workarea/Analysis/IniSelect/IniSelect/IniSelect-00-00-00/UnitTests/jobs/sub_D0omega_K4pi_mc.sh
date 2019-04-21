#!/bin/bash
{ boss.exe "/besfs/users/deboer/BOSS_IniSelect/jobs/sim/D0omega_K4pi/100,000_events/sim_D0omega_K4pi.txt"; } &> "/besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/IniSelect/IniSelect/IniSelect-00-00-00/UnitTests/data/excl/sim_D0omega_K4pi.log"
{ boss.exe "/besfs/users/deboer/BOSS_IniSelect/jobs/rec/D0omega_K4pi/100,000_events/rec_D0omega_K4pi.txt"; } &> "/besfs/users/deboer/BOSS_IniSelect/workarea/Analysis/IniSelect/IniSelect/IniSelect-00-00-00/UnitTests/data/excl/rec_D0omega_K4pi.log"
