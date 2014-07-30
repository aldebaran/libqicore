/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <queue>

#include <alerror/alerror.h>
#include <almath/tools/altrigonometry.h>
#include <almathinternal/interpolations/alinterpolation.h>
#include <alcommon/alproxy.h>

#include <qi/log.hpp>
#include <qi/type/objecttypebuilder.hpp>

#include "timeline_p.hpp"
#include <qicore-compat/timeline.hpp>
#include <qicore-compat/model/actuatorlistmodel.hpp>
#include <qicore-compat/model/actuatorcurvemodel.hpp>
#include <qicore-compat/model/keymodel.hpp>
#include <qicore-compat/model/tangentmodel.hpp>

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

qiLogCategory("qiCore.Timeline");

namespace qi
{

TimelinePrivate::TimelinePrivate(AnyObject motion, Timeline *timeline)
  : _executer(new asyncExecuter(1000 / 25)),
    _fps(0),
    _enabled(false),
    _currentFrame(0),
    _startFrame(0),
    _endFrame(-1),
    _lastFrame(-1),
    _currentDoInterpolationMoveOrderId(-1),
    _name("Timeline"),
    _resourcesAcquisition(AnimationModel::MotionResourcesHandler_Passive),
    _methodMonitor(),
    _framesFlagsMap(),
    _timeline(timeline),
    _isValid(true)
{
  try
  {
    boost::shared_ptr<AL::ALProxy> proxyMotion(new AL::ALProxy(motion, "ALMotion"));
    _motionProxy = boost::shared_ptr<AL::ALMotionProxy>(new AL::ALMotionProxy(proxyMotion));
  }
  catch (AL::ALError& e)
  {
    qiLogError() << "Cannot create proxy on ALMotion :" << std::endl << e.what() << std::endl;
    _isValid = false;
  }
}

TimelinePrivate::~TimelinePrivate()
{
  boost::unique_lock<boost::recursive_mutex> lock(_methodMonitor);

  stop();

  delete _executer;
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
    qiLogError() << _name << e.what() << std::endl;
  }
}

void TimelinePrivate::play()
{
  qiLogDebug() << "Play timeline " << _name;
  boost::unique_lock<boost::recursive_mutex> lock(_methodMonitor);

  if (_enabled == false || _fps == 0)
  {
    if (_currentFrame == _startFrame)
    {
      ++_currentFrame;
      update();
    }
    return;
  }

  _executer->playExecuter(boost::bind(&TimelinePrivate::update, this));
}

void TimelinePrivate::pause()
{
  qiLogDebug() << "Pause timeline " << _name;
  _executer->waitUntilPauseExecuter();
  killMotionOrders();
}

void TimelinePrivate::stop(bool join)
{
  qiLogDebug() << "Stopping timeline " << _name;
  _executer->stopExecuter(join);

  {
    boost::unique_lock<boost::recursive_mutex> lock(_methodMonitor);

    killMotionOrders();
  }

  qiLogDebug() << "Timeline " << _name << " stopped";
}

void TimelinePrivate::goTo(int pFrame)
{
  qiLogDebug() << "goto timeline with : " << pFrame;
  boost::unique_lock<boost::recursive_mutex> lock(_methodMonitor);
  _currentFrame = pFrame;
  killMotionOrders();
  update();
}

void TimelinePrivate::goTo(const std::string& pFrame)
{
  qiLogDebug() << "goto timeline with : " << pFrame;
  std::map<std::string, int>::iterator iter =
    _framesFlagsMapRev.find(pFrame);
  if (iter == _framesFlagsMapRev.end())
  {
    qiLogWarning() << "Unknown frame to go to: " << pFrame;
    return;
  }
  goTo(iter->second);
}

int TimelinePrivate::getSize() const
{
  boost::unique_lock<boost::recursive_mutex> lock(_methodMonitor);
  return _endFrame - _startFrame;
}

