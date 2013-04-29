/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <qicore-compat/model/actuatorcurvemodel.hpp>
#include "xmlutils.hpp"
#include "actuatorcurvemodel_p.hpp"

#include <alerror/alerror.h>
#include <almath/tools/altrigonometry.h>
#include <almathinternal/interpolations/alinterpolation.h>

#include "math.h"
#include <limits>

namespace qi
{
  ActuatorCurveModelPrivate::ActuatorCurveModelPrivate(const std::string &name, const std::string &actuator, bool recordable, bool mute, int unit, const std::list<KeyModelPtr> &keys) :
    _name(name),
    _actuator(actuator),
    _recordable(recordable),
    _mute(mute),
    _unit(unit),
    _keys(keys),
    _isValid(true)
  {

  }

  ActuatorCurveModelPrivate::ActuatorCurveModelPrivate(boost::shared_ptr<const AL::XmlElement> elt)
  {
    std::string unit;
    elt->getAttribute("name",       _name);
    elt->getAttribute("actuator",   _actuator);
    elt->getAttribute("recordable", _recordable);
    elt->getAttribute("mute",       _mute);
    elt->getAttribute("unit",       unit);

    if(unit == "None")
    {
      _unit = ActuatorCurveModel::UnitType_Undefined;
    }
    else
    {
      elt->getAttribute("unit",       _unit);
    }

    AL::XmlElement::CList keys = elt->children("Key", "");

    _keys = XmlUtils::constructObjects<KeyModel>(keys);
    _isValid = XmlUtils::verifyObjects<KeyModel>(_keys);

  }

  ActuatorCurveModel::ActuatorCurveModel(const std::string &name, const std::string &actuator, bool recordable, bool mute, UnitType unit, const std::list<boost::shared_ptr<KeyModel> > &keys) :
    _p(new ActuatorCurveModelPrivate(name, actuator, recordable, mute, unit, keys))
  {

  }

  ActuatorCurveModel::ActuatorCurveModel(boost::shared_ptr<const AL::XmlElement> elt) :
    _p(new ActuatorCurveModelPrivate(elt))
  {
  }

  ActuatorCurveModel::~ActuatorCurveModel()
  {
    delete _p;
  }

  const std::string& ActuatorCurveModel::getName() const
  {
    return _p->_name;
  }

  const std::string& ActuatorCurveModel::getActuator() const
  {
    return _p->_actuator;
  }

  bool ActuatorCurveModel::getRecordable() const
  {
    return _p->_recordable;
  }

  bool ActuatorCurveModel::getMute() const
  {
    return _p->_mute;
  }

  ActuatorCurveModel::UnitType ActuatorCurveModel::getUnit() const
  {
    return static_cast<ActuatorCurveModel::UnitType>( _p->_unit );
  }

  const std::list<KeyModelPtr>& ActuatorCurveModel::getKeys() const
  {
    return _p->_keys;
  }

  bool ActuatorCurveModel::isValid() const
  {
    return _p->_isValid;
  }

  void ActuatorCurveModel::setName(const std::string& name)
  {
    _p->_name = name;
  }

  void ActuatorCurveModel::setActuator(const std::string& actuator)
  {
    _p->_actuator = actuator;
  }

  void ActuatorCurveModel::setRecordable(bool recordable)
  {
    _p->_recordable = recordable;
  }

  void ActuatorCurveModel::setMute(bool mute)
  {
    _p->_mute = mute;
  }

  void ActuatorCurveModel::setUnit(UnitType unit)
  {
    _p->_unit = unit;
  }

  void ActuatorCurveModel::addKey(KeyModelPtr key)
  {
    _p->_keys.push_front(key);
  }
}
