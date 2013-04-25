/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef OUTPUT_H_
#define OUTPUT_H_

#include <string>
#include <alserial/alserial.h>
#include <qicore-compat/api.hpp>

namespace qi
{
  class OutputModelPrivate;

  class QICORECOMPAT_API OutputModel {
  public:
    enum OutputType {
      OutputType_Dynamic = 0,
      OutputType_Bang    = 1,
      OutputType_Number  = 2
    };

    enum OutputNature {
      OutputNature_Undef    = 0,
      OutputNature_Stopped  = 1,
      OutputNature_Punctual = 2
    };

    OutputModel(boost::shared_ptr<const AL::XmlElement> elt);
    OutputModel(const std::string &name,
                int type,
                int type_size,
                int nature,
                bool inner,
                const std::string &tooltip,
                int id);
    virtual ~OutputModel();

    const std::string& getName() const;
    int getType() const;
    int getTypeSize() const;
    int getNature() const;
    bool getInner() const;
    const std::string& getTooltip() const;
    int getId() const;

    void setName(const std::string& name);
    void setType(int type);
    void setTypeSize(int type_size);
    void setNature(int nature);
    void setInner(bool inner);
    void setTooltip(const std::string& tooltip);
    void setId(int id);

  private:
    QI_DISALLOW_COPY_AND_ASSIGN(OutputModel);
    OutputModelPrivate* _p;
  };
  typedef boost::shared_ptr<OutputModel> OutputModelPtr;
}

#endif /* !OUTPUT_H_ */