void TimelinePrivate::setFPS(int pFps)
{
  boost::unique_lock<boost::recursive_mutex> lock(_methodMonitor);

  _fps = pFps;
  _executer->setInterval(1000 / _fps);
}

int TimelinePrivate::getFPS() const
{
  boost::unique_lock<boost::recursive_mutex> lock(_methodMonitor);
  return _fps;
}

void TimelinePrivate::setAnimation(AnimationModel* anim)
{
  boost::unique_lock<boost::recursive_mutex> lock(_methodMonitor);

  if(!anim)
    return;

  if(_executer->isPlaying())
    stop();

  _actuatorCurves.clear();

  _name       = anim->path();
  _fps        = anim->fps();
  _enabled    = true;
  _startFrame = anim->startFrame();
  _endFrame   = anim->endFrame();

  _currentFrame = _startFrame;
  _executer->setInterval(1000 / _fps);

  _resourcesAcquisition = anim->resourcesAcquisition();

  _lastFrame = 0;
  std::list<ActuatorCurveModelPtr> list = anim->actuatorList()->actuatorsCurve();
  std::list<ActuatorCurveModelPtr>::const_iterator it    = list.begin();
  std::list<ActuatorCurveModelPtr>::const_iterator itEnd = list.end();
  for(; it != itEnd; ++it)
  {
    ActuatorCurveModelPtr curve = *it;

    if( !curve->mute() )
    {
      int lastKeyFrame = curve->lastKeyFrame();
      _actuatorCurves.push_back(curve);
      if( lastKeyFrame > _lastFrame )
        _lastFrame = lastKeyFrame;
    }
    rebuildBezierAutoTangents(curve);
  }

  // if _endFrame = -1, meaning end frame is "undefined" => attached to last motion keyframe
  // then use last motion keyframe to stop timeline
  if(_endFrame == -1)
    _endFrame = _lastFrame;
}

void TimelinePrivate::startFlowdiagram(int index)
{
  _timeline->startFlowdiagram(index);
}

void TimelinePrivate::startFlowdiagramAsync(int index)
{
  qiLogDebug() << "Start Flowdiagram, frames : " << index;
  boost::thread* t = new boost::thread(boost::bind(&TimelinePrivate::startFlowdiagram, this, index));

  _flowdiagrams.push_back(t);
}

void TimelinePrivate::stopFlowdiagram()
{
  _timeline->stopFlowdiagram(-1);

  foreach(boost::thread *t, _flowdiagrams)
  {
    t->join();
    delete t;
  }
}

