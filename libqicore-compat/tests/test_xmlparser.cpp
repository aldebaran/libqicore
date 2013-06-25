/**
* @author Aldebaran Robotics
* Aldebaran Robotics (c) 2012 All Rights Reserved
*/

#include <gtest/gtest.h>

#include <qi/application.hpp>
#include <qi/log.hpp>

#include <qicore-compat/model/choregrapheprojectmodel.hpp>

#include <qicore-compat/model/boxinterfacemodel.hpp>
#include <qicore-compat/model/resourcemodel.hpp>
#include <qicore-compat/model/inputmodel.hpp>
#include <qicore-compat/model/outputmodel.hpp>
#include <qicore-compat/model/bitmapmodel.hpp>
#include <qicore-compat/model/choicemodel.hpp>
#include <qicore-compat/model/contentsmodel.hpp>
#include <qicore-compat/model/contentmodel.hpp>

#include <qicore-compat/model/animationmodel.hpp>
#include <qicore-compat/model/actuatorlistmodel.hpp>
#include <qicore-compat/model/actuatorcurvemodel.hpp>
#include <qicore-compat/model/keymodel.hpp>
#include <qicore-compat/model/tangentmodel.hpp>

#include <qicore-compat/model/behaviorsequencemodel.hpp>
#include <qicore-compat/model/behaviorlayermodel.hpp>
#include <qicore-compat/model/behaviorkeyframemodel.hpp>

#include <qicore-compat/model/flowdiagrammodel.hpp>
#include <qicore-compat/model/linkmodel.hpp>
#include <qicore-compat/model/boxinstancemodel.hpp>
#include <qicore-compat/model/parametervaluemodel.hpp>

std::string valid_boxinterface;

