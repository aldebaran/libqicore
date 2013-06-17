/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef ACTUATORCURVE_P_H_
#define ACTUATORCURVE_P_H_

#include <map>

#include <alserial/alserial.h>
#include <qicore-compat/model/keymodel.hpp>

namespace qi
{
  class ActuatorCurveModelPrivate {
    friend class ActuatorCurveModel;

  public:
    ActuatorCurveModelPrivate(const std::string &name,
                              const std::string &actuator,
                              bool recordable,
                              bool mute,
                              int unit,
                              const KeyModelMap &keys);
    ActuatorCurveModelPrivate(boost::shared_ptr<const AL::XmlElement> elt);

  private:
    std::string _name;
    std::string _actuator;
    bool _recordable;
    bool _mute;
    int _unit;
    std::map<int, KeyModelPtr> _keys;
    bool _isValid;
    int _lastKeyFrame;
  };
}

#endif /* !ACTUATIRCURVE_P_H_ */
