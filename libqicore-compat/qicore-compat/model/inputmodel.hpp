/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef INPUT_H_
#define INPUT_H_

#include <alserial/alserial.h>
#include <qicore-compat/api.hpp>

namespace qi
{
  class InputModelPrivate;

  class QICORECOMPAT_API InputModel {
  public:
    enum InputType {
      InputType_Dynamic = 0,
      InputType_Bang    = 1,
      InputType_Number  = 2,
      InputType_String  = 3,
      InputType_Error   = -1
    };

    enum InputNature  {
      InputNature_OnLoad   = 0,
      InputNature_Event    = 1,
      InputNature_OnStart  = 2,
      InputNature_OnStop   = 3,
      InputNature_STMValue = 4,
      InputNature_Error    = -1
    };

    InputModel(boost::shared_ptr<const AL::XmlElement> elt);
    InputModel(const std::string &name,
               InputType type,
               int type_size,
               InputNature nature,
               bool inner,
               const std::string &tooltip,
               int id);
    InputModel(const std::string &name,
               InputType type,
               int type_size,
               const std::string &stm_value_name,
               bool inner,
               const std::string &tooltip,
               int id);

    virtual ~InputModel();

    const std::string& getName() const;
    InputType getType() const;
    int getTypeSize() const;
    InputNature getNature() const;
    const std::string& getSTMValueName() const;
    bool getInner() const;
    const std::string& getTooltip() const;
    int getId() const;

    void setName(const std::string& name);
    void setType(InputType type);
    void setTypeSize(int type_size);
    void setNature(InputNature nature);
    void setSTMValueName(const std::string& stm_value_name);
    void setInner(bool inner);
    void setTooltip(const std::string& tooltip);
    void setId(int id);

  private:
    QI_DISALLOW_COPY_AND_ASSIGN(InputModel);
    InputModelPrivate* _p;
  };
  typedef boost::shared_ptr<InputModel> InputModelPtr;
}

#endif /* !INPUT_H_ */