TEST(XmlParser, LoadXARFile)
{
  qi::ChoregrapheProjectModel xar(valid_boxinterface);
  //Parse XAR File
  ASSERT_TRUE(xar.loadFromFile());

  EXPECT_EQ(xar.name(),          "behavior/xar/tests");
  EXPECT_EQ(xar.formatVersion(), "4");

  qi::BoxInstanceModelPtr rootBox = xar.rootBox();
  ASSERT_TRUE(rootBox);

  EXPECT_EQ(rootBox->name(), "root");
  EXPECT_EQ(rootBox->id(),   -1);
  EXPECT_EQ(rootBox->x(),     0);
  EXPECT_EQ(rootBox->y(),     0);
  EXPECT_EQ(rootBox->path(), "test_boxinterface.xml");
  EXPECT_EQ(rootBox->parametersValue().size(), 0);
  EXPECT_EQ(rootBox->behaviorPath(), valid_boxinterface);

  qi::BoxInterfaceModelPtr rootBoxInterface = rootBox->interface();
  ASSERT_TRUE(rootBoxInterface);
  EXPECT_EQ(rootBoxInterface->path(), rootBox->path());
  EXPECT_EQ(rootBoxInterface->uuid(), "");
  EXPECT_EQ(rootBoxInterface->boxVersion(), "1.0.0.0");
  EXPECT_EQ(rootBoxInterface->name(), "root");
  EXPECT_EQ(rootBoxInterface->tooltip(), "Root box of Choregraphe's project. Highest level possible.");
  EXPECT_EQ(rootBoxInterface->plugin(), "None");
  EXPECT_EQ(rootBoxInterface->formatVersion(), "4");

  std::list<qi::BitmapModelPtr> bitmaps = rootBoxInterface->bitmaps();
  ASSERT_EQ(bitmaps.size(), 1);

  qi::BitmapModelPtr bitmap = bitmaps.front();
  ASSERT_TRUE(bitmap);
  EXPECT_EQ(bitmap->path(), "media/images/box/root.png");

  std::list<qi::ResourceModelPtr> resources = rootBoxInterface->resources();
  ASSERT_EQ(resources.size(), 2);

  qi::ResourceModelPtr resource = resources.front();
  ASSERT_TRUE(resource);
  EXPECT_EQ(resource->name(), "Head2");
  EXPECT_EQ(resource->lockTypeString(), "Lock");
  EXPECT_EQ(resource->lockType(), qi::ResourceModel::LockType_Lock);
  EXPECT_EQ(resource->timeout(), 0);

  std::list<qi::ParameterModelPtr> parameters = rootBoxInterface->parameters();
  ASSERT_EQ(parameters.size(), 4);

  qi::ParameterModelPtr parameter = parameters.front();
  ASSERT_TRUE(parameter);
  EXPECT_EQ(parameter->metaProperty().name(), "File path");
  EXPECT_EQ(parameter->inheritsFromParent(), true);
  EXPECT_EQ(qi::Signature(parameter->metaProperty().signature()).isConvertibleTo(qi::ParameterModel::signatureRessource()), 1.0f);
  EXPECT_EQ(parameter->defaultValue().toString().empty(), true);
  EXPECT_EQ(parameter->customChoice(), false);
  EXPECT_EQ(parameter->password(), false);
  EXPECT_EQ(parameter->tooltip(),  "Path of the file (including its name) which is going to be sent on the box output.");
  EXPECT_EQ(parameter->metaProperty().uid(), 4);

  qi::InputModelMap inputs = rootBoxInterface->inputs();
  ASSERT_EQ(inputs.size(), 3);

  qi::InputModelPtr input = inputs.at(1);
  ASSERT_TRUE(input);
  EXPECT_EQ(input->metaMethod().name(), "onLoad");
  EXPECT_EQ(qi::Signature(input->metaMethod().parametersSignature()).isConvertibleTo(qi::Signature("_")), 0.95f);
  EXPECT_EQ(input->nature(), qi::InputModel::InputNature_OnLoad);
  EXPECT_EQ(input->stmValueName(), "");
  EXPECT_EQ(input->inner(), true);
  EXPECT_EQ(input->metaMethod().description(), "Signal sent when diagram is loaded.");
  EXPECT_EQ(input->metaMethod().uid(), 1);


  qi::OutputModelMap outputs = rootBoxInterface->outputs();
  ASSERT_EQ(outputs.size(), 1);

  qi::OutputModelPtr output = outputs[4];
  ASSERT_TRUE(output);
  EXPECT_EQ(output->metaSignal().name(), "onStopped");
  EXPECT_EQ(qi::Signature(output->metaSignal().parametersSignature()).isConvertibleTo(qi::Signature("_")), 0.95f);
  EXPECT_EQ(output->nature(), qi::OutputModel::OutputNature_Stopped);
  EXPECT_EQ(output->inner(), false);
  EXPECT_EQ(output->tooltip(), "Signal sent when box behavior is finished.");
  EXPECT_EQ(output->metaSignal().uid(), 4);

  qi::ContentsModelPtr contents = rootBoxInterface->contents();
  ASSERT_TRUE(contents);
  EXPECT_EQ(contents->contents().size(), 3);
  qi::ContentModelPtr content = contents->contents().front();
  ASSERT_TRUE(content);
  EXPECT_EQ(content->checksum(), "");
  EXPECT_EQ(content->path(), "test_bhs.bhs");
  EXPECT_EQ(content->type(), qi::ContentModel::ContentType_BehaviorSequence);

  qi::AnimationModel *anim = rootBox->content(qi::ContentModel::ContentType_Animation).ptr<qi::AnimationModel>();
  ASSERT_TRUE(anim);
  EXPECT_EQ(anim->path(), "test_anim.anim");
  EXPECT_EQ(anim->fps(), 25);
  EXPECT_EQ(anim->startFrame(), 0);
  EXPECT_EQ(anim->endFrame(), -1);
  EXPECT_EQ(anim->resourcesAcquisition(), qi::AnimationModel::MotionResourcesHandler_Passive);
  EXPECT_EQ(anim->size(), 300);
  EXPECT_EQ(anim->formatVersion(), "4");

  qi::ActuatorListModelPtr actuatorList = anim->actuatorList();
  ASSERT_TRUE(actuatorList);
  EXPECT_EQ(actuatorList->model(), "None");

  std::list<qi::ActuatorCurveModelPtr> actuatorsCurve = actuatorList->actuatorsCurve();
  ASSERT_EQ(actuatorsCurve.size(), 25);
  qi::ActuatorCurveModelPtr actuatorCurve = actuatorsCurve.front();
  ASSERT_TRUE(actuatorCurve);
  EXPECT_EQ(actuatorCurve->name(), "value");
  EXPECT_EQ(actuatorCurve->actuator(), "RAnkleRoll");
  EXPECT_EQ(actuatorCurve->recordable(), true);
  EXPECT_EQ(actuatorCurve->mute(), false);
  EXPECT_EQ(actuatorCurve->unit(), qi::ActuatorCurveModel::UnitType_Undefined);
  EXPECT_EQ(actuatorCurve->lastKeyFrame(), 28);

  std::map<int, qi::KeyModelPtr> keys = actuatorCurve->keys();
  ASSERT_EQ(keys.size(), 2);
  qi::KeyModelPtr key = keys.begin()->second;
  ASSERT_TRUE(key);
  EXPECT_EQ(key->frame(), 23);
  EXPECT_EQ(key->value(), -1.4f);
  EXPECT_EQ(key->smooth(), false);
  EXPECT_EQ(key->symmetrical(), false);

  qi::TangentModelPtr tangentLeft = key->leftTangent();
  qi::TangentModelPtr tangentRight = key->rightTangent();
  ASSERT_TRUE(tangentLeft);
  ASSERT_TRUE(tangentRight);
  qi::TangentModelPtr tangent = tangentRight;
  EXPECT_EQ(tangent->side(), qi::TangentModel::Side_Right);
  EXPECT_EQ(tangent->interpType(), qi::TangentModel::InterpolationType_Bezier);
  EXPECT_EQ(tangent->ordinateParam(), 7.0f);
  EXPECT_EQ(tangent->abscissaParam(), 4.0f);

  qi::FlowDiagramModel *flowdiagram = rootBox->content(qi::ContentModel::ContentType_FlowDiagram).ptr<qi::FlowDiagramModel>();
  ASSERT_TRUE(flowdiagram);
  EXPECT_EQ(flowdiagram->path(), "test_fld.fld");
  EXPECT_EQ(flowdiagram->scale(), 59.4604f);
  EXPECT_EQ(flowdiagram->formatVersion(), "4");

  std::list<qi::LinkModelPtr> links = flowdiagram->links();
  ASSERT_EQ(links.size(), 38);
  qi::LinkModelPtr link = links.front();
  ASSERT_TRUE(link);
  EXPECT_EQ(link->inputTowner(), 0);
  EXPECT_EQ(link->indexOfInput(), 4);
  EXPECT_EQ(link->outputTowner(), 26);
  EXPECT_EQ(link->indexOfOutput(), 4);

  qi::BoxInstanceModelMap boxes = flowdiagram->boxsInstance();
  EXPECT_EQ(boxes.size(), 2);

  qi::BehaviorSequenceModel *behaviorSequence = rootBox->content(qi::ContentModel::ContentType_BehaviorSequence).ptr<qi::BehaviorSequenceModel>();
  ASSERT_TRUE(behaviorSequence);
  EXPECT_EQ(behaviorSequence->path(), "test_bhs.bhs");
  EXPECT_EQ(behaviorSequence->fps(), 25);
  EXPECT_EQ(behaviorSequence->startFrame(), 0);
  EXPECT_EQ(behaviorSequence->endFrame(), -1);
  EXPECT_EQ(behaviorSequence->size(), 300);
  EXPECT_EQ(behaviorSequence->formatVersion(), "4");

  std::list<qi::BehaviorLayerModelPtr> layers = behaviorSequence->behaviorsLayer();
  ASSERT_EQ(layers.size(), 2);
  qi::BehaviorLayerModelPtr layer = layers.front();
  ASSERT_TRUE(layer);
  EXPECT_EQ(layer->name(), "behavior_layer2");
  EXPECT_EQ(layer->mute(), false);

  std::list<qi::BehaviorKeyFrameModelPtr> keysFrame = layer->behaviorsKeyFrame();
  ASSERT_EQ(keysFrame.size(), 1);
  qi::BehaviorKeyFrameModelPtr keyFrame = keysFrame.front();
  ASSERT_TRUE(keyFrame);
  EXPECT_EQ(keyFrame->name(), "Say : Pssst");
  EXPECT_EQ(keyFrame->index(), 76);
  EXPECT_EQ(keyFrame->bitmap(), "");
  EXPECT_EQ(keyFrame->path(), "B2_1_Say_Pssst0.fld");
  ASSERT_TRUE(keyFrame->diagram());
}


