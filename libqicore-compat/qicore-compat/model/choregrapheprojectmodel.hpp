/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef CHOREGRAPHEPROJECT_H_
# define CHOREGRAPHEPROJECT_H_

# include <qicore-compat/api.hpp>

# include <string>
# include <boost/shared_ptr.hpp>

namespace qi {
  class ChoregrapheProjectModelPrivate;
  class BoxInstanceModel;

  class QICORECOMPAT_API ChoregrapheProjectModel {
  public:
    ChoregrapheProjectModel(const std::string &dir = "",
                            const std::string &name = "",
                            const std::string &formatVersion = "4",
                            boost::shared_ptr<BoxInstanceModel> rootBox = boost::shared_ptr<BoxInstanceModel>());

    virtual ~ChoregrapheProjectModel();

    bool loadFromFile();

    const std::string& getName()          const;
    const std::string& getFormatVersion() const;

    boost::shared_ptr<BoxInstanceModel> getRootBox() const;

    void setName(const std::string& name);
    void setFormatVersion(const std::string& formatVersion);
    void setRootBox(boost::shared_ptr<BoxInstanceModel> rootBox);

  protected:
    QI_DISALLOW_COPY_AND_ASSIGN(ChoregrapheProjectModel);
    ChoregrapheProjectModelPrivate* _p;
  };

}

#endif /* !CHOREGRAPHEPROJECT_H_ */
