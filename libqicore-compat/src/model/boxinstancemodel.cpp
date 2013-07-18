/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <qicore-compat/model/boxinstancemodel.hpp>
#include <qicore-compat/model/parametermodel.hpp>
#include <qicore-compat/model/contentmodel.hpp>
#include <qicore-compat/model/contentsmodel.hpp>
#include "boxinstancemodel_p.hpp"

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

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
    _path(interface->path()),
    _interface(interface),
    _parameters(),
    _isValid(true),
    _parent()
  {
  }

  BoxInstanceModelPrivate::BoxInstanceModelPrivate(boost::shared_ptr<const AL::XmlElement> elt,
                                                   const std::string &dir, BoxInstanceModelPtr parent)
  {
    std::string path;
    _isValid = true;
    elt->getAttribute("name", _name);
    elt->getAttribute("id",   _id);
    elt->getAttribute("x",    _x);
    elt->getAttribute("y",    _y);
    elt->getAttribute("path", path);

    _parent = parent;
    _path = dir + "/" + path;
    qiLogDebug() << "BoxInterface path : " << _path;
    _interface = BoxInterfaceModelPtr(new BoxInterfaceModel(_path));

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

        _parameters.insert(std::pair<int, ParameterValueModelPtr>(param->id(), param));

      }
    }
  }

  bool BoxInstanceModelPrivate::addParameterValue(ParameterValueModelPtr value)
  {
    ParameterModelPtr param = _interface->findParameter(value->id());

    if(!value->isValid())
    {
      qiLogError() << "Invalid ParameterValue, in BoxInterface.name = "
                           << _interface->name()
                           << " ParameterValue is not initialized."
                           << std::endl;
      return false;
    }

    if (!param)
    {
      qiLogError() << "Invalid ParameterValue, in BoxInterface.name = "
                           << _interface->name()
                           << " Parameter with id = "
                           << value->id()
                           << " does not exist."
                           << std::endl;
      return false;
    }

    if(Signature(param->metaProperty().signature()).isConvertibleTo(Signature(value->value().signature())) < 1.0f)
    {
      qiLogDebug() << "Xml File : " << _interface->path();
      qiLogDebug() << "Parameter.signature = " << param->metaProperty().signature().toString();
      qiLogDebug() << "ParameterValue.signature = " << value->value().signature().toString();
      qiLogError() << "Invalid ParameterValue, in BoxInterface.name = \""
                   << _interface->name() << "\""
                   << " ParameterValue have not the same signature as Parameter.name ="
                   << param->metaProperty().name() << "."
                   << std::endl;
      return false;
    }

    if (!param->checkInterval(value))
    {
      qiLogError() << "Invalid ParameterValue, in BoxInterface.name = "
                           << _interface->name()
                           << " ParameterValue is not in interval of Parameter."
                           << std::endl;
      return false;
    }
    _parameters.insert(std::pair<int, ParameterValueModelPtr>(value->id(), value));
    return true;
  }

  BoxInstanceModel::BoxInstanceModel(const std::string &name, int id, int x, int y, boost::shared_ptr<BoxInterfaceModel> interface) :
    _p(new BoxInstanceModelPrivate(name, id, x, y, interface))
  {
  }

  BoxInstanceModel::BoxInstanceModel(boost::shared_ptr<const AL::XmlElement> elt, const std::string &dir, BoxInstanceModelPtr parent) :
    _p( new BoxInstanceModelPrivate(elt, dir, parent) )
  {
  }

  BoxInstanceModel::~BoxInstanceModel()
  {
    delete _p;
  }

  const std::string& BoxInstanceModel::name() const
  {
    return _p->_name;
  }

  int BoxInstanceModel::id() const
  {
    return _p->_id;
  }

  std::string BoxInstanceModel::uid() const
  {
    std::ostringstream oss;
    if(_p->_id >= 0)
      oss << _p->_id;
    else
      oss << 1;

    std::string uniqueId = this->name() + std::string("_") + oss.str();
    //TODO use a regexp
    boost::replace_all(uniqueId, " ", "_");
    boost::erase_all(uniqueId, "(");
    boost::erase_all(uniqueId, ")");
    boost::erase_all(uniqueId, ">");
    boost::erase_all(uniqueId, ".");
    boost::erase_all(uniqueId, "?");
    return uniqueId;
  }

  int BoxInstanceModel::x() const
  {
    return _p->_x;
  }

  int BoxInstanceModel::y() const
  {
    return _p->_y;
  }

  std::string BoxInstanceModel::behaviorPath() const
  {
    return boost::filesystem::path(_p->_path).parent_path().string();
  }

  std::string BoxInstanceModel::path() const
  {
    return boost::filesystem::path(_p->_path).filename().string();
  }

  BoxInterfaceModelPtr BoxInstanceModel::interface() const
  {
    return _p->_interface;
  }

  AnyReference BoxInstanceModel::parameter(int id)
  {
    //Search the user value
    std::map<int, ParameterValueModelPtr>::iterator it = _p->_parameters.find(id);

    if(it != _p->_parameters.end())
      return it->second->value();

    //Return default value
    ParameterModelPtr parameter = _p->_interface->findParameter(id);
    if(!parameter)
    {
      qiLogError() << "Parameter " << id << " not found";
      return AnyReference();
    }

    return parameter->defaultValue();
  }

  std::list<ParameterValueModelPtr> BoxInstanceModel::parametersValue() const
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

  const std::string& BoxInstanceModel::plugin() const
  {
    return _p->_interface->plugin();
  }

  boost::shared_ptr<BoxInstanceModel> BoxInstanceModel::parent()
  {
    return _p->_parent;
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
    _p->_path = interface->path();
  }

  bool BoxInstanceModel::addParameterValue(ParameterValueModelPtr value)
  {
    return _p->addParameterValue(value);
  }

  AnyReference BoxInstanceModel::content(ContentModel::ContentType type)
  {
    qiLogDebug() << "Content Load";
    ContentModelPtr content = _p->_interface->contents()->findContent(type);

    if(!content)
      return AnyReference();

    qiLogDebug() << "Content found";
    return content->content(shared_from_this());
  }

  bool BoxInstanceModel::isValid() const
  {
    return _p->_isValid;
  }
}
