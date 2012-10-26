/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef TIMELINE_H_
# define TIMELINE_H_

# include <string>

# include <boost/shared_ptr.hpp>

# include <albehavior/altimeline.h>

namespace AL
{
  class ALBroker;
}

class TimelinePrivate;

class Timeline: public AL::ALTimeline
{
  public:
    Timeline(boost::shared_ptr<AL::ALBroker> broker);
    ~Timeline();

    bool loadFromFile(std::string fileName);

    virtual void play(void);
    virtual void pause(void);
    virtual void stop(void);

    virtual void goTo(const int &pFrame);
    // TODO: Remove this method
    virtual void goTo(const std::string &pFrameName);

    virtual int getSize() const;
    virtual int getFPS() const;
    virtual void setFPS(const int fps);

    void waitForTimelineCompletion();

  private:
    TimelinePrivate* _p;
};

#endif /* !TIMELINE_H_ */