TEST(XmlParser, Bitmap)
{
  qi::Bitmap bitmap("box.png");

  ASSERT_EQ(bitmap.path(), "box.png");
  bitmap.setPath("b.png");
  ASSERT_EQ(bitmap.path(), "b.png");
}

TEST(XmlParser, Ressource)
{
  qi::ResourceModel resource("test", qi::ResourceModel::LockType_Lock, 0);

  ASSERT_EQ(resource.name(), "test");
  resource.setName("test2");
  ASSERT_EQ(resource.name(), "test2");

  ASSERT_EQ(resource.lockType(), qi::ResourceModel::LockType_Lock);
  ASSERT_EQ(resource.lockTypeString(), "Lock");

  resource.setLockType(qi::ResourceModel::LockType_StopOnDemand);
  ASSERT_EQ(resource.lockType(), qi::ResourceModel::LockType_StopOnDemand);
  ASSERT_EQ(resource.lockTypeString(), "Stop on demand");

  resource.setLockType(qi::ResourceModel::LockType_CallbackOnDemand);
  ASSERT_EQ(resource.lockType(), qi::ResourceModel::LockType_CallbackOnDemand);
  ASSERT_EQ(resource.lockTypeString(), "Callback on demand");

  resource.setLockType(qi::ResourceModel::LockType_PauseOnDemand);
  ASSERT_EQ(resource.lockType(), qi::ResourceModel::LockType_PauseOnDemand);
  ASSERT_EQ(resource.lockTypeString(), "Pause on demand");

  resource.setLockType(qi::ResourceModel::LockType_Error);
  ASSERT_EQ(resource.lockType(), qi::ResourceModel::LockType_Error);
  ASSERT_EQ(resource.lockTypeString(), "");

  ASSERT_EQ(resource.timeout(), 0);
  resource.setTimeout(42);
  ASSERT_EQ(resource.timeout(), 42);
}

TEST(XmlParser, Parameter)
{
  qi::ParameterModel parameter("Test", 0.0, 0.0, 600.0, false, false, false, "tooltip", 1);
  EXPECT_TRUE(parameter.isValid());

  EXPECT_EQ(parameter.getChoices().size(), 0);
  EXPECT_TRUE(parameter.addChoice(qi::ChoiceModelPtr(new qi::ChoiceModel(1.0))));
  EXPECT_FALSE(parameter.addChoice(qi::ChoiceModelPtr(new qi::ChoiceModel(false))));
  EXPECT_EQ(parameter.getChoices().size(), 1);

  qi::ChoiceModelPtr choice = parameter.getChoices().front();
  EXPECT_EQ(choice->value().toDouble(), 1.0);

  //Test double value
  ASSERT_EQ(qi::Signature(parameter.metaProperty().signature()).isConvertibleTo(qi::Signature::fromType(qi::Signature::Type_Double)), 1.0f);
  EXPECT_EQ(parameter.defaultValue().toDouble(), 0.0);
  EXPECT_EQ(parameter.min().toDouble(), 0.0);
  EXPECT_EQ(parameter.max().toDouble(), 600.0);

  EXPECT_FALSE(parameter.setValue(-0.4));
  EXPECT_EQ(parameter.defaultValue().toDouble(), 0.0);
  EXPECT_TRUE(parameter.setValue(45.0));
  EXPECT_EQ(parameter.defaultValue().toDouble(), 45.0);

  //Test integer value
  parameter.setMetaProperty(1, "Test", "i");
  ASSERT_FALSE(parameter.isValid());
  ASSERT_TRUE(parameter.setValue(42, 3, 42));
  ASSERT_TRUE(parameter.isValid());
  ASSERT_EQ(qi::Signature(parameter.metaProperty().signature()).isConvertibleTo(qi::Signature::fromType(qi::Signature::Type_Int32)), 1.0f);
  ASSERT_EQ(parameter.defaultValue().toInt(), 42);
  ASSERT_EQ(parameter.min().toInt(),          3);
  ASSERT_EQ(parameter.max().toInt(),          42);

  //Test string value
  parameter.setMetaProperty(1, "Test", "s");
  ASSERT_TRUE(parameter.setValue("Foo"));
  EXPECT_EQ(qi::Signature(parameter.metaProperty().signature()).isConvertibleTo(qi::Signature::fromType(qi::Signature::Type_String)), 1.0f);
  EXPECT_EQ(parameter.defaultValue().toString(), "Foo");

  //Test bool value
  parameter.setMetaProperty(1, "Test", "b");
  parameter.setValue(true);
  ASSERT_EQ(qi::Signature(parameter.metaProperty().signature()).isConvertibleTo(qi::Signature::fromType(qi::Signature::Type_Bool)), 1.0f);
  EXPECT_TRUE(parameter.defaultValue().to<bool>());

  //Test resource
  parameter.setMetaProperty(1, "Test", "s<Resource>");
  parameter.setValue("file.ext");
  EXPECT_EQ(qi::Signature(parameter.metaProperty().signature()).toString(), qi::ParameterModel::signatureRessource().toString());
  EXPECT_EQ(parameter.defaultValue().toString(), "file.ext");

  //Test other attribute
  EXPECT_EQ(parameter.metaProperty().name(),"Test");
  EXPECT_FALSE(parameter.inheritsFromParent());
  EXPECT_FALSE(parameter.customChoice());
  EXPECT_FALSE(parameter.password());
  EXPECT_EQ(parameter.tooltip(),"tooltip");
  EXPECT_EQ(parameter.metaProperty().uid(), 1);
  parameter.setMetaProperty(2, "Test1", "s");
  parameter.setInheritsFromParent(true);
  parameter.setCustomChoice(true);
  parameter.setPassword(true);
  parameter.setTooltip("tooltip1");
  EXPECT_EQ(parameter.metaProperty().name(),"Test1");
  EXPECT_TRUE(parameter.inheritsFromParent());
  EXPECT_TRUE(parameter.customChoice());
  EXPECT_TRUE(parameter.password());
  EXPECT_EQ(parameter.tooltip(),"tooltip1");
  EXPECT_EQ(parameter.metaProperty().uid(), 2);
}


