#!/usr/bin/env python

## Copyright (c) 2013 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.
from naoqi import ALProxy

class ResourceManager:
  def __init__(self, resources, owner, timeout):
    self.releaseAsked = True
    self.resourceManager = ALProxy('ALResourceManager')
    self.resources = resources
    self.owner = owner
    self.timeout = timeout

  def releaseResource(self):
    if not self.resources:
      return
    self.releaseAsked = True
    try:
      self.resourceManager.releaseResources(self.resources, self.owner)
    except:
      pass

  def waitResourcesCallback(self, callback):
    if not self.resources:
      return
    self.releaseAsked = False
    self.resourceManager.waitForResourcesTree(self.resources, self.owner, callback, self.timeout)

  def waitResourceFree(self):
    if not self.resources:
      return
    isFree = False
    self.releaseAsked = False
    isFree = self.resourceManager.areResourcesFree(self.resources)
    while (not isFree) and (not releaseAsked) :
      time.sleep(0.5)
      isFree = self.resourceManager.areResourcesFree(self.resources)
    if releaseAsked:
      raise Exception()

  def waitResources(self):
    if not self.resources:
      return
    self.resourceManager.waitForResourcesTree(self.resources, self.owner, "", self.timeout)
