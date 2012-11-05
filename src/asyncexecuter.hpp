/**
 * @author Julien Freche
 * Aldebaran Robotics (c) 2007-2012 All Rights Reserved
 */

#pragma once

#ifndef ASYNC_FRAME_EXECUTER_H
# define ASYNC_FRAME_EXECUTER_H

# include <boost/thread.hpp>
# include <boost/bind.hpp>

namespace qi
{

/* Class that handle a thread used to do some work asynchronously */
/* The thread will call update() every n milliseconds */
class asyncExecuter
{
  public:
    asyncExecuter(unsigned int interval);
    virtual ~asyncExecuter();

    unsigned int getInterval();
    void setInterval(unsigned int interval);

    void waitForExecuterCompletion();
    bool isPlaying();

    void playExecuter();
    void pauseExecuter();
    void stopExecuter();

  protected:
    virtual bool update() = 0;

  private:
    void executerLoop();

    boost::thread                         _executerThread;

    boost::mutex                          _pauseRequestMutex;
    boost::condition_variable             _pauseRequestCondition;
    bool                                  _pauseRequest;

    boost::mutex                          _isPlayingMutex;
    boost::condition_variable             _isPlayingCondition;
    bool                                  _isPlaying;

    unsigned int                          _interval;
};

};

#endif /* !ASYNC_EXECUTER_H_ */