TEST(XmlParser, Choice)
{
  qi::ChoiceModel boolChoice(true);
  qi::ChoiceModel intChoice(42);
  qi::ChoiceModel doubleChoice(42.42);
  qi::ChoiceModel stringChoice(std::string("str"));

  EXPECT_EQ(qi::Signature(boolChoice.value().signature()).isConvertibleTo(qi::Signature::fromType(qi::Signature::Type_Bool)), 1.0f);
  EXPECT_TRUE(boolChoice.value().to<bool>());

  EXPECT_EQ(qi::Signature(intChoice.value().signature()).isConvertibleTo(qi::Signature::fromType(qi::Signature::Type_Int32)), 1.0f);
  EXPECT_EQ(intChoice.value().toInt(), 42);

  EXPECT_EQ(qi::Signature(doubleChoice.value().signature()).isConvertibleTo(qi::Signature::fromType(qi::Signature::Type_Double)), 1.0f);
  EXPECT_EQ(doubleChoice.value().toDouble(), 42.42);

  EXPECT_EQ(qi::Signature(stringChoice.value().signature()).isConvertibleTo(qi::Signature::fromType(qi::Signature::Type_String)), 1.0f);
  EXPECT_EQ(stringChoice.value().toString(), "str");

  boolChoice.setValue(42);
  EXPECT_EQ(qi::Signature(boolChoice.value().signature()).isConvertibleTo(qi::Signature::fromType(qi::Signature::Type_Int32)), 1.0f);
  EXPECT_EQ(boolChoice.value().toInt(), 42);

  boolChoice.setValue(42.42);
  EXPECT_EQ(qi::Signature(boolChoice.value().signature()).isConvertibleTo(qi::Signature::fromType(qi::Signature::Type_Double)), 1.0f);
  EXPECT_EQ(boolChoice.value().toDouble(), 42.42);

  boolChoice.setValue("42.42");
  EXPECT_EQ(qi::Signature(boolChoice.value().signature()).isConvertibleTo(qi::Signature::fromType(qi::Signature::Type_String)), 1.0f);
  EXPECT_EQ(boolChoice.value().toString(), "42.42");

  boolChoice.setValue(false);
  EXPECT_EQ(qi::Signature(boolChoice.value().signature()).isConvertibleTo(qi::Signature::fromType(qi::Signature::Type_Bool)), 1.0f);
  EXPECT_FALSE(boolChoice.value().to<bool>());

  boolChoice.setValue("file.ext");
  EXPECT_EQ(qi::Signature(boolChoice.value().signature()).isConvertibleTo(qi::ParameterModel::signatureRessource()), 1.0f);
  EXPECT_EQ(boolChoice.value().toString(), "file.ext");

}

