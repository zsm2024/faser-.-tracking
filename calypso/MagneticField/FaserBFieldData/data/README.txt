brief info for FASERBField_xyz.txt
file format           : txt
coordinate system     : xyz
Dimensions            : x[-100.,100.],y[-100.,100.],z[0.,4400.]
Length units          : mm
bField unit           : T
nBins                 : [41,41,880]
first quadrant/octant : false

The file FASERBField_xyz.txt was created on 21.08.2019. It contains full magnetic field map (all octants) of the magnetic flux density in cartesian coordinates for different positions, spanning from -0.1m to 0.1m in x/y and from 0m to 4.4m in z. The first row gives the x position, the second the y position, the third the z position, the fourth the magnetic flux density in x (Bx), the fith the magnetic flux density in y (By), the sixth row is the magnetic flux density in z (Bz), and the last the total magnitude of the flux density. The positions are given in 0.05m steps and  have 41 bins in x/y and 880 bins in z. The length unit is millimeter (mm) and the unit of the magnetic flux density is Tesla(T).

The file FASERBField_xyz.root was created on 21.08.2019 from the the file FASERBField_xyz.txt using the script 'calypso/Tracker/MagneticField/translateBFieldMap.cpp' with the following command:

translateBFieldMap("FASERBField_xyz.txt","FASERBField_xyz.root","bField",false,1.,1.)

It contains the full magnetic field map (all octants) of the magnetic flux density in cartesian coordinates for different positions, spanning from -0.1m to 0.1m in x/y and from 0m to 4.4m in z. The positions are given in 0.05m steps and  have 41 bins in r/x/y 880 bins in z. The length unit is millimeter (mm) and the unit of the magnetic flux density is Tesla(T). The file contains a tree ‘bField’ which has the following branches describing the coordinates of the positions and the different components of the magnetic flux density: ‘x’,’y’,’z’,’Bx’,’By’,’Bz’.