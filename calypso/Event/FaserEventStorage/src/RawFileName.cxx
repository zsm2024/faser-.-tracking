#include "FaserEventStorage/RawFileName.h"
#include "FaserEventStorage/EventStorageIssues.h"
//-----------------
// includes for ers
#include "ers/ers.h"
//-----------------

#include <iostream>
#include <sstream>
#include <iomanip>
#include <boost/tokenizer.hpp>


namespace daq {

  /**
   * Contructor with ingredients given
   */
  RawFileName::RawFileName(std::string ProjectTag,
			   unsigned int RunNumber,
			   std::string StreamType,
			   std::string StreamName,
			   unsigned int LumiBlockNumber,
			   std::string ApplicationName,
			   std::string ProductionStep,
			   std::string DataType,
			   unsigned int FileSequenceNumber,
			   std::string Extension): m_core_known(false), 
						   m_trailer_known(false) 
  {
    ERS_DEBUG(1," Constructor with explicit fields");

    buildFileName(ProjectTag,
		  RunNumber,
		  StreamType,
		  StreamName,
		  LumiBlockNumber,
		  ApplicationName,
		  ProductionStep,
		  DataType,
		  FileSequenceNumber,
		  Extension);
  }

  /**
   * Constructor with the FileNameCore, a FileSequenceNumber and 
   * the Extension only - 
   *  Does not alter the FileNameCore, just attaches trailer fields 
   *  to the fileNameCore; also, tries to interpret the resulting filename
   *  Note: delimiter must be inside the Extension (e.g., ".data")
   */
  RawFileName::RawFileName(std::string FileNameCore,
			   unsigned int FileSequenceNumber,
			   std::string Extension): m_core_known(false), 
						   m_trailer_known(false)
  {
    ERS_DEBUG(1," Constructor with fileNameCore, sequenceNumber & extension");

    setDefaults(); // in case the file name is not interpretable

    buildFileNameTrailers(FileNameCore, 
			  FileSequenceNumber,
			  Extension);
    
    m_core_known = interpretFileName(m_fileName);
  }
    
  
  /**
   * Contructor with just the filename given: for interpretation
   */
  RawFileName::RawFileName(std::string FileName): m_core_known(false), 
						  m_trailer_known(false)
  {
    ERS_DEBUG(1," Constructor with fileName only, for interpretation");

    setDefaults(); // in case the file name is not interpretable
    
    m_fileName = FileName;
    m_fileNameCore = m_fileName; // can not know any better now... 

    m_core_known = interpretFileName(m_fileName); //if OK, made m_fileNameCore
  }

  /**
   * Destructor
   */
  RawFileName::~RawFileName(){
    ERS_DEBUG(1," Destructor!");
  }
  


  std::string RawFileName::getCurrentFileName(bool writing)
  {
    if (writing)
      setTailers(m_fileSequenceNumber, RAWFILENAME_EXTENSION_UNFINISHED);
    else
      setTailers(m_fileSequenceNumber, RAWFILENAME_EXTENSION_FINISHED);
    
    return fileName();
  }


  std::string RawFileName::getCoreName()
  {
    return m_fileNameCore;
  }

  unsigned int RawFileName::getIndex()
  {
    return m_fileSequenceNumber;
  }
  
  
  void RawFileName::fileAlreadyExists()
  {
    m_fileSequenceNumber+=100;
    
  }
  
  void RawFileName::advance()
  {
    m_fileSequenceNumber+=1;
    
  }




  /**
   * given the ingedients, contruct the fileName
   */
  void RawFileName::buildFileName(std::string ProjectTag,
				  unsigned int RunNumber,
				  std::string StreamType,
				  std::string StreamName,
				  unsigned int LumiBlockNumber,
				  std::string ApplicationName,
				  std::string ProductionStep,
				  std::string DataType,
				  unsigned int FileSequenceNumber,
				  std::string Extension) 
  {
    buildFileNameCore(ProjectTag,
		      RunNumber,
		      StreamType,
		      StreamName,
		      LumiBlockNumber,
		      ApplicationName,
		      ProductionStep,
		      DataType);

    buildFileNameTrailers(m_fileNameCore, // result of buildFileNameCore() 
			  FileSequenceNumber,
			  Extension);
    
    return;
  }
  
