# BOSS IniSelect

**This repository has been migrated to the [BOSS Starter Kit](https://github.com/redeboer/BOSS_StarterKit) repository.** The Starter Kit contains the essentials for getting started with BOSS and some useful tools. The details are implemented as submodules, so that users can choose themselves which components they would like to use.

# Original `README.md`

This repository serves as an extra layer to the BESIII Offline Software System (BOSS). For more information on BOSS, see the [BOSS GitBook](https://besiii.gitbook.io/boss).

The BOSS IniSelect repository contains BOSS packages for **initial event selection** (hence the name). These packages are to facilitate analysis procedures in BESIII by further modulating the procedure for selecting tracks from a DST file. The main feature is the [TrackSelector](https://redeboer.github.io/BOSS_IniSelect/classTrackSelector.html), which is a *base algorithm* that takes care of all procedures required by an initial event selection. The specifics of your own analysis are defined in an algorithm that you derive from the `TrackSelector`.

Originally, this repository was contained within the [BOSS Afterburner](https://github.com/redeboer/BOSS_Afterburner). The repository has been split into BOSS IniSelect for the **initial event selection** and BOSS Afterburner for **final event selection**. The reason is that IniSelect is integrated with BOSS and therefore has to run on the IHEP server, while the Afterburner is perferably run locally (so you can use the newest versions of `gcc`, ROOT, etc).


## Major functions

* A `bash` script framework for generating and splitting job files. This allows you to quickly generate a collection of job submit files any type of simulation, reconstruction, or analysis job. Just give a directory of DST files and the number of files you want per job and the paths to the DST files will be distributed accordingly. The same goes for random number generation etc required for simulation and reconstruction jobs.

* A base-derived algorithm structure (`TrackSelector` and its derived classes) for **initial event selection**. Specifics of an analysis are defined in the derived classes so that you don't have to worry about the basics, like creating a collection of tracks. This makes the code for the actual analysis procedure cleaner.

* An `NTupleContainer` faciliates the booking procedure for `NTuple`s.

* A [framework for **unit testing**](https://github.com/redeboer/BOSS_IniSelect/tree/master/workarea/Analysis/IniSelect/IniSelect-00-00-00/UnitTests). You can (should!) define boolean test cases for the output of algorithms or objects you are designing. These unit tests (1) serve as a 'playground' in which you can experiment with your code and (2) *helps you make sure that your code keeps functioning as you want it to* if you decide to modify it. See [here](https://github.com/redeboer/BOSS_IniSelect/blob/master/workarea/Analysis/IniSelect/IniSelect-00-00-00/src/UnitTests/Test_ParticleDatabase.cxx) for an example of what such a test looks like.

* A [`ParticleDatabase`](https://redeboer.github.io/BOSS_IniSelect/classParticleDatabase.html): just fill in a name or PDG code of the particle you are looking for and you have access to properties like mass, charge, and lifetime through the resulting [`Particle`](https://redeboer.github.io/BOSS_IniSelect/classParticle.html) object. No need to initialize and the database access methods are `static`.

* Selections of identified tracks are assembled to [`TrackCollection`](`https://redeboer.github.io/BOSS_IniSelect/classTrackCollection.html`) objects in algorithms you derive of `TrackSelector`. Other objects that are of help at this level too are:

    * Kalman kinematic fit and vertex fit procedure is contained within objects [`KinematicFitter`](https://redeboer.github.io/BOSS_IniSelect/classKinematicFitter.html) and [`VertexFitter`](https://redeboer.github.io/BOSS_IniSelect/classVertexFitter.html).

    * [`CutObject`](https://redeboer.github.io/BOSS_IniSelect/classCutObject.html) helps recording cut flow. The cut names are defined automatically as corresponding job properties and can be set at run time through the job options file.

    * [`JobSwitch`](https://redeboer.github.io/BOSS_IniSelect/classJobSwitch.html) object: if this object is declared in the header and constructor of the `TrackSelector` or a derived algorithm, it allows one to set its corresponding switch (`true`/`false`) through the job options.

    * [`KKFitResults`](https://redeboer.github.io/BOSS_IniSelect/classKKFitResult.html), an abstract container for storing fit results from the Kalman kinematic fit procedure. It also allows one to compare different fit results and to write the one which is best.

* Beginnings of exception handling (`try`-`throw`-`catch`) through the objects in `namespace` [`Error`](https://redeboer.github.io/BOSS_IniSelect/namespaceTSGlobals_1_1Error.html) instead of status codes.

* A [handy `bash` script](https://github.com/redeboer/BOSS_IniSelect/blob/master/InstallRepository.sh) for *downloading and installing* the latest versions of HEP frameworks like ROOT, CLHEP and Gaudi.


## How to get it and contribute?

You can clone this repository to the IHEP server (or your local pc) using the following command. It is advised that you clone this repository to BESIII file system folder (i.e., first `cd /besfs/users/${USER}`).

```bash
git clone ssh://git@github.com/redeboer/BOSS_IniSelect.git
```

To contribute, please have a look at the [contributing page](https://besiii.gitbook.io/boss/appendices/contributing) on GitBook.


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
