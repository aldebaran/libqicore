/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef INPUT_P_H_
#define INPUT_P_H_

#include <alserial/alserial.h>
#include <qi/type/metamethod.hpp>
#include <qicore-compat/model/inputmodel.hpp>

namespace qi
{
  class InputModelPrivate {
    friend class InputModel;
  public:
    InputModelPrivate(boost::shared_ptr<const AL::XmlElement> elt);
    InputModelPrivate(const std::string &name,
                      const Signature &signature,
                      InputModel::InputNature nature,
                      const std::string &STMValueName,
                      bool inner,
                      const std::string &tooltip,
                      unsigned int id);

  private:
    MetaMethod _metaMethod;
    InputModel::InputNature _nature;
    std::string _stmValueName;
    bool _inner;
  };
}

#endif /* !OUTPUT_P_H_ */
