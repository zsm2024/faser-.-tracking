#
#  LegacySelector.py
#
#  Class to select values from legacy ntuples
#
#  E.Torrence, Sept. 2019
#
#
# Use Python3-style print statements
from __future__ import print_function

class LegacySelector ():
    """Class to select values from Legacy ntuple"""

    def __init__(self):
        """Constructor"""

        self.conf = None

    # Pass MonitorValue conf object
    def init(self, conf):
        """Initialization"""

        # Just save full conf object
        self.conf = conf

    # Return list of values from data object event
    def get1DValue(self, event, varName):

        # print ("LegacySelector.get1DValue called for", varName)

        values = []

        #varName = self.conf.xvar

        # Variable name should first give the list, followed by the specific parameter
        # Only the event number doesn't follow this pattern
        if varName == 'fEventNumber':
            # OK, this is weird, event is actually the tree, while event.event is the FaserEvent class.  
            # For the lists, we can get them directly from event...
            return event.event.GetEventNumber()

        theListName = varName.split('.')[0]

        theList = getattr(event, theListName)

        # Loop over truth list
        for obj in theList:
            # Try to parse the variable string
            # Find chunks separated by a point
            # Each is either a method or an attribute
            theObj = obj
            varList = self.conf.xvar.split('.')[1:]      # Skip the list [1:]
            for varName in varList:
                # Is this an attribute or a method?
                if callable(getattr(theObj, varName)):
                    theObj = getattr(theObj, varName)()  # Method (function)
                else:
                    theObj = getattr(theObj, varName)    # Attribute (value)

            # Once we have walked through the list, have our value
            values.append(theObj)


        return values
