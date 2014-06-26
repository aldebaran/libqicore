/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef PARAMETERVALUE_H_
#define PARAMETERVALUE_H_

#include <qicore-compat/api.hpp>
#include <alserial/alserial.h>
#include <qi/signature.hpp>
#include <qi/anyvalue.hpp>

namespace qi {
  class ParameterValueModelPrivate;
  class BoxInterfaceModel;

  class QICORECOMPAT_API ParameterValueModel : private boost::noncopyable {
  public:
    ParameterValueModel(int id, AutoAnyReference value);
    ParameterValueModel(boost::shared_ptr<const AL::XmlElement> elt, boost::shared_ptr<BoxInterfaceModel> interface);

    virtual ~ParameterValueModel();

    int id()                  const;
    AnyReference value() const;

    void setId(int id);
    void setValueDefault(AutoAnyReference value);

    bool isValid() const;

  private:
    ParameterValueModelPrivate* _p;
  };
  typedef boost::shared_ptr<ParameterValueModel> ParameterValueModelPtr;
}

#endif /* !PARAMETERVALUE_H_ */
