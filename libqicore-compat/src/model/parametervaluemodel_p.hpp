/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef PARAMETERVALUE_P_H_
#define PARAMETERVALUE_P_H_

#include <alserial/alserial.h>
#include <qitype/genericvalue.hpp>

#include <qicore-compat/model/parametermodel.hpp>
#include <qicore-compat/model/boxinterfacemodel.hpp>

namespace qi {
  class ParameterValueModelPrivate {
    friend class ParameterValueModel;
  public:
    ParameterValueModelPrivate(int id, AutoGenericValuePtr value);
    ParameterValueModelPrivate(boost::shared_ptr<const AL::XmlElement> elt, BoxInterfaceModelPtr interface);

  private:
    int _id;
    GenericValuePtr _value;
    bool _isValid;
  };
}

# endif /* !PARAMETERVALUE_P_H_ */
