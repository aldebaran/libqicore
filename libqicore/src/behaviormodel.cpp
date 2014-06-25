#include <boost/algorithm/string.hpp>
#include <qi/jsoncodec.hpp>
#include <qicore/behavior.hpp>

namespace qi {

  static inline std::pair<std::string, std::string> splitString2(const std::string& s, char sep)
  {
    size_t p = s.find_first_of(sep);
    if (p == s.npos)
      throw std::runtime_error("separator not found");
    return std::make_pair(s.substr(0, p), s.substr(p+1));
  }

  void BehaviorModel::clear()
  {
    nodes.clear();
    transitions.clear();
  }

  void BehaviorModel::load(std::istream& is)
  {
    /* uid model factory prop1 = jsonVal1 prop2 = jsonval2
    *  uid a.b -> c.d [filterExp]
    *
    */
    char line[1024];
    while (is.good())
    {
      line[0] = 0;
      is.getline(line, 1024);
      std::string s(line);
      boost::trim(s);
      if (s.empty() || s[0] == '#')
        continue;
      std::stringstream st(s);
      std::string uid, p1, p2;
      bool isTransition = false;
      st >> uid >> p1 >> p2;
      if (p2.empty())
        continue;
      if (p2 == "->")
      {
        isTransition = true;
        st >> p2;
      }
      if (!isTransition)
      {
        BehaviorModel::Node& state = nodes[uid];
        state.uid = uid;
        state.interface = p1;
        state.factory = p2;

        std::string parameters;
        std::getline(st,parameters);
        size_t p = 0;
        std::string::const_iterator begin = parameters.begin();
        while (p <= parameters.size())
        {
          size_t eq = parameters.find_first_of('=', p);
          if (eq == parameters.npos)
          { // check for remaining data
            std::string remain = parameters.substr(p);
            boost::trim(remain);
            if (!remain.empty())
              throw std::runtime_error("Trailing data: " + remain);
            break;
          }
          std::string name = parameters.substr(p, eq-p);
          boost::algorithm::trim(name);
          qi::AnyValue gValue;
          size_t end = decodeJSON(begin + eq + 1, begin + parameters.size(), gValue) - begin;
          state.parameters[name] = gValue;
          p = end;
        }
      }
      else
      {
        BehaviorModel::Transition transition;
        std::string parameters;
        std::getline(st,parameters);
        boost::trim(parameters);
        if (!parameters.empty())
          transition.filter = decodeJSON(parameters);
        transition.uid = uid;
        transition.src = splitString2(p1, '.');
        transition.dst = splitString2(p2, '.');
        transitions[transition.uid] = transition;
      }
    }
  }

  void displayModel(const BehaviorModel& model)
  {
    std::cout << "display model" << std::endl;
    for (qi::BehaviorModel::NodeMap::const_iterator n = model.nodes.begin();
         n != model.nodes.end();
         ++n)
    {
      std::cout << "Node:" << std::endl << (*n).second.uid << std::endl
                << (*n).second.interface << std::endl
                << (*n).second.factory << std::endl
                << "****" << std::endl;

      std::cout << "Parameters:" << std::endl;
      for (qi::BehaviorModel::ParameterMap::const_iterator p = (*n).second.parameters.begin();
           p != (*n).second.parameters.end();
           ++p)
      {
        std::cout << (*p).first << std::endl;
      }
      std::cout << "***************************" << std::endl;
    }

    for (qi::BehaviorModel::TransitionMap::const_iterator t = model.transitions.begin();
         t != model.transitions.end();
         ++t)
    {
      std::cout << "Transition:" << std::endl << (*t).second.uid << std::endl
                << (*t).second.src.first << " | " << (*t).second.src.second << std::endl
                << (*t).second.dst.first << " | " << (*t).second.dst.second << std::endl
                << "*****************" << std::endl;
    }
  }
}