TEST(XmlParser, Input)
{
  qi::InputModel input("Foo",
                       "i",
                       qi::InputModel::InputNature_OnLoad,
                       false,
                       "tooltip",
                       42);

  qi::InputModel inputstm("Foo2",
                          "(s)",
                          "stm_value_name",
                          false,
                          "tooltip",
                          42);

  EXPECT_EQ(input.metaMethod().name(), "Foo");
  EXPECT_EQ(qi::Signature(input.metaMethod().parametersSignature()).isConvertibleTo(qi::Signature::fromType(qi::Signature::Type_Int32)), 1.0f);
  EXPECT_EQ(input.nature(), qi::InputModel::InputNature_OnLoad);
  EXPECT_EQ(input.stmValueName(), "");
  EXPECT_EQ(input.inner(), false);
  EXPECT_EQ(input.metaMethod().description(), "tooltip");
  EXPECT_EQ(input.metaMethod().uid(), 42);

  EXPECT_EQ(inputstm.metaMethod().name(), "Foo2");
  EXPECT_GT(qi::Signature(inputstm.metaMethod().parametersSignature()).isConvertibleTo(qi::Signature("(s)")), 1.0f);
  EXPECT_EQ(inputstm.nature(), qi::InputModel::InputNature_STMValue);
  EXPECT_EQ(inputstm.stmValueName(), "stm_value_name");
  EXPECT_EQ(inputstm.inner(), false);
  EXPECT_EQ(inputstm.metaMethod().description(), "tooltip");
  EXPECT_EQ(inputstm.metaMethod().uid(), 42);

  input.setMetaMethod("Fa", "s", "Tooltip2", 314159);
  EXPECT_EQ(input.metaMethod().name(), "Fa");
  EXPECT_EQ(qi::Signature(input.metaMethod().parametersSignature()).isConvertibleTo(qi::Signature::fromType(qi::Signature::Type_String)), 1.0f);
  EXPECT_EQ(input.metaMethod().description(), "Tooltip2");
  EXPECT_EQ(input.metaMethod().uid(), 314159);

  input.setNature(qi::InputModel::InputNature_Event);
  EXPECT_EQ(input.nature(), qi::InputModel::InputNature_Event);

  input.setSTMValueName("grep");
  EXPECT_EQ(input.nature(), qi::InputModel::InputNature_STMValue);
  EXPECT_EQ(input.stmValueName(), "grep");

  input.setInner(true);
  EXPECT_EQ(input.inner(), true);
}

TEST(XmlParser, Output)
{
  qi::OutputModel output("Foo",
                         "i",
                         qi::OutputModel::OutputNature_Punctual,
                         false,
                         "tooltip",
                         42);

  EXPECT_EQ(output.metaSignal().name(), "Foo");
  EXPECT_EQ(qi::Signature(output.metaSignal().parametersSignature()).isConvertibleTo(qi::Signature::fromType(qi::Signature::Type_Int32)), 1.0f);
  EXPECT_EQ(output.nature(), qi::OutputModel::OutputNature_Punctual);
  EXPECT_EQ(output.inner(), false);
  EXPECT_EQ(output.tooltip(), "tooltip");
  EXPECT_EQ(output.metaSignal().uid(), 42);

  output.setMetaSignal("Fa", "s", 314159);
  EXPECT_EQ(output.metaSignal().name(), "Fa");
  EXPECT_EQ(qi::Signature(output.metaSignal().parametersSignature()).isConvertibleTo(qi::Signature::fromType(qi::Signature::Type_String)), 1.0f);
  EXPECT_EQ(output.metaSignal().uid(), 314159);

  output.setNature(qi::OutputModel::OutputNature_Stopped);
  EXPECT_EQ(output.nature(), qi::OutputModel::OutputNature_Stopped);

  output.setInner(true);
  EXPECT_EQ(output.inner(), true);

  output.setTooltip("Tooltip2");
  EXPECT_EQ(output.tooltip(), "Tooltip2");

}

TEST(XmlParser, Content)
{
  qi::ContentModel content(qi::ContentModel::ContentType_PythonScript, "main.py", "");
  EXPECT_EQ(content.type(), qi::ContentModel::ContentType_PythonScript);
  EXPECT_EQ(content.path(), "main.py");
  EXPECT_EQ(content.checksum(), "");

  content.setType(qi::ContentModel::ContentType_Animation);
  EXPECT_EQ(content.type(), qi::ContentModel::ContentType_Animation);

  content.setPath("main.anim");
  EXPECT_EQ(content.path(), "main.anim");

  content.setChecksum("1234");
  EXPECT_EQ(content.checksum(), "1234");

}

TEST(XmlParse, BoxInterface)
{
  qi::BoxInterfaceModel interface("box.xml",
                             "1",
                             "1",
                             "box",
                             "tooltip",
                             "45");

  EXPECT_EQ(interface.path(), "box.xml");
  EXPECT_EQ(interface.uuid(), "1");
  EXPECT_EQ(interface.boxVersion(), "1");
  EXPECT_EQ(interface.name(), "box");
  EXPECT_EQ(interface.tooltip(), "tooltip");
  EXPECT_EQ(interface.formatVersion(), "45");
  EXPECT_EQ(interface.plugin(), "None");
  EXPECT_EQ(interface.bitmaps().size(), 0);
  EXPECT_EQ(interface.resources().size(), 0);
  EXPECT_EQ(interface.parameters().size(), 0);
  EXPECT_EQ(interface.inputs().size(), 0);
  EXPECT_EQ(interface.outputs().size(), 0);
  EXPECT_TRUE(interface.contents());
  EXPECT_EQ(0, interface.contents()->contents().size());

  interface.setPath("box2.xml");
  EXPECT_EQ(interface.path(), "box2.xml");

  interface.setUuid("2");
  EXPECT_EQ(interface.uuid(), "2");

  interface.setBoxVersion("2");
  EXPECT_EQ(interface.boxVersion(), "2");

  interface.setName("box2");
  EXPECT_EQ(interface.name(), "box2");

  interface.setTooltip("tooltip2");
  EXPECT_EQ(interface.tooltip(), "tooltip2");

  interface.setFormatVersion("452");
  EXPECT_EQ(interface.formatVersion(), "452");

  interface.setPlugin("None2");
  EXPECT_EQ(interface.plugin(), "None2");

  interface.addBitmap(qi::BitmapModelPtr(new qi::Bitmap("toto")));
  EXPECT_EQ(interface.bitmaps().size(), 1);

  interface.addResource(qi::ResourceModelPtr(new qi::ResourceModel("toto", qi::ResourceModel::LockType_CallbackOnDemand, 0)));
  EXPECT_EQ(interface.resources().size(), 1);

  qi::ParameterModelPtr parameter(new qi::ParameterModel("Test", 0.0, 0.0, 600.0, false, false, false, "tooltip", 1));

  EXPECT_TRUE(interface.addParameter(parameter));
  EXPECT_EQ(interface.parameters().size(), 1);

  interface.addInput(qi::InputModelPtr(new qi::InputModel("Foo",
                                                          "s",
                                                          qi::InputModel::InputNature_OnLoad,
                                                          false,
                                                          "tooltip",
                                                          42)));
  EXPECT_EQ(interface.inputs().size(), 1);

  interface.addOutput(qi::OutputModelPtr(new qi::OutputModel("Foo",
                                                             "s",
                                                             qi::OutputModel::OutputNature_Punctual,
                                                             false,
                                                             "tooltip",
                                                             42)));
  EXPECT_EQ(interface.outputs().size(), 1);

  interface.addContent(qi::ContentModelPtr(new qi::ContentModel(qi::ContentModel::ContentType_PythonScript, "main.py", "")));
  EXPECT_TRUE(interface.contents());
  EXPECT_EQ(interface.contents()->contents().size(), 1);

}

