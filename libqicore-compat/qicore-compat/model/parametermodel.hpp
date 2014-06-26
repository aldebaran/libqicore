/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef PARAMETER_H_
#define PARAMETER_H_

#include <alserial/alserial.h>
#include <qi/type/metaproperty.hpp>
#include <qi/anyvalue.hpp>
#include <qicore-compat/api.hpp>

namespace qi
{
  class ParameterModelPrivate;
  class ChoiceModel;
  class ParameterValueModel;

  class QICORECOMPAT_API ParameterModel : private boost::noncopyable
  {
  public:
    ParameterModel(const std::string &name,
                   AutoAnyReference defaultValue,
                   bool inheritsFromParent,
                   bool customChoice,
                   bool password,
                   const std::string &tooltip,
                   int id,
                   bool resource = false);
    ParameterModel(const std::string &name,
                   AutoAnyReference defaultValue,
                   AutoAnyReference min,
                   AutoAnyReference max,
                   bool inheritsFromParent,
                   bool customChoice,
                   bool password,
                   const std::string &tooltip,
                   int id);
    ParameterModel(boost::shared_ptr<const AL::XmlElement> elt);
    virtual ~ParameterModel();

    const MetaProperty& metaProperty() const;
    bool inheritsFromParent() const;
    AnyReference defaultValue() const;
    AnyReference min() const;
    AnyReference max() const;
    bool customChoice() const;
    bool password() const;
    const std::string& tooltip() const;

    void setMetaProperty(unsigned int id, const std::string &name, const Signature &sig);
    void setInheritsFromParent(bool inheritsFromParent);
    bool setValue(AutoAnyReference value);
    bool setValue(AutoAnyReference value, AutoAnyReference min, AutoAnyReference max);
    void setCustomChoice(bool custom_choice);
    void setPassword(bool password);
    void setTooltip(const std::string& tooltip);

    bool addChoice(boost::shared_ptr<ChoiceModel> choice);
    const std::list<boost::shared_ptr<ChoiceModel> >& getChoices() const;

    bool checkInterval(boost::shared_ptr<ParameterValueModel> value) const;

    bool isValid() const;

    static const Signature &signatureRessource();

  private:
    ParameterModelPrivate *_p;
  };
  typedef boost::shared_ptr<ParameterModel> ParameterModelPtr;
}
#endif /* !PARAMETER_H_ */
