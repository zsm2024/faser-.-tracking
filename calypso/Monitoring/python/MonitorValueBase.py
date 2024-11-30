#
#  MonitorValueBase.py
#
#  Defines base classes to implement monitored values
#
#  Functions
#    readMonitorValueFromRoot - read MonitorValue from ROOT file
#    monitorValueFromJSON - construct MonitorValue from JSON
#
#  Classes
#    MonitorValueBase
#    MonitorConfigBase
#
#  E.Torrence, Aug. 2019
#
#
# Use Python3-style print statements
from __future__ import print_function

import sys
import ROOT
import json

# Utility function to create object instance from ROOT file
def readMonitorValueFromRoot(f, tag):
    """Read class named tag from open root file f"""

    # Objects stored in subdirectory, make sure it exists
    if not f.cd(tag):
        print("Object", tag, "not present in file!")
        f.ls()
        raise ObjectNotFoundError("Error: directory not found for object "+tag+"!")
    
    #print ("Changed to directory", tag)
    #f.ls()

    # Read histogram first
    histName = tag+"_h"
    #print("Reading histogram", histName)
    hist = ROOT.gROOT.FindObject(histName)
    if hist is None:
        f.cd("..")
        raise ObjectNotFoundError("Error: unable to read histogram", histName,"!")

    # Read JSON string stored in a TNamed object
    objectName = tag+"_j"
    tobj = ROOT.gROOT.FindObject(objectName)
    if tobj is None:
        f.cd("..")
        raise ObjectNotFoundError("Error: unable to read JSON from",objectName, "!")

    # Get JSON string from TNamed
    jsonString = tobj.GetTitle()

    #print(jsonString)
    #obj = pickle.loads(unicode(pickleString.GetTitle(), 'utf-8'))

    # Call our utility function to create a new (un-initalized) object
    obj = monitorValueFromJSON(json.loads(jsonString))

    # And put the histogram back
    obj.hist = hist

    # Return to the parent directory in the ROOT file
    f.cd("..")

    # Return the recreated object
    return obj


# Utility to create a new class from a JSON description
# This does not initialize the class
def monitorValueFromJSON(jsonDict):

    # jsonDict = json.loads(jsonString)

    # Get the object class name
    className = jsonDict.get('type', None)
    if className is None:
        print ("monitorValueFromJSON: no class type specified!")
        return None

    # Find the class object in our module
    # This is some voodoo
    # Monitoring is the package name
    # className is both the class but also the submodule to search in
    # The fromlist=[] needs to specify the submodules
    # So this works as long as each MV type has a unique module
    # Need a better way to do this (perhaps using __init__.py?
    class_ = getattr(__import__("Monitoring."+className, fromlist=[className]), className)

    # Instantiate this class
    obj = class_()

    if obj is None:
        f.cd("..")
        raise ObjectNotFoundError("Error: unable to instantiate object", objectName, "!")

    # Update the configuration from the JSON file
    obj.conf.fromJSON(jsonDict)

    # Also instantiate the selector
    className = jsonDict.get('selector', None)

    if className is None:
        obj.selector = None
        print ("monitorValueFromJSON: No selector specified for value", jsonDict["tag"])

    else:
        # Find the class name in our module
        class_ = getattr(__import__("Monitoring."+className, fromlist=[className]), className)
    # And instantiate this class
        obj.selector = class_()

        if obj.selector is None:
            raise ObjectNotFoundError("Error: unable to instantiate object", objectName, "!")

    # Done with selector object

    # Don't call obj.init here, make user do this
    return obj

# Error in case histogram can't be read
class ObjectNotFoundError(KeyError):
     pass

class MonitorValueBase ():
    """Base class for monitored values"""

    def __init__(self):
        """Dummy Constructor"""

        # Configuration object to store all needed parameters
        self.conf = None

        # Histogram (or other ROOT class) containing data
        self.hist = None

        # Selector class to fill histogram
        self.selector = None

    # Common init functions
    def init(self):    

        if self.conf is None:
            print ("MonitorValueBase: init() called with no valid configuration!")
            return

        # Set type if not configured
        actualType = self.__class__.__name__
        if self.conf.type is None:
            self.conf.type = actualType

        # Check that type is correct
        elif actualType != self.conf.type:
            print ("actualType", actualType, "does not match configured type", self.conf.type,"!")
            print ("...overriding conf.type")
            self.conf.type = actualType

        # Initialize the selector
        if self.selector is not None:
            self.selector.init(self.conf)


    def fill(self, v):
        # Dummy fill function
        pass

    def plot(self, c):
        # Dummy plot function, must pass canvas in case we want to make log scale
        pass

    def dump(self):
        """Write histogram to specified stream (stdout by default)"""

        #print("Dump called for", self.conf.tag)
        print(self.conf)
        if self.hist is not None:
            self.hist.Print()

    def __str__(self):
        return str(self.conf)

    def writeRoot(self, f):
        """Write hist and conf to open root file f"""

        # Make a subdirectory to contain the components and switch
        cwd = f.mkdir(self.conf.tag)
        cwd.cd()

        # Write the histogram first (if it isn't NULL)
        if self.hist is not None:
            self.hist.Write()

        # Also write the conf JSON to TString
        # ROOT name is tag_p
        jsonString = self.conf.toJSON()
        ts = ROOT.TNamed(self.conf.tag+"_j", jsonString)
        ts.Write()

        # Also write pickled object to TString
        # pickleString = pickle.dumps(self)
        # ts = ROOT.TNamed(self.tag+"_p", pickleString.decode("utf-8"))
        # ts.Write()

        # Now go back to starting directory to not screw things up
        cwd.cd("..")

    # These aren't needed any more
    # Remove hist from elements to pickle (we save this separately)
    def __getstate__(self):
        state = self.__dict__.copy()
        # Don't pickle hist
        del state["hist"]
        return state

    def __setstate__(self, state):
        self.__dict__.update(state)
        # Add hist back since it doesn't exist in the pickle
        self.hist = None

#
# Base class for configuration objects
class MonitorConfigBase:

    def __init__(self):
        """Constructor provides default values"""
        self.tag = ""    # Unique identifier string
        self.type = None # Class name of instantiated MonitorValue object
        self.desc = ""   # Helpful description string

    def toJSON(self, indentLevel=0):
        """Return JSON string describing values in this class"""
        return json.dumps(self, default=lambda o: o.__dict__,
                          sort_keys=True, indent=indentLevel)

    def fromJSONStr(self, jsonString):
        """Update class attributes using values read in from JSON string"""

        # Note, values not specified will not be changed
        jsonDict = json.loads(jsonString)
        self.fromJSON(jsonDict)

    def fromJSON(self, jsonDict):
        """Update class attributes using values in parsed JSON dict"""

        # This will only work for simple parameters
        for (key, val) in jsonDict.iteritems():
            setattr(self, key, val)

    def __str__(self):
        return self.toJSON(indentLevel=2)

