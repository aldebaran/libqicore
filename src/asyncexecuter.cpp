/**
 * @author Julien Freche
 * Aldebaran Robotics (c) 2007-2012 All Rights Reserved
 */

#include <qi/os.hpp>
#include "asyncexecuter.hpp"

namespace qi
{

asyncExecuter::asyncExecuter(unsigned int interval)
  : _pauseRequest (false),
    _isPlaying (false),
    _interval (interval)
{
}

asyncExecuter::~asyncExecuter()
{
  stopExecuter();
}

void asyncExecuter::playExecuter()
{
  {
    boost::mutex::scoped_lock lock(_pauseRequestMutex);

    if (_pauseRequest)
    {
      _pauseRequest = false;
      _pauseRequestCondition.notify_all();
    }
  }

  {
    boost::mutex::scoped_lock lock(_isPlayingMutex);

    if (_isPlaying)
      return;
    _isPlaying = true;
  }

  _executerThread = boost::thread(boost::bind(&asyncExecuter::executerLoop, this));
}

void asyncExecuter::pauseExecuter()
{
  boost::mutex::scoped_lock pauseLock(_pauseRequestMutex);
  _pauseRequest = true;
}

void asyncExecuter::waitUntilPauseExecuter()
{
  boost::mutex::scoped_lock pauseLock(_pauseRequestMutex);
  _pauseRequest = true;
  _pauseRequestCondition.wait(pauseLock);
}

void asyncExecuter::stopExecuter()
{
  {
    boost::mutex::scoped_lock pauseLock(_isPlayingMutex);
    if (!_isPlaying)
      return;
    _isPlaying = false;
  }

  /* First force the thread to be in an interruptible point */
  /* Avoid some race conditions */
  waitUntilPauseExecuter();

  _executerThread.interrupt();
  _executerThread.join();

  _isPlayingCondition.notify_all();
}

void asyncExecuter::executerLoop()
{
  /* Prevent a deadlock if user stops the thread but
     the thread is not yet created */
  if (boost::this_thread::interruption_requested())
    return;

  while (true)
  {
    qi::os::msleep(_interval);

    /* Notify users that wait for pause */
    _pauseRequestCondition.notify_all();
    {
      boost::mutex::scoped_lock pauseLock(_pauseRequestMutex);
      while (_pauseRequest)
        _pauseRequestCondition.wait(pauseLock);
    }

    if (!update())
      break;
  }

  {
    boost::mutex::scoped_lock pauseLock(_isPlayingMutex);
    _isPlaying = false;
  }
  _isPlayingCondition.notify_all();
}

void asyncExecuter::waitForExecuterCompletion()
{
  boost::mutex::scoped_lock lock(_isPlayingMutex);

  if (_isPlaying)
  {
    _isPlayingCondition.wait(lock);
  }
}

unsigned int asyncExecuter::getInterval()
{
  return _interval;
}

void asyncExecuter::setInterval(unsigned int interval)
{
  _interval = interval;
}

bool asyncExecuter::isPlaying()
{
  return _isPlaying;
}

};
