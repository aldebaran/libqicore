/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <qicore-compat/model/parametermodel.hpp>

#include <qicore-compat/model/parametervaluemodel.hpp>
#include "parametervaluemodel_p.hpp"

#include <qi/log.hpp>
qiLogCategory("QiCore-Compat.ParameterValueModel");

namespace qi {
  ParameterValueModelPrivate::ParameterValueModelPrivate(int id, AutoAnyReference value) :
    _id(id),
    _value(value.clone()),
    _isValid(true)
  {
  }

  ParameterValueModelPrivate::ParameterValueModelPrivate(boost::shared_ptr<const AL::XmlElement> elt, BoxInterfaceModelPtr interface)
  {
    elt->getAttribute("id", _id);

    ParameterModelPtr param = interface->findParameter(_id);
    if(!param)
    {
      qiLogError() << "Parameter not found." << std::endl;
      _isValid = false;
      return;
    }

    Signature type(param->metaProperty().signature());
    _isValid = true;
    if(type.isConvertibleTo(Signature::fromType(Signature::Type_Bool)) == 1.0f)
    {
      bool valueBool;
      elt->getAttribute("value", valueBool);
      _value = AnyValue(valueBool).clone();
    }
    else if(type.isConvertibleTo(Signature::fromType(Signature::Type_Int32)) == 1.0f)
    {
      int valueInt;
      elt->getAttribute("value", valueInt);
      _value = AnyValue(valueInt).clone();
    }
    else if(type.isConvertibleTo(Signature::fromType(Signature::Type_Double)) == 1.0f)
    {
      double valueDouble;
      elt->getAttribute("value", valueDouble);
      _value = AnyValue(valueDouble).clone();
    }
    else if(type.isConvertibleTo(Signature::fromType(Signature::Type_String)) == 1.0f)
    {
      std::string valueString;
      elt->getAttribute("value", valueString);
      _value = AnyValue(valueString).clone();
    }
    else
    {
      qiLogError() << "Bad type : " << type.toString();
      _isValid = false;
    }
  }

  ParameterValueModel::ParameterValueModel(int id, AutoAnyReference value) :
    _p(new ParameterValueModelPrivate(id, value))
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

  int ParameterValueModel::id() const
  {
    return _p->_id;
  }

  AnyReference ParameterValueModel::value() const
  {
    return _p->_value;
  }

  void ParameterValueModel::setId(int id)
  {
    _p->_id = id;
  }

  void ParameterValueModel::setValueDefault(AutoAnyReference value)
  {
    _p->_value = value.clone();
  }

  bool ParameterValueModel::isValid() const
  {
    return _p->_isValid;
  }
}