  /**
   * given the ingedients, contruct the fileNameCore 
   * and set m_core_known = true
   */
  void RawFileName::buildFileNameCore(std::string ProjectTag,
				      unsigned int RunNumber,
				      std::string StreamType,
				      std::string StreamName,
				      unsigned int LumiBlockNumber,
				      std::string ApplicationName,
				      std::string ProductionStep,
				      std::string DataType)
  {
    if (ProjectTag == "") { ProjectTag = "data_test"; } // default
    m_project = ProjectTag;
    m_runNumber = RunNumber;
    m_streamType = StreamType;
    m_streamName = StreamName;

    std::ostringstream stream;
    stream << m_streamType << "_" << m_streamName;
    m_stream = stream.str();
    
    m_lumiBlockNumber = LumiBlockNumber;
    m_applicationName = ApplicationName;
    m_productionStep = ProductionStep;
    m_dataType = DataType;
    
    std::ostringstream n;
    n << ProjectTag << RAWFILENAME_DELIMITER; 
    n << std::setw(RAWFILENAME_RUN_NUMBER_LENGTH) 
      << std::setfill('0');
    n << RunNumber << RAWFILENAME_DELIMITER;
    n << StreamType << "_";
    n << StreamName << RAWFILENAME_DELIMITER;
    n << ProductionStep << RAWFILENAME_DELIMITER;
    n << DataType;
    m_datasetName = n.str();

    n << RAWFILENAME_DELIMITER << "_lb";
    n << std::setw(RAWFILENAME_LB_NUMBER_LENGTH) 
      << std::setfill('0');
    n << LumiBlockNumber << RAWFILENAME_DELIMITER;
    n << "_" << ApplicationName;

    m_fileNameCore = n.str();

    m_core_known = true; // even the fileNameCore is OK to handle fileName
  }
  

  /**
   * construct a complete FileName, by appending a FileSequenceNumber and 
   * an Extension to the given FileNameCore
   * and set m_trailer_known = true
   */
  void RawFileName::buildFileNameTrailers(std::string FileNameCore, 
					  unsigned int FileSequenceNumber,
					  std::string Extension) 
  {
    m_fileNameCore = FileNameCore;
    m_fileSequenceNumber = FileSequenceNumber;
    m_extension = Extension;
    
    std::ostringstream n;
    n << FileNameCore;
    n << RAWFILENAME_DELIMITER
      << "_"
      << std::setw(RAWFILENAME_FILE_NUMBER_LENGTH) 
      << std::setfill('0');
    n << FileSequenceNumber;
    if ( Extension != "" ) {
      if (Extension.c_str()[0] != RAWFILENAME_DELIMITER.c_str()[0])
	{
	  n << RAWFILENAME_DELIMITER << Extension;
	  m_extension = RAWFILENAME_DELIMITER + Extension;
	} 
      else 
	{
	  n << Extension; // delimiter is already in Extension
	}
    }
    m_fileName = n.str();
    
    m_trailer_known = true;
  }

