#!/usr/bin/env python

## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

""" Contains a class that holds behaviorLayer informations
.. module:: converter
"""

import converter.node as node

class BehaviorLayer(node.Node):
    """ Stores informations about BehaviorLayer in the xar format
    """

    def __init__(self, attrs):
        super(BehaviorLayer, self).__init__("BehaviorLayer")
        self.name = attrs.getValue("name")
        self.mute = attrs.getValue("mute")

        self.behavior_keyframes = []
        self._function_map = {'BehaviorKeyframe' : BehaviorLayer.attach_child}

    def attach_child(self, child):
        self.behavior_keyframes.append(child)
