/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef LINK_P_H_
#define LINK_P_H_

#include <alserial/alserial.h>

namespace qi {

  class LinkModelPrivate
  {
    friend class LinkModel;
  public:
    LinkModelPrivate();
    LinkModelPrivate(int inputtowner, int indexofinput, int outputtowner, int indexofoutput);
    LinkModelPrivate(boost::shared_ptr<const AL::XmlElement> elt);

  private:
    int _inputTowner;
    int _indexOfInput;
    int _outputTowner;
    int _indexOfOutput;
  };
}

#endif /* !LINK_P_H_ */
