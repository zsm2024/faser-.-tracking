/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Header: /build/atlas/cvs/atlas/offline/InnerDetector/InDetDetDescr/InDetIdentifier/test/test_indet_id.cxx,v 1.11.2.1 2009-03-26 21:54:32 jtseng Exp $ 
  
#include "IdDictParser/IdDictParser.h"  
#include "Identifier/Range.h" 
#include "Identifier/IdentifierHash.h" 
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "GaudiKernel/System.h" 
#include <iostream> 
 
typedef Identifier::size_type  size_type ;

// static void tab (size_t level) 
// { 
//     for (size_t i = 0; i < level; ++i) std::cout << " "; 
// } 
 
//  static ExpandedIdentifier
//  pixel_id ( int barrel_ec,  
//  	   int layer_disk, 
//  	   int phi_module, 
//  	   int eta_module,      
//  	   int phi_index,
//  	   int eta_index) 
//  {
//      // Build identifier
//      ExpandedIdentifier result;
//      result << 1 << barrel_ec << layer_disk << phi_module << eta_module;
//      result << phi_index << eta_index;

//      assert (result.last_error () == ExpandedIdentifier::none);

//      return result;
//}

static void
check_sct_timing(IdDictMgr& idd)
{
    
    std::cout << "=========>  Begin  check_sct_timing " << std::endl;
    std::cout << "=========>  Begin  check_sct_timing " << std::endl;

    // Check the timing for sct 

    FaserSCT_ID sct_id;

    longlong startOfUserTime    = System::userTime( System::microSec );
    longlong startOfKernelTime  = System::kernelTime   ( System::microSec );
    longlong startOfElapsedTime = System::ellapsedTime ( System::microSec );

    if(sct_id.initialize_from_dictionary (idd)) {
	std::cout << "check_lar_neighbour_timing - cannot init from dict" << std::endl;
    }

    std::cout << "Timing for init from dictionary " << std::endl;
    
    /// following lines could be platform dependent!
    longlong deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    longlong deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    longlong deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "tries, user, kernel, elapsed " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << std::endl;


    /// Loop over all channel hashes
    size_type hash_max = sct_id.wafer_hash_max ();

    std::cout << "Number of channels " <<  hash_max
	      << std::endl;

    int nloops = 10000;
    float num  = hash_max*nloops;
    Identifier::value_type n = 0;


    /// Empty loop

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;

    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
	    n += i;
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Empty loop " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    float user    = (float)deltaUser/num;
    float kernel  = (float)deltaKernel/num;
    float elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    
    /// Wafer Identifier access

    /// Store in object the measured times
    Identifier wafer_id;
    IdContext channelContext = sct_id.wafer_context();
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

//    const std::vector<Identifier>*       em_vec = sct_id.em_vec();
//    std::vector<Identifier>::const_iterator       em_vec_begin = sct_id.em_vec()->begin();

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
	    n += sct_id.wafer_id(i).get_compact();
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID from hash " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    
    /// Identifier and barrel_ec field access

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
	    wafer_id = sct_id.wafer_id(i);
	    n += sct_id.station(wafer_id);
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID from hash and access barrel_ec field " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    
    /// Identifier and layer_disk field access

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
//		sct_id.get_id(i, wafer_id, &channelContext);
	    wafer_id = sct_id.wafer_id(i);
	    n += sct_id.layer(wafer_id);
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID from hash and access layer_disk field " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    

    /// Identifier and phi_module field access

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
//		sct_id.get_id(i, wafer_id, &channelContext);
	    wafer_id = sct_id.wafer_id(i);
	    n += sct_id.phi_module(wafer_id);
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID from hash and access phi_module field " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    
    /// Identifier and eta_module field access

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
	    wafer_id = sct_id.wafer_id(i);
	    n += sct_id.eta_module(wafer_id);
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID from hash and access eta_module field " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    
    /// Identifier and side field access

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
	    wafer_id = sct_id.wafer_id(i);
	    n += sct_id.side(wafer_id);
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID from hash and access side field " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    nloops = 1000;
    num  = hash_max*nloops;

    /// Identifier access and hash access again

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
//		sct_id.get_id(i, wafer_id, &channelContext);
	    wafer_id = sct_id.wafer_id(i);
	    n += sct_id.wafer_hash(wafer_id);
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID from hash and get hash back again " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    

    nloops = 3000;
    num    = hash_max*nloops;

    /// Identifier and wafer fields access

    /// Store in object the measured times
    int barrel_ec;
    int layer_disk;
    int phi_module;
    int eta_module;
    int side;
    
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
	    wafer_id   = sct_id.wafer_id(i);
	    barrel_ec  = sct_id.station(wafer_id);
	    layer_disk = sct_id.layer(wafer_id);
	    phi_module = sct_id.phi_module(wafer_id);
	    eta_module = sct_id.eta_module(wafer_id);
	    side       = sct_id.side(wafer_id);
	    n         += barrel_ec + layer_disk + eta_module + phi_module + side;
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID and wafer fields " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    
    nloops = 1000;
    num    = hash_max*nloops;


    /// Identifier and wafer fields access and wafer ID again

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<hash_max; ++i) {
	    ++n;
//		sct_id.get_id(i, wafer_id, &channelContext);
	    wafer_id = sct_id.wafer_id(i);
	    barrel_ec  = sct_id.station(wafer_id);
	    layer_disk = sct_id.layer(wafer_id);
	    phi_module = sct_id.phi_module(wafer_id);
	    eta_module = sct_id.eta_module(wafer_id);
	    side       = sct_id.side(wafer_id);
	    n         += barrel_ec + layer_disk + eta_module + phi_module + side +
              sct_id.wafer_id(barrel_ec, layer_disk, phi_module, eta_module, side).get_compact();
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get ID and wafer fields and wafer ID again " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    nloops = 100000;

    // Save sct ids

    unsigned int nstripids = 20000;
    num     = nstripids*nloops;
    Identifier stripid;
    std::vector<Identifier> stripids(nstripids);

    FaserSCT_ID::const_expanded_id_iterator	first_sct = sct_id.strip_begin();  
    FaserSCT_ID::const_expanded_id_iterator	last_sct  = sct_id.strip_end();
    unsigned int nids = 0;
    for (; first_sct != last_sct && nids < nstripids; ++first_sct, ++nids) {
	const ExpandedIdentifier& exp_id = *first_sct;
	stripids[nids] =  sct_id.strip_id (exp_id[2],
					   exp_id[3], 
					   exp_id[4],
					   exp_id[5],
					   exp_id[6],
					   exp_id[7]);
    }

    /// sct Identifier access - empty loop

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

//    const std::vector<Identifier>*       em_vec = sct_id.em_vec();
//    std::vector<Identifier>::const_iterator       em_vec_begin = sct_id.em_vec()->begin();

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<nstripids; ++i) {
	    ++n;
	    n += i;
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get sct ID empty loop " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;

    nloops = 10000;
    num    = nstripids*nloops;

    /// Get sct id

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

//    const std::vector<Identifier>*       em_vec = sct_id.em_vec();
//    std::vector<Identifier>::const_iterator       em_vec_begin = sct_id.em_vec()->begin();

    n = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<nstripids; ++i) {
	    ++n;
	    stripid = stripids[i];
	    n += stripid.get_compact();
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get sct ID from vector " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;

    nloops = 1000;
    num    = nstripids*nloops;

    int strip;
    
    /// sct Identifier field access

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    nids = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<nstripids; ++i) {
	    ++n;
	    stripid = stripids[i];
	    barrel_ec  = sct_id.station(stripid);
	    layer_disk = sct_id.layer(stripid);
	    phi_module = sct_id.phi_module(stripid);
	    eta_module = sct_id.eta_module(stripid);
	    side       = sct_id.side(stripid);
	    strip      = sct_id.strip(stripid);
	    n         += barrel_ec + layer_disk + eta_module + phi_module + 
              side + strip + stripid.get_compact();
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get sct ID fields " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    nloops = 300;
    num    = nstripids*nloops;

    /// sct Identifier field access and id back

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    nids = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<nstripids; ++i) {
	    ++n;
	    stripid = stripids[i];
	    barrel_ec  = sct_id.station(stripid);
	    layer_disk = sct_id.layer(stripid);
	    phi_module = sct_id.phi_module(stripid);
	    eta_module = sct_id.eta_module(stripid);
	    side       = sct_id.side(stripid);
	    strip      = sct_id.strip(stripid);
	    n         += barrel_ec + layer_disk + eta_module + phi_module + 
              side + strip + stripid .get_compact()+ 
              sct_id.strip_id(barrel_ec, 
                              layer_disk,
                              phi_module,
                              eta_module,
                              side,
                              strip).get_compact();
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get sct ID fields and sct id back " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    

    /// sct Identifier get wafer id

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    nids = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<nstripids; ++i) {
	    ++n;
	    stripid = stripids[i];
	    n         +=  stripid.get_compact() + sct_id.wafer_id(stripid).get_compact();
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get sct ID then wafer id " << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    

    /// sct Identifier get wafer id and eta/phi index

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    nids = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<nstripids; ++i) {
	    ++n;
	    stripid = stripids[i];
	    strip      = sct_id.strip(stripid);
	    n         += strip + stripid.get_compact() +
              sct_id.wafer_id(stripid).get_compact();
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get sct, wafer id and and eta/phi index" << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    
    

    /// sct Identifier get wafer id and eta/phi index and strip id again

    /// Store in object the measured times
    startOfUserTime    = System::userTime( System::microSec );
    startOfKernelTime  = System::kernelTime   ( System::microSec );
    startOfElapsedTime = System::ellapsedTime ( System::microSec );

    n = 0;
    nids = 0;
    for (int j = 0; j < nloops; ++j) {
	for (size_type i=0; i<nstripids; ++i) {
	    ++n;
	    stripid = stripids[i];
	    strip      = sct_id.strip(stripid);
	    wafer_id   = sct_id.wafer_id(stripid);
	    n         += strip + stripid.get_compact() + wafer_id.get_compact() +
              sct_id.strip_id(wafer_id, strip).get_compact();
	}
    }

    /// following lines could be platform dependent!
    deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "Get sct, wafer id and and eta/phi index and stripid again" << std::endl;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << n << " " 
	      << std::endl;
    user    = (float)deltaUser/num;
    kernel  = (float)deltaKernel/num;
    elapsed = (float)deltaElapsed/num;
    std::cout << "tries, user, kernel, elapsed " 
	      << num << " " 
	      << user << " " 
	      << kernel << " "
	      << elapsed << " " 
	      << std::endl;
    

}

