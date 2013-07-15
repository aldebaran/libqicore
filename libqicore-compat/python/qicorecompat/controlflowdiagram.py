#!/usr/bin/env python

## Copyright (c) 2013 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

import qi

class ControlFlowdiagram:
  def __init__(self):
    self.delay = qi.Property("i")
    self.startFlowdiagram = qi.Signal("(m)")
    self.stopFlowdiagram = qi.Signal("(m)")

  def start(self, value):
    if(self.delay.value() == 0):
      self.startFlowdiagram(value)

  def startDelay(self, index):
    if(self.delay.value() == index):
      self.startFlowdiagram(index)

  def stop(self, value):
      self.stopFlowdiagram(value)

  def __onLoad__(self):
      pass

  def __onUnload__(self):
      pass
