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
    ~asyncExecuter();

    unsigned int getInterval() const;
    void setInterval(unsigned int interval);

    void waitForExecuterCompletion();
    bool isPlaying();

    void playExecuter(boost::function<bool (void)>);
    /*
     * Send the message to executer to pause as soon as possible
     * Note that when exiting this function, the worker thread may
     * not be paused yet.
     */
    void pauseExecuter();
    /*
     * Send the message to executer to pause and wait for it
     * Use this function to be sure that update() will not be called
     * once again.
     * When exiting this function, the executer is paused
     */
    void waitUntilPauseExecuter();
    void stopExecuter(bool join = true);

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

    boost::function<bool (void)>          _callback;
};

};

#endif /* !ASYNC_EXECUTER_H_ */