TEST(XmlParse, Contents)
{
  qi::ContentsModel contents;

  EXPECT_EQ(contents.contents().size(), 0);
  contents.addContent(qi::ContentModelPtr(new qi::ContentModel(qi::ContentModel::ContentType_PythonScript, "main.py", "")));
  EXPECT_EQ(contents.contents().size(), 1);
}

TEST(XmlParse, Animation)
{

  qi::AnimationModel anim("anim.anim");

  EXPECT_EQ(anim.path(), "anim.anim");
  EXPECT_EQ(anim.fps(), 25);
  EXPECT_EQ(anim.startFrame(), 0);
  EXPECT_EQ(anim.endFrame(), -1);
  EXPECT_EQ(anim.size(), 0);
  EXPECT_EQ(anim.formatVersion(), "4");
  EXPECT_EQ(anim.resourcesAcquisition(), qi::AnimationModel::MotionResourcesHandler_Passive);
  EXPECT_FALSE(anim.actuatorList());

  anim.setPath("anim.a");
  EXPECT_EQ(anim.path(), "anim.a");

  anim.setFPS(20);
  EXPECT_EQ(anim.fps(), 20);

  anim.setStartFrame(1);
  EXPECT_EQ(anim.startFrame(), 1);

  anim.setEndFrame(2);
  EXPECT_EQ(anim.endFrame(), 2);

  anim.setResourcesAcquisition(qi::AnimationModel::MotionResourcesHandler_Aggressive);
  EXPECT_EQ(anim.resourcesAcquisition(), qi::AnimationModel::MotionResourcesHandler_Aggressive);

  anim.setSize(24);
  EXPECT_EQ(anim.size(), 24);

  anim.setFormatVersion("6");
  EXPECT_EQ(anim.formatVersion(), "6");

  anim.setActuatorList(qi::ActuatorListModelPtr( new qi::ActuatorListModel("Blitz")));
  EXPECT_TRUE(anim.actuatorList());
}

TEST(XmlParse, ActuatorList)
{
  qi::ActuatorListModel actuatorList("Loup");

  ASSERT_EQ(actuatorList.model(), "Loup");
  ASSERT_EQ(actuatorList.actuatorsCurve().size(), 0);

  actuatorList.setModel("Vayne");
  ASSERT_EQ(actuatorList.model(), "Vayne");

  actuatorList.addActuatorCurve(qi::ActuatorCurveModelPtr(new qi::ActuatorCurveModel("Grab", "Grab", true, true, qi::ActuatorCurveModel::UnitType_Degree)));
  ASSERT_EQ(actuatorList.actuatorsCurve().size(), 1);
}

TEST(XmlParser, ActuatorCurve)
{
  qi::ActuatorCurveModel curve("Grab", "Grab", true, false, qi::ActuatorCurveModel::UnitType_Degree);

  ASSERT_EQ(curve.name(), "Grab");
  ASSERT_EQ(curve.actuator(), "Grab");
  ASSERT_EQ(curve.recordable(), true);
  ASSERT_EQ(curve.mute(), false);
  ASSERT_EQ(curve.keys().size(), 0);
  ASSERT_EQ(curve.lastKeyFrame(), -1);

  curve.setName("Grab2");
  ASSERT_EQ(curve.name(), "Grab2");

  curve.setActuator("Grab1");
  ASSERT_EQ(curve.actuator(), "Grab1");

  curve.setRecordable(false);
  ASSERT_EQ(curve.recordable(), false);

  curve.setMute(true);
  ASSERT_EQ(curve.mute(), true);

  curve.addKey(qi::KeyModelPtr(new qi::KeyModel(12, 3.14159f, true, true)));
  ASSERT_EQ(curve.keys().size(), 1);
  ASSERT_EQ(curve.lastKeyFrame(), 12);

}

