/**
 * @author Jerome Vuarand
 * Copyright (c) Aldebaran Robotics 2009 All Rights Reserved \n
 *
 */

#include "serial.hpp"

namespace AL
{
  std::string Serial::save(const qi::ActuatorCurve::Side& value)
  {
    std::string str;
    switch(value)
    {
#define ENUM_ENTRY(value, name) case value: str = name; break;
  ENUM_ENTRY( qi::ActuatorCurve::LEFT,  "left" )
  ENUM_ENTRY( qi::ActuatorCurve::RIGHT, "right" )
#undef ENUM_ENTRY
    default:
      ;
    }
    return str;
  }

  bool Serial::load(const std::string& str, qi::ActuatorCurve::Side& value)
  {
    if (false);
#define ENUM_ENTRY(val, name) else if (str==name) value = val;
  ENUM_ENTRY( qi::ActuatorCurve::LEFT,  "left" )
  ENUM_ENTRY( qi::ActuatorCurve::RIGHT, "right" )
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