bool TimelinePrivate::update()
{
  boost::unique_lock<boost::recursive_mutex> _lock(_methodMonitor);

  /* Send commands to the Flowdiagram if needed */
  std::map<int, std::string>::const_iterator it = _framesFlagsMap.find(_currentFrame);

  //Todo launch signal in a boost::thread
  if (it != _framesFlagsMap.end())
    startFlowdiagramAsync(it->first);

  if (_enabled == false)
  {
    // update is not necessary
    return true;
  }

  // if on the end frame
  if ( ((_endFrame >= 1) && (_currentFrame >= _endFrame))
    || (_currentFrame < _startFrame))
  {
    _currentFrame = _startFrame;
    qiLogDebug() << "Killing motion orders";
    killMotionOrders();

    qiLogDebug() << "Timeline is done";

    // Do *not* join, we are in the thread here, we would attempt to join with
    // ourself
    stop(false);

    _timeline->onTimelineFinished();

    return false;
  }

  // try to call motion commands
  bool motionWorked = executeCurveMotionCommand();

  // move on to the next frame
  if(motionWorked && _executer->isPlaying())
    ++_currentFrame;
  return true;
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
  if (_executer->isPlaying())
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
    ActuatorCurveModelPtr actuatorCurve = _actuatorCurves[i];

    // FIXME : speedLimit
    float speedLimit = std::numeric_limits<float>::max();
    float valueIncrementLimit = speedLimit/_fps;

    float interpolatedValue = getInterpolatedValue(*actuatorCurve, currentFrame, valueIncrementLimit);
    float actuatorValue     = getMotionValue(*actuatorCurve, interpolatedValue);

    actuatorNames.push_back(actuatorCurve->actuator());
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
    qiLogError() << _name << " Error during .postAngleInterpolationWithSpeed in motion : Error #= " << e.what() << std::endl;
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
    ActuatorCurveModelPtr curve = _actuatorCurves[i];

    AL::ALValue times_i;
    AL::ALValue keys_i;

    // filters keys after start frame
    std::map<int, KeyModelPtr> noFiltredkeys = curve->keys();
    std::deque<std::pair<int, KeyModelPtr> > filteredKeys;
    remove_copy_if(noFiltredkeys.begin(), noFiltredkeys.end(), back_inserter(filteredKeys),
        boost::bind(&std::map<int, KeyModelPtr>::value_type::first, _1) < startFrame );

    // for each key
    size_t size = filteredKeys.size();

    // if there are nothing to change for this actuator from current frame to the end,
    // then just pass to next actuator.
    if(size == 0)
      continue;

    names.push_back(curve->actuator());

    float scale;
    switch(curve->unit())
    {
    case ActuatorCurveModel::UnitType_Degree:
      scale = AL::Math::TO_RAD;
      break;

    case ActuatorCurveModel::UnitType_Percent:
      scale = 1.f;
      break;

    // backport compatibility
    case ActuatorCurveModel::UnitType_Undefined:
    default:
      if(curve->actuator() =="LHand" || curve->actuator()=="RHand")
        scale = 1.f;
      else
        scale = AL::Math::TO_RAD;
    }

    times_i.arraySetSize(size);
    keys_i.arraySetSize(size);
    for (size_t j=0; j<size; ++j)
    {
      int frame = filteredKeys[j].first;
      KeyModelPtr key = filteredKeys[j].second;

      // build command parameters for that key
      times_i[j] = float(frame - startFrame) / _fps;
      keys_i[j] = AL::ALValue::array(
            key->value() * scale,
            AL::ALValue::array(key->leftTangent()->interpType(),
                               key->leftTangent()->abscissaParam() / _fps,
                               key->leftTangent()->ordinateParam() * scale),
            AL::ALValue::array(key->rightTangent()->interpType(),
                               key->rightTangent()->abscissaParam() / _fps,
                               key->rightTangent()->ordinateParam() * scale)
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
  if(_resourcesAcquisition != AnimationModel::MotionResourcesHandler_Passive)
  {
    // Ask to know if the order prepared is possible.
    try
    {
      if(!_motionProxy->areResourcesAvailable(names))
      {
        if(_resourcesAcquisition == AnimationModel::MotionResourcesHandler_Waiting)
          return false; // we will not execute anything, and we just wait fot another turn.

        // If in aggressive mode, then kill all tasks using the same resources, mouahahahahah !
        if(_resourcesAcquisition == AnimationModel::MotionResourcesHandler_Aggressive)
          _motionProxy->killTasksUsingResources(names);
      }
    }
    catch(AL::ALError& e)
    {
      // do nothing, just keep reading the timeline !
      qiLogError() << _name << ":sendInterpolationCommand failed with the error:\n" << e.what() << std::endl;
    }
  }

  try
  {
    _currentDoInterpolationMoveOrderId = _motionProxy->post.angleInterpolationBezier(names, times, keys);
  }
  catch(AL::ALError& e)
  {
    qiLogError() << _name << " Error during .angleInterpolationBezier in motion : Error #= " << e.what() << std::endl;
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

std::string TimelinePrivate::getName() const
{
  boost::unique_lock<boost::recursive_mutex> lock(_methodMonitor);
  return _name;
}

// Returns the nearest neighbor (right and left) key frames in the timeline for a given key frame
// if given key is before the first key, both neighbors are set to this first key
// if after the last key, both neighbors are set to this last key
void TimelinePrivate::getNeighborKeysOf(const ActuatorCurveModel& curve,
                                        const int indexKey,
                                        int& indexLeftKey,
                                        KeyModelPtr& leftKey,
                                        int& indexRightKey,
                                        KeyModelPtr& rightKey)
{
  const std::map<int, KeyModelPtr>& keys = curve.keys();

  if (keys.empty())
  {
    indexLeftKey = -1;
    indexRightKey = -1;
    return;
  }

  std::map<int, KeyModelPtr>::const_iterator it = keys.begin();

  indexLeftKey = it->first;
  leftKey = it->second;

  if (indexKey > indexLeftKey)
  {
    while (it != keys.end())
    {
      indexRightKey = it->first;
      rightKey = it->second;
      if (indexKey < indexRightKey)
      {
        return;
      }
      indexLeftKey = indexRightKey;
      leftKey = rightKey;
      it++;
    }
  }
  else
  {
    // given key before first key => neighbors are both this first key
    indexRightKey = indexLeftKey;
    rightKey = leftKey;
  }
}

// Returns the value for a given frame based on interpolation on the two nearest frames
float TimelinePrivate::getInterpolatedValue(const ActuatorCurveModel& curve,
                                            const int& indexKey,
                                            const float& valueIncrementLimit)
{
  AL::Math::Interpolation::ALInterpolationBezier interpolator;

  int indexLeftKey;
  KeyModelPtr lKey;
  int indexRightKey;
  KeyModelPtr rKey;


  getNeighborKeysOf(curve, indexKey, indexLeftKey, lKey, indexRightKey, rKey);

  AL::Math::Interpolation::Key leftKey(lKey->value(),
                                       AL::Math::Interpolation::Tangent(static_cast<AL::Math::Interpolation::InterpolationType>(lKey->leftTangent()->interpType()),
                                                                        AL::Math::Position2D(lKey->leftTangent()->abscissaParam(),
                                                                                             lKey->leftTangent()->ordinateParam())),
                                       AL::Math::Interpolation::Tangent(static_cast<AL::Math::Interpolation::InterpolationType>(lKey->rightTangent()->interpType()),
                                                                        AL::Math::Position2D(lKey->rightTangent()->abscissaParam(),
                                                                                             lKey->rightTangent()->ordinateParam())));
  AL::Math::Interpolation::Key rightKey(rKey->value(),
                                        AL::Math::Interpolation::Tangent(static_cast<AL::Math::Interpolation::InterpolationType>(rKey->leftTangent()->interpType()),
                                                                         AL::Math::Position2D(rKey->leftTangent()->abscissaParam(),
                                                                                              rKey->leftTangent()->ordinateParam())),
                                        AL::Math::Interpolation::Tangent(static_cast<AL::Math::Interpolation::InterpolationType>(rKey->rightTangent()->interpType()),
                                                                         AL::Math::Position2D(rKey->rightTangent()->abscissaParam(),
                                                                                              rKey->rightTangent()->ordinateParam())));

  int keyIntervalSize = indexRightKey - indexLeftKey + 1;

  if (keyIntervalSize > 1)
  {
    // FIXME : minValue, maxValue
    float minValue = -std::numeric_limits<float>::max();
    float maxValue = std::numeric_limits<float>::max();

    std::vector<float> actuatorValues = interpolator.interpolate(keyIntervalSize, leftKey, rightKey,
                                                                 minValue, maxValue, valueIncrementLimit, 1);

    return actuatorValues[indexKey - indexLeftKey];
  }
  else
  {
    return leftKey.fValue;
  }
}

float TimelinePrivate::getMotionValue(const ActuatorCurveModel& curve, float value)
{
  ActuatorCurveModel::UnitType curveUnit = curve.unit();
  float result = value;
  switch(curveUnit)
  {
  case ActuatorCurveModel::UnitType_Degree:
    result *= AL::Math::TO_RAD;
    break;

  case ActuatorCurveModel::UnitType_Percent:
    break;

  case ActuatorCurveModel::UnitType_Undefined:
  default:
    if(curve.actuator() != "LHand" && curve.actuator() != "RHand")
    {
      result *= AL::Math::TO_RAD;
      break;
    }
  }

  return result;
}

bool TimelinePrivate::updateBezierAutoTangents(const int& currentIndex, KeyModelPtr key, const int &leftIndex, CKeyModelPtr &lNeighbor, const int &rightIndex, CKeyModelPtr &rNeighbor)
{
  using AL::Math::Position2D;

  // :TODO: jvuarand 20100406: merge that function with the one in Choregraphe

  //Note JB Desmottes 19-05-09 : we now consider that whenever the method is
  //  called, tangent params will change, and thus we do not need to inform the
  //  caller whether they really changed or not. In some cases, this will lead to
  //  unecessary updates.
  //  Example : current key is a minimum of the curve, and neighbor value (not the
  //  index) has changed. In that case, params of current key do not change.

  if (key->leftTangent()->interpType()==TangentModel::InterpolationType_BezierAuto || key->rightTangent()->interpType()==TangentModel::InterpolationType_BezierAuto)
  {
    float alpha = 1.0f/3.0f;
    float beta = 0.0f;

    if (lNeighbor && rNeighbor)
    {
      float value = key->value();
      float lvalue = lNeighbor->value();
      float rvalue = rNeighbor->value();
      if ((value < rvalue || value < lvalue)
        && (value > rvalue || value > lvalue))
      {
        if (currentIndex>=0 && leftIndex>=0 && rightIndex>=0)
        {
          beta = (rvalue - lvalue) / (rightIndex-leftIndex);

          // anti overshooting
          float tgtHeight = alpha * (rightIndex - currentIndex) * beta;
          if (fabs(tgtHeight) > fabs(rvalue - value))
          {
            beta *= (rvalue - value) / tgtHeight;
          }
          tgtHeight = alpha * (currentIndex - leftIndex) * beta;
          if (fabs(tgtHeight) > fabs(value - lvalue))
          {
            beta *= (value - lvalue) / tgtHeight;
          }
        }
      }
    }

    // set parameters into model
    if (key->leftTangent()->interpType()==TangentModel::InterpolationType_BezierAuto)
    {
      Position2D offset = Position2D(-alpha, -alpha*beta) * float(currentIndex - leftIndex);
      // :NOTE: for test purposes, you can force serialization of BEZIER_AUTO in Choregraphe, and enable assert below
      //assert(AL::Math::distanceSquared(key->fLeftTangent.fOffset, offset) < 0.01f);
      //key->fLeftTangent.fOffset = offset;

      key->leftTangent()->setAbscissaParam(offset.x);
      key->leftTangent()->setOrdinateParam(offset.y);
    }
    if (key->rightTangent()->interpType()==TangentModel::InterpolationType_BezierAuto)
    {
      Position2D offset = Position2D(alpha, alpha*beta) * float(rightIndex - currentIndex);
      // :NOTE: for test purposes, you can force serialization of BEZIER_AUTO in Choregraphe, and enable assert below
      //assert(AL::Math::distanceSquared(key->fRightTangent.fOffset, offset) < 0.01f);
      //key->fRightTangent.fOffset = offset;

      key->rightTangent()->setAbscissaParam(offset.x);
      key->rightTangent()->setOrdinateParam(offset.y);
    }

    return true;
  }
  else
    return false;
}

void TimelinePrivate::rebuildBezierAutoTangents(ActuatorCurveModelPtr curve)
{

  std::map<int, KeyModelPtr> keys = curve->keys();
  for (std::map<int, KeyModelPtr>::iterator it=keys.begin(), itEnd=keys.end(); it!=itEnd; ++it)
  {
    if (it->second->leftTangent()->interpType()==TangentModel::InterpolationType_BezierAuto
      || it->second->rightTangent()->interpType()==TangentModel::InterpolationType_BezierAuto)
    {
      int currentIndex = it->first;
      KeyModelPtr key = it->second;
      // get left neighbor, if any
      int leftIndex = currentIndex;
      CKeyModelPtr lNeighbor = CKeyModelPtr();
      if (it!=keys.begin())
      {
        std::map<int, KeyModelPtr>::iterator left = it;
        --left;
        leftIndex = left->first;
        lNeighbor = left->second;
      }
      // get right neighbor, if any
      int rightIndex = currentIndex;
      CKeyModelPtr rNeighbor = CKeyModelPtr();
      std::map<int, KeyModelPtr>::iterator right = it;
      ++right;
      if (right!=keys.end())
      {
        rightIndex = right->first;
        rNeighbor  = right->second;
      }
      // adjust this key
      updateBezierAutoTangents(currentIndex, key, leftIndex, lNeighbor, rightIndex, rNeighbor);
    }
  }
}

/* -- Public -- */
Timeline::Timeline( AnyObject motion)
  : _p (new TimelinePrivate(motion, this))
{
}

Timeline::~Timeline()
{
  delete _p;
}

void Timeline::play()
{
  _p->play();
}

void Timeline::pause()
{
  _p->pause();
}

void Timeline::stop()
{
  _p->stop();
}

void Timeline::goTo(const int &pFrame)
{
  _p->goTo(pFrame);
}

void Timeline::goTo(const std::string &pFrame)
{
  _p->goTo(pFrame);
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

void Timeline::setAnimation(AnimationModel* anim)
{
  _p->setAnimation(anim);
}

void Timeline::setFrames(const std::map<int, std::string> &frames)
{
  _p->_framesFlagsMap = frames;
}

void Timeline::setFrameNames(const std::map<std::string, int> &frames)
{
  _p->_framesFlagsMapRev = frames;
}

void Timeline::waitForTimelineCompletion()
{
  _p->_executer->waitForExecuterCompletion();
}

bool Timeline::isValid() const
{
  return _p->_isValid;
}

static bool _qiregisterTimeline()
{
  ::qi::ObjectTypeBuilder<Timeline> b;
  b.advertise("play", &Timeline::play);
  b.advertise("pause", &Timeline::pause);
  b.advertise("stop", &Timeline::stop);
  b.advertise("goTo",
      static_cast<void(Timeline::*)(const int&)>(&Timeline::goTo));
  b.advertise("goTo",
      static_cast<void(Timeline::*)(const std::string&)>(&Timeline::goTo));
  b.advertise("gotoAndStop", &Timeline::gotoAndStop<std::string>);
  b.advertise("gotoAndStop", &Timeline::gotoAndStop<int>);
  b.advertise("gotoAndPlay", &Timeline::gotoAndPlay<std::string>);
  b.advertise("gotoAndPlay", &Timeline::gotoAndPlay<int>);
  b.advertise("setFPS", &Timeline::setFPS);
  b.advertise("setFrames", &Timeline::setFrames);
  b.advertise("setFrameNames", &Timeline::setFrameNames);
  b.advertise("setAnimation", &Timeline::setAnimation);
  b.advertise("waitForTimelineCompletion", &Timeline::waitForTimelineCompletion);
  b.advertise("startFlowdiagram", &Timeline::startFlowdiagram);
  b.advertise("stopFlowdiagram", &Timeline::stopFlowdiagram);
  b.advertise("onTimelineFinished", &Timeline::onTimelineFinished);
  b.registerType();
  return true;
}

static bool _qi_registration_Timeline = _qiregisterTimeline();

}
