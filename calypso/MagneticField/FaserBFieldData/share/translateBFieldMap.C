/*
 * translateBFieldMap.cpp
 *
 *  Created on: 29 Jan 2018
 *      Author: jhrdinka
 */

/// This root file translates a txt/csv bField map into a root ttree

/// @param inFile The path to the txt/csv input file
/// @param outFile The name of the output root file
/// cooridantes instead of cartesian coordinates
/// @param bScalor A scalor for the magnetic field to be applied
/// @param lScalor A scalor to the length to be applied
void
translateBFieldMap(std::string inFile,
                   std::string outFile,
                   double      bScalor = 1.,
                   double      lScalor = 1.)
{

  std::cout << "Registering new ROOT output File : " << outFile << std::endl;

  TFile* rootfile = TFile::Open(outFile.c_str(), "RECREATE");
  if (!rootfile) 
  {
    std::cout << "Could not open '" << outFile << std::endl;
    return;
  }
  if ( rootfile->cd() == false )
  {
    std::cout << "Could not change directory to " << outFile << std::endl;
    return; // could not make it current directory
  }

  TTree* tree = new TTree( "BFieldMap", "BFieldMap version FASER 1" );
  TTree* tmax = new TTree( "BFieldMapSize", "Buffer size information" );

  /// [1] Read in field map file
  std::cout << "Opening new txt/csv input File : " << inFile << std::endl;
  std::ifstream map_file(inFile.c_str(), std::ios::in);

  std::vector<double> xRaw, yRaw, zRaw, bxRaw, byRaw, bzRaw;

  std::string line;
  double      xpos = 0., ypos = 0., zpos = 0.;
  double      bx = 0., by = 0., bz = 0.;
  while (std::getline(map_file, line)) {
    if (line.empty() || line[0] == '%' || line[0] == '#'
        || line.find_first_not_of(' ') == std::string::npos)
      continue;

    std::istringstream tmp(line);
    char c;
    tmp >> xpos >> c >> ypos >> c  >> zpos >> c >> bx >> c >> by >> c >> bz;
    if (xRaw.empty() || xpos * lScalor > xRaw.back()) xRaw.push_back(xpos * lScalor);
    if (yRaw.empty() || ypos * lScalor > yRaw.back()) yRaw.push_back(ypos * lScalor);
    if (zRaw.empty() || zpos * lScalor > zRaw.back()) zRaw.push_back(zpos * lScalor);
    bxRaw.push_back(bx * bScalor);
    byRaw.push_back(by * bScalor);
    bzRaw.push_back(bz * bScalor);
  }
  map_file.close();
  std::cout << "Read " << xRaw.size() << "/" << yRaw.size() << "/" << zRaw.size() << " mesh positions" << std::endl;
  std::cout << "Read " << bxRaw.size() << " field values" << std::endl;

  int id;
  double xmin, xmax, ymin, ymax, zmin, zmax;
  double bscale;
  int nmeshx, nmeshy, nmeshz;
  double *meshx, *meshy, *meshz;
  int nfield;
  short *fieldx, *fieldy, *fieldz;

  // prepare arrays - need to know the maximum sizes
  unsigned maxmeshx(0), maxmeshy(0), maxmeshz(0), maxfield(0);
  maxmeshx = 2 * xRaw.size() - 1;
  maxmeshy = 2 * yRaw.size() - 1;
  maxmeshz = (200 + 1500 + 200 + 1000 + 200 + 1000 + 200)/5 + 1; // 861 z bins
  maxfield = maxmeshx * maxmeshy * maxmeshz;

  // store the maximum sizes
  tmax->Branch( "maxmeshx", &maxmeshx, "maxmeshx/i");
  tmax->Branch( "maxmeshy", &maxmeshy, "maxmeshy/i");
  tmax->Branch( "maxmeshz", &maxmeshz, "maxmeshz/i");
  tmax->Branch( "maxfield", &maxfield, "maxfield/i");
  tmax->Fill();

    // prepare buffers
  meshx = new double[maxmeshx];
  meshy = new double[maxmeshy];
  meshz = new double[maxmeshz];
  fieldx = new short[maxfield];
  fieldy = new short[maxfield];
  fieldz = new short[maxfield];
  // define the tree branches
  tree->Branch( "id", &id, "id/I" );
  tree->Branch( "xmin", &xmin, "xmin/D" );
  tree->Branch( "xmax", &xmax, "xmax/D" );
  tree->Branch( "ymin", &ymin, "ymin/D" );
  tree->Branch( "ymax", &ymax, "ymax/D" );
  tree->Branch( "zmin", &zmin, "zmin/D" );
  tree->Branch( "zmax", &zmax, "zmax/D" );
  tree->Branch( "bscale", &bscale, "bscale/D" );
  tree->Branch( "nmeshx", &nmeshx, "nmeshx/I" );
  tree->Branch( "meshx", meshx, "meshx[nmeshx]/D" );
  tree->Branch( "nmeshy", &nmeshy, "nmeshy/I" );
  tree->Branch( "meshy", meshy, "meshy[nmeshy]/D" );
  tree->Branch( "nmeshz", &nmeshz, "nmeshz/I" );
  tree->Branch( "meshz", meshz, "meshz[nmeshz]/D" );
  tree->Branch( "nfield", &nfield, "nfield/I" );
  tree->Branch( "fieldx", fieldx, "fieldx[nfield]/S" );
  tree->Branch( "fieldy", fieldy, "fieldy[nfield]/S" );
  tree->Branch( "fieldz", fieldz, "fieldz[nfield]/S" );

  id = 1;
  xmin = -xRaw.back();
  xmax =  xRaw.back();
  ymin = -yRaw.back();
  ymax =  yRaw.back();

// DIPOLE-00 values
//  zmin = -1700.0;
//  zmax =  2600.0;

// For DIPOLE-01 (May 2020)
// Upstream:  -1562.32  ->  -62.32
// Central:     137.726 -> 1137.726
// Downstream: 1337.726 -> 2337.726
  zmin = -1762.3;
  zmax =  2537.7;

  nmeshx = maxmeshx;
  nmeshy = maxmeshy;
  nmeshz = maxmeshz;
  nfield = maxfield;
  // the field values are stored as short integers; this is the conversion to kT
  // the raw field values are in Tesla; we multiply them by 10000 to store them as short ints
  // multiplying the short ints by 1e-7 then gives the field in units of kT
  bscale = 1.0e-7;  
  // store mesh values, including the ones we are creating
  for ( int j = 0; j < nmeshx; j++ ) {
    if (j < xRaw.size())
    {
      meshx[j] = -xRaw[xRaw.size() - 1 - j];
    }
    else
    {
      meshx[j] = xRaw[j - xRaw.size() + 1];
    }
  }
  for ( int j = 0; j < nmeshy; j++ ) {
    if (j < yRaw.size())
    {
      meshy[j] = -yRaw[yRaw.size() - 1 - j];
    }
    else
    {
      meshy[j] = yRaw[j - yRaw.size() + 1];
    }
  }
  for ( int j = 0; j < nmeshz; j++ ) {
    meshz[j] = zmin + j * 5.0;
  }
  // Now the tricky part...
  int bxrow = ((int)zRaw.size()) * ((int)yRaw.size());
  int byrow = ((int)zRaw.size());
  int jField = 0;
  for (int ix = -(((int)xRaw.size())-1); ix <= (((int)xRaw.size())-1); ix++)
  {
    bool mirrorVertical = (ix < 0);
    int ixRaw = std::abs(ix);
    for (int iy = -(((int)yRaw.size())-1); iy <= (((int)yRaw.size())-1); iy++)
    {
      mirrorVertical = (mirrorVertical && (iy >= 0))||(!mirrorVertical && (iy < 0));
      double verticalSign = (mirrorVertical ? -1.0 : 1.0);
      int iyRaw = std::abs(iy);      
      for (int iz = -340; iz <= 520; iz++)
      {
        int izRaw;
        bool mirrorAxial = false;
        double fieldDecay = 1.0;
        if (iz < -300) // upstream fringe region
        {
          izRaw = 140;
          double z = iz * 5.0;
          fieldDecay = erfc(0.0088*(100-(z+1600)))/2.0;
        }
        else if (iz <= -150) // upstream region, since table has long dipole after short
        {
          izRaw = 140 + (iz + 300);
        }
        else if (iz <= 140) // upstream mirror region, including first gap
        {
          izRaw = 140 - iz;
          mirrorAxial = true;
        }
        else if (iz <= 380) // central region, including second gap
        {
          izRaw = iz - 140;
        }
        else if (iz <= 480) // downstream mirror region
        {
          izRaw = 620 - iz;
          mirrorAxial = true;
        }
        else
        {
          izRaw = 140;
          double z = iz * 5.0;
          fieldDecay = erfc(0.0088*(100+(z-2500)))/2.0;
          mirrorAxial = true;
        }
        double axialSign = (mirrorAxial ? -1.0 : 1.0);
        // rotate the field by 90 degrees so it's horizontal
        double bxTemp = -byRaw[ixRaw * bxrow + iyRaw * byrow + izRaw];
        double byTemp =  bxRaw[ixRaw * bxrow + iyRaw * byrow + izRaw];
        double bzTemp =  bzRaw[ixRaw * bxrow + iyRaw * byrow + izRaw];
        // flip the vertical component (or not) based on the quadrant
        byTemp *= verticalSign;
        // flip the axial component (or not) based on where we are
        bzTemp *= axialSign;
        // if we are in either of the end regions, decrease the field
        fieldx[jField] = (short) (10000 * (fieldDecay * bxTemp));
        fieldy[jField] = (short) (10000 * (fieldDecay * byTemp));
        fieldz[jField] = (short) (10000 * (fieldDecay * bzTemp));
        jField++;
      }
    }
  }
  if (jField != maxfield)
  {
    std::cout << "Added " << jField << " field entries, but expected " << maxfield << std::endl;
  }
  tree->Fill();
  rootfile->Write();
  rootfile->Close();
// clean up
  delete[] meshx;
  delete[] meshy;
  delete[] meshz;
  delete[] fieldx;
  delete[] fieldy;
  delete[] fieldz;
}
