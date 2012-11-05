/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <almath/tools/altrigonometry.h>
#include <alcommon/alproxy.h>
#include <alcommon/albroker.h>

#include <alserial/alserial.h>
#include <qi/log.hpp>

#include <qicore/timeline.hpp>
#include "timeline_private.hpp"
#include "xml_utils.hpp"
#include "actuator_curve.hpp"

TimelinePrivate::TimelinePrivate(boost::shared_ptr<AL::ALBroker> broker)
  : asyncExecuter(1000 / 25),
    _fps(0),
    _enabled(false),
    _startFrame(0),
    _endFrame(-1),
    _lastFrame(-1),
    _currentDoInterpolationMoveOrderId(-1),
    _name("Timeline"),
    _resourcesAcquisition(PASSIVE),
    _methodMonitor()
{
  try
  {
    _memoryProxy = broker->getMemoryProxy();
    _motionProxy = broker->getMotionProxy();
  }
  catch (AL::ALError& e)
  {
    qiLogError("Timeline") << "Cannot create proxy on ALMotion :" << std::endl << e.toString() << std::endl;
  }
}

TimelinePrivate::~TimelinePrivate(void)
{
  boost::unique_lock<boost::recursive_mutex> lock(_methodMonitor);

  stop();
  try
  {
    _memoryProxy->removeMicroEvent(_name);
  }
  catch(AL::ALError&)
  {
  }

  /* Outputs can be triggered here ? */

  for (std::vector<ActuatorCurve*>::iterator it = _actuatorCurves.begin();
        it != _actuatorCurves.end(); it++)
    delete (*it);
}

void TimelinePrivate::loadFromXml(boost::shared_ptr<const AL::XmlElement> elt)
{
  boost::unique_lock<boost::recursive_mutex> lock(_methodMonitor);
  if(elt == NULL)
    return;

  if (isPlaying())
    stop();

  /* Remove previous ActuatorCurves if needed */
  if (!_actuatorCurves.empty())
    for (std::vector<ActuatorCurve*>::iterator it = _actuatorCurves.begin();
          it != _actuatorCurves.end(); it++)
      delete (*it);

  int handler;
  if(elt->getAttribute("resources_acquisition", handler))
    _resourcesAcquisition = MotionResourcesHandler(handler);

  elt->getAttribute("fps", _fps, 0);
  elt->getAttribute("enable", _enabled);
  elt->getAttribute("start_frame", _startFrame, 0);
  elt->getAttribute("end_frame", _endFrame, -1);
  _currentFrame = _startFrame;
  setInterval(1000 / _fps);

  // load actuator list
  {
    AL::XmlElement::CList list = elt->children(XmlUtils::fActuatorCurveBeacon, XmlUtils::fActuatorListBeacon);
    _lastFrame = 0;
    for (AL::XmlElement::CList::const_iterator it=list.begin(), itEnd=list.end(); it!=itEnd; ++it)
    {
      boost::shared_ptr<const AL::XmlElement> curve = *it;
      bool mute = false;
      // don't create muted curves
      if (!curve->getAttribute("mute", mute) || !mute)
      {
        ActuatorCurve* newCurve = new ActuatorCurve();
        int lastKeyframe = newCurve->loadFromXml(curve);
        _actuatorCurves.push_back(newCurve);
        if (lastKeyframe > _lastFrame)
          _lastFrame = lastKeyframe;
      }
    }

    // if _endFrame = -1, meaning end frame is "undefined" => attached to last motion keyframe
    // then use last motion keyframe to stop timeline
    if(_endFrame == -1)
      _endFrame = _lastFrame;
  }
}

void TimelinePrivate::killMotionOrders()
{
  boost::unique_lock<boost::recursive_mutex> lock(_methodMonitor);

  // stop curves
  try
  {
    if(_currentDoInterpolationMoveOrderId != -1)
    {
      _motionProxy->stop(_currentDoInterpolationMoveOrderId);
      _currentDoInterpolationMoveOrderId = -1;
    }
  }
  catch(AL::ALError& e)
  {
    qiLogError("Timeline") << _name << e.toString() << std::endl;
  }
}

void TimelinePrivate::play()
{
  boost::unique_lock<boost::recursive_mutex> lock(_methodMonitor);

  if (_enabled == false || _fps == 0)
  {
    if (_currentFrame == _startFrame)
      ++_currentFrame;
    return;
  }

  playExecuter();
}

void TimelinePrivate::pause()
{
  pauseExecuter();
  killMotionOrders();
}

void TimelinePrivate::stop()
{
  boost::unique_lock<boost::recursive_mutex> lock(_methodMonitor);

  stopExecuter();
  killMotionOrders();
  _currentFrame = -1;
  _currentFrame = _startFrame;
}

void TimelinePrivate::goTo(const int &pFrame)
{
  boost::unique_lock<boost::recursive_mutex> lock(_methodMonitor);
  if (!_enabled)
    return;
  _currentFrame = pFrame;
  killMotionOrders();
}

