/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef BITMAP_P_H_
#define BITMAP_P_H_

#include <string>
#include <qicore-compat/model/bitmapmodel.hpp>

namespace qi
{
  class BitmapModelPrivate
  {
    friend class Bitmap;
  public:
    BitmapModelPrivate();
    BitmapModelPrivate(const std::string &path);
    BitmapModelPrivate(boost::shared_ptr<const AL::XmlElement> elt);

  private:
    std::string _path;
  };
}

#endif /* !BITMAP_P_H_ */
