/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef CHOICE_H_
#define CHOICE_H_

#include <alserial/alserial.h>
#include <qi/anyvalue.hpp>

#include <qicore-compat/api.hpp>
#include <qicore-compat/model/parametermodel.hpp>

namespace qi
{
  class ChoiceModelPrivate;

  class QICORECOMPAT_API ChoiceModel : private boost::noncopyable {
  public:
    explicit ChoiceModel(AutoAnyReference value);
    ChoiceModel(boost::shared_ptr<const AL::XmlElement> elt, const Signature &type);
    virtual ~ChoiceModel();

    AnyReference value() const;

    void setValue(AutoAnyReference value);

  private:
    ChoiceModelPrivate* _p;
  };
  typedef boost::shared_ptr<ChoiceModel> ChoiceModelPtr;
}

#endif/* !CHOICE_H_ */
