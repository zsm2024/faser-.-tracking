# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from PyUtils.MetaReader import read_metadata
from AthenaCommon.Logging import logging
from functools import lru_cache

msg = logging.getLogger('AutoConfigFlags')

# Module level cache of file-metadata:
_fileMetaData = dict()

class DynamicallyLoadMetadata:
    def __init__(self, filename):
        self.metadata = {}
        self.filename = filename
        self.metAccessLevel = 'lite'
        thisFileMD = read_metadata(filename, None, 'lite')
        self.metadata.update(thisFileMD[self.filename])
        msg.debug("Loaded using 'lite' %s", str(self.metadata))

    def _loadMore(self):
        thisFileMD = read_metadata(self.filename, None, 'peeker')
        self.metadata.update(thisFileMD[self.filename])

    def get(self, key, default):
        if key in self.metadata:
            return self.metadata[key]
        if self.metAccessLevel != 'peeker':
            msg.info("Looking into the file in 'peeker' mode as the configuration requires more details: %s ", key)
            self.metAccessLevel = 'peeker'
            self._loadMore()
            if key in self.metadata:
                return self.metadata[key]
        return default

    def __contains__(self, key):
        return self.get(key, None) is not None

    def __getitem__(self, key):
        result =  self.get(key, None)
        if result is None:
            raise RuntimeError("Key {} not found".format(key))
        return result

def GetFileMD(filenames):

    filename=filenames[0]
    if filename == '_FASER_GENERIC_INPUTFILE_NAME_':
        raise RuntimeError('Input file name not set, instead _FASER_GENERIC_INPUTFILE_NAME_ found. Cannot read metadata.')
    if filename not in _fileMetaData:
        if len(filenames)>1:
            msg.info("Multiple input files. Use the first one for auto-configuration")
        msg.info("Obtaining metadata of auto-configuration by peeking into %s", filename)
        _fileMetaData[filename] = DynamicallyLoadMetadata(filename)

    return _fileMetaData[filename]


def _initializeGeometryParameters(geoTag):
    """Read geometry database for all detectors"""

    from FaserGeoModel import CommonGeoDB
    from FaserGeoModel.FaserGeoDBInterface import FaserGeoDBInterface

    dbGeomCursor = FaserGeoDBInterface(geoTag)
    dbGeomCursor.ConnectAndBrowseGeoDB()

    params = { 'Common' : CommonGeoDB.InitializeGeometryParameters(dbGeomCursor) }

    return params


@lru_cache(maxsize=4)  # maxsize=1 should be enough for most jobs
def DetDescrInfo(geoTag):
    """Query geometry DB for detector description. Returns dictionary with
    detector description. Queries DB for each tag only once.

    geoTag: geometry tag (e.g. FASER-01)
    """
    detDescrInfo = _initializeGeometryParameters(geoTag)
    detDescrInfo["geomTag"] = geoTag
    return detDescrInfo


@lru_cache(maxsize=4)  # maxsize=1 should be enough for most jobs
def getDefaultDetectors(geoTag):
    """Query geometry DB for detector description.
    Returns a set of detectors used in a geometry tag.

    geoTag: geometry tag (e.g. FASER-01)
    """
    detectors = set()
    detectors.add('Veto')
    detectors.add('Trigger')
    detectors.add('Preshower')
    detectors.add('FaserSCT')
    detectors.add('Ecal')
    detectors.add('UpstreamDipole')
    detectors.add('CentralDipole')
    detectors.add('DownstreamDipole')
    
    # print("DetDescrInfo Run = ", DetDescrInfo(geoTag)['Common']['Run'])
    # print("DetDescrInfo TrackerType = ", DetDescrInfo(geoTag)['Common']['TrackerType'])
    # print("DetDescrInfo FaserNuType = ", DetDescrInfo(geoTag)['Common']['FaserNuType'])

    # if DetDescrInfo(geoTag)['Common']['Run'] == 'RUN4':
    #     detectors.add('ITkPixel')
    #     detectors.add('ITkStrip')
    # else:
    #     detectors.add('Pixel')
    #     detectors.add('SCT')
    #     detectors.add('TRT')
    #     detectors.add('BCM')
    # TODO: wait for special table in the geo DB
    # if DetDescrInfo(geoTag)['Common']['Run'] == 'RUN4':
    #     detectors.add('BCMPrime')

    return detectors


# Based on RunDMCFlags.py
def getRunToTimestampDict():
    # this wrapper is intended to avoid an initial import
    from .RunToTimestampData import RunToTimestampDict
    return RunToTimestampDict


def getInitialTimeStampsFromRunNumbers(runNumbers):
    """This is used to hold a dictionary of the form
    {152166:1269948352889940910, ...} to allow the
    timestamp to be determined from the run.
    """
    run2timestampDict =  getRunToTimestampDict()
    timeStamps = [run2timestampDict.get(runNumber,1) for runNumber in runNumbers] # Add protection here?
    return timeStamps
