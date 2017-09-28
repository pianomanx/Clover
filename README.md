## Clover fork by RehabMan

This github project is a fork of the Clover bootloader/boot manager.

The 'svn' branch is a git-svn conversion to git from the Clover on sourceforge: https://sourceforge.net/p/cloverefiboot

The 'svn' branch and other branches are merge compatible with TheRacerMaster's fork here: https://github.com/al3xtjames/Clover

The 'master' branch is the current work in the fork.

The 'versioning' branch has some work arounds for bugs/problems with 'git-svn' and merged branches.  The master branch is generally merged with the versioning branch after updating the svn branch using 'git svn rebase' (to keep in sync with Clover on sourceforge).


### Downloads:

Pre-built Clover from my master branch is available on Bitbucket:

https://bitbucket.org/RehabMan/clover/downloads/


### RehabMan added features/fixes

- MountESP script is fixed for APFS configurations on 10.13.  It means the installer works for installing to the ESP on 10.13

- allow finer control over ACPI/SSDT/Generate/PStates with addition of Generate/APSN, Generate/APLF, Generate/PluginType options.  So, with Haswell or later, you can enable CPU PM with only config.plist/ACPI/SSDT/Generate/PluginType=true.  And with Sandy or Ivy, for use with SSDT.aml generated from ssdtPRgen.sh, but you need to drop the OEM tables and use Generate=true, you can disable APLF/APSN generation with Generate/PStates=true, Generate/CStates=true, Generate/APLF=false, Generate/APSN=false.  The new settings are backward compatible with old config.plist files (no change in behavior when the new options are missing, all new options are default true).  Note that because APSN/APLF are part of Generate/PStates, they have no effect unless Generate/PStates=true, whereas PluginType is independent, and is effective regardless of Generate/PStates selection.

- restore "Without caches (-f) " option in Clover "spacebar" menu

- remove -gux_ options from Clover "spacebar" menu (those options are just clutter, as GenericUSBXHCI.kext is not used with Intel xHCI)


### Building

A fork of Mikey1979's Build_Clover.command script (https://github.com/Micky1979/Build_Clover.git) is available here:

https://github.com/RehabMan/Build_Clover

It has the change required to use my repo instead of Clover on sourceforge, along with a few other changes.

Obtain copy of the script:
```
mkdir ~/Projects
cd ~/Projects
git clone https://github.com/RehabMan/Build_Clover.git clover.git
cd clover.git
```

Then you can invoke the script and use the menus:
```
./Build_Clover.git
```

The script will automatically clone master from my Clover fork.  If you wanted to use your own fork, modify the script as needed.


### A note regarding the 'svn' branch

You will not be able to reproduce the 'svn' branch using 'git svn clone'.  Something has changed in git svn or the Clover repo since the al3xtjames fork was created on github.  If you wish to have a copy of the svn branch with a connection to the svn repo, start with the 'base_git-svn.git.zip' that you find in this project.

To use it, download the base_git-svn.git.zip file from bitbucket (https://bitbucket.org/RehabMan/clover/downloads/base_git-svn.git.zip), then unzip into a folder of your choice.  Now you will have a copy of the 'svn' branch, but it is not up-to-date with current svn (it is Clover r4200).  You will notice that remote 'origin' points to 'https://github.com/al3xtjames/Clover' (you can see with 'git remote -v').

At this point, you can add other remotes, merge, and/or use git-svn rebase.

For example, this is my setup procedure:

```
# rename origin to theracermaster
git remote rename origin theracermaster
```

```
# add this repo as a remote
git remote add rehabman https://github.com/RehabMan/Clover
git fetch --all
```

```
# set upstream of svn branch to rehabman
git branch -u rehabman/svn
```

```
# checkout various branches
git checkout versioning
git checkout master
```

The procedure above is only needed if you wish to be able to 'git svn rebase' from the svn branch.  For normal use, you can just clone the repo as usual:

```
mkdir ~/Projects
cd ~/Projects
git clone https://github.com/RehabMan/Clover clover.git
```


### A note regarding versioning and 'git-svn rebase'

In order to update to commits from sourceforge (which is using svn), 'git svn rebase' from the svn branch is used:
```
git checkout svn
git svn rebase
```

For example, this is the output we get as of Clover r4207:
```
M	rEFIt_UEFI/Platform/boot.h
M	rEFIt_UEFI/Platform/gma.c
M	rEFIt_UEFI/Platform/kernel_patcher.c
r4201 = 0b9090561ba99bd51b7125a91751a7083cdeeb13 (refs/remotes/git-svn)
M	rEFIt_UEFI/Platform/Platform.h
M	rEFIt_UEFI/Platform/Settings.c
M	rEFIt_UEFI/Platform/kext_inject.c
M	rEFIt_UEFI/refit/lib.h
M	rEFIt_UEFI/refit/menu.c
r4202 = 0e1c326eefc41a343369fdb13d4a70976e7355cd (refs/remotes/git-svn)
M	rEFIt_UEFI/Platform/Settings.c
M	rEFIt_UEFI/Platform/kext_inject.c
r4203 = 6717b4608cfc0a7c41fe6f3b315fd496a9d43a4d (refs/remotes/git-svn)
M	rEFIt_UEFI/Platform/Platform.h
M	rEFIt_UEFI/Platform/Settings.c
M	rEFIt_UEFI/Platform/kext_inject.c
M	rEFIt_UEFI/refit/lib.h
M	rEFIt_UEFI/refit/menu.c
r4204 = c53e6dbd7aa66776c570f5df04f35526e737c3bb (refs/remotes/git-svn)
M	rEFIt_UEFI/libeg/image.c
M	rEFIt_UEFI/libeg/libscreen.c
M	rEFIt_UEFI/refit/main.c
r4205 = d40ce2df202edfef742836497e0d0f140b0e7d06 (refs/remotes/git-svn)
M	rEFIt_UEFI/refit/menu.c
r4206 = 778ca3c3d90c8dec71abdd2c74bed5936f7b7dbc (refs/remotes/git-svn)
M	rEFIt_UEFI/libeg/image.c
r4207 = 6b9fe519f39e431b06814678cf7cb2e058322848 (refs/remotes/git-svn)
First, rewinding head to replay your work on top of it...
Fast-forwarded svn to refs/remotes/git-svn.
```

From there, I update _svnver.txt using the versioning branch:
```
git checkout versioning
git merge svn
./sync_version.sh
```

The output:
```
Switched to branch 'svn'
Your branch is up-to-date with 'rehabman/svn'.
Switched to branch 'versioning'
Your branch is up-to-date with 'rehabman/versioning'.
git commit -m "update _svnver.txt 4207"
```

The commit for the new _svnver.txt has not been executed yet, although the command to be used is output by the sync_version.sh script.  If you check 'git diff --staged', you will see the change to _svnver.txt.  Note: If the versioning branch is already up-to-date with respect to current Clover on sourceforge, there will be no changes to commit.

If there are changes to commit, use the command that was output by the sync_version.sh script.  In the case above, it would be:

```
git commit -m "update _svnver.txt 4207"
```

After that, you can switch back to the master branch and merge the updated code from the versioning branch:
```
git checkout master
git merge versioning
```

Generally, I will try to keep all branches in my fork closely in sync with Clover on sourceforge svn.