  /**
   * Default values for the filename fields 
   * If filename is valid, the correct values will be returned when asked.
   */
  void RawFileName::setDefaults() {
    m_project            = RAWFILENAME_DEFAULT_STRING;
    m_runNumber          = RAWFILENAME_DEFAULT_UINT;
    m_streamType         = RAWFILENAME_DEFAULT_STRING;
    m_streamName         = RAWFILENAME_DEFAULT_STRING;
    m_stream             = RAWFILENAME_DEFAULT_STRING;
    m_lumiBlockNumber    = RAWFILENAME_DEFAULT_UINT;
    m_applicationName    = RAWFILENAME_DEFAULT_STRING;
    m_productionStep     = RAWFILENAME_DEFAULT_STRING;
    m_dataType           = RAWFILENAME_DEFAULT_STRING;
    m_fileSequenceNumber = RAWFILENAME_DEFAULT_UINT;
    m_extension          = RAWFILENAME_DEFAULT_STRING;

    
    m_fileNameCore       = RAWFILENAME_DEFAULT_STRING;
    m_fileName           = RAWFILENAME_DEFAULT_STRING;

    m_datasetName        = RAWFILENAME_DEFAULT_STRING;

    m_core_known         = false;
    m_trailer_known      = false;
  }

  /**
   * Modify the File Trailer fileds (FileSequence Number and  Extension)
   */
  void RawFileName::setTailers(unsigned int fsn_i, std::string extension_s) 
  {
    m_fileSequenceNumber = fsn_i;
    m_extension = extension_s;
    
    buildFileNameTrailers(m_fileNameCore,
			  m_fileSequenceNumber,
			  m_extension);
    
    m_core_known = interpretFileName(m_fileName);
  }

  /**
   * Modify the FileSequenceNumber at the file trailer fields
   */
  void RawFileName::setFileSequenceNumber( unsigned int fsn_i ) 
  {
    m_fileSequenceNumber = fsn_i;
    if ( !m_trailer_known ) {
      m_extension = RAWFILENAME_DEFAULT_STRING;
    }
    
    buildFileNameTrailers(m_fileNameCore,
			  m_fileSequenceNumber,
			  m_extension);
    
    m_core_known = interpretFileName(m_fileName);
  }

  /**
   * Modify the File Extension at the file trailer fields
   */
  void RawFileName::setExtension( std::string extension_s ) 
  {
    m_extension = extension_s;
    if ( !m_trailer_known ) {
      m_fileSequenceNumber = RAWFILENAME_DEFAULT_UINT;
      std::string mystream = "File Sequence number not known. Set this first, before trying to set the extension";
      FaserEventStorage::RawFileNameIssue myissue(ERS_HERE, mystream.c_str() );
      throw myissue;
    }
    
    buildFileNameTrailers(m_fileNameCore,
			  m_fileSequenceNumber,
			  m_extension);
    
    m_core_known = interpretFileName(m_fileName);
  }


