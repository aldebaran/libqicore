#include <boost/program_options.hpp>

#include <qi/applicationsession.hpp>

#include <qicore/logprovider.hpp>

#ifdef WITH_BREAKPAD
#include <breakpad/breakpad.h>
#endif

static std::vector<std::string> modules;
static std::vector<std::string> objects;
static std::vector<std::string> functions;
static bool disableBreakpad = false;
static bool disableLogging = false;
static std::string launcherName;

static int exitStatus = 0;

qiLogCategory("qilaunch");

_QI_COMMAND_LINE_OPTIONS(
  "Launcher options",
  ("module,m", value<std::vector<std::string> >(&modules), "Load given module (can be set multiple times) (deprecated, use --object)")
  ("object,o", value<std::vector<std::string> >(&objects), "Load given object (syntax: yourmodule.YourObject) (can be set multiple times)")
  ("function,f", value<std::vector<std::string> >(&functions), "Call given function (syntax: yourmodule.yourFunction) (can be set multiple times)")
  ("no-breakpad,b", bool_switch(&disableBreakpad), "Disable breakpad")
  ("no-logging,l", bool_switch(&disableLogging), "Disable remote logging")
  ("name,n", value<std::string>(&launcherName), "Name of the launcher used to prefix logs and breakpad dump files")
)

void stopOnError(qi::Future<void> fut, const std::string& name)
{
  if (fut.hasValue())
    return;
  else if (fut.hasError())
    qiLogFatal() << name << " has finished with an error: " << fut.error();
  else if (fut.isCanceled())
    qiLogFatal() << name << " has been canceled";
  else
    assert(false && "future in incoherent state");

  exitStatus = 1;
  qi::Application::stop();
}

int main(int argc, char** argv)
{
  qi::ApplicationSession app(argc, argv);

  if (launcherName.empty())
  {
    qiLogFatal() << "No launcher name, set one with --name or -n";
    return 1;
  }

  if (!modules.empty())
    qiLogWarning() << "--module is deprecated, use --object";

  objects.insert(objects.end(), modules.begin(), modules.end());

  if (objects.empty() && functions.empty())
  {
    qiLogFatal() << "No object to load and no function to call, add one with --object or --function";
    return 1;
  }

#ifdef WITH_BREAKPAD
  boost::scoped_ptr<BreakpadExceptionHandler> eh;
  if (!disableBreakpad)
  {
    eh.reset(new BreakpadExceptionHandler(BREAKPAD_DUMP_DIR));
    eh->setBuildTag(launcherName);
  }
#endif

  try
  {
    qiLogInfo() << "Connection to service directory at " << app.url().str();
    app.startSession();

    try
    {
      if (!disableLogging)
        qi::initializeLogging(app.session(), launcherName);
    }
    catch (std::exception &e)
    {
      qiLogWarning() << "Logs initialization failed with the following error: " << e.what();
    }

    for (auto& object : objects)
    {
      qiLogInfo() << "Loading object " << object;
      app.session()->loadService(object);
    }

    for (auto& function: functions)
    {
      qiLogInfo() << "Calling function " << function;
      qi::Future<void> fut = app.session()->asyncCallModule<void>(function);
      fut.thenR<void>(boost::bind(stopOnError, _1, function));
    }

    app.run();
  }
  catch (std::exception& e)
  {
    qiLogFatal() << e.what();
    return 1;
  }
  catch (...)
  {
    qiLogFatal() << "Unknown exception";
    return 1;
  }

  return exitStatus;
}
