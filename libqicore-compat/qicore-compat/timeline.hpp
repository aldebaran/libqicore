/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef TIMELINE_H_
#define TIMELINE_H_

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/python.hpp>
#include <qicore-compat/api.hpp>
#include <qitype/anyobject.hpp>

namespace AL
{
  class ALBroker;
}

namespace qi
{

class TimelinePrivate;
class AnimationModel;

/// Timeline class
class QICORECOMPAT_API Timeline
{
  public:
    /// Constructor, take a memory and motion object
    Timeline(AnyObject motion = AnyObject(), PyInterpreterState *mainInterpreterState = 0);
    ~Timeline();

    /// Play the timeline
    void play();
    /// Pause the timeline
    void pause();
    /// Stop the timeline
    void stop();

    /// Goto to a frame
    void goTo(const int &pFrame);
    /// Goto to a frame
    void goTo(const std::string &pFrame);

    /// Return the number of frames in the timeline
    int getSize() const;
    /// Return FPS
    int getFPS() const;
    /// Change the FPS
    void setFPS(const int fps);
    /// Change the animation
    void setAnimation(AnimationModel* anim);
    /// Set frames label
    void setFrames(const std::map<int, std::string> &frames);
    /// Set frames label
    void setFrameNames(const std::map<std::string, int> &frames);

    /// Wait untile the timeline execution is completed
    void waitForTimelineCompletion();

    bool isValid() const;

    qi::Signal<int> startFlowdiagram;
    qi::Signal<int> stopFlowdiagram;

    qi::Signal<void> onTimelineFinished;

  private:
    TimelinePrivate* _p;
};

}

#endif /* !TIMELINE_H_ */
