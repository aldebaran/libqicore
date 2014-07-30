/**
 * @author Remy Varannes
 * @author Victor Paleologue
 * @author Julien Freche
 * Aldebaran Robotics (c) 2007-2012 All Rights Reserved
 */

#ifndef TIMELINEPRIVATE_H
#define TIMELINEPRIVATE_H

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/bind.hpp>

#include <alvalue/alvalue.h>
#include <alproxies/almemoryproxy.h>
#include <alproxies/almotionproxy.h>

#include <qi/anyobject.hpp>
#include <qicore-compat/timeline.hpp>
#include <qicore-compat/model/animationmodel.hpp>
#include <qicore-compat/model/actuatorcurvemodel.hpp>
#include <qicore-compat/model/keymodel.hpp>

#include "asyncexecuter.hpp"

namespace AL
{
  class XmlElement;
}

namespace qi
{

class TimelinePrivate
{
public:
  friend class Timeline;

  TimelinePrivate(AnyObject motion, Timeline* timeline);
  virtual ~TimelinePrivate();

  /**
   * loadFromXml.
   * load data about a timeline from a tinyXml element.
   * @param elt Pointer to data about the timeline in xml.
   */
  void loadFromXml(boost::shared_ptr<const AL::XmlElement> elt);

  void play();
  void pause();
  void stop(bool join = true);
  void goTo(int pFrame);
  void goTo(const std::string& pFrame);
  int getSize() const;
  int getFPS() const;
  void setFPS(int pFps);
  void setAnimation(AnimationModel *anim);

  bool getEnabled() const;
  int getCurrentFrame() const;
  void setCurrentFrame(int pFrame);

  /**
   * setName will also insert an empty frame number in the STM, so Choregraphe
   * does not complaing about not finding a value.
   */
  std::string getName() const;

private:
  bool update();

  void killMotionOrders();
  void updateFrameInSTM();

  /**
   * Call this method at each update.
   * Returns true if success, false otherwise.
   * If false is returned, then we should not go to next keyframe...
   */
  bool executeCurveMotionCommand();

  /**
   * Interpolation computed for when we do gotos while stopped.
   */
  bool singleInterpolationCommand(int currentFrame);

  /**
   * executeCurveMotionCommand will call this method build all objects required to go further
   */
  bool prepareInterpolationCommand(int startFrame);

  /**
   * prepareInterpolationCommand will call this method, that will check for resources and then send command if everything is ok.
   */
  bool sendInterpolationCommand(const std::vector<std::string>& names, const AL::ALValue& times, const AL::ALValue& keys);
  void killTimer();
  void TimerLoop(int interval);

  static void getNeighborKeysOf(const ActuatorCurveModel& curve,
                                const int indexKey,
                                int& indexLeftKey,
                                KeyModelPtr& leftKey,
                                int& indexRightKey,
                                KeyModelPtr& rightKey);
  static float getInterpolatedValue(const ActuatorCurveModel& curve,
                                    const int& indexKey,
                                    const float& valueIncrementLimit);
  static float getMotionValue(const ActuatorCurveModel& curve, float value);
  static void rebuildBezierAutoTangents(ActuatorCurveModelPtr curve);
  static bool updateBezierAutoTangents(const int &currentIndex, KeyModelPtr key, const int& leftIndex, CKeyModelPtr& lNeighbor, const int& rightIndex, CKeyModelPtr& rNeighbor);
  void startFlowdiagram(int index);
  void startFlowdiagramAsync(int index);
  void stopFlowdiagram();

  asyncExecuter*                        _executer;
  boost::shared_ptr<AL::ALMemoryProxy>  _memoryProxy;
  boost::shared_ptr<AL::ALMotionProxy>  _motionProxy;
  int                                   _fps;
  std::vector<ActuatorCurveModelPtr>  _actuatorCurves;
  bool                                  _enabled;
  int                                   _currentFrame;
  int                                   _startFrame;
  /**
   * When this frame is encountered, exit the timeline.
   */
  int                                   _endFrame;
  int                                   _lastFrame;
  /**
   * The current 'angleInterpolationBezier' command.\n
   * Equals -1 if no interpolation at the moment.
   */
  int                                   _currentDoInterpolationMoveOrderId;
  /**
   * The name of the variable written in the stm to tell
   * choregraphe which frame is currently playing.\n
   */
  std::string                           _name;
  AnimationModel::MotionResourcesHandler _resourcesAcquisition;
  mutable boost::recursive_mutex        _methodMonitor;

  std::map<int, std::string>            _framesFlagsMap;
  std::map<std::string, int>            _framesFlagsMapRev;
  AnimationModel* _animation;
  Timeline* _timeline;
  bool _isValid;

  std::vector<boost::thread*> _flowdiagrams;
};

}

#endif /* !TIMELINEPRIVATE_H */
