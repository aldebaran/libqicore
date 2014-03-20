/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef LINK_H_
#define LINK_H_

#include <qicore-compat/api.hpp>
#include <alserial/alserial.h>
#include <boost/noncopyable.hpp>

namespace qi {
  class LinkModelPrivate;

  class QICORECOMPAT_API LinkModel : private boost::noncopyable {
  public:
    LinkModel(int inputtowner, int indexofinput, int outputtowner, int indexofoutput);
    LinkModel(boost::shared_ptr<const AL::XmlElement> elt);
    virtual ~LinkModel();

    int inputTowner()   const;
    int indexOfInput()  const;
    int outputTowner()  const;
    int indexOfOutput() const;

    void setInputTowner(int input);
    void setIndexOfInput(int index);
    void setOutputTowner(int output);
    void setIndexOfOutput(int index);

  private:
    LinkModelPrivate* _p;
  };
  typedef boost::shared_ptr<LinkModel> LinkModelPtr;
}

#endif /* !LINK_H_ */
