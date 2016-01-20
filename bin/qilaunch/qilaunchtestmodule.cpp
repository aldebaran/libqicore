#include <qi/application.hpp>
#include <qi/log.hpp>
#include <qi/anymodule.hpp>
#include <qi/session.hpp>

qiLogCategory("testmodule");

struct MyService
{
  explicit MyService(qi::SessionPtr s)
    : i(42)
  {
    s->services(); // segfault?
  }
  int f() const
  {
    return i;
  }
  int i;
};
QI_REGISTER_OBJECT(MyService, f)

void testMyService(qi::SessionPtr session)
{
  qi::AnyObject my = session->service("MyService");
  if (my.call<int>("f") != 42)
    throw std::runtime_error("Fail");
  qi::Application::stop();
}

void func()
{
  qiLogInfo() << "func called";
  qi::Application::stop();
}

void initmodule(qi::ModuleBuilder* mb) {
  mb->advertiseFactory<MyService, qi::SessionPtr>("MyService");
  mb->advertiseMethod("func", &func);
  mb->advertiseMethod("testMyService", &testMyService);
}
QI_REGISTER_MODULE("qilaunchtestmodule", &initmodule);
