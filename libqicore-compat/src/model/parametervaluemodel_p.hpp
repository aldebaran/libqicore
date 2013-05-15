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
    ParameterValueModelPrivate();
    ParameterValueModelPrivate(boost::shared_ptr<const AL::XmlElement> elt, BoxInterfaceModelPtr interface);

  private:
    int _id;
    ParameterModel::ContentType _type;
    GenericValue _value;
  };
}

# endif /* !PARAMETERVALUE_P_H_ */
