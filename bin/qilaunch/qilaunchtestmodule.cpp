#include <qi/application.hpp>
#include <qi/log.hpp>
#include <qi/anymodule.hpp>
#include <qi/jsoncodec.hpp>
#include <qi/session.hpp>

qiLogCategory("testmodule");

struct MyData
{
  std::string peripateticien;
  int dodecahedre;
};

QI_TYPE_STRUCT(MyData, peripateticien, dodecahedre)

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

void funcWithSession(qi::SessionPtr)
{
  qiLogInfo() << "Func called with session";
  qi::Application::stop();
}

void funcWithArgs(double ceciNestPasUnNombre, std::string tangaCestMieux, MyData ohMy)
{
  qiLogInfo() << "Func called with session and: " << ceciNestPasUnNombre
              << ", " << tangaCestMieux << ", " << qi::encodeJSON(ohMy);
  qi::Application::stop();
}

void funcWithSessionAndArgs(qi::SessionPtr, double ceciNestPasUnNombre, std::string tangaCestMieux, MyData ohMy)
{
  qiLogInfo() << "Func called with session and: " << ceciNestPasUnNombre
              << ", " << tangaCestMieux << ", " << qi::encodeJSON(ohMy);
  qi::Application::stop();
}

void initmodule(qi::ModuleBuilder* mb) {
  mb->advertiseFactory<MyService, qi::SessionPtr>("MyService");
  mb->advertiseMethod("func", &func);
  mb->advertiseMethod("funcWithSession", &funcWithSession);
  mb->advertiseMethod("funcWithArgs", &funcWithArgs);
  mb->advertiseMethod("funcWithSessionAndArgs", &funcWithSessionAndArgs);
  mb->advertiseMethod("testMyService", &testMyService);
}
QI_REGISTER_MODULE("qilaunchtestmodule", &initmodule);
