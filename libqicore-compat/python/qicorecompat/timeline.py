#!/usr/bin/env python

## Copyright (c) 2013 Aldebaran Robotics. All rights reserved.
## Use of this source code is governed by a BSD-style license that can be
## found in the COPYING file.

class Timeline:
  def __init__(self, fps):
    self.fps = fps

  def setTimeline(self, timeline, frames):
    self.timeline = timeline
    self.frames = frames

  def play(self):
    self.timeline.call('play')

  def pause(self):
    self.timeline.call('pause')

  def stop(self):
    self.timeline.call('stop')

  def gotoAndStop(self, frame):
    if isinstance(frame, basestring):
      if frame in self.frames:
        self.gotoAndStop(self.frames[frame])
    if isinstance(frame, int):
      self.timeline.call('pause')
      self.timeline.call('goTo', frame)

  def gotoAndPlay(self, frame):
    if isinstance(frame, basestring):
      if frame in self.frames:
        self.gotoAndPlay(self.frames[frame])
    if isinstance(frame, int):
      self.timeline.call('goTo', frame)
      self.timeline.call('play')

  def getFPS(self):
    return self.fps

  def setFPS(self, fps):
    self.timeline.call('setFPS', fps)
