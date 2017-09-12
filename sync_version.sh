#!/bin/bash
# update _svnver.txt to match the svn version in the svn branch
git checkout svn
git svn info | grep ^Revision: | tr -cd [:digit:] >_svnver_temp.txt
git checkout versioning
cp _svnver_temp.txt _svnver.txt
svn_ver=$(cat _svnver.txt)
git add _svnver.txt
echo git commit -m \""update _svnver.txt $svn_ver"\"
