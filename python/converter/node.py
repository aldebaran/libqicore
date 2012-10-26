#!/usr/bin/env python

## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

class node:
  def __init__(self, name):
    self.nodeName = name
    self._functionMap = None

  def printName(self):
    print(self.nodeName)

  def attachAttribute(self, name, attrs):
    if (name in self._functionMap.keys()):
      self._functionMap[name](self, attrs)
