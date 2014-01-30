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
    _interval (interval),
    _callback (0)
{
}

asyncExecuter::~asyncExecuter()
{
  stopExecuter();
}

void asyncExecuter::playExecuter(boost::function<bool (void)> f)
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

  _callback = f;
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

  if (!_isPlaying)
    return;

  if (_pauseRequest == true)
    return;
  _pauseRequest = true;
  _pauseRequestCondition.wait(pauseLock);
}

void asyncExecuter::stopExecuter(bool join)
{
  {
    boost::mutex::scoped_lock pauseLock(_isPlayingMutex);
    if (!_isPlaying)
      return;
    _isPlaying = false;
  }

  _executerThread.interrupt();
  if (join)
    _executerThread.join();

  _callback = 0;
}

void asyncExecuter::executerLoop()
{
  while (true)
  {
    if (boost::this_thread::interruption_requested())
      break;

    qi::os::msleep(_interval);

    /* Notify users that wait for pause */
    _pauseRequestCondition.notify_all();
    {
      boost::mutex::scoped_lock pauseLock(_pauseRequestMutex);
      while (_pauseRequest)
        _pauseRequestCondition.wait(pauseLock);
    }

    if (!_callback())
      break;
  }

  _callback = 0;

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

unsigned int asyncExecuter::getInterval() const
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
