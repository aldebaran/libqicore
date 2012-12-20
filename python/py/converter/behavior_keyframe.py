#!/usr/bin/env python

## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

""" Contains a class that holds behaviorKeyFrame informations
.. module:: converter
"""

import converter.node as node

class BehaviorKeyFrame(node.Node):
    """ Stores informations about BehaviorKeyFrame in the xar format
    """

    def __init__(self, attrs):
        super(BehaviorKeyFrame, self).__init__("BehaviorKeyFrame")
        self.name = attrs.getValue("name")
        self.index = attrs.getValue("index")
        self.child = None

        self._function_map = { 'Box' : BehaviorKeyFrame.attach_child,
                               'Diagram' : BehaviorKeyFrame.attach_child,
                               'Timeline' : BehaviorKeyFrame.attach_child }

    def attach_child(self, child):
        self.child = child

