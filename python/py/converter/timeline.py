#!/usr/bin/env python

## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

import converter.node as node
import converter.xar_types as xar_types

class Timeline(node.Node):

    def __init__(self, attrs):
        super(Timeline, self).__init__("Timeline")

        self.name = ""
        self.fps = attrs.getValue("fps")
        self.resources_acquisition = attrs.getValue("resources_acquisition")
        self.size = attrs.getValue("size")
        self.enable = attrs.getValue("enable")
        self.start_frame = attrs.getValue("start_frame")
        self.end_frame = attrs.getValue("end_frame")
        self.scale = attrs.getValue("scale")

        self.watches = ''
        self.behavior_layers = []
        self.actuator_list = []
        self._function_map = { 'watches' : Timeline.attach_watches,
                               'ActuatorCurve' : Timeline.attach_actuator_curve,
                               'Key' : Timeline.attach_key,
                               'BehaviorLayer' : Timeline.attach_child }

    def attach_watches(self, attrs):
        #Dummy Value
        self.watches = attrs.getValue('dummy')

    def attach_child(self, child):
        self.behavior_layers.append(child)

    def attach_actuator_curve(self, attrs):
        actuator = xar_types.actuatorCurve(attrs.getValue("name"),
                                           attrs.getValue("actuator"),
                                           attrs.getValue("recordable"),
                                           attrs.getValue("mute"),
                                           attrs.getValue("alwaysVisible"))
        self.actuator_list.append(actuator)

    def attach_key(self, attrs):
        actuator = self.actuator_list[len(self.actuator_list) - 1]
        actuator.keys.append(xar_types.key(attrs.getValue("frame"),
                                           attrs.getValue("value")))

