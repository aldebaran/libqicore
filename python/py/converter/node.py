#!/usr/bin/env python

## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

class Node(object):

    def __init__(self, name):
        self.node_name = name
        self._function_map = None

    def print_name(self):
        print(self.node_name)

    def attach_attribute(self, name, attrs):
        if (name in self._function_map.keys()):
            self._function_map[name](self, attrs)

