/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef INPUT_H_
#define INPUT_H_

#include <alserial/alserial.h>
#include <qi/type/metamethod.hpp>
#include <qicore-compat/api.hpp>

namespace qi
{
  class InputModelPrivate;

  class QICORECOMPAT_API InputModel : private boost::noncopyable {
  public:
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
               const Signature &signature,
               InputNature nature,
               bool inner,
               const std::string &tooltip,
               int id);
    InputModel(const std::string &name,
               const Signature &signature,
               const std::string &stm_value_name,
               bool inner,
               const std::string &tooltip,
               int id);

    virtual ~InputModel();


    const MetaMethod& metaMethod() const;
    InputNature nature() const;
    const std::string& stmValueName() const;
    bool inner() const;


    void setMetaMethod(const std::string &name,
                       const Signature &signature,
                       const std::string &tooltip,
                       unsigned int id);
    void setNature(InputNature nature);
    void setSTMValueName(const std::string& stm_value_name);
    void setInner(bool inner);

  private:
    InputModelPrivate* _p;
  };
  typedef boost::shared_ptr<InputModel> InputModelPtr;
  typedef std::map<int, InputModelPtr> InputModelMap;
}

#endif /* !INPUT_H_ */
