/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <qicore-compat/model/outputmodel.hpp>
#include "outputmodel_p.hpp"

//-------------------------------Private Class----------------------------------------//

namespace qi
{
  OutputModelPrivate::OutputModelPrivate() :
    _name(),
    _type(),
    _typeSize(),
    _nature(),
    _inner(),
    _tooltip(),
    _id()
  {
  }

  OutputModelPrivate::OutputModelPrivate(const std::string &name,
                                         int type,
                                         int type_size,
                                         int nature,
                                         bool inner,
                                         const std::string &tooltip,
                                         int id) :
    _name(name),
    _type(type),
    _typeSize(type_size),
    _nature(nature),
    _inner(inner),
    _tooltip(tooltip),
    _id(id)
  {
  }

  OutputModelPrivate::OutputModelPrivate(boost::shared_ptr<const AL::XmlElement> elt)
  {
    elt->getAttribute("name",      _name);
    elt->getAttribute("type",      _type);
    elt->getAttribute("type_size", _typeSize);
    elt->getAttribute("nature",    _nature);
    elt->getAttribute("inner",     _inner);
    elt->getAttribute("tooltip",   _tooltip);
    elt->getAttribute("id",        _id);
  }

  //--------------------------------Public Class-----------------------------------------//
  OutputModel::OutputModel(const std::string &name,
                           int type,
                           int type_size,
                           int nature,
                           bool inner,
                           const std::string &tooltip,
                           int id) :
    _p(new OutputModelPrivate(name, type, type_size, nature, inner, tooltip, id))
  {
  }

  OutputModel::OutputModel(boost::shared_ptr<const AL::XmlElement> elt) :
    _p(new OutputModelPrivate(elt))
  {
  }

  OutputModel::~OutputModel()
  {
    delete _p;
  }

  //--------------------------------Getter-----------------------------------------//
  const std::string& OutputModel::getName() const
  {
    return _p->_name;
  }

  int OutputModel::getType() const
  {
    return _p->_type;
  }

  int OutputModel::getTypeSize() const
  {
    return _p->_typeSize;
  }

  int OutputModel::getNature() const
  {
    return _p->_nature;
  }

  bool OutputModel::getInner() const
  {
    return _p->_inner;
  }

  const std::string& OutputModel::getTooltip() const
  {
    return _p->_tooltip;
  }

  int OutputModel::getId() const
  {
    return _p->_id;
  }

  void OutputModel::setName(const std::string& name)
  {
    _p->_name = name;
  }

  void OutputModel::setType(int type)
  {
    _p->_type = type;
  }

  void OutputModel::setTypeSize(int type_size)
  {
    _p->_typeSize = type_size;
  }

  void OutputModel::setNature(int nature)
  {
    _p->_nature = nature;
  }

  void OutputModel::setInner(bool inner)
  {
    _p->_inner = inner;
  }

  void OutputModel::setTooltip(const std::string& tooltip)
  {
    _p->_tooltip = tooltip;
  }

  void OutputModel::setId(int id)
  {
    _p->_id = id;
  }
}