  /**
   * given the fileName, reconstruct the ingredients
   */
  bool RawFileName::interpretFileName(std::string FileName)
  {

    ERS_DEBUG(1," interpretFileName: " << FileName 
	      << "  (m_core_known = " << m_core_known
	      << " , m_trailer_known = " << m_trailer_known << " )");

    /*
      std::cout << " interpretFileName: FileName = " << FileName
		<< "  (m_core_known = " << m_core_known
		<< " , m_trailer_known = " << m_trailer_known << " )"
		<< std::endl;
    */

    // Before diving into interpretation, check if work is already done
    if ( m_core_known && m_trailer_known ) return true;
    if ( FileName == "" ) {
      m_core_known = false;
      m_trailer_known = false;
      return false; 
    }

    // Try to interpretate the filename fields

    // useful things for the spliting of the fileName in fields:
    // when spliting with boost tokenizer :
    typedef boost::tokenizer<boost::char_separator<char> >  tokenizer;
    boost::char_separator<char> sep(RAWFILENAME_DELIMITER.c_str(), 
				    "", boost::keep_empty_tokens);
    // when spliting with std::string::find() :
    std:: string::size_type sepPos;

    // Assuming that the file finishes like: ._sequenceNumber.extension(s)
    // we say that after the last "_" in the filename, we have the trailer,
    // and before that "_" we have the fileNameCore

    sepPos=FileName.rfind("._");
    if(sepPos==std::string::npos) {
      ERS_DEBUG(3,"File name does not contain ._ ");
      return false;
    }

    // fileNameCore:
    m_fileNameCore = FileName.substr(0,sepPos);

    ERS_DEBUG(3,"Checking for . at the end of " << m_fileNameCore );
    if (m_fileNameCore.c_str()[m_fileNameCore.size()-1] == '.') {
      m_fileNameCore = m_fileNameCore.substr(0, m_fileNameCore.size()-1);
    }
    ERS_DEBUG(3,"After removing possible . at the end: " << m_fileNameCore );

    // tail:
    std::string trailers_onestring;
    trailers_onestring = FileName.substr(sepPos+2,std::string::npos); //to end

    std::vector<std::string> trailers;
    /***
	trailers = split(trailers_onestring,
	RAWFILENAME_DELIMITER.c_str()[0]);
    ***/
    tokenizer trailer_tokens(trailers_onestring, sep);
    trailers.assign(trailer_tokens.begin(), trailer_tokens.end());



    // A) interpret trailers first:
    // ----------------------------
    // if the 1st filed in the trailer is a number,
    // we can intepret the trailer, else not
    if ( !m_trailer_known) {
      try {
	m_fileSequenceNumber //  = atoi( trailers[0].c_str() ); 
	  = convertToUINT( trailers[0] );
	m_trailer_known = true;
	std::ostringstream myext;
	for (unsigned int i=1; i !=  trailers.size(); i++) {
	  if (i != 1) myext << RAWFILENAME_DELIMITER;
	  myext << trailers[i];
	}
	m_extension = myext.str();

	// build the file name again and set m_trailer_known = true 
	buildFileNameTrailers(m_fileNameCore, 
			      m_fileSequenceNumber,
			      m_extension);
	m_trailer_known = true;
	
      } catch (std::exception& ex) {
	ERS_DEBUG(1," Can not interpret fileSequenceNumber: " << ex.what() );
	m_trailer_known = false;
	m_fileSequenceNumber = RAWFILENAME_DEFAULT_UINT;
	m_extension          = RAWFILENAME_DEFAULT_STRING;
      }
    } // try only if trailer details are not known

    ERS_DEBUG(3," =====> Trailer string: " << trailers_onestring
	      << " has " << trailers_onestring.size() << " characters"
	      << " ;  FileName: " << FileName 
	      << "  , fileNameCore: " << m_fileNameCore
	      << " , m_fileSequenceNumber = " << m_fileSequenceNumber
	      << " , m_extension  = " << m_extension );


    // B) Try to interpret the fileNameCore now:
    // -----------------------------------------
    if (m_core_known) {
      return true;
    }

    // we are here only if m_core_known is false: interepret now
    std::vector<std::string> fields;
    // fields = split( FileName, RAWFILENAME_DELIMITER.c_str()[0] );
    tokenizer core_tokens(FileName, sep);
    fields.assign(core_tokens.begin(), core_tokens.end());
    for ( std::vector<std::string>::iterator it=fields.begin();
	  it != fields.end(); ++it) {
      ERS_DEBUG(3, " field = " << *it);
    }
 
    ERS_DEBUG(3," number of . delimited fields found: " <<  fields.size());

    bool can_interpret_core;
    if (fields.size() >= 7 && fields[4] == "RAW") {
      can_interpret_core=true;
    } else {
      can_interpret_core=false;
    } 
    
    if ( !can_interpret_core ) {
      ERS_DEBUG(1," Can not interpret the core of FileName: " << FileName 
		<< " , so keeping " << m_fileNameCore << " as core.");
      m_core_known = false;

    } else {

      m_project     = fields[0];
      try {
	m_runNumber // = atoi( fields[1].c_str() ); 
	  = convertToUINT( fields[1] );
      } catch (std::exception& ex) {
	ERS_DEBUG(1," Can not interpret run-number : " << ex.what() );
	return false; // done with intrepreting the core...
      }

      m_stream      = fields[2];

      sepPos=m_stream.find("_");
      if(sepPos==std::string::npos) {
	ERS_DEBUG(3,"Stream does not contain _ ");
	return false;
      }
      // first part is streamType:
      m_streamType = m_stream.substr(0,sepPos);
      // second part is streamName:
      m_streamName = m_stream.substr(sepPos+1,std::string::npos); // to end

      m_productionStep = fields[3];
      m_dataType = fields[4];
      m_datasetName = "datasetName";

      try {
        sepPos = fields[5].find("b");
	m_lumiBlockNumber = 
	  convertToUINT( fields[5].substr(sepPos+1, std::string::npos) );
	// atoi ( fields[5].substr(sepPos+1, std::string::npos).c_str() ); 
      } catch (std::exception& ex) {
	ERS_DEBUG(1," Can not interpret lumi-block : " << ex.what() );
	// m_lumiBlockNumber = 0;
	return false; // done with intrepreting the core...
      }
      
      sepPos = fields[6].find("_");
      m_applicationName = fields[6].substr(sepPos+1, std::string::npos);
      
      // build the nameCore again and set m_core_known = true
      buildFileNameCore(m_project,
			m_runNumber,
			m_streamType,
			m_streamName,
			m_lumiBlockNumber,
			m_applicationName,
			m_productionStep,
			m_dataType);
      m_core_known=true;
    } // if ( !can_interpret_core ) )

    return m_core_known;
  } 