int TimelinePrivate::getSize() const
{
  boost::unique_lock<boost::recursive_mutex> lock(_methodMonitor);
  return _endFrame - _startFrame;
}

void TimelinePrivate::setFPS(const int pFps)
{
  boost::unique_lock<boost::recursive_mutex> lock(_methodMonitor);

  _fps = pFps;
  setInterval(1000 / _fps);
}

int TimelinePrivate::getFPS() const
{
  boost::unique_lock<boost::recursive_mutex> lock(_methodMonitor);
  return _fps;
}

void TimelinePrivate::stimulateStoppedOutputs()
{
  /* Can change a value in almemory to acknowledge that
     timeline execution is finished */
}

bool TimelinePrivate::update(void)
{
  boost::unique_lock<boost::recursive_mutex> _lock(_methodMonitor);
  if (_enabled == false)
  {
    // update is not necessary
    return true;
  }

  // if on the end frame
  // or if on the last motion keyframe with no behaviour layers
  if ( ((_endFrame >= 1) && (_currentFrame >= _endFrame))
    || (_currentFrame < _startFrame))
  {
    updateFrameInSTM();
    stimulateStoppedOutputs();
    _currentFrame = _startFrame;
    killMotionOrders();

    return false;
  }

  // try to call motion commands
  bool motionWorked = executeCurveMotionCommand();

  // move on to the next frame
  if(motionWorked && isPlaying())
    ++_currentFrame;
  return true;
}

void TimelinePrivate::updateFrameInSTM()
{
  // set the value of the current frame for this timeline in the stm
  if(!_name.empty())
  {
    try
    {
      _memoryProxy->raiseMicroEvent(_name, (int) _currentFrame);
    }
    catch(AL::ALError& e)
    {
      qiLogError("Timeline") << _name << " Error During STM access : Error #=" << e.toString() << std::endl;
    }
  }
}

bool TimelinePrivate::executeCurveMotionCommand()
{
  if (_currentDoInterpolationMoveOrderId != -1)
  {
    // no need to do anything, the command was already sent before or we are at the last keyframe
    return true;
  }

  // send new command to motion
  bool isprepared = false;
  if (isPlaying())
    isprepared = prepareInterpolationCommand(_currentFrame);
  else
    isprepared = singleInterpolationCommand(_currentFrame);

  return isprepared;
}

bool TimelinePrivate::singleInterpolationCommand(int currentFrame)
{
  std::vector<std::string> actuatorNames;
  std::vector<float> actuatorValues;

  for (size_t i=0; i < _actuatorCurves.size(); ++i)
  {
    ActuatorCurve* actuatorCurve = _actuatorCurves[i];

    // FIXME : speedLimit
    float speedLimit = std::numeric_limits<float>::max();
    float valueIncrementLimit = speedLimit/_fps;

    float interpolatedValue = actuatorCurve->getInterpolatedValue(currentFrame, valueIncrementLimit);
    float actuatorValue = actuatorCurve->getMotionValue(interpolatedValue);

    actuatorNames.push_back(actuatorCurve->name());
    actuatorValues.push_back(actuatorValue);
  }

  if(actuatorNames.empty())
    return true;

  // FIXME : motorSpeed
  try
  {
    _motionProxy->post.angleInterpolationWithSpeed(actuatorNames, actuatorValues, 0.25f);
  }
  catch(const AL::ALError& e)
  {
    qiLogError("Timeline") << _name << " Error during .postAngleInterpolationWithSpeed in motion : Error #= " << e.toString() << std::endl;
  }
  return true;
}

bool TimelinePrivate::prepareInterpolationCommand(int startFrame)
{
  std::vector<std::string> names;
  AL::ALValue times;
  AL::ALValue keys;

  size_t size = _actuatorCurves.size();

  // EPOT FS#1967 : at the moment, Motion does not accept empty commands...
  // Therefore, we only add actuators that are modified in the command we send.

  for (size_t i=0; i<size; ++i)
  {
    ActuatorCurve* curve = _actuatorCurves[i];

    AL::ALValue times_i;
    AL::ALValue keys_i;

    // filters keys after start frame
    std::deque<std::pair<int, ActuatorCurve::Key> > filteredKeys;
    remove_copy_if(curve->keys().begin(), curve->keys().end(), back_inserter(filteredKeys),
        boost::bind(&std::map<int, ActuatorCurve::Key>::value_type::first, _1) < startFrame );

    // for each key
    size_t size = filteredKeys.size();

    // if there are nothing to change for this actuator from current frame to the end,
    // then just pass to next actuator.
    if(size == 0)
      continue;

    names.push_back(curve->name());

    float scale;
    switch(curve->curveUnit())
    {
    case ActuatorCurve::DEGREE_UNIT:
      scale = AL::Math::TO_RAD;
      break;

    case ActuatorCurve::PERCENT_UNIT:
      scale = 1.f;
      break;

    // backport compatibility
    case ActuatorCurve::UNKNOWN_UNIT:
    default:
      if(curve->name()=="LHand" || curve->name()=="RHand")
        scale = 1.f;
      else
        scale = AL::Math::TO_RAD;
    }

    times_i.arraySetSize(size);
    keys_i.arraySetSize(size);
    for (size_t j=0; j<size; ++j)
    {
      int frame = filteredKeys[j].first;
      ActuatorCurve::Key key = filteredKeys[j].second;

      // build command parameters for that key
      times_i[j] = float(frame - startFrame) / _fps;
      keys_i[j] = AL::ALValue::array(
        key.fValue * scale,
        AL::ALValue::array(key.fLeftTangent.fType, key.fLeftTangent.fOffset.x / _fps, key.fLeftTangent.fOffset.y * scale),
        AL::ALValue::array(key.fRightTangent.fType, key.fRightTangent.fOffset.x / _fps, key.fRightTangent.fOffset.y * scale)
      );
    }
    times.arrayPush(times_i);
    keys.arrayPush(keys_i);
  }

  if(names.empty())
    return true;

  return sendInterpolationCommand(names, times, keys);
}

