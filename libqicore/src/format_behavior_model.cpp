/*
** Author(s):
**  - Cedric GESTES <gestes@aldebaran-robotics.com>
**
** Copyright (C) 2014 Aldebaran Robotics
*/

#include <iostream>
#include <qi/log.hpp>
#include <qilang/visitor.hpp>
#include <qilang/node.hpp>
#include <qilang/formatter.hpp>
#include <qi/os.hpp>
#include "formatter_p.hpp"
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "cpptype.hpp"
#include <qi/anyvalue.hpp>
#include <qicore/behavior.hpp>
#include <stack>

qiLogCategory("qigen.behaviorModel");


namespace qilang {


// Construct a BehaviorModel
class QiLangGenBehaviorModel: public DefaultNodeVisitor
{
public:

  virtual void doAccept(Node* node) { node->accept(this); }

  std::vector<qi::BehaviorModel>       modelsList;
  std::stack<qi::BehaviorModel::Node*> nodesStack;
  qi::BehaviorModel*                   currentModel;
  qi::BehaviorModel::Node*             currentNode;

  QiLangGenBehaviorModel() :
    currentModel(0),
    currentNode(0)
  {
  }

  std::vector<qi::BehaviorModel> convert(const NodePtrVector& nodes)
  {
    for (NodePtrVector::const_iterator nodeItr = nodes.begin();
         nodeItr != nodes.end();
         ++nodeItr)
    {
      accept((*nodeItr));
    }

    return modelsList;
  }

  void visitStmt(PackageNode* node) {
    std::cout << "PackageNode not implemented" << std::endl;
    // throw std::runtime_error("not implemented");
  }

  void visitStmt(ImportNode* node) {
    throw std::runtime_error("not implemented");
  }
  /* object TYPE Id                  > ObjectDefNode(TypeExprNode, str, vector<stmt>)
   *  prop blblbl: data              > PropertyDefNode(str, ConstDataNode)
   *  at slot signal                 > AtNode(str, str)
   * end
   */
  void visitStmt(ObjectDefNode *node) {

    // std::cout << "ObjectNode: " << std::endl;

    boost::shared_ptr<CustomTypeExprNode> expr;
    expr = boost::dynamic_pointer_cast<qilang::CustomTypeExprNode>(node->type);

    if (!expr)
      throw std::runtime_error("SimpleTypeExprNode cast error");

    if (expr->value == "Graph")
    {
      currentModel = new qi::BehaviorModel();
      currentModel->name = node->name;
      // iterate over all statements
      for (unsigned int i = 0; i < node->values.size(); ++i) {
        accept(node->values.at(i));
      }

      modelsList.push_back(*currentModel);
    }
    else
    {
      currentNode = &(currentModel->nodes[node->name]);
      currentNode->uid = node->name;
      currentNode->factory = expr->value;
      currentNode->interface = "Whatever";

      nodesStack.push(currentNode);
      // iterate over all statements
      for (unsigned int i = 0; i < node->values.size(); ++i) {
        accept(node->values.at(i));
      }
      currentNode = nodesStack.top();
      nodesStack.pop();
    }
  }

  void visitStmt(PropertyDefNode *node) {
    qi::AnyValue data = qilang::toAnyValue(node->data);

    if (currentNode->parameters.find(node->name) != currentNode->parameters.end())
      qiLogWarning() << "Redefinition of property " << node->name << std::endl;

    currentNode->parameters[node->name] = data;
  }

  std::pair<std::string, std::string> resolveTransition(const std::string& trans) {
    size_t p = trans.find_first_of('.');
    if (p != std::string::npos) {
      return std::make_pair(trans.substr(0, p), trans.substr(p+1));
    }
    return std::make_pair(currentNode->uid, trans);
  }

  void visitStmt(AtNode* node) {

    std::string transition_uid = node->receiver + "|" + node->sender();
    qi::BehaviorModel::Transition& currentTransition = currentModel->transitions[transition_uid];
    currentTransition.uid = transition_uid;
    currentTransition.src = resolveTransition(node->sender());
    currentTransition.dst = resolveTransition(node->receiver);
  }

  void visitStmt(VarDefNode* node) {
    throw std::runtime_error("unimplemented");
  }

  void visitStmt(CommentNode* node) {
  }

};


  std::vector<qi::BehaviorModel> genBehaviorModel(const NodePtrVector& nodes) {
    return QiLangGenBehaviorModel().convert(nodes);
  }

}

namespace qi {

  qi::BehaviorModel loadBehaviorModel(const qilang::NodePtrVector& nodes) {
    return genBehaviorModel(nodes)[0];
  }
}
