#!/usr/bin/env python

## Copyright (c) 2013 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

import logging
import allog

class BehaviorLogHandler(logging.Handler):
  def __init__(self):
    logging.Handler.__init__(self)

  def emit(self, record):
    level_to_function = {
      logging.DEBUG: allog.debug,
      logging.INFO: allog.info,
      logging.WARNING: allog.warning,
      logging.ERROR: allog.error,
      logging.CRITICAL: allog.fatal,
    }
    function = level_to_function.get(record.levelno, allog.debug)
    function(record.name + " " + record.pathname + " " + str(record.lineno) + ": " + record.getMessage(),
             "behavior.box",
             "",   # record.filename in this case is simply '<string>'
             record.funcName,
             record.lineno)

class Behavior(object):
  def __init__(self, name):
    self.name = name
    self.logger = logging.getLogger(name)
    self.behaviorloghandler = BehaviorLogHandler()
    self.logger.addHandler(self.behaviorloghandler)
    self.logger.setLevel(logging.DEBUG)
    self.resource = False
    self.parameters = {}
    self._lastCommand = 0

  def getName(self):
    return self.name

  def getParameter(self, name):
    if(name in self.parameters):
      self.logger.debug("Parmater " + self.name + "." + name + " = " + str(self.parameters[name].value()))
      return self.parameters[name].value()
    else:
      self.logger.error("Parmeter " + self.name + "." + name + " not found")

  def setParameter(self, name, value):
    if(name in self.parameters):
      self.parameters[name].setValue(value)

  def stimulateIO(self, name, *args):
    if name + "Signal" in dir(self):
      signal = getattr(self, name + "Signal")
      self.logger.debug("Send signal " + self.name + "." + name)
      if None in args:
        signal("None")
      else:
        signal(*args)
    else:
      self.logger.error("Signal " + name + " not found")

  def _safeCallOfUserMethod(self, functionName, functionArg):
    try:
      self.logger.debug("Call of user methods " + self.name + "." + functionName)
      if(functionName in dir(self)):
        func = getattr(self, functionName)
        if(func.im_func.func_code.co_argcount == 2):
          func(functionArg)
        else:
          func()
      return True
    except BaseException, err:
      import traceback
      import __main__
      if('onError' in dir(self)):
        try:
          self.onError(self.getName() + ':' + str(err))
        except BaseException, err2:
          self.logger.error(traceback.format_exc())
          __main__.session.service('Behavior').onTaskError(self.name, traceback.format_exc())
      else:
        self.logger.error(traceback.format_exc())
        __main__.session.service('Behavior').onTaskError(self.name, traceback.format_exc())
      return False

  def __onLoad__(self):
    return self._safeCallOfUserMethod('onLoad',None)

  def __onUnload__(self):
    if(self.resource):
      self.releaseResource()
    self._safeCallOfUserMethod('onUnload' ,None)

  def log(self, p):
    self.logger.info(p)

  def handleSTM(self, pDataName, pValue):
    if pDataName in self.stminput:
      self.logger.debug("STM Input " + pDataName + " in " + self.name + " launch signal")
      self.stminput[pDataName](pValue)
    else:
      self.logger.error("STM Input " + pDataName + " in " + self.name + " not found")

  def setParentTimeline(self, timeline):
    self.parentTimeline = timeline

  def getParentTimeline(self):
    return self.parentTimeline
