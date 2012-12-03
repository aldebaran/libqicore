#!/usr/bin/env python

## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

import node
import xar_types

class behaviorLayer(node.node):
  def __init__(self, attrs):
    node.node.__init__(self, "BehaviorLayer")
    self.name = attrs.getValue("name")
    self.mute = attrs.getValue("mute")

    self.behaviorKeyFrames = []
    self._functionMap = { 'BehaviorKeyframe' : behaviorLayer.attachChild}

  def attachChild(self, child):
    self.behaviorKeyFrames.append(child)
