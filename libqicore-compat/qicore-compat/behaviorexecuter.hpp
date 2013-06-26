/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2013 All Rights Reserved
*/

#ifndef BEHAVIORLOADER_HPP_
#define BEHAVIORLOADER_HPP_

#include <qicore-compat/api.hpp>
#include <qicore-compat/model/boxinstancemodel.hpp>
#include <qimessaging/session.hpp>

namespace qi
{
  namespace compat {
    class BehaviorExecuterPrivate;
    class QICORECOMPAT_API BehaviorExecuter
    {
    public:
      BehaviorExecuter(const std::string &dir, qi::Session &session, bool debug);
      virtual ~BehaviorExecuter();

      void execute();
      bool load();
      qi::AnyObject behaviorService();
    private:
      QI_DISALLOW_COPY_AND_ASSIGN(BehaviorExecuter);
      BehaviorExecuterPrivate* _p;
    };
  }
}

#endif /* !BEHAVIORLOADER_HPP_ */
