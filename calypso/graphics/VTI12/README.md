To run on Calypso MC data (from an installation (run) directory):

% source ./setup.sh

% export VP1PLUGINPATH=/home/yourname/path/to/runfolder/lib

% vti12 -mc -noautoconf -nosortdbreplicas &lt;input HITS or EVNT file&gt;

Note that VP1PLUGINPATH can be ninja-changed by asetup, and if it does not include the Calypso installation library folder, nothing will work.  Also note that it must be an ABSOLUTE (not relative) path!

To run:

python -m VTI12Algs.VTI12AlgConfig --filesInput=my.HITS.pool.root

Currently the geometry and conditions tags, FASERNU-04 and OFLCOND-FASER-03 respectively, are hard-coded.