  /**
   * contruct the complain
   */
  std::string RawFileName::complain()
  {
    std::stringstream mystream;
    mystream <<  "FileName " << m_fileName << " has failed interpretation";
    return mystream.str();
  }


  /**
   * Get methods:
   * ------------
   */
  std::string RawFileName::project() { 
    if ( hasValidCore() )  {
      return m_project;
    } else {
      throw FaserEventStorage::RawFileNameIssue(ERS_HERE, complain().c_str() );
      //FaserEventStorage::RawFileNameIssue myIssue(ERS_HERE, complain() );
      // ers::warning(myIssue);
      //throw myIssue;
    }
  }

  std::string RawFileName::datasetName() { 
    if ( hasValidCore() ) { 
      return m_datasetName;
    } else {
      throw FaserEventStorage::RawFileNameIssue(ERS_HERE, complain().c_str() );
    }
  }

  unsigned int RawFileName::runNumber() { 
    if ( hasValidCore() ) { 
      return m_runNumber;
    } else {
      throw FaserEventStorage::RawFileNameIssue(ERS_HERE, complain().c_str() );
    }
  }


  std::string RawFileName::streamType() { 
    if ( hasValidCore() ) { 
      return m_streamType; 
    } else {
      throw FaserEventStorage::RawFileNameIssue(ERS_HERE, complain().c_str() );
    }
  }

  std::string RawFileName::streamName() { 
    if ( hasValidCore() ) { 
      return m_streamName; 
    } else {
      throw FaserEventStorage::RawFileNameIssue(ERS_HERE, complain().c_str() );
    }
  }


  std::string RawFileName::stream() {
    if ( hasValidCore() ) { 
      return m_stream; 
    } else {
      throw FaserEventStorage::RawFileNameIssue(ERS_HERE, complain().c_str() );
    }
  }

  unsigned int RawFileName::lumiBlockNumber() { 
    if ( hasValidCore() ) { 
      return m_lumiBlockNumber; 
    } else {
      throw FaserEventStorage::RawFileNameIssue(ERS_HERE, complain().c_str() );
    }
  }

  std::string RawFileName::applicationName() { 
    if ( hasValidCore() ) { 
      return m_applicationName;
    } else {
      throw FaserEventStorage::RawFileNameIssue(ERS_HERE, complain().c_str() );
    }
  }

  std::string RawFileName::productionStep() { 
    if ( hasValidCore() ) { 
      return m_productionStep;
    } else {
      throw FaserEventStorage::RawFileNameIssue(ERS_HERE, complain().c_str() );
    }
  }

