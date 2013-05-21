/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <qicore-compat/model/boxinstancemodel.hpp>
#include <qicore-compat/model/parametermodel.hpp>
#include "boxinstancemodel_p.hpp"


#include <qi/log.hpp>
qiLogCategory("QiCore-Compat.BoxInstanceModel");

namespace qi {
  BoxInstanceModelPrivate::BoxInstanceModelPrivate() :
    _name(),
    _id(),
    _x(),
    _y(),
    _path(),
    _parameters()
  {
  }

  BoxInstanceModelPrivate::BoxInstanceModelPrivate(const std::string &name, int id, int x, int y, boost::shared_ptr<BoxInterfaceModel> interface) :
    _name(name),
    _id(id),
    _x(x),
    _y(y),
    _path(interface->getPath()),
    _interface(interface),
    _parameters(),
    _isValid(true)
  {
  }

  BoxInstanceModelPrivate::BoxInstanceModelPrivate(boost::shared_ptr<const AL::XmlElement> elt,
                                                   const std::string &dir)
  {
    std::string path;
    _isValid = true;
    elt->getAttribute("name", _name);
    elt->getAttribute("id",   _id);
    elt->getAttribute("x",    _x);
    elt->getAttribute("y",    _y);
    elt->getAttribute("path", _path);

    path = dir + "/" + _path;
    _interface = BoxInterfaceModelPtr(new BoxInterfaceModel(path));

    if(!_interface->loadFromFile())
      _isValid = false;

    if(_isValid)
    {
      AL::XmlElement::CList parameters = elt->children("ParameterValue", "");

      for(AL::XmlElement::CList::const_iterator it = parameters.begin(),
          itEnd = parameters.end();
          it != itEnd; ++it)
      {
        ParameterValueModelPtr param(new ParameterValueModel(*it, _interface));

        if(!addParameterValue(param))
          _isValid = false;

        _parameters.insert(std::pair<int, ParameterValueModelPtr>(param->getId(), param));

      }
    }
  }

  bool BoxInstanceModelPrivate::addParameterValue(ParameterValueModelPtr value)
  {
    ParameterModelPtr param = _interface->findParameter(value->getId());

    if(!value->isValid())
    {
      qiLogError() << "Invalid ParameterValue, in BoxInterface.name = "
                           << _interface->getName()
                           << " ParameterValue is not initialized."
                           << std::endl;
      return false;
    }

    if (!param)
    {
      qiLogError() << "Invalid ParameterValue, in BoxInterface.name = "
                           << _interface->getName()
                           << " Parameter with id = "
                           << value->getId()
                           << " does not exist."
                           << std::endl;
      return false;
    }

    if(Signature(param->metaProperty().signature()).isConvertibleTo(Signature(value->getValue().signature())) < 1.0f)
    {
      qiLogDebug() << "Xml File : " << _interface->getPath();
      qiLogDebug() << "Parameter.signature = " << param->metaProperty().signature();
      qiLogDebug() << "ParameterValue.signature = " << value->getValue().signature();
      qiLogError() << "Invalid ParameterValue, in BoxInterface.name = \""
                   << _interface->getName() << "\""
                   << " ParameterValue have not the same signature as Parameter.name ="
                   << param->metaProperty().name() << "."
                   << std::endl;
      return false;
    }

    if (!param->checkInterval(value))
    {
      qiLogError() << "Invalid ParameterValue, in BoxInterface.name = "
                           << _interface->getName()
                           << " ParameterValue is not in interval of Parameter."
                           << std::endl;
      return false;
    }

    _parameters.insert(std::pair<int, ParameterValueModelPtr>(value->getId(), value));
    return true;
  }

  BoxInstanceModel::BoxInstanceModel(const std::string &name, int id, int x, int y, boost::shared_ptr<BoxInterfaceModel> interface) :
    _p(new BoxInstanceModelPrivate(name, id, x, y, interface))
  {
  }

  BoxInstanceModel::BoxInstanceModel(boost::shared_ptr<const AL::XmlElement> elt, const std::string &dir) :
    _p( new BoxInstanceModelPrivate(elt, dir) )
  {
  }

  BoxInstanceModel::~BoxInstanceModel()
  {
    delete _p;
  }

  const std::string& BoxInstanceModel::getName() const
  {
    return _p->_name;
  }

  int BoxInstanceModel::getId() const
  {
    return _p->_id;
  }

  int BoxInstanceModel::getX() const
  {
    return _p->_x;
  }

  int BoxInstanceModel::getY() const
  {
    return _p->_y;
  }

  const std::string& BoxInstanceModel::getPath() const
  {
    return _p->_path;
  }

  BoxInterfaceModelPtr BoxInstanceModel::getInterface() const
  {
    return _p->_interface;
  }

  std::list<ParameterValueModelPtr> BoxInstanceModel::getParametersValue() const
  {
    std::list<ParameterValueModelPtr> rest;

    for(std::map<int, ParameterValueModelPtr>::const_iterator it = _p->_parameters.begin(),
                                                             itEnd = _p->_parameters.end();
        it != itEnd; ++it)
    {
      rest.push_front(it->second);
    }

    return rest;
  }

  const std::string& BoxInstanceModel::getPlugin() const
  {
    return _p->_interface->getPlugin();
  }

  void BoxInstanceModel::setName(const std::string& name)
  {
    _p->_name = name;
  }

  void BoxInstanceModel::setId(int id)
  {
    _p->_id = id;
  }

  void BoxInstanceModel::setX(int x)
  {
    _p->_x = x;
  }

  void BoxInstanceModel::setY(int y)
  {
    _p->_y = y;
  }

  void BoxInstanceModel::setBoxInterface(BoxInterfaceModelPtr interface)
  {
    _p->_parameters.clear();
    _p->_interface = interface;
    _p->_path = interface->getPath();
  }

  bool BoxInstanceModel::addParameterValue(ParameterValueModelPtr value)
  {
    return _p->addParameterValue(value);
  }

  bool BoxInstanceModel::isValid() const
  {
    return _p->_isValid;
  }
}
