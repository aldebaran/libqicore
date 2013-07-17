#!/usr/bin/env python

## Copyright (c) 2013 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.


''' ALFrameManager compatibility layer (deprcated)'''
class FrameManager:
  def __init__(self):
    self.fps = {}
    self.timeline = {}

  def getTimelineFps(self, name):
    if name in self.fps:
      return self.fps[name]
    raise BaseException("timeline " + name + " not found")

  def setTimelineFps(self, name, fps):
    if name in self.timeline:
      self.timeline[name].setFPS(fps)
    else:
      raise BaseException("timeline " + name + " not found")

  def addTimeline(self, name, fps, timeline):
    self.timeline[name] = timeline
    self.fps[name] = fps

  def getBehaviorPath(self, behaviorId):
    return self.behaviorPath

  def setBehaviorPath(self, behaviorPath):
    self.behaviorPath = behaviorPath