  std::string RawFileName::dataType() { 
    if ( hasValidCore() ) { 
      return m_productionStep;
    } else {
      throw FaserEventStorage::RawFileNameIssue(ERS_HERE, complain().c_str() );
    }
  }


  unsigned int RawFileName::fileSequenceNumber() {
    if ( hasValidTrailer() )  {
      return m_fileSequenceNumber;
    } else {
      throw FaserEventStorage::RawFileNameIssue(ERS_HERE, complain().c_str() );
    }
  }

  std::string RawFileName::extension() { 
    if ( hasValidTrailer() )  {
      return m_extension;
    } else {
      throw FaserEventStorage::RawFileNameIssue(ERS_HERE, complain().c_str() );
    }
  }


  /**
   * print the content of this object
   */
  void RawFileName::print()
  {
    std::stringstream mystream;

    mystream << " full fileName: " << m_fileName  << std::endl;
    mystream << " fileNameCore:  " << m_fileNameCore  << std::endl;
    if(m_core_known)
      mystream << " datasetName:   " << m_datasetName << std::endl;
    
    
    if (!m_core_known) {
      mystream << "   Not all core fields known " << std::endl;
    } else {
      mystream << "   project   = " << m_project << std::endl;
      mystream << "   runNo     = " << m_runNumber << std::endl;
      mystream << "   stream    = " << m_stream 
<< " ( type: " << m_streamType 
<< " , name: " << m_streamName << " ) "
<< std::endl;
      mystream << "   prodStep  = " << m_productionStep << std::endl;
      mystream << "   dataType  = " << m_dataType << std::endl;
      mystream << "   lumiBlock = " << m_lumiBlockNumber << std::endl;
      mystream << "   appName   = " << m_applicationName << std::endl;
    } // end-if m_core_known

    if ( !m_trailer_known ) {
      mystream << "   Tailer fields not known " << std::endl;
    } else {
      mystream << "   seqNumber = " << m_fileSequenceNumber << std::endl;
      mystream << "   extension = " << m_extension << std::endl;
    } // end-if m_trailer_known

    mystream << std::endl;
    
    std::string tmp = mystream.str();

    ERS_LOG(tmp.c_str());

    return;
  }


  /**
   * split a string in its fields, using a ONE-CHARACTER-LONG delimiter
   */
  std::vector<std::string> RawFileName::split(std::string const & s, 
      char delimiter) 
  {    
    std::vector<std::string> container;

    typedef boost::tokenizer<boost::char_separator<char> >  tokenizer;

    std::ostringstream os;
    os << delimiter;
    boost::char_separator<char> sep(os.str().c_str(), 
    "", boost::keep_empty_tokens);

    tokenizer tokens(s, sep);
    container.assign(tokens.begin(), tokens.end());

    return container;
  }

  /**
   * string to integer conversion
   */ 
  unsigned int RawFileName::convertToUINT(const std::string & s) 
  {
    unsigned int x = 0;
    std::istringstream i(s);
    // check if ALL characters in "s" are numbers; otherwise throw
    for ( unsigned int it = 0; it != s.size(); ++it) {
      std::ostringstream temp_out; temp_out << s.c_str()[it];
      std::istringstream temp_in(temp_out.str());
      x=0;
      if (!(temp_in >> x)) {
std::string mystream = "Failed convertToUINT(\"" + s + "\")";
FaserEventStorage::RawFileNameIssue myissue(ERS_HERE, mystream.c_str() );
// ers::warning(ci);
throw myissue;
// throw daq::BadConversion("convertToUINT(\"" + s + "\")");
      }
    }
    
    x=0;
    if (!(i >> x)) {
      std::string mystream = "Failed convertToUINT(\"" + s + "\")";
      FaserEventStorage::RawFileNameIssue myissue(ERS_HERE, mystream.c_str() );
      // ers::warning(ci);
      throw myissue;
      // throw daq::BadConversion("convertToUINT(\"" + s + "\")");
    }
    
    return x;
  }

}

