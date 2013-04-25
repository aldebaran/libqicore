/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef OUTPUT_P_H_
#define OUTPUT_P_H_

#include <alserial/alserial.h>

namespace qi
{
  class OutputModelPrivate
  {
    friend class OutputModel;
  public:
    OutputModelPrivate();
    OutputModelPrivate(boost::shared_ptr<const AL::XmlElement> elt);
    OutputModelPrivate(const std::string &name,
                       int type,
                       int type_size,
                       int nature,
                       bool inner,
                       const std::string &tooltip,
                       int id);
  private:
    std::string _name;
    int _type;
    int _typeSize;
    int _nature;
    bool _inner;
    std::string _tooltip;
    int _id;
  };

}

#endif /* !OUTPUT_P_H_ */
