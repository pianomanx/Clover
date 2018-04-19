## Clover fork by RehabMan

This github project is a fork of the Clover bootloader/boot manager.

The 'svn' branch is a git-svn conversion to git from the Clover on sourceforge: https://sourceforge.net/p/cloverefiboot

The 'svn' branch and other branches are merge compatible with TheRacerMaster's fork here: https://github.com/al3xtjames/Clover

The 'master' branch is the current work in the fork.

The 'versioning' branch has some work arounds for bugs/problems with 'git-svn' and merged branches.  The master branch is generally merged with the versioning branch after updating the svn branch using 'git svn rebase' (to keep in sync with Clover on sourceforge).


### Downloads:

Pre-built Clover from my master branch is available on Bitbucket:

https://bitbucket.org/RehabMan/clover/downloads/

### Known Problems

Known problems specific to the RehabMan build will be placed here.
Currently N/A.


### RehabMan added features/fixes

Released as Clover_v2.4k_r4444.RM-4652.c1f8602f.zip on bitbucket, 2018-04-19:

- update to latest Clover svn (4444)


Released as Clover_v2.4k_r4423.RM-4614.cf0897a3.zip on bitbucket, 2018-04-01:

- updated to latest Clover svn (4423)

- updated AptioMemoryFix.efi (built from latest sources)


Released as Clover_v2.4k_r4398.RM-4578.541fa44f.zip on bitbucket, 2018-02-04:

- updated to latest Clover svn (4398)

- now includes AptioMemoryFix.efi

- reverted change that omits OsxFatBinaryDrv-64.efi (this EFI driver is needed by Snow Leopard and Lion)


Released as Clover_v2.4k_r4359.RM-4506.7036cf0a.zip on bitbucket, 2017-12-15:

- no change, just build identifiers (git commit hash) fixed due to git commit log snafu


Released as Clover_v2.4k_r4359.RM-4506.c5fc0346.zip on bitbucket, 2017-12-15 (redacted):

- fixed a bug when AutoMerge=true and SortedOrder specified would cause a merged table to be also treated as an add-on SSDT.  (Note: SortedOrder is not needed when using AutoMerge=true, but it should not result in duplicate table insertion)


Released as Clover_v2.4k_r4359.RM-4503.86e25a09.zip on bitbucket, 2017-12-15 (redacted):

- fixed a bug with config.plist/ACPI/DSDT/Fixes/FixHeaders (NUL characters should not have been translated to spaces)

- fixed problems with AutoMerge=true when table headers needed fixing (header fixing was causing problems with table id comparisons)... note that the above bug (where table headers were unecessarily being modified), exposed this bug a bit easier/more often.

- updated to latest Clover svn (4359)


Released as Clover_v2.4k_r4334.RM-4467.cac64919.zip on bitbucket, 2017-12-05:

- updated to latest Clover svn (4334)


Released as Clover_v2.4k_r4330.RM-4456.06db6224.pkg on bitbucket, 2017-12-01:

- a DropTables attempt to drop a merged table will be ignored (debug in Clover bootlog)

- fixing bugs

- updated to latest Clover svn (4330)


Released as Clover_v2.4k_r4328.RM-4448.d22dddfd.pkg on bitbucket, 2017-11-31 (redacted):

- restored "without caches" option (AGAIN!)

- fixed an issue with config.plist/ACPI/DSDT/Patches as it concerned non-merged SSDTs from ACPI/patched when there were NULL entries in XSDT

- added code to deal correctly with NULL entries in XSDT and RSDT (related to above issue)

- added code to deal with double-NULL (original Clover AcpiPatcher.c terminated the XSDT/RSDT at the position of double-NULL)

- internal: lots of code cleanup/simplification in AcpiPatcher.c

- updated to latest Clover svn (4328)


Released as Clover_v2.4k_r4318.RM-4429.43710d68.pkg on bitbucket, 2017-11-24 (redacted):

- fixed an issue with AutoMerge when SSDTs have the same OEM table ID

- fixed an issue with AutoMerge when DropTables was used to drop one or more SSDTs

- added addional diagnostics to the Clover bootlog for AutoMerge

- internal: cleanup UINTN vs. INTN and -1 usage/portability regarding MatchIndex in AcpiPatcher.c

- internal: fixed a few memory leaks and did some minor cleanup

- updated to latest Clover svn (4318)


Released as Clover_v2.4k_r4270.RM-4359.cc5db5ba.pkg on bitbucket, 2017-10-28 (redacted):

- added config.plist/ACPI/SSDT/NoDynamicExtract (boolean) option, default false.  When set true, this flag will disable extraction of dynamic SSDTs when using F4.  Dynamic SSDTs are rarely needed and usually just cause confusion (mistakenly putting them in ACPI/patched).


