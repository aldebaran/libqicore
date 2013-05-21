/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef PARAMETER_H_
#define PARAMETER_H_

#include <alserial/alserial.h>
#include <qitype/metaproperty.hpp>
#include <qitype/genericvalue.hpp>
#include <qicore-compat/api.hpp>

namespace qi
{
  class ParameterModelPrivate;
  class ChoiceModel;
  class ParameterValueModel;

  class QICORECOMPAT_API ParameterModel
  {
  public:
    static const Signature &Resource;

    ParameterModel(const std::string &name,
                   AutoGenericValuePtr defaultValue,
                   bool inheritsFromParent,
                   bool customChoice,
                   bool password,
                   const std::string &tooltip,
                   int id,
                   bool resource = false);
    ParameterModel(const std::string &name,
                   AutoGenericValuePtr defaultValue,
                   AutoGenericValuePtr min,
                   AutoGenericValuePtr max,
                   bool inheritsFromParent,
                   bool customChoice,
                   bool password,
                   const std::string &tooltip,
                   int id);
    ParameterModel(boost::shared_ptr<const AL::XmlElement> elt);
    virtual ~ParameterModel();

    const MetaProperty& metaProperty() const;
    bool getInheritsFromParent() const;
    GenericValuePtr getDefaultValue() const;
    GenericValuePtr getMin() const;
    GenericValuePtr getMax() const;
    bool getCustomChoice() const;
    bool getPassword() const;
    const std::string& getTooltip() const;

    void setMetaProperty(unsigned int id, const std::string &name, const Signature &sig);
    void setInheritsFromParent(bool inheritsFromParent);
    bool setValue(AutoGenericValuePtr value);
    bool setValue(AutoGenericValuePtr value, AutoGenericValuePtr min, AutoGenericValuePtr max);
    void setCustomChoice(bool custom_choice);
    void setPassword(bool password);
    void setTooltip(const std::string& tooltip);

    bool addChoice(boost::shared_ptr<ChoiceModel> choice);
    const std::list<boost::shared_ptr<ChoiceModel> >& getChoices() const;

    bool checkInterval(boost::shared_ptr<ParameterValueModel> value) const;

    bool isValid() const;

  private:
    QI_DISALLOW_COPY_AND_ASSIGN(ParameterModel);
    ParameterModelPrivate *_p;
  };
  typedef boost::shared_ptr<ParameterModel> ParameterModelPtr;
}
#endif /* !PARAMETER_H_ */
