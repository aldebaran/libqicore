/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <alserial/alserial.h>
#include <qicore-compat/model/bitmapmodel.hpp>
#include "bitmapmodel_p.hpp"

namespace qi
{
  BitmapModelPrivate::BitmapModelPrivate() :
    _path()
  {
  }

  BitmapModelPrivate::BitmapModelPrivate(const std::string &path) :
    _path(path)
  {
  }

  BitmapModelPrivate::BitmapModelPrivate(boost::shared_ptr<const AL::XmlElement> elt)
  {
    elt->getAttribute("path", _path);
  }

  Bitmap::Bitmap(const std::string &path) :
    _p( new BitmapModelPrivate(path))
  {
  }

  Bitmap::Bitmap(boost::shared_ptr<const AL::XmlElement> elt) :
    _p( new BitmapModelPrivate(elt))
  {
  }

  Bitmap::~Bitmap()
  {
    delete _p;
  }

  const std::string& Bitmap::path() const
  {
    return _p->_path;
  }

  void Bitmap::setPath(const std::string &path)
  {
    _p->_path = path;
  }
}
