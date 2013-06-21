/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <qi/log.hpp>

#include <qicore-compat/model/inputmodel.hpp>
#include "inputmodel_p.hpp"

qiLogCategory("QiCore-Compat.InputModel");
//-------------------------------Private Class----------------------------------------//

namespace qi
{
  InputModelPrivate::InputModelPrivate(const std::string &name,
                                       const Signature &signature,
                                       InputModel::InputNature nature,
                                       const std::string &STMValueName,
                                       bool inner,
                                       const std::string &tooltip,
                                       unsigned int id) :
    _metaMethod(),
    _nature(nature),
    _stmValueName(STMValueName),
    _inner(inner)
  {
    MetaMethodBuilder builder("v", name, signature.toString(), tooltip);
    builder.setUid(id);
    _metaMethod = builder.metaMethod();
  }

  InputModelPrivate::InputModelPrivate(boost::shared_ptr<const AL::XmlElement> elt)
  {
    int id;
    int nature;
    std::string signature;
    std::string name;
    std::string tooltip;
    elt->getAttribute("name",           name);
    elt->getAttribute("signature",           signature);
    elt->getAttribute("tooltip",        tooltip);
    elt->getAttribute("id",             id);
    qiLogDebug() << "Construct InputModel from xml file with : "
                 << "name = " << name
                 << ", id = " << id << ", signature = " << signature;

    MetaMethodBuilder builder("v", name, signature, tooltip);
    builder.setUid(id);
    _metaMethod = builder.metaMethod();

    elt->getAttribute("nature",         nature);
    _nature = static_cast<InputModel::InputNature>( nature );

    elt->getAttribute("stm_value_name", _stmValueName, std::string(""));
    elt->getAttribute("inner",          _inner);
  }

  //--------------------------------Public Class-----------------------------------------//
  InputModel::InputModel(const std::string &name,
                         const Signature &signature,
                         InputNature nature,
                         bool inner,
                         const std::string &tooltip,
                         int id) :
    _p(new InputModelPrivate(name, signature, nature, "", inner, tooltip, id))
  {
  }

  InputModel::InputModel(const std::string &name,
                         const Signature &signature,
                         const std::string &STMValueName,
                         bool inner,
                         const std::string &tooltip,
                         int id) :
    _p(new InputModelPrivate(name,
                             signature,
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

  const MetaMethod& InputModel::metaMethod() const
  {
    return _p->_metaMethod;
  }

  InputModel::InputNature InputModel::nature() const
  {
    return _p->_nature;
  }

  const std::string& InputModel::stmValueName() const
  {
    return _p->_stmValueName;
  }

  bool InputModel::inner() const
  {
    return _p->_inner;
  }

  void InputModel::setMetaMethod(const std::string &name,
                                 const Signature &signature,
                                 const std::string &tooltip,
                                 unsigned int id)
  {
    MetaMethodBuilder builder("v", name, signature.toString(), tooltip);
    builder.setUid(id);
    _p->_metaMethod = builder.metaMethod();
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
}
