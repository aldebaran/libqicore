/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef TIMELINE_H_
# define TIMELINE_H_

# include <string>

# include <boost/shared_ptr.hpp>
# include <Python.h>

# include <qicore/api.hpp>

namespace AL
{
  class ALBroker;
}

namespace qi
{

class TimelinePrivate;
class StateMachine;

/// Timeline class
class QICORE_API Timeline
{
  public:
    /// Constructor, take a ALBroker to speak with the outside world
    Timeline(boost::shared_ptr<AL::ALBroker> broker);
    ~Timeline();

    /// Load the timline stored in the file pointed to by filename
    bool loadFromFile(std::string fileName);

    /// Play the timeline
    void play(void);
    /// Pause the timeline
    void pause(void);
    /// Stop the timeline
    void stop(void);

    /// Goto to a frame
    void goTo(const int &pFrame);

    /// Return the number of frames in the timeline
    int getSize() const;
    /// Return FPS
    int getFPS() const;
    /// Change the FPS
    void setFPS(const int fps);

    /// Wait untile the timeline execution is completed
    void waitForTimelineCompletion();

    /// Register a python callable that will be called when timeline is done
    void registerOnStoppedCallback(PyObject*);

    /// Add a flag to the timeline, to drive the stateMachine
    void addFlag(int frame, std::string stateName);
    /// Set the stateMachine to drive
    void setStateMachine(StateMachine* sm);

  private:
    TimelinePrivate* _p;
};

};

#endif /* !TIMELINE_H_ */
