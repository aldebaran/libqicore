/**
 * @author Emmanuel Pot
 * Copyright (c) Aldebaran Robotics 2008 All Rights Reserved
 */

#ifndef XML_UTILS_HH
#define XML_UTILS_HH

// .:: System headers ::

#include <string>
#include "actuatorcurve.hpp"

class XmlUtils
{
public:
  static const std::string fChoregrapheProjectBeacon;
  static const std::string fChoregrapheBoxBeacon;
  static const std::string fBehaviorKeyframeBeacon;
  static const std::string fBehaviorLayerBeacon;
  static const std::string fBoxBeacon;
  static const std::string fDiagramBeacon;
  static const std::string fInputBeacon;
  static const std::string fLinkBeacon;
  static const std::string fOutputBeacon;
  static const std::string fParameterBeacon;
  static const std::string fResourceBeacon;
  static const std::string fMotionKeyframeBeacon;
  static const std::string fMotionLayerBeacon;
  static const std::string fTimelineBeacon;
  static const std::string fActuatorCurveBeacon;
  static const std::string fActuatorListBeacon;
  static const std::string fAudioFileBeacon;
};

namespace AL
{
  namespace Serial
  {
    std::string save(const ActuatorCurve::Side& value);
    bool load(const std::string& str, ActuatorCurve::Side& value);

    std::string save(const AL::Math::Interpolation::InterpolationType& value);
    bool load(const std::string& str, AL::Math::Interpolation::InterpolationType& value);
  }
}

#endif // !XML_UTILS_HH
