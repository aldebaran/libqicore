/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef OUTPUT_H_
#define OUTPUT_H_

#include <string>
#include <alserial/alserial.h>

#include <qi/type/metasignal.hpp>

#include <qicore-compat/api.hpp>

namespace qi
{
  class OutputModelPrivate;

  class QICORECOMPAT_API OutputModel : private boost::noncopyable {
  public:
    enum OutputNature {
      OutputNature_Undef    = 0,
      OutputNature_Stopped  = 1,
      OutputNature_Punctual = 2
    };

    OutputModel(boost::shared_ptr<const AL::XmlElement> elt);
    OutputModel(const std::string &name,
                const Signature &signature,
                int nature,
                bool inner,
                const std::string &tooltip,
                int id);
    virtual ~OutputModel();

    const MetaSignal& metaSignal() const;
    int nature() const;
    bool inner() const;
    const std::string& tooltip() const;

    void setMetaSignal(const std::string &name,
                       const Signature &signature,
                       unsigned int id);
    void setNature(int nature);
    void setInner(bool inner);
    void setTooltip(const std::string& tooltip);

  private:
    OutputModelPrivate* _p;
  };
  typedef boost::shared_ptr<OutputModel> OutputModelPtr;
  typedef std::map<int, OutputModelPtr> OutputModelMap;
}

#endif /* !OUTPUT_H_ */
