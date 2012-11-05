/**
 * @author Emmanuel Pot
 * Aldebaran Robotics (c) 2008 All Rights Reserved
 */

#include <sstream>

// .:: Personal headers ::
#include "xml_utils.hpp"

const std::string XmlUtils::fChoregrapheProjectBeacon = "ChoregrapheProject";
const std::string XmlUtils::fChoregrapheBoxBeacon   = "ChoregrapheBox";
const std::string XmlUtils::fBehaviorKeyframeBeacon = "BehaviorKeyframe";
const std::string XmlUtils::fBehaviorLayerBeacon    = "BehaviorLayer";
const std::string XmlUtils::fBoxBeacon              = "Box";
const std::string XmlUtils::fDiagramBeacon          = "Diagram";
const std::string XmlUtils::fInputBeacon            = "Input";
const std::string XmlUtils::fLinkBeacon             = "Link";
const std::string XmlUtils::fOutputBeacon           = "Output";
const std::string XmlUtils::fParameterBeacon        = "Parameter";
const std::string XmlUtils::fResourceBeacon        = "Resource";
const std::string XmlUtils::fMotionKeyframeBeacon   = "MotionKeyframe";
const std::string XmlUtils::fMotionLayerBeacon      = "MotionLayer";
const std::string XmlUtils::fTimelineBeacon         = "Timeline";
const std::string XmlUtils::fActuatorCurveBeacon    = "ActuatorCurve";
const std::string XmlUtils::fActuatorListBeacon     = "ActuatorList";
const std::string XmlUtils::fAudioFileBeacon        = "Waveforms";

namespace AL
{
  std::string Serial::save(const ActuatorCurve::Side& value)
  {
    std::string str;
    switch(value)
    {
#define ENUM_ENTRY(value, name) case value: str = name; break;
  ENUM_ENTRY( ActuatorCurve::LEFT,  "left" )
  ENUM_ENTRY( ActuatorCurve::RIGHT, "right" )
#undef ENUM_ENTRY
    default:
      ;
    }
    return str;
  }

  bool Serial::load(const std::string& str, ActuatorCurve::Side& value)
  {
    if (false);
#define ENUM_ENTRY(val, name) else if (str==name) value = val;
  ENUM_ENTRY( ActuatorCurve::LEFT,  "left" )
  ENUM_ENTRY( ActuatorCurve::RIGHT, "right" )
#undef ENUM_ENTRY
    else
      return false;
    return true;
  }

  std::string Serial::save(const AL::Math::Interpolation::InterpolationType& value)
  {
    std::string str;
    switch(value)
    {
#define ENUM_ENTRY(value, name) case value: str = name; break;
  ENUM_ENTRY( AL::Math::Interpolation::CONSTANT, "constant" )
  ENUM_ENTRY( AL::Math::Interpolation::LINEAR, "linear" )
  ENUM_ENTRY( AL::Math::Interpolation::BEZIER, "bezier" )
  // in auto mode, bezier are still saved in the file, so don't treat specially here
  // :WARN: if the params are removed in the future (since they are redundant), we will have to recompute them here
  //ENUM_ENTRY( AL::Math::Interpolation::BEZIER, "bezier auto" )
#undef ENUM_ENTRY
    default:
      ;
    }
    return str;
  }

  bool Serial::load(const std::string& str, AL::Math::Interpolation::InterpolationType& value)
  {
    if (false);
#define ENUM_ENTRY(val, name) else if (str==name) value = val;
  ENUM_ENTRY( AL::Math::Interpolation::CONSTANT, "constant" )
  ENUM_ENTRY( AL::Math::Interpolation::LINEAR, "linear" )
  ENUM_ENTRY( AL::Math::Interpolation::BEZIER, "bezier" )
  // in auto mode, bezier are still saved in the file, so don't treat specially here
  // :WARN: if the params are removed in the future (since they are redundant), we will have to recompute them here
  ENUM_ENTRY( AL::Math::Interpolation::BEZIER_AUTO, "bezier auto" )

  // below is compat, feel free to remove
  ENUM_ENTRY( AL::Math::Interpolation::LINEAR, "none" )
  ENUM_ENTRY( AL::Math::Interpolation::CONSTANT, "piecewise constant" )
  ENUM_ENTRY( AL::Math::Interpolation::BEZIER, "bezier sym" )
  ENUM_ENTRY( AL::Math::Interpolation::BEZIER, "bezier smooth" )
  ENUM_ENTRY( AL::Math::Interpolation::BEZIER, "bezier harsh" )
#undef ENUM_ENTRY
    else
      return false;
    return true;
  }
}