bool TimelinePrivate::sendInterpolationCommand(const std::vector<std::string>& names, const AL::ALValue& times, const AL::ALValue& keys)
{
  if(_resourcesAcquisition != PASSIVE)
  {
    // Ask to know if the order prepared is possible.
    try
    {
      if(!_motionProxy->areResourcesAvailable(names))
      {
        if(_resourcesAcquisition == WAITING)
          return false; // we will not execute anything, and we just wait fot another turn.

        // If in aggressive mode, then kill all tasks using the same resources, mouahahahahah !
        if(_resourcesAcquisition == AGGRESSIVE)
          _motionProxy->killTasksUsingResources(names);
      }
    }
    catch(AL::ALError& e)
    {
      // do nothing, just keep reading the timeline !
      qiLogError("Timeline") << _name << ":sendInterpolationCommand failed with the error:\n" << e.toString() << std::endl;
    }
  }

  try
  {
    _currentDoInterpolationMoveOrderId = _motionProxy->post.angleInterpolationBezier(names, times, keys);
  }
  catch(AL::ALError& e)
  {
    qiLogError("Timeline") << _name << " Error during .angleInterpolationBezier in motion : Error #= " << e.toString() << std::endl;
  }

  return true;
}

bool TimelinePrivate::getEnabled() const
{
  boost::unique_lock<boost::recursive_mutex> lock(_methodMonitor);
  return _enabled;
}

int TimelinePrivate::getCurrentFrame() const
{
  boost::unique_lock<boost::recursive_mutex> lock(_methodMonitor);
  return _currentFrame;
}

void TimelinePrivate::setCurrentFrame(int pFrame)
{
  boost::unique_lock<boost::recursive_mutex> lock(_methodMonitor);
  if(pFrame < 0)
    return;
  _currentFrame = pFrame;
}

void TimelinePrivate::setName(const std::string& var)
{
  boost::unique_lock<boost::recursive_mutex> lock(_methodMonitor);
  _name = var;

  // insert value of frame = -1 in the stm (used by choregraphe)
  if(_name != "")
  {
    try
    {
      _memoryProxy->raiseMicroEvent(_name, (int) -1);
    }
    catch(AL::ALError& e)
    {
      qiLogError("Timeline") << _name << " Error During STM access : Error #=" << e.toString() << std::endl;
    }
  }
}

std::string TimelinePrivate::getName() const
{
  boost::unique_lock<boost::recursive_mutex> lock(_methodMonitor);
  return _name;
}

/* -- Public -- */

Timeline::Timeline(boost::shared_ptr<AL::ALBroker> broker)
  : AL::ALTimeline(),
    _p (new TimelinePrivate(broker))
{
}

Timeline::~Timeline()
{
  _p->stop();
  delete _p;
}

bool Timeline::loadFromFile(std::string fileName)
{
  std::string errorMsg;
  boost::shared_ptr<AL::XmlDocument> xmlFile = AL::XmlDocument::loadFromXmlFile(fileName, NULL, &errorMsg);

  if(!xmlFile)
  {
    qiLogError("Timeline") << "Failed to open the given file : " << fileName << std::endl << errorMsg << std::endl;

    return false;
  }

  boost::shared_ptr<const AL::XmlElement> elt = xmlFile->root()->firstChild("Timeline");
  _p->loadFromXml(elt);
  _p->setName(fileName);

  return true;
}

void Timeline::play(void)
{
  _p->play();
}

void Timeline::pause(void)
{
  _p->pause();
}

void Timeline::stop(void)
{
  _p->stop();
}

void Timeline::goTo(const int &pFrame)
{
  _p->goTo(pFrame);
}

// TODO: Remove this method
void Timeline::goTo(const std::string &pFrameName)
{
  std::cerr << "Method is no more implemented !" << std::endl;
}

int Timeline::getSize() const
{
  return _p->getSize();
}

int Timeline::getFPS() const
{
  return _p->getFPS();
}

void Timeline::setFPS(const int fps)
{
  _p->setFPS(fps);
}

void Timeline::waitForTimelineCompletion()
{
  _p->waitForExecuterCompletion();
}