TEST(XmlParser, Key)
{
  qi::KeyModel key(1, 3.14159f, true, true);

  ASSERT_EQ(key.frame(), 1);
  ASSERT_EQ(key.value(), 3.14159f);
  ASSERT_EQ(key.smooth(), true);
  ASSERT_EQ(key.symmetrical(), true);
  ASSERT_TRUE(key.rightTangent());
  ASSERT_TRUE(key.leftTangent());

  key.setFrame(2);
  ASSERT_EQ(key.frame(), 2);

  key.setValue(42.42f);
  ASSERT_EQ(key.value(), 42.42f);

  key.setSmooth(false);
  ASSERT_EQ(key.smooth(), false);

  key.setSymmetrical(false);
  ASSERT_EQ(key.symmetrical(), false);

  qi::TangentModelPtr t1(new qi::TangentModel(qi::TangentModel::Side_Left, qi::TangentModel::InterpolationType_Bezier, 3.0, 1.0));
  qi::TangentModelPtr t2(new qi::TangentModel(qi::TangentModel::Side_Left, qi::TangentModel::InterpolationType_Bezier, 3.0, 1.0));

  ASSERT_FALSE(key.setTangents(t1, t2));
  t2->setSide(qi::TangentModel::Side_Right);
  ASSERT_TRUE(key.setTangents(t1, t2));

}

TEST(XmlParser, Tangent)
{
  qi::TangentModel tangent(qi::TangentModel::Side_Right, qi::TangentModel::InterpolationType_Bezier, 3.0, 1.0);

  ASSERT_EQ(tangent.side(), qi::TangentModel::Side_Right);
  ASSERT_EQ(tangent.interpType(), qi::TangentModel::InterpolationType_Bezier);
  ASSERT_EQ(tangent.abscissaParam(), 3.0f);
  ASSERT_EQ(tangent.ordinateParam(), 1.0f);

  tangent.setSide(qi::TangentModel::Side_Left);
  ASSERT_EQ(tangent.side(), qi::TangentModel::Side_Left);

  tangent.setInterpType(qi::TangentModel::InterpolationType_Constant);
  ASSERT_EQ(tangent.interpType(), qi::TangentModel::InterpolationType_Constant);

  tangent.setAbscissaParam(2.0);
  ASSERT_EQ(tangent.abscissaParam(), 2.0f);

  tangent.setOrdinateParam(4.0);
  ASSERT_EQ(tangent.ordinateParam(), 4.0f);
}

TEST(XmlParser, BehaviorSequence)
{
  qi::BehaviorSequenceModel seq("bhs.bhs", qi::BoxInstanceModelPtr(), 25, 0, 61, 3000, "42");

  ASSERT_EQ(seq.path(), "bhs.bhs");
  ASSERT_EQ(seq.fps(), 25);
  ASSERT_EQ(seq.startFrame(), 0);
  ASSERT_EQ(seq.endFrame(), 61);
  ASSERT_EQ(seq.size(), 3000);
  ASSERT_EQ(seq.formatVersion(), "42");
  ASSERT_EQ(seq.behaviorsLayer().size(), 0);

  seq.setPath("bhs2.bhs");
  ASSERT_EQ(seq.path(), "bhs2.bhs");

  seq.setFPS(27);
  ASSERT_EQ(seq.fps(), 27);

  seq.setStartFrame(1);
  ASSERT_EQ(seq.startFrame(), 1);

  seq.setEndFrame(60);
  ASSERT_EQ(seq.endFrame(), 60);

  seq.setSize(3001);
  ASSERT_EQ(seq.size(), 3001);

  seq.setFormatVersion("43");
  ASSERT_EQ(seq.formatVersion(), "43");

  seq.addBehaviorLayer(qi::BehaviorLayerModelPtr(new qi::BehaviorLayerModel("toto", true)));
  ASSERT_EQ(seq.behaviorsLayer().size(), 1);


}

TEST(XmlParser, BehaviorLayer)
{
  qi::BehaviorLayerModel layer("toto", true);

  ASSERT_EQ(layer.name(), "toto");
  ASSERT_EQ(layer.mute(), true);
  ASSERT_EQ(layer.behaviorsKeyFrame().size(), 0);

  layer.setName("aaa");
  ASSERT_EQ(layer.name(), "aaa");

  layer.setMute(false);
  ASSERT_EQ(layer.mute(), false);

  layer.addBehaviorKeyFrame(qi::BehaviorKeyFrameModelPtr( new qi::BehaviorKeyFrameModel("titi", 1, "png", "fld")));
  ASSERT_EQ(layer.behaviorsKeyFrame().size(), 1);
}

TEST(XmlParser, BehaviorKeyFrame)
{
  qi::BehaviorKeyFrameModel keyFrame("titi", 1, "png", "fld");

  ASSERT_EQ(keyFrame.name(), "titi");
  ASSERT_EQ(keyFrame.index(), 1);
  ASSERT_EQ(keyFrame.bitmap(), "png");
  ASSERT_EQ(keyFrame.path(), "fld");

  keyFrame.setName("bb");
  ASSERT_EQ(keyFrame.name(), "bb");

  keyFrame.setIndex(2);
  ASSERT_EQ(keyFrame.index(), 2);

  keyFrame.setBitmap("jpg");
  ASSERT_EQ(keyFrame.bitmap(), "jpg");

  keyFrame.setPath(".fld");
  ASSERT_EQ(keyFrame.path(), ".fld");
}

