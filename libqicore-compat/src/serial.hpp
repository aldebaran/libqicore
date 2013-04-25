/**
 * @author Jerome Vuarand
 * Copyright (c) Aldebaran Robotics 2009 All Rights Reserved \n
 *
 */

#pragma once

#ifndef SERIAL_H_
#define SERIAL_H_

#include "actuatorcurve.hpp"

namespace AL
{
  namespace Serial
  {
    std::string save(const qi::ActuatorCurve::Side& value);
    bool load(const std::string& str, qi::ActuatorCurve::Side& value);

    std::string save(const AL::Math::Interpolation::InterpolationType& value);
    bool load(const std::string& str, AL::Math::Interpolation::InterpolationType& value);
  }
}

#endif /* !SERIAL_H */
