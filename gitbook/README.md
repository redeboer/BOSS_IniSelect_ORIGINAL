# Introduction

{% hint style="warning" %}
Feedback on these pages is very welcome! See [About](appendices/about.md) for contact details.
{% endhint %}

## The _BOSS Afterburner_ repository

This repository has been set up as an extension to the BOSS analysis framework of the [BESIII collaboration](http://bes3.ihep.ac.cn). Initially, it started as a collection of notes on the use of BOSS, but these notes have been migrated to the [BOSS GitBook](https://besiii.gitbook.io/boss).

The _BOSS Afterburner_ is aimed at facilitating light hadron spectroscopy within BESIII by providing an **additional layer to BOSS**. The aim is to **provide a platform where analysers can continuously collaborate on code for analysis tasks** — on the analysis level, the ability to easily implement feedback or updates to the analysis framework are vital.

The repository essentially contains three components:

1. The [tutorial pages](https://besiii.gitbook.io/boss) that you are currently looking at. These pages describe the philosophy and use of _The BOSS Afterburner_.
2. A [base-derived algorithm structure for the **initial event selection**](https://redeboer.github.io/BOSS_Afterburner/group__BOSS.html). This part of the repository is integrated in BOSS and is designed to facilitate collaboration on code for initial event selection in light hadron spectroscopy at BESIII.
3. An [analysis framework for **final event selection**](https://redeboer.github.io/BOSS_Afterburner/group__BOSS__Afterburner.html). This part of the repository is designed to formalise and standardise loading, plotting, and fitting procedures that you perform on the output of the initial event selection.

The source code for all of the above is [available on GitHub](https://github.com/redeboer/BOSS_Afterburner) and can be contributed to through `git`.

{% hint style="info" %}
If you do not have an IHEP networking account, it is better to check out the official [Offline Software page](http://english.ihep.cas.cn/bes/doc/2247.html) of BESIII — this framework can only be of use if you are a member of the BESIII collaboration and if you have access to the software of this collaboration. You can also have a look at the links in the section [Further reading of the BOSS GitBook](https://besiii.gitbook.io/boss/appendices/references).
{% endhint %}

## Accessing _The BOSS Afterburner_

There are three channels through which you can access _The BOSS Afterburner_ online.

1. **GitBook** \([redeboer.gitbook.io/boss\_afterburner](https://redeboer.gitbook.io/boss_afterburner)\), which provides an accessible graphical interface for the tutorial-like pages on using BOSS, on the initial event selection packages, and on the final event selection framework in this repository.
2. **GitPages** \([redeboer.github.io/BOSS\_Afterburner](https://redeboer.github.io/BOSS_Afterburner/)\), which contains class documentation for the initial and final event selection components of _The BOSS Afterburner_. This platform can be consulted for more details about the design of these two frameworks.
3. **GitHub** \([github.com/redeboer/BOSS\_Afterburner](https://github.com/redeboer/BOSS_Afterburner)\), which is the complete repository itself. It contains the source code \(Markdown, C++, HTML, bash, etc.\) for all components. Collaboration on all of the above is possible through this platform, using `git`.

More briefly put, GitHub 'feeds' the content on GitBook and GitPages.

![Means of accessing The BOSS Afterburner](.gitbook/assets/boss_repository-1.png)

