/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2013 All Rights Reserved
*/

#ifndef BEHAVIORLOADER_HPP_
#define BEHAVIORLOADER_HPP_

#include <qicore-compat/api.hpp>
#include <qicore-compat/model/boxinstancemodel.hpp>
#include <qi/session.hpp>

namespace qi
{
  namespace compat {
    class BehaviorExecuterPrivate;
    class QICORECOMPAT_API BehaviorExecuter : private boost::noncopyable
    {
    public:
      BehaviorExecuter(const std::string &dir, boost::shared_ptr<qi::Session> session, bool debug);
      virtual ~BehaviorExecuter();

      void execute();
      bool load();
      qi::AnyObject behaviorService();
    private:
      BehaviorExecuterPrivate* _p;
    };
  }
}

#endif /* !BEHAVIORLOADER_HPP_ */
