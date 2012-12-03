#!/usr/bin/env python

## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

import node
import xar_types

class timeline(node.node):
  def __init__(self, attrs):
    node.node.__init__(self, "Timeline")

    self.name = ""
    self.fps = attrs.getValue("fps")
    self.resources_acquisition = attrs.getValue("resources_acquisition")
    self.size = attrs.getValue("size")
    self.enable = attrs.getValue("enable")
    self.start_frame = attrs.getValue("start_frame")
    self.end_frame = attrs.getValue("end_frame")
    self.scale = attrs.getValue("scale")

    self.watches = ''
    self.behaviorLayers = []
    self.actuatorList = []
    self._functionMap = { 'watches' : timeline.attachWatches,
                          'diagram' : timeline.attachDiagram,
                          'ActuatorCurve' : timeline.attachActuatorCurve,
                          'Key' : timeline.attachKey,
                          'BehaviorLayer' : timeline.attachChild}

  def attachWatches(self, attrs):
    #Dummy Value
    self.watches = attrs.getValue('dummy')

  def attachChild(self, child):
    self.behaviorLayers.append(child)

  def attachBehaviorKeyFrame(self, attrs):
    self.behaviorKeyFrames = xar_types.behaviorKeyFrame(attrs.getValue('name'),
                                                        attrs.getValue('index'),
                                                        attrs.getValue('bitmap'))

  def attachDiagram(self, diagram):
    self.diagram = diagram

  def attachActuatorCurve(self, attrs):
    self.actuatorList.append(xar_types.actuatorCurve(attrs.getValue("name"),
                                                      attrs.getValue("actuator"),
                                                      attrs.getValue("recordable"),
                                                      attrs.getValue("mute"),
                                                      attrs.getValue("alwaysVisible")))

  def attachKey(self, attrs):
    (self.actuatorList[len(self.actuatorList) - 1]).keys.append(xar_types.key(attrs.getValue("frame"),
                                                                                attrs.getValue("value")))
