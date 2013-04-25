/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef INPUT_P_H_
#define INPUT_P_H_

#include <alserial/alserial.h>
#include <qicore-compat/model/inputmodel.hpp>

namespace qi
{
  class InputModelPrivate {
    friend class InputModel;
  public:
    InputModelPrivate();
    InputModelPrivate(boost::shared_ptr<const AL::XmlElement> elt);
    InputModelPrivate(const std::string &name,
                      InputModel::InputType type,
                      int type_size,
                      InputModel::InputNature nature,
                      const std::string &STMValueName,
                      bool inner,
                      const std::string &tooltip,
                      int id);

  private:
    std::string _name;
    InputModel::InputType _type;
    int _typeSize;
    InputModel::InputNature _nature;
    std::string _stmValueName;
    bool _inner;
    std::string _tooltip;
    int _id;
  };
}

#endif /* !OUTPUT_P_H_ */
