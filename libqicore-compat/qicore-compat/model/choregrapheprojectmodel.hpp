/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef CHOREGRAPHEPROJECT_H_
#define CHOREGRAPHEPROJECT_H_

#include <qicore-compat/api.hpp>

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

namespace qi {
  class ChoregrapheProjectModelPrivate;
  class BoxInstanceModel;

  class QICORECOMPAT_API ChoregrapheProjectModel : private boost::noncopyable {
  public:
    ChoregrapheProjectModel(const std::string &dir = "",
                            const std::string &name = "",
                            const std::string &formatVersion = "4",
                            boost::shared_ptr<BoxInstanceModel> rootBox = boost::shared_ptr<BoxInstanceModel>());

    virtual ~ChoregrapheProjectModel();

    bool loadFromFile();

    const std::string& name()          const;
    const std::string& formatVersion() const;

    boost::shared_ptr<BoxInstanceModel> rootBox() const;

    void setName(const std::string& name);
    void setFormatVersion(const std::string& formatVersion);
    void setRootBox(boost::shared_ptr<BoxInstanceModel> rootBox);

  protected:
    ChoregrapheProjectModelPrivate* _p;
  };

}

#endif /* !CHOREGRAPHEPROJECT_H_ */
