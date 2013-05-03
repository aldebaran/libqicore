/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef CHOREGRAPHEPROJECT_P_H_
#define CHOREGRAPHEPROJECT_P_H_



#include <qicore-compat/model/boxinstancemodel.hpp>


namespace qi {
  class ChoregrapheProjectModelPrivate {
    friend class ChoregrapheProjectModel;
  public:
    ChoregrapheProjectModelPrivate(const std::string &dir,
                              const std::string &name,
                              const std::string &formatVersion,
                              boost::shared_ptr<BoxInstanceModel> rootBox);

    bool loadFromFile();

  private:
    std::string _dir;
    std::string _name;
    std::string _formatVersion;
    boost::shared_ptr<BoxInstanceModel> _rootBox;
  };
}

#endif
