/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <qicore-compat/model/inputmodel.hpp>
#include "inputmodel_p.hpp"

//-------------------------------Private Class----------------------------------------//

namespace qi {
  InputModelPrivate::InputModelPrivate() :
    _name(),
    _type(),
    _typeSize(),
    _nature(),
    _stmValueName(),
    _inner(),
    _tooltip(),
    _id()
  {
  }

  InputModelPrivate::InputModelPrivate(const std::string &name,
                                       InputModel::InputType type,
                                       int type_size,
                                       InputModel::InputNature nature,
                                       const std::string &STMValueName,
                                       bool inner,
                                       const std::string &tooltip,
                                       int id) :
    _name(name),
    _type(type),
    _typeSize(type_size),
    _nature(nature),
    _stmValueName(STMValueName),
    _inner(inner),
    _tooltip(tooltip),
    _id(id)
  {
  }

  InputModelPrivate::InputModelPrivate(boost::shared_ptr<const AL::XmlElement> elt)
  {
    int type, nature;
    elt->getAttribute("name",           _name);
    elt->getAttribute("type",           type);
    elt->getAttribute("type_size",      _typeSize);
    elt->getAttribute("nature",         nature);
    elt->getAttribute("stm_value_name", _stmValueName, std::string(""));
    elt->getAttribute("inner",          _inner);
    elt->getAttribute("tooltip",        _tooltip);
    elt->getAttribute("id",             _id);

    _type   = static_cast<InputModel::InputType>( type );
    _nature = static_cast<InputModel::InputNature>( nature );
  }

  //--------------------------------Public Class-----------------------------------------//
  InputModel::InputModel(const std::string &name,
                         InputType type,
                         int type_size,
                         InputNature nature,
                         bool inner,
                         const std::string &tooltip,
                         int id) :
    _p(new InputModelPrivate(name, type, type_size, nature, "", inner, tooltip, id))
  {
  }

  InputModel::InputModel(const std::string &name,
                         InputType type,
                         int type_size,
                         const std::string &STMValueName,
                         bool inner,
                         const std::string &tooltip,
                         int id) :
    _p(new InputModelPrivate(name,
                             type,
                             type_size,
                             InputModel::InputNature_STMValue,
                             STMValueName,
                             inner,
                             tooltip,
                             id)
       )
  {
  }

  InputModel::InputModel(boost::shared_ptr<const AL::XmlElement> elt) :
    _p(new InputModelPrivate(elt))
  {
  }

  InputModel::~InputModel()
  {
    delete _p;
  }

  const std::string& InputModel::getName() const
  {
    return _p->_name;
  }

  InputModel::InputType InputModel::getType() const
  {
    return _p->_type;
  }

  int InputModel::getTypeSize() const
  {
    return _p->_typeSize;
  }

  InputModel::InputNature InputModel::getNature() const
  {
    return _p->_nature;
  }

  const std::string& InputModel::getSTMValueName() const
  {
    return _p->_stmValueName;
  }

  bool InputModel::getInner() const
  {
    return _p->_inner;
  }

  const std::string& InputModel::getTooltip() const
  {
    return _p->_tooltip;
  }

  int InputModel::getId() const
  {
    return _p->_id;
  }

  void InputModel::setName(const std::string& name)
  {
    _p->_name = name;
  }

  void InputModel::setType(InputModel::InputType type)
  {
    _p->_type = type;
  }

  void InputModel::setTypeSize(int type_size)
  {
    _p->_typeSize = type_size;
  }

  void InputModel::setNature(InputModel::InputNature nature)
  {
    _p->_nature = nature;
  }

  void InputModel::setSTMValueName(const std::string& stm_value_name)
  {
    _p->_nature = InputModel::InputNature_STMValue;
    _p->_stmValueName = stm_value_name;
  }

  void InputModel::setInner(bool inner)
  {
    _p->_inner = inner;
  }

  void InputModel::setTooltip(const std::string& tooltip)
  {
    _p->_tooltip = tooltip;
  }

  void InputModel::setId(int id)
  {
    _p->_id = id;
  }
}
