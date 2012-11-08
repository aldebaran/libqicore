/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#pragma once

#ifndef BOX_H_
# define BOX_H_

# include <alcommon/albroker.h>

# include <qicore/api.hpp>

namespace qi
{

class BoxPrivate;
class Timeline;
class StateMachine;

class QICORE_API Box
{
  public:
    Box();
    ~Box();

    void setBroker(boost::shared_ptr<AL::ALBroker> broker);
    void setName(std::string name);
    std::string getName() const;

    void setPath(std::string path);
    std::string getPath() const;

    Timeline* getTimeline() const;
    void setTimeline(Timeline*);
    bool hasTimeline() const;

    StateMachine* getStateMachine() const;
    void setStateMachine(StateMachine*);
    bool hasStateMachine() const;

    BoxPrivate*           _p;
};

};

#endif /* !BOX_H_ */
