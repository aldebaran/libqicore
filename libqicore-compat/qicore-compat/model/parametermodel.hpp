/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef PARAMETER_H_
#define PARAMETER_H_

#include <alserial/alserial.h>
#include <qicore-compat/api.hpp>

namespace qi
{
  class ParameterModelPrivate;
  class ChoiceModel;

  class QICORECOMPAT_API ParameterModel
  {
  public:
    enum ContentType {
      ContentType_Bool      = 0,
      ContentType_Int       = 1,
      ContentType_Double    = 2,
      ContentType_String    = 3,
      ContentType_Ressource = 4,
      ContentType_Error    = -1
    };

    ParameterModel();
    ParameterModel(boost::shared_ptr<const AL::XmlElement> elt);
    virtual ~ParameterModel();

    const std::string& getName() const;
    bool getInheritsFromParent() const;
    ContentType getContentType() const;
    bool getDefaultValueBool() const;
    int getDefaultValueInt() const;
    int getMinInt() const;
    int getMaxInt() const;
    double getDefaultValueDouble() const;
    double getMinDouble() const;
    double getMaxDouble() const;
    std::string getDefaultValueString() const;
    std::string getDefaultValueRessource() const;
    bool getCustomChoice() const;
    bool getPassword() const;
    const std::string& getTooltip() const;
    int getId() const;

    void setName(const std::string& name);
    void setInheritsFromParent(bool inherits_from_parent);
    void setDefaultValueBool(bool default_value);
    bool setDefaultValueInt(int default_value);
    bool setMinInt(int min);
    bool setMaxInt(int max);
    bool setDefaultValueDouble(double default_value);
    bool setMinDouble(double min);
    bool setMaxDouble(double max);
    void setDefaultValueString(const std::string& default_value);
    void setDefaultValueRessource(const std::string& path);
    void setCustomChoice(bool custom_choice);
    void setPassword(bool password);
    void setTooltip(const std::string& tooltip);
    void setId(int id);

    bool addChoice(boost::shared_ptr<ChoiceModel> choice);
    const std::list<boost::shared_ptr<ChoiceModel> >& getChoices() const;

  private:
    QI_DISALLOW_COPY_AND_ASSIGN(ParameterModel);
    ParameterModelPrivate *_p;
  };
  typedef boost::shared_ptr<ParameterModel> ParameterModelPtr;
}
#endif /* !PARAMETER_H_ */
