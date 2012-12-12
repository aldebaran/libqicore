#!/usr/bin/env python

## Copyright (c) 2012 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

import os
import re

def _construct_init_code(box):
    init_code = ("qicore_legacy.BehaviorLegacy.__init__(self, \""
                 + box.name
                 + "\")"
                 + os.linesep)

    return init_code.rstrip()

def _add_inheritance(code):
    code = code.replace("(GeneratedClass):",
                        "(qicore_legacy.BehaviorLegacy):", 1)
    return code

def _generate_class(box, code):
    code += ("class " + box.name
             + "_class" + "(GeneratedClass):" + os.linesep
             + "  def __init__(self):" + os.linesep
             + "    GeneratedClass.__init__(self)" + os.linesep*2)
    return code

def _replace_not_supported_code(box, code):
    code = code.replace("ALFrameManager", "self")
    code = code.replace("ALProxy(\"self\")", "self")

    # FIXME: This value cannot be acquired at this time...
    code = code.replace("self.getTimelineFps(self.getName())", "25")

    # SetFPS has only a meaning when Timeline has an actuator curve
    if (box.child and box.child.actuator_list):
        code = code.replace("self.setTimelineFps(self.getName(), newfps)",
                            "self.getTimeline().setFPS(newfps)")
    else:
        code = code.replace("self.setTimelineFps(self.getName(), newfps)",
                            "pass")
    return code

def patch(box):
    code = box.script.content
    code = code.lstrip()
    if not code:
        code = _generate_class(box, code)
    code = code.replace("MyClass", box.name + "_class", 1)
    # Replace tabs to normalize code
    code = code.replace("\t", " " * 4)
    code = ( "#!/usr/bin/env python" + os.linesep
            + "# -*- coding: utf-8 -*-" + os.linesep * 2
            + "import time" + os.linesep
            + "import random" + os.linesep * 2
            + "from naoqi import *" + os.linesep
            + "import qicore" + os.linesep
            + "import qicore_legacy" + os.linesep * 2
            + code)
    code = _add_inheritance(code)
    init_code = _construct_init_code(box)
    regex = re.compile(r"try: # disable autoBind"
                        + r"\s*GeneratedClass\.__init__\(self, False\)"
                        + r"\s*except TypeError: # if NAOqi < 1\.14"
                        + r"\s*GeneratedClass\.__init__\( self \)")

    if (regex.search(code)):
        code = regex.sub(init_code, code, 1)
    elif ("GeneratedClass.__init__(self)" in code):
        code = code.replace("GeneratedClass.__init__(self)", init_code)
    else:
        code = code.replace("GeneratedClass.__init__(self, False)", init_code)
    code = _replace_not_supported_code(box, code)
    return code

