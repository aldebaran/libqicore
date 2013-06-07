/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef BOXINSTANCE_P_H_
#define BOXINSTANCE_P_H_

#include <map>

#include <alserial/alserial.h>
#include <qicore-compat/model/parametervaluemodel.hpp>
#include <qicore-compat/model/boxinterfacemodel.hpp>
#include <qicore-compat/model/boxinstancemodel.hpp>

namespace qi
{

  class BoxInstanceModelPrivate
  {
    friend class BoxInstanceModel;
  public:
    BoxInstanceModelPrivate();
    BoxInstanceModelPrivate(boost::shared_ptr<const AL::XmlElement> elt, const std::string &dir, BoxInstanceModelPtr parent);
    BoxInstanceModelPrivate(const std::string &name,
                            int id,
                            int x,
                            int y,
                            boost::shared_ptr<BoxInterfaceModel> interface);

    bool addParameterValue(ParameterValueModelPtr value);

  private:
    std::string _name;
    int _id;
    int _x;
    int _y;
    std::string _path;
    BoxInterfaceModelPtr                  _interface;
    std::map<int, ParameterValueModelPtr> _parameters;
    bool _isValid;
    BoxInstanceModelPtr _parent;
  };

}
#endif /* !BOXINSTANCE_P_H_ */
