/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef ACTUATORCURVE_H_
#define ACTUATORCURVE_H_

#include <map>

#include <alserial/alserial.h>
#include <qicore-compat/api.hpp>
#include <qicore-compat/model/keymodel.hpp>
#include <boost/noncopyable.hpp>

namespace qi
{
  class ActuatorCurveModelPrivate;

  class QICORECOMPAT_API ActuatorCurveModel : private boost::noncopyable {
  public:
    enum UnitType
    {
      UnitType_Undefined = -1,
      UnitType_Degree    =  0,
      UnitType_Percent   =  1
    };

    ActuatorCurveModel(const std::string &name,
                       const std::string &actuator,
                       bool recordable,
                       bool mute,
                       UnitType unit,
                       const KeyModelMap &keys = KeyModelMap());
    ActuatorCurveModel(boost::shared_ptr<const AL::XmlElement> elt);
    virtual ~ActuatorCurveModel();

    const std::string& name() const;
    const std::string& actuator() const;
    bool recordable() const;
    bool mute() const;
    UnitType unit() const;
    const std::map<int, boost::shared_ptr<KeyModel> >& keys() const;
    int lastKeyFrame() const;

    bool isValid() const;

    void setName(const std::string &name);
    void setActuator(const std::string &actuator);
    void setRecordable(bool recordable);
    void setMute(bool mute);
    void setUnit(UnitType unit);

    void addKey(boost::shared_ptr<KeyModel> key);

  private:
    ActuatorCurveModelPrivate* _p;
  };
  typedef boost::shared_ptr<ActuatorCurveModel> ActuatorCurveModelPtr;
}

#endif /* !ACTUATORCURVE_H_ */
