#include <qicore/file.hpp>
#include <qi/session.hpp>
#include <qi/application.hpp>
#include <qi/type/dynamicobjectbuilder.hpp>
#include <testsession/testsessionpair.hpp>
#include <testsession/testsession.hpp>

#include "imagestore.hpp"

qiLogCategory("test.qi.fileExample");

namespace bob
{
  void printTranferProgress(double progress)
  {
    qiLogInfo() << ">>>> File Transfer Progress = " << (progress * 100.0) << "%";
  }

  void workOnImageFile(const std::string& imageFilePath)
  {
    qiLogInfo() << "Working on image file at " << imageFilePath << " ...";
    // we fake working on it...
    boost::this_thread::sleep_for(boost::chrono::milliseconds(300));
    qiLogInfo() << "Working on image file at " << imageFilePath << " - DONE";
  }

  void storeImage(alice::ImageStorePtr imageStore, const std::string& name, const std::string& imageFilePath)
  {
    qiLogInfo() << "Storing image file at " << imageFilePath << " into the ImageStore...";

    // First open the file with read-only shareable access.
    qi::FilePtr file = qi::openLocalFile(imageFilePath);

    // Now we can share reading access to this file.
    imageStore->storeImage(file, name);

    qiLogInfo() << "Storing image file at " << imageFilePath << " into the ImageStore - DONE";
  }

  void processImage(alice::ImageStorePtr imageStore, const std::string& imageFile, const std::string& imageFilePath)
  {
    // We acquire read-only access to the file and retrieve it locally.
    qi::FilePtr file = imageStore->getImage(imageFile);
    qi::copyToLocal(file, imageFilePath);

    // We don't need the remote access anymore.
    file.reset();

    // Now work on the file located at `fileLocation`.
    workOnImageFile(imageFilePath);
  }

  void processImageWithProgress(alice::ImageStorePtr imageStore, const std::string& imageFile, const std::string& imageFilePath)
  {
    // We acquire read-only access to the file.
    qi::FilePtr file = imageStore->getImage(imageFile);

    // We prepare the operation without launching it yet:
    qi::FileOperationPtr fileOp = qi::prepareCopyToLocal(file, imageFilePath);

    // We want to see the progress so we plug a logging function.
    fileOp->progress.connect(&printTranferProgress);

    // Launch the copy and wait for it to end before continuing.
    fileOp->start().wait(); // Don't wait for futures in real code, you should .connect() instead.

    // We don't need the remote access anymore.
    file.reset();

    // Now work on the file located at `fileLocation`.
    workOnImageFile(imageFilePath);
  }

  void doSomeWork(qi::SessionPtr clientSession, const std::string& imagePath)
  {
    qi::AnyObject aliceServices = clientSession->service("AliceServices");
    alice::ImageStorePtr imageStore = aliceServices.call<alice::ImageStorePtr>("imageStore");
    assert(imageStore);
    storeImage(imageStore, "bobimage", imagePath);
    processImage(imageStore, "bobimage", "./tempfile");
    processImageWithProgress(imageStore, "bobimage", "./tempfile");
  }
}

void setupAliceServer(qi::SessionPtr serverSession)
{
  qi::DynamicObjectBuilder objectBuilder;
  objectBuilder.advertiseMethod("imageStore", &alice::getImageStore);
  qi::AnyObject aliceServices = objectBuilder.object();
  serverSession->registerService("AliceServices", aliceServices);
}

int main(int argc, char** argv)
{
  ::TestMode::forceTestMode(TestMode::Mode_SD);
  qi::Application app(argc, argv);

  if (argc != 2)
    return EXIT_FAILURE;

  const qi::Path imageFilePath(argv[1]);
  TestSessionPair sessionPair;
  setupAliceServer(sessionPair.server());
  bob::doSomeWork(sessionPair.client(), imageFilePath);

  return EXIT_SUCCESS;
}