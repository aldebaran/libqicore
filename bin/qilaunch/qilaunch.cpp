#include <boost/program_options.hpp>

#include <qi/applicationsession.hpp>

#include <qicore/logprovider.hpp>

#ifdef WITH_BREAKPAD
#include <breakpad/breakpad.h>
#endif

static std::vector<std::string> modules;
static bool disableBreakpad = false;
static bool disableLogging = false;
static std::string launcherName;

qiLogCategory("qilaunch");

_QI_COMMAND_LINE_OPTIONS(
  "Launcher options",
  ("module,m", value<std::vector<std::string> >(&modules), "Load given module (can be set multiple times)")
  ("no-breakpad,b", bool_switch(&disableBreakpad), "Disable breakpad")
  ("no-logging,l", bool_switch(&disableLogging), "Disable remote logging")
  ("name,n", value<std::string>(&launcherName), "Name of the launcher used to prefix logs and breakpad dump files")
)

int main(int argc, char** argv)
{
  qi::ApplicationSession app(argc, argv);

  if (launcherName.empty())
  {
    qiLogFatal() << "No launcher name, set one with --name or -n";
    return 1;
  }

  if (modules.empty())
  {
    qiLogFatal() << "No module to load, add one with --module or -m";
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
    app.start();

    try
    {
      if (!disableLogging)
        qi::initializeLogging(app.session(), launcherName);
    }
    catch (std::exception &e)
    {
      qiLogWarning() << "Logs initialization failed with the following error: " << e.what();
    }

    for (unsigned i = 0; i < modules.size(); ++i)
    {
      qiLogInfo() << "Loading module " << modules[i];
      app.session()->loadService(modules[i]);
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

  return 0;
}
