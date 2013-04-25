/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef CHOICE_H_
#define CHOICE_H_

#include <alserial/alserial.h>

#include <qicore-compat/api.hpp>
#include <qicore-compat/model/parametermodel.hpp>

namespace qi
{
  class ChoiceModelPrivate;

  class QICORECOMPAT_API ChoiceModel {
  public:
    explicit ChoiceModel(bool value);
    explicit ChoiceModel(int value);
    explicit ChoiceModel(double value);
    explicit ChoiceModel(const std::string &value);
    ChoiceModel(boost::shared_ptr<const AL::XmlElement> elt,  ParameterModel::ContentType type);
    virtual ~ChoiceModel();

    bool getValueBool();
    int getValueInt();
    double getValueDouble();
    std::string getValueString();
    int getType();

    void setValueBool(bool value);
    void setValueInt(int value);
    void setValueDouble(double value);
    void setValueString(const std::string& value);

  private:
    QI_DISALLOW_COPY_AND_ASSIGN(ChoiceModel);
    ChoiceModelPrivate* _p;
  };
  typedef boost::shared_ptr<ChoiceModel> ChoiceModelPtr;
}

#endif/* !CHOICE_H_ */
