#!/usr/bin/env python

## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

""" Contains a class that holds node informations
.. module:: converter
"""

class Node(object):
    """ Parent class of any object in the xar format
    """

    def __init__(self, name):
        self.node_name = name
        self._function_map = None

    def print_name(self):
        print(self.node_name)

    def attach_attribute(self, name, attrs):
        if (name in self._function_map.keys()):
            self._function_map[name](self, attrs)

