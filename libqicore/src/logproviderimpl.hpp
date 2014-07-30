/*
** Author(s):
**  - Herve Cuche <hcuche@aldebaran-robotics.com>
**  - Matthieu Nottale <mnottale@aldebaran-robotics.com>
**
** Copyright (C) 2013 Aldebaran Robotics
*/

#ifndef LOGPROVIDERIMPL_HPP_
# define LOGPROVIDERIMPL_HPP_

# include <set>

# include <boost/thread.hpp>

# include <qi/log.hpp>
# include <qi/os.hpp>
# include <qi/periodictask.hpp>

# include <qicore/api.hpp>

# include <qicore/logmessage.hpp>
# include <qicore/loglistener.hpp>
# include <qicore/logmanager.hpp>
# include <qicore/logprovider.hpp>

namespace qi
{
  /** Registers to a local or remote Logger service
   *  Sends local logger message to it
   *  Honors commands from it to configure local logger verbosity.
   *  @threadSafe
   */
  class LogProviderImpl : public LogProvider
  {
  public:
    LogProviderImpl(LogManagerPtr logger);
    virtual ~LogProviderImpl();

    virtual void setLevel(qi::LogLevel level);
    virtual void addFilter(const std::string& filter,
                           qi::LogLevel level);
    virtual void setFilters(const std::vector<std::pair<std::string, qi::LogLevel> >& filters);

  private:
    void sendLogs();
    virtual void log(qi::LogLevel level,
                     qi::os::timeval tv,
                     const char* category,
                     const char* message,
                     const char* file,
                     const char* function,
                     int line);

  private:
    std::set<std::string> _setCategories;
    boost::mutex _setCategoriesMutex;
    LogManagerPtr _logger;
    qi::log::SubscriberId _subscriber;
    qi::Atomic<int> _ready;

    qi::PeriodicTask sendTask;
  };

  class ModuleBuilder;
  void registerLogProvider(qi::ModuleBuilder* mb);

} // !qi

#endif // !LOGPROVIDERIMPL_HPP_
