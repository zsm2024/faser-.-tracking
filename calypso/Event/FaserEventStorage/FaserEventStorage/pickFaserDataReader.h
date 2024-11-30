/**
 \brief Function to choose the correct reader of the data files.

 It recognizes data file formats of 2003 and later.
 It chooses a file I/O library for plain disk files, Castor 
 files or dCache files.

 \file EventStorage/pickDataReader.h
 \author Szymon Gadomski
 \date Feb 2004 - Feb 2007
*/

#ifndef FASEREVENTSTORAGE_PICKDATAREADER_H
#define FASEREVENTSTORAGE_PICKDATAREADER_H

#include "FaserEventStorage/DataReader.h"
#include <string>

/**
   \param fileName the name of the data file to read
*/
FaserEventStorage::DataReader * pickFaserDataReader(std::string fileName);



#endif
