/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef BITMAP_H_
#define BITMAP_H_

#include <alserial/alserial.h>
#include <qicore-compat/api.hpp>
#include <boost/noncopyable.hpp>

namespace qi {
  class BitmapModelPrivate;

  class QICORECOMPAT_API Bitmap : private boost::noncopyable
  {
  public:
    Bitmap(const std::string &path);
    Bitmap(boost::shared_ptr<const AL::XmlElement> elt);
    virtual ~Bitmap();

    const std::string& path() const;
    void setPath(const std::string &path);

  private:
    BitmapModelPrivate *_p;
  };
  typedef boost::shared_ptr<Bitmap> BitmapModelPtr;
}

#endif /* !BITMAP_H_ */
