/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef LINK_H_
#define LINK_H_

#include <qicore-compat/api.hpp>
#include <alserial/alserial.h>

namespace qi {
  class LinkModelPrivate;

  class QICORECOMPAT_API LinkModel {
  public:
    LinkModel(int inputtowner, int indexofinput, int outputtowner, int indexofoutput);
    LinkModel(boost::shared_ptr<const AL::XmlElement> elt);
    virtual ~LinkModel();

    int getInputTowner()   const;
    int getIndexOfInput()  const;
    int getOutputTowner()  const;
    int getIndexOfOutput() const;

    void setInputTowner(int input);
    void setIndexOfInput(int index);
    void setOutputTowner(int output);
    void setIndexOfOutput(int index);

  private:
    QI_DISALLOW_COPY_AND_ASSIGN(LinkModel);
    LinkModelPrivate* _p;
  };
  typedef boost::shared_ptr<LinkModel> LinkModelPtr;
}

#endif /* !LINK_H_ */
