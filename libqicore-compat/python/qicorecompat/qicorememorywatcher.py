#!/usr/bin/env python

## Copyright (c) 2013 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

from naoqi import *
import qi

class QiCoreMemoryWatcher(ALModule):
    """ALMemoryWatcher"""
    def __init__(self):
        ALModule.__init__(self, "QiCoreMemoryWatcher")
        self.almemoryEvent = qi.Signal("(ss)")
        self.almemoryName = qi.Property("s")
        self.almemory = ALProxy("ALMemory")

    def subscribe(self, name):
        value = self.almemoryName.value()
        if(not value in self.almemory.getDataList(value)):
            return ""

        self.almemory.subscribeToEvent(value, "ALMemoryWatcher", onEvent)

    def onEvent(self, pDataName, pValue, *args):
        """OnEvent"""
        self.almemoryEvent(pDataName, pValue)

    def __onUnload__(self):
        pass