Released as Clover_v2.4k_r4265.RM-4346.38d5ad43.zip on bitbucket, 2017-10-26 (redacted):

- internal: automatic calculation of embedded image/font sizes

- internal: rewrote platformdata.c to use single data structure array (easier to read/maintain)

- internal: removed need to manually maintain padding/alignment in Platform.h (easier to maintain)

- internal: some comments regarding existing memory leaks (more review + fixes to come)

- fixed code that was calling FreePages for data allocated with FreePool (AcpiPatcher.c calls to egLoadFile)

- added back ability to boot with or without injected kexts

- changed installer backup default to save one backup (intead of none) when installing to the EFI partition

- "Cancel hibernate" is not presented when boot target is not hibernated

- fixed spacebar and escape key handling within the GUI

- config.plist/SystemParameters/InjectKexts=Detect works in my version. Broken in official Clover from r4242.

- friendly names for config.plist/ACPI/DSDT/Fixes. Removed the bitmask suffix and made capitalization consistent.

    - "AddDTGP_0001" -> "AddDTGP"
    - "FixDarwin_0002" -> "FixDarwin"
    - "FixShutdown_0004" -> "FixShutdown"
    - "AddMCHC_0008" -> "AddMCHC"
    - "FixHPET_0010" -> "FixHPET"
    - "FakeLPC_0020" -> "FakeLPC"
    - "FixIPIC_0040" -> "FixIPIC"
    - "FixSBUS_0080" -> "FixSBUS"
    - "FixDisplay_0100" -> "FixDisplay"
    - "FixIDE_0200" -> "FixIDE"
    - "FixSATA_0400" -> "FixSATA"
    - "FixFirewire_0800" -> "FixFirewire"
    - "FixUSB_1000" -> "FixUSB"
    - "FixLAN_2000" -> "FixLAN"
    - "FixAirport_4000" -> "FixAirport"
    - "FixHDA_8000" -> "FixHDA"
    - "FixDarwin7_10000" -> "FixDarwin7"
    - "FIX_RTC_20000" -> "FixRTC"
    - "FIX_TMR_40000" -> "FixTMR"
    - "AddIMEI_80000" -> "AddIMEI"
    - "FIX_INTELGFX_100000" -> "FixIntelGfx"
    - "FIX_WAK_200000" -> "FixWAK"
    - "DeleteUnused_400000" -> "DeleteUnused"
    - "FIX_ADP1_800000" -> "FixADP1"
    - "AddPNLF_1000000" -> "AddPNLF"
    - "FIX_S3D_2000000" -> "FixS3D"
    - "FIX_ACST_4000000" -> "FixACST"
    - "AddHDMI_8000000" -> "AddHDMI"
    - "FixRegions_10000000" -> "FixRegions"
    - "FixHeaders_20000000" -> "FixHeaders"
    - "FixMutex" (no old name for "FixMutex" as it is new)

- config.plist/ACPI/DSDT/Fixes/FixMutex.  This patch fixes any Mutex declared with a non-zero SyncLevel.  Acquiring such a Mutex in macOS/OS X causes ACPI abort (even when used in a legal scenario).  Non-zero SyncLevel Mutex objects are one of the common causes of ACPI battery method failure.

- config.plist/ACPI/AutoMerge boolean (default false).  When set true, it changes the way files are handled in ACPI/patched.  Instead of adding such files to the end of the XSDT (eg. treating as add-on table/SSDT), if the signature, index, and OemTableId matches an existing OEM table, it will replace that table.  With this feature, much like we take for granted with DSDT, it is possible to patch individual SSDTs (or other tables) simply by placing the patched file in ACPI/patched.  No need to mess with DropOem or DropTables.  And the original order is maintained.  The matching for SSDTs is based on naming, where the naming convention used by the F4 extractor is used to identify the SSDT position within the XSDT.  As an example, if your ACPI/origin had an SSDT-6-SaSsdt.aml, and you wished to patch it, you can simply patch the file as needed and place in ACPI/patched.  Same if you put it in ACPI/patched as SSDT-6.aml.  Becuase some OEM ACPI sets do not use unique text in the OEM table-id field, Clover uses both the OEM table-id and the number that is part of the file name to determine the location of the original in the XDST.  If you stick with the names provided in ACPI/origin, it can't go wrong.

- config.plist/ACPI/SSDT/NoOemTableId boolean (default false).  When set true, the OEM table-id is NOT added to the end of the file name in a Clover F4 ACPI dump to ACPI/origin.

- strip trailing spaces from SSDT names when the OEM table-id is added as a suffix (when NoOemTableId=false).


Released as Clover_v2.4k_r4210.zip on bitbucket, 2017-09-09:

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