TEST(XmlParser, FlowDiagram)
{
  qi::FlowDiagramModel flowDiagram("d.fld", qi::BoxInstanceModelPtr(), 12.0, "53");

  ASSERT_EQ(flowDiagram.path(), "d.fld");
  ASSERT_EQ(flowDiagram.scale(), 12.0);
  ASSERT_EQ(flowDiagram.formatVersion(), "53");
  ASSERT_EQ(flowDiagram.links().size(), 0);
  ASSERT_EQ(flowDiagram.boxsInstance().size(), 0);

  flowDiagram.setPath("c.fld");
  ASSERT_EQ(flowDiagram.path(), "c.fld");

  flowDiagram.setScale(1.0);
  ASSERT_EQ(flowDiagram.scale(), 1.0);

  flowDiagram.setFormatVersion("4");
  ASSERT_EQ(flowDiagram.formatVersion(), "4");

  flowDiagram.addLink(qi::LinkModelPtr(new qi::LinkModel(1, 2, 3, 4)));
  ASSERT_EQ(flowDiagram.links().size(), 1);

  qi::BoxInterfaceModelPtr interface(new qi::BoxInterfaceModel("box.xml", "1", "1","box", "tooltip", "45"));

  flowDiagram.addBoxInstance(qi::BoxInstanceModelPtr(new qi::BoxInstanceModel("root", 1, 1, 1, interface)));
  ASSERT_EQ(flowDiagram.boxsInstance().size(), 1);
}

TEST(XmlParser, Link)
{
  qi::LinkModel link(1, 2, 3, 4);

  ASSERT_EQ(link.inputTowner(), 1);
  ASSERT_EQ(link.indexOfInput(), 2);
  ASSERT_EQ(link.outputTowner(), 3);
  ASSERT_EQ(link.indexOfOutput(), 4);

  link.setInputTowner(5);
  ASSERT_EQ(link.inputTowner(), 5);

  link.setIndexOfInput(6);
  ASSERT_EQ(link.indexOfInput(), 6);

  link.setOutputTowner(7);
  ASSERT_EQ(link.outputTowner(), 7);

  link.setIndexOfOutput(8);
  ASSERT_EQ(link.indexOfOutput(), 8);
}

TEST(XmlParser, BoxInstance)
{
  qi::BoxInterfaceModelPtr interface(new qi::BoxInterfaceModel("box.xml", "1", "1","box", "tooltip", "45"));
  qi::ParameterModelPtr parameter(new qi::ParameterModel("Test", 0.0, 0.0, 600.0, false, false, false, "tooltip", 0));

  interface->addParameter(parameter);

  qi::BoxInstanceModel box("root", 1, 2, 3, interface);
  EXPECT_TRUE(box.isValid());
  EXPECT_EQ(box.name(), "root");
  EXPECT_EQ(box.id(), 1);
  EXPECT_EQ(box.x(), 2);
  EXPECT_EQ(box.y(), 3);
  EXPECT_EQ(box.parametersValue().size(), 0);
  EXPECT_EQ(box.path(), "box.xml");
  EXPECT_EQ(box.interface(), interface);
  //qi::GenericValuePtr value = box.parameter("Test");
  //EXPECT_EQ(0.0, value.toDouble());

  box.setName("ra");
  EXPECT_EQ(box.name(), "ra");

  box.setId(2);
  EXPECT_EQ(box.id(), 2);

  box.setX(3);
  EXPECT_EQ(box.x(), 3);

  box.setY(4);
  EXPECT_EQ(box.y(), 4);

  qi::ParameterValueModelPtr param(new qi::ParameterValueModel(0, "Toto"));
  EXPECT_FALSE(box.addParameterValue(param));

  param->setValueDefault(12.5);
  EXPECT_TRUE(box.addParameterValue(param));
  EXPECT_EQ(box.parametersValue().size(), 1);

  qi::BoxInterfaceModelPtr interface2(new qi::BoxInterfaceModel("ra.xml", "1", "1","box", "tooltip", "45"));
  box.setBoxInterface(interface2);
  EXPECT_EQ(box.parametersValue().size(), 0);
  EXPECT_EQ(box.interface(), interface2);
  EXPECT_EQ(box.path(), "ra.xml");
}

TEST(XmlParser, ParameterValue)
{
  qi::ParameterValueModel value(0, 12.5);

  EXPECT_EQ(value.id(), 0);
  value.setId(1);
  EXPECT_EQ(value.id(), 1);

  EXPECT_EQ(qi::Signature(value.value().signature()).isConvertibleTo(qi::Signature::fromType(qi::Signature::Type_Double)), 1.0f);
  EXPECT_EQ(value.value().toDouble(), 12.5);

  value.setValueDefault(true);
  EXPECT_EQ(qi::Signature(value.value().signature()).isConvertibleTo(qi::Signature::fromType(qi::Signature::Type_Bool)), 1.0f);
  EXPECT_EQ(value.value().to<bool>(), true);

  value.setValueDefault(1);
  EXPECT_EQ(qi::Signature(value.value().signature()).isConvertibleTo(qi::Signature::fromType(qi::Signature::Type_Int32)), 1.0f);
  EXPECT_EQ(value.value().toInt(), 1);

  value.setValueDefault("test");
  EXPECT_EQ(qi::Signature(value.value().signature()).isConvertibleTo(qi::Signature::fromType(qi::Signature::Type_String)), 1.0f);
  EXPECT_EQ(value.value().toString(), "test");

  value.setValueDefault("file.ext");
  EXPECT_EQ(qi::Signature(value.value().signature()).isConvertibleTo(qi::ParameterModel::signatureRessource()), 1.0f);
  EXPECT_EQ(value.value().toString(), "file.ext");
}

int main(int argc, char **argv)
{
  qi::Application(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);

  if(argc < 2)
  {
    std::cerr << "Usage: test_xmlparser /path/to/valid_box_interface_format.xml"
              << std::endl;

    return 2;
  }

  valid_boxinterface = std::string(argv[1]);
  return RUN_ALL_TESTS();
}
