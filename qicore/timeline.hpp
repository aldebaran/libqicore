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

class QICORE_API Timeline
{
  public:
    Timeline(boost::shared_ptr<AL::ALBroker> broker);
    ~Timeline();

    bool loadFromFile(std::string fileName);

    void play(void);
    void pause(void);
    void stop(void);

    void goTo(const int &pFrame);

    int getSize() const;
    int getFPS() const;
    void setFPS(const int fps);

    void waitForTimelineCompletion();

    /* Function that will be called when Timeline is finished */
    void registerOnStoppedCallback(PyObject*);

    void addFlag(int frame, std::string stateName);
    void setStateMachine(StateMachine* sm);

  private:
    TimelinePrivate* _p;
};

};

#endif /* !TIMELINE_H_ */
