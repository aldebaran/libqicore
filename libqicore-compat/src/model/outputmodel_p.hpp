/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef OUTPUT_P_H_
#define OUTPUT_P_H_

#include <alserial/alserial.h>
#include <qi/signature.hpp>
#include <qi/type/metasignal.hpp>

namespace qi
{
  class OutputModelPrivate
  {
    friend class OutputModel;
  public:
    OutputModelPrivate(boost::shared_ptr<const AL::XmlElement> elt);
    OutputModelPrivate(const std::string &name,
                       const Signature &signature,
                       int nature,
                       bool inner,
                       const std::string &tooltip,
                       unsigned int id);
  private:
    MetaSignal _metaSignal;
    int _nature;
    bool _inner;
    std::string _tooltip;
  };

}

#endif /* !OUTPUT_P_H_ */
