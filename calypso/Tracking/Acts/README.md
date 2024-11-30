
1) Go to the installation (run) directory

2) source ./setup.sh

3) To write the tracking geometry, type the command: FaserActsWriteTrackingGeometry.py

4) To run the extrapolator, type the command: FaserActsExtrapolationAlg.py

5) The track finding requires a json file with the correct material in the directory in which you run it. A copy can be found on lxplus: /afs/cern.ch/user/t/tboeckh/public/material/material-maps.json  
   If you do not want to use the material you have to comment out this line: ConfigFlags.TrackingGeometry.MaterialSource = "Input"

6) To enable additional output set the noDiagnostics flag to False

7) To do the track finding, type the command: CombinatorialKalmanFilterAlg.py

8) To do the track finding and re-fit the best tracks, type the command: CKF2.py  
   (this is the recommended approach)
