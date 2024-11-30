This package stores the production reconstruction scripts.

The following scripts run production reconstruction jobs:
* `faser_reco.py` - this is the python script for the main reconstruction job.

* `submit_faser_reco.sh` - bash script to set up the environment and run the faser_reco job.  This can be called from condor or other batch scheduling systems.


Production reco is intended to be run from a specific git tag.

To see the available tags, use
```
git tag -l "reco/*"
```
and to check out a tag simply use the tag name in the checkout command
```
git checkout reco/r0001
```

To make a production reco tag, the easiest is to just tag a commit
```
git tag -a reco/r0001 9fceb02 -m "tag message"
```

This needs to be pushed to the upstream master
```
git push origin reco/r0001
git push upstream reco/r0001
```
If you need to delete a tag from the repository
```
git push origin --delete reco/r0001
```

To checkout a branch based on a tag (to be able to make fixes):
```
git checkout -b mybranch reco/r0001
```

To find the updated files between two tags, and the changes in those files
```
git diff tag1 tag2 --stat
git diff tag1 tag2 -- filename
```
