#!/usr/bin/env python

## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

# Helping class to handle optionnal values
class attributes():
  def __init__(self, attrs):
    self._attrs = attrs
  def getValue(self, name):
    if (name in self._attrs.keys()):
      return self._attrs.getValue(name)
    else:
      return None

class script():
  def __init__(self):
    self.language = None
    self.content = None

# used for input and ouput
class port():
  def __init__(self, name, type, type_size, nature, inner, tooltip, id):
    self.name = name
    self.type = type
    self.type_size = type_size
    self.nature = nature
    self.inner = inner
    self.tooltip = tooltip
    self.id = id

class resource():
  def __init__(self, name, type, timeout):
    self.name = name
    self.type = type
    self.timeout = int(timeout)

class parameter():
  def __init__(self, name, inherits_from_parent, content_type, value,
                     default_value, min, max, tooltip, id, custom_choice):
    self.name = name
    self.inherits_from_parent = inherits_from_parent
    self.content_type = content_type
    self.value = value
    self.default_value = default_value
    self.min = min
    self.max = max
    self.tooltip = tooltip
    self.id = id
    self.custom_choice = custom_choice

class key():
  def __init__(self, frame, value):
    self.frame = frame
    self.value = value

class actuatorCurve():
  def __init__(self, name, actuator, recordable, mute, alwaysVisible):
    self.name = name
    self.actuator = actuator
    self.recordable = recordable
    self.mute = mute
    self.alwaysVisible = alwaysVisible
    self.keys = []

class link():
  def __init__(self, inputowner, indexofinput, outputowner, indexofoutput):
    self.inputowner = inputowner
    self.inputName = ""
    self.indexofinput = indexofinput
    self.outputowner = outputowner
    self.outputName = ""
    self.indexofoutput = indexofoutput