static void
check_sct_decoding(IdDictMgr& idd)
{
    
    std::cout << "=========>  check_sct_decoding" << std::endl;
    std::cout << "=========>  check_sct_decoding" << std::endl;

    FaserSCT_ID sct_id;

    if(sct_id.initialize_from_dictionary (idd)) {
	std::cout << "check_sct_decoding - cannot init from dict" << std::endl;
    }

    sct_id.set_do_checks(true);

    sct_id.test_wafer_packing	();

}

int main (int argc, char* argv[])  
{  
    if (argc < 2) return (1);  
  

    longlong startOfUserTime    = System::userTime( System::microSec );
    longlong startOfKernelTime  = System::kernelTime   ( System::microSec );
    longlong startOfElapsedTime = System::ellapsedTime ( System::microSec );



    IdDictParser parser;  

    
//    parser.register_external_entity("InnerDetector", "InDetIdDictFiles/IdDictInnerDetector_IBL-01.xml");
//    parser.register_external_entity("LArCalorimeter", "IdDictLArCalorimeter_H8_2004.xml");
//    parser.register_external_entity("InnerDetector", "IdDictInnerDetector_slhc.xml");
//    parser.register_external_entity("InnerDetector", "IdDictInnerDetector_Cosmic.xml");
//    parser.register_external_entity("InnerDetector", "IdDictInnerDetector_CTB2004.xml");

    IdDictMgr& idd = parser.parse (argv[1]);  
//    IdDictMgr& idd = parser.parse (argv[1], "initial_layout");  


    std::cout << "Timing for parsing dictionaries " << std::endl;
    
    /// following lines could be platform dependent!
    longlong deltaUser    = System::userTime    ( System::microSec ) - startOfUserTime   ;
    ///
    longlong deltaKernel  = System::kernelTime  ( System::microSec ) - startOfKernelTime ;
    ///
    longlong deltaElapsed = System::ellapsedTime( System::microSec ) - startOfElapsedTime ;

    std::cout << "tries, user, kernel, elapsed " 
	      << deltaUser << " " 
	      << deltaKernel << " "
	      << deltaElapsed << " " 
	      << std::endl;




    std::cout << "got dict mgr " << std::endl;
//      std::cout << "regenerate for tag = initial_layout " << std::endl;
//      idd.generate_implementation ("initial_layout");  


    check_sct_decoding(idd);
    check_sct_timing(idd);

    return 0;  
}  
