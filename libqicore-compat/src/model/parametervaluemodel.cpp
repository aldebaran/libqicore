/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <qicore-compat/model/parametermodel.hpp>

#include <qicore-compat/model/parametervaluemodel.hpp>
#include "parametervaluemodel_p.hpp"

namespace qi {
  ParameterValueModelPrivate::ParameterValueModelPrivate() :
    _id(),
    _type(ParameterModel::ContentType_Error)
  {
    int defaultvalue = 0;
    _value = GenericValue(defaultvalue);
  }

  ParameterValueModelPrivate::ParameterValueModelPrivate(boost::shared_ptr<const AL::XmlElement> elt, BoxInterfaceModelPtr interface)
  {
    elt->getAttribute("id", _id);

    ParameterModelPtr param = interface->findParameter(_id);

    _type = param->getContentType();
    switch(_type)
    {
    case ParameterModel::ContentType_Bool:
      bool valueBool;
      elt->getAttribute("value", valueBool);
      _value = GenericValue(valueBool);
      break;

    case ParameterModel::ContentType_Double:
      double valueDouble;
      elt->getAttribute("value", valueDouble);
      _value = GenericValue(valueDouble);
      break;

    case ParameterModel::ContentType_Int:
      int valueInt;
      elt->getAttribute("value", valueInt);
      _value = GenericValue(valueInt);
      break;

    case ParameterModel::ContentType_Ressource:
    case ParameterModel::ContentType_String:
    {
      std::string valueString;
      elt->getAttribute("value", valueString);
      _value = GenericValue(valueString);
    }
      break;

    default:
      break;
    }
  }

  ParameterValueModel::ParameterValueModel() :
    _p(new ParameterValueModelPrivate())
  {
  }

  ParameterValueModel::ParameterValueModel(boost::shared_ptr<const AL::XmlElement> elt, BoxInterfaceModelPtr interface) :
    _p(new ParameterValueModelPrivate(elt, interface))
  {
  }

  ParameterValueModel::~ParameterValueModel()
  {
    delete _p;
  }

  int ParameterValueModel::getId() const
  {
    return _p->_id;
  }

  int ParameterValueModel::getType() const
  {
    return _p->_type;
  }

  bool ParameterValueModel::getValueBool() const
  {
    return _p->_value.to<bool>();
  }

  int ParameterValueModel::getValueInt() const
  {
    return _p->_value.toInt();
  }

  double ParameterValueModel::getValueDouble() const
  {
    return _p->_value.to<double>();
  }

  std::string ParameterValueModel::getValueString() const
  {
    return _p->_value.toString();
  }

  void ParameterValueModel::setId(int id)
  {
    _p->_id = id;
  }

  void ParameterValueModel::setValueBool(bool value)
  {
    _p->_type  = ParameterModel::ContentType_Bool;
    _p->_value = GenericValue(value);
  }

  void ParameterValueModel::setValueInt(int value)
  {
    _p->_type  = ParameterModel::ContentType_Int;
    _p->_value = GenericValue(value);
  }

  void ParameterValueModel::setValueDouble(double value)
  {
    _p->_type  = ParameterModel::ContentType_Double;
    _p->_value = GenericValue(value);
  }

  void ParameterValueModel::setValueString(const std::string& value)
  {
    _p->_type  = ParameterModel::ContentType_String;
    _p->_value = GenericValue(value);
  }
}
