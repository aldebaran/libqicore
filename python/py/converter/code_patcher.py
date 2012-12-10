#!/usr/bin/env python

## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

import os
import sys
import re

from xar_types import *

class patcher:
  def __init__(self, box):
    self._box = box
    self._code = box.script.content.lstrip()
    self._addedMethods = ""
    self._indentForInit = 4

  def findInitIndentation(self):
    splittedStr = self._code.split(os.linesep)
    for s in splittedStr:
      if (("GeneratedClass.__init__(self)" in s)
          or ("GeneratedClass.__init__(self, False)" in s)):
        self._indentForInit = len(s) - len(s.lstrip())

  def constructInitCode(self):
    indent = self._indentForInit
    initCode = "qicoreLegacy.BehaviorLegacy.__init__(self, \"" + self._box.name + "\")" + os.linesep

    return initCode.rstrip()

  def addInheritance(self):
    self._code = self._code.replace("(GeneratedClass):", "(qicoreLegacy.BehaviorLegacy):", 1)

  def generateClass(self):
    self._code += ("class " + self._box.name + "_class" + "(GeneratedClass):" + os.linesep
                    + "  def __init__(self):" + os.linesep
                    + "    GeneratedClass.__init__(self)" + os.linesep + os.linesep)

  def replace_not_supported_code(self):
    # Code that require a translation ...
    warning = "# /!!\ This code has been removed by the qicore Converter, no more supported..."
    self._code = self._code.replace("ALFrameManager", "self")
    self._code = self._code.replace("ALProxy(\"self\")", "self")

    #FIXME: This value cannot be acquired at this time...
    self._code = self._code.replace("self.getTimelineFps(self.getName())", "25")

    # SetFPS has only a meaning when Timeline has an actuator curve
    if (self._box.child and len(self._box.child.actuatorList) != 0):
      self._code = self._code.replace("self.setTimelineFps(self.getName(), newfps)", "self.getTimeline().setFPS(newfps)")
    else:
      self._code = self._code.replace("self.setTimelineFps(self.getName(), newfps)", "pass")

  def patch(self):
    if (self._code.lstrip() == ""):
      self.generateClass()
    self._code = self._code.replace("MyClass", self._box.name + "_class", 1)
    # Replace tabs to normalize code
    self._code = self._code.replace("\t", " " * 4)
    self._code = ( "#!/usr/bin/env python" + os.linesep
                  + "# -*- coding: utf-8 -*-" + os.linesep * 2
                  + "import time" + os.linesep
                  + "from naoqi import *" + os.linesep
                  + "import qicore" + os.linesep
                  + "import qicoreLegacy" + os.linesep * 2
                  + self._code)
    self.findInitIndentation()
    self.addInheritance()
    initCode = self.constructInitCode()
    compiledRE = re.compile("try: # disable autoBind\s*GeneratedClass\.__init__\(self, False\)\s*except TypeError: # if NAOqi < 1\.14\s*GeneratedClass\.__init__\( self \)")

    if (compiledRE.search(self._code)):
      self._code = compiledRE.sub(initCode, self._code, 1)
    elif ("GeneratedClass.__init__(self)" in self._code):
      self._code = self._code.replace("GeneratedClass.__init__(self)", initCode)
    else:
      self._code = self._code.replace("GeneratedClass.__init__(self, False)", initCode)
    self.replace_not_supported_code()
    self._code += self._addedMethods
    return self._code

