/*
** Author(s):
**  - Cedric GESTES <gestes@aldebaran-robotics.com>
**
** Copyright (C) 2014 Aldebaran Robotics
*/

#include <iostream>
#include <qi/log.hpp>
#include <qilang/node.hpp>
#include <qilang/formatter.hpp>
#include <qi/os.hpp>
#include "formatter_p.hpp"
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "cpptype.hpp"
#include <qitype/anyvalue.hpp>
#include <qicore/behavior.hpp>
#include <stack>

qiLogCategory("qigen.behaviorModel");


namespace qilang {

static inline std::pair<std::string, std::string> splitString2(const std::string& s, char sep)
{
  size_t p = s.find_first_of(sep);
  if (p == s.npos)
    throw std::runtime_error("separator not found");
  return std::make_pair(s.substr(0, p), s.substr(p+1));
}


// Construct a BehaviorModel
class QiLangGenBehaviorModel:
    public StmtNodeVisitor
{
public:

  virtual void acceptStmt(const StmtNodePtr &node)           { node->accept(this); }

  std::vector<qi::BehaviorModel> modelsList;
  std::stack<qi::BehaviorModel::Node*> nodesStack;
  qi::BehaviorModel* currentModel;
  qi::BehaviorModel::Node* currentNode;

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


  virtual void accept(const NodePtr& node) {
    switch (node->kind()) {
      case NodeKind_Stmt:
        acceptStmt(boost::dynamic_pointer_cast<StmtNode>(node));
        break;
      case NodeKind_ConstData:
      case NodeKind_Decl:
      case NodeKind_Expr:
      case NodeKind_TypeExpr:
        throw std::runtime_error("Not implemented");
    }
  }

protected:

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

      // iterate over all statements
      for (unsigned int i = 0; i < node->values.size(); ++i) {
        acceptStmt(node->values.at(i));
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
        acceptStmt(node->values.at(i));
      }
      currentNode = nodesStack.top();
      nodesStack.pop();
    }
  }

  void visitStmt(PropertyDefNode *node) {


    qi::AnyValue data = qilang::toAnyValue(node->data);

    //TODO check for already existing
    currentNode->parameters[node->name] = data;
  }


  void visitStmt(AtNode* node) {

    std::string transition_uid = node->receiver + "|" + node->sender;
    qi::BehaviorModel::Transition& currentTransition = currentModel->transitions[transition_uid];
    currentTransition.uid = transition_uid;
    currentTransition.src = splitString2(node->sender, '.');
    currentTransition.dst = splitString2(node->receiver, '.');
  }

  void visitStmt(VarDefNode* node) {
    throw std::runtime_error("unimplemented");
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
