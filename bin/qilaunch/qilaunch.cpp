#include <boost/program_options.hpp>

#include <qi/applicationsession.hpp>

#include <qicore/logprovider.hpp>

#include <QtCore/QCoreApplication>
#include <boost/thread.hpp>

#ifdef WITH_BREAKPAD
#include <breakpad/breakpad.h>
#endif

static std::vector<std::string> modules;
static std::vector<std::string> objects;
static std::vector<std::string> functions;
static bool keepRunning = false;
static bool disableBreakpad = false;
static bool disableLogging = false;
static bool qtEventloop = false;
static std::string launcherName;

static int exitStatus = 0;

qiLogCategory("qilaunch");

_QI_COMMAND_LINE_OPTIONS(
  "Launcher options",
  ("module,m", value<std::vector<std::string> >(&modules), "Load given module (can be set multiple times) (deprecated, use --object)")
  ("object,o", value<std::vector<std::string> >(&objects), "Load given object (syntax: yourmodule.YourObject) (can be set multiple times)")
  ("function,f", value<std::vector<std::string> >(&functions), "Call given function (syntax: yourmodule.yourFunction) (can be set multiple times)")
  ("keep-running,k", bool_switch(&keepRunning), "Keep running after the function(s) has(have) returned (implied by -o)")
  ("no-breakpad,b", bool_switch(&disableBreakpad), "Disable breakpad")
  ("no-logging,l", bool_switch(&disableLogging), "Disable remote logging")
  ("qt-eventloop,q", bool_switch(&qtEventloop), "Enable a Qt Eventloop")
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

// Copied from naoqicore
static void handleQTCoreApplication(int argc, char* argv[])
{
  // Create Main loop QT
  QCoreApplication app(argc, argv);

  bool exited = false;
  while (!exited)
  {
    try
    {
      // Start Main loop QT
      app.exec();
      exited = true;
    }
    catch (std::exception& e)
    {
      qiLogError() << "Uncaught QT main loop exception detected: " << e.what();
    }
    catch (...)
    {
      qiLogError() << "Uncaught QT main loop unknown exception detected.";
    }
  }
}

int main(int argc, char** argv)
{
  qi::ApplicationSession app(argc, argv);

  std::shared_ptr<boost::thread> thread;
  if (qtEventloop)
    thread = std::make_shared<boost::thread>(handleQTCoreApplication, argc, argv);

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
  if (!disableBreakpad)
  {
    // allocate so that it lives even after main termination
    auto eh = new BreakpadExceptionHandler(BREAKPAD_DUMP_DIR);
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

    for (const auto& object : objects)
    {
      qiLogInfo() << "Loading object " << object;
      app.session()->loadService(object);
    }

    std::vector<qi::Future<void>> futures;
    futures.reserve(functions.size());
    for (const auto& function: functions)
    {
      qiLogInfo() << "Calling function " << function;
      qi::Future<void> fut = app.session()->callModule<void>(function);
      futures.emplace_back(fut.then(boost::bind(stopOnError, _1, function)));
    }

    if (!objects.empty() || keepRunning)
      app.run();
    else
      // just wait for the functions to finish
      for (const auto& future : futures)
        future.wait();
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

  if (qtEventloop)
  {
    QCoreApplication::exit();
    thread->interrupt();
    thread->join();
  }

  return exitStatus;
}
