/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef PARAMETER_P_H_
# define PARAMETER_P_H_

#include <qicore-compat/model/choicemodel.hpp>
#include <qitype/genericvalue.hpp>
#include <alserial/alserial.h>

namespace qi
{
  class ParameterModelPrivate
  {
    friend class ParameterModel;
  public:
    ParameterModelPrivate();

    ParameterModelPrivate(boost::shared_ptr<const AL::XmlElement> elt);

    template <typename T>
    bool inInterval(T value, T min, T max);

  private:
    std::string _name;
    bool _inheritsFromParent;
    ParameterModel::ContentType _contentType;
    GenericValue _defaultValue;
    GenericValue _min;
    GenericValue                          _max;
    bool                                  _customChoice;
    bool                                  _password;
    std::string                           _tooltip;
    int                                   _id;
    std::list<boost::shared_ptr<ChoiceModel> > _choices;
  };

  template <typename T>
  bool ParameterModelPrivate::inInterval(T value, T min, T max)
  {
    if(value < min || value > max)
      return false;

    return true;
  }
}
#endif /* !PARAMETER_P_H_ */
