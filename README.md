# BOSS IniSelect

This repository serves as an extra layer to the BESIII Offline Software System (BOSS). For more information on BOSS, see the [BOSS GitBook](https://besiii.gitbook.io/boss).

The BOSS IniSelect repository contains BOSS packages for **initial event selection** (hence the name). These packages are to facilitate analysis procedures in BESIII by further modulating the procedure for selecting tracks from a DST file. The main feature is the [TrackSelector]()

Originally, this repository was contained within the [BOSS Afterburner](https://github.com/redeboer/BOSS_Afterburner). The repository has been split into BOSS IniSelect for the **initial event selection** and BOSS Afterburner for **final event selection**. The reason is that IniSelect is integrated with BOSS and therefore has to run on the IHEP server, while the Afterburner is perferably run locally (so you can use the newest versions of `gcc`, ROOT, etc).


## How to get it and contribute?

You can clone this repository to the IHEP server (or your local pc) using the following command. It is advised that you clone this repository to BESIII file system folder (i.e., first `cd /besfs/users/${USER}`).

```bash
  git clone ssh://git@github.com/redeboer/BOSS_IniSelect.git
```


## Overview of all related repositories

### [BOSS GitBook](https://besiii.gitbook.io/boss)

This GitBook contains basic *step-by-step tutorials* on BOSS and BESIII analysis procedures in general. In addition, this repository contains a growing *inventory of useful BOSS packages*. The source code is [available on GitHub](https://github.com/redeboer/BOSS_Gitbook) and is [open for contributions](https://besiii.gitbook.io/boss/) by BESIII members.

### BOSS IniSelect

A repository of BOSS packages used for **initial** event selection.

| Platform | Material            | URL |
|:---------|:--------------------|:----|
| GitHub   | Source code         | [github.com/redeboer/BOSS_IniSelect](https://github.com/redeboer/BOSS_IniSelect) |
| GitPages | Class documentation | [redeboer.github.io/BOSS_IniSelect](https://redeboer.github.io/BOSS_IniSelect/index.html) |
| GitBook  | Tutorial pages\*    | [redeboer.gitbook.io/boss_iniselect](https://redeboer.gitbook.io/boss_afterburner/) |

\*The tutorial pages are shared with the *Afterburner*.

### BOSS Afterburner

A repository of objects and scripts that can be used for **final** event selection. Here, you process the output of the BOSS IniSelect repository.

| Platform | Material            | URL |
|:---------|:--------------------|:----|
| GitHub   | Source code         | [github.com/redeboer/BOSS_Afterburner](https://github.com/redeboer/BOSS_Afterburner) |
| GitPages | Class documentation | [redeboer.github.io/BOSS_Afterburner](https://redeboer.github.io/BOSS_Afterburner/index.html) |
| GitBook  | Tutorial pages\*    | [redeboer.gitbook.io/boss_afterburner](https://redeboer.gitbook.io/boss_afterburner/) |

\*The tutorial pages are shared with *IniSelect*.
