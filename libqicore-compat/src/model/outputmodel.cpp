/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <qicore-compat/model/outputmodel.hpp>
#include "outputmodel_p.hpp"

//-------------------------------Private Class----------------------------------------//

namespace qi
{
  OutputModelPrivate::OutputModelPrivate(const std::string &name,
                                         const Signature &signature,
                                         int nature,
                                         bool inner,
                                         const std::string &tooltip,
                                         unsigned int id) :

    _metaSignal(id, name, signature.toString()),
    _nature(nature),
    _inner(inner),
    _tooltip(tooltip)
  {
  }

  OutputModelPrivate::OutputModelPrivate(boost::shared_ptr<const AL::XmlElement> elt)
  {
    std::string signature;
    std::string name;
    int id;
    elt->getAttribute("name",      name);
    elt->getAttribute("signature",      signature);
    elt->getAttribute("id",        id);
    _metaSignal = MetaSignal(id, name, signature);

    elt->getAttribute("nature",    _nature);
    elt->getAttribute("inner",     _inner);
    elt->getAttribute("tooltip",   _tooltip);
  }

  //--------------------------------Public Class-----------------------------------------//
  OutputModel::OutputModel(const std::string &name,
                           const Signature &signature,
                           int nature,
                           bool inner,
                           const std::string &tooltip,
                           int id) :
    _p(new OutputModelPrivate(name, signature, nature, inner, tooltip, id))
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
  const MetaSignal& OutputModel::metaSignal() const
  {
    return _p->_metaSignal;
  }

  int OutputModel::nature() const
  {
    return _p->_nature;
  }

  bool OutputModel::inner() const
  {
    return _p->_inner;
  }

  const std::string& OutputModel::tooltip() const
  {
    return _p->_tooltip;
  }

  void OutputModel::setMetaSignal(const std::string &name,
                                  const Signature &signature,
                                  unsigned int id)
  {
    _p->_metaSignal = MetaSignal(id, name, signature.toString());
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
}
