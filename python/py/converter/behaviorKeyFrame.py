#!/usr/bin/env python

## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

import node
import xar_types

class behaviorKeyFrame(node.node):
  def __init__(self, attrs):
    node.node.__init__(self, "BehaviorKeyFrame")
    self.name = attrs.getValue("name")
    self.index = attrs.getValue("index")

    self.child = None
    self._functionMap = { 'Box' : behaviorKeyFrame.attachChild,
                          'Diagram' : behaviorKeyFrame.attachChild,
                          'Timeline' : behaviorKeyFrame.attachChild }

  def attachChild(self, child):
    self.child = child
