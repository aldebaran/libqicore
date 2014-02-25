/*
** Author(s):
**  - Herve Cuche <hcuche@aldebaran-robotics.com>
**  - Matthieu Nottale <mnottale@aldebaran-robotics.com>
**
** Copyright (C) 2013 Aldebaran Robotics
*/

#ifndef LOGPROVIDER_HPP_
# define LOGPROVIDER_HPP_

# include <boost/shared_ptr.hpp>

# include <qi/log.hpp>

# include <qicore/logmessage.hpp>
# include <qicore/logmanager_proxy.hpp>
# include <qicore/loglistener_proxy.hpp>

# include <set>
# include <string>
# include <utility> // std::pair
# include <vector>

namespace qi
{
  class LogProvider;
  typedef boost::shared_ptr<LogProvider> LogProviderPtr;

  // Register local provider to service
  qi::Future<int> registerToLogger(LogManagerProxyPtr logger);

  /** Registers to a local or remote Logger service
 *  Sends local logger message to it
 *  Honors commands from it to configure local logger verbosity.
 *  @threadSafe
 */
  class LogProvider
  {
    public:
      LogProvider(LogManagerProxyPtr logger);
      ~LogProvider();

      void setVerbosity(qi::LogLevel level);
      void setCategory(const std::string& cat,
                       qi::LogLevel level);
      void clearAndSet(const std::vector<std::pair<std::string, qi::LogLevel> >& data);

    private:
      void log(qi::LogLevel level,
               qi::os::timeval tv,
               const char* category,
               const char* message,
               const char* file,
               const char* function,
               int line);

    private:
      std::set<std::string> _setCategories;
      boost::mutex _setCategoriesMutex;
      LogManagerProxyPtr    _logger;
      qi::log::Subscriber   _subscriber;
      qi::Atomic<int>       _ready;
  };
} // !qi

#endif // !LOGPROVIDER_HPP_
