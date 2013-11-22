## Copyright (c) 2013 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

class QiBuildProject(object):
    def __init__(self, name):
        self.name = name

    def configure(self):
        raise NotImplementedError

    def make(self):
        raise NotImplementedError

    def do_install(self, dest):
        raise NotImplementedError
        pass
