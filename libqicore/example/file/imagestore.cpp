#include "imagestore.hpp"

qiLogCategory("test.qi.fileExample.imageStore");

#include <map>

#include <boost/filesystem/operations.hpp>

QI_TYPE_INTERFACE(alice::ImageStore);

namespace alice
{
  class ImageStoreImpl
    : public ImageStore
  {
  public:
    ImageStoreImpl()
    {
    }

    ~ImageStoreImpl()
    {
      for (FileRegistry::iterator it = _fileRegistry.begin(); it != _fileRegistry.end(); ++it)
      {
        boost::filesystem::remove(it->second);
      }
    }

    void storeImage(qi::FilePtr imageFile, std::string name)
    {
      // Note that ideally this would be implemented in an asynchronous way,
      // but for simplicity we will do it synchronously.

      // First, make a local copy in a temporary files directory:
      const std::string tempFilePath = generateTemporaryFilePath();

      // This call will block until the end because it returns a FutureSync.
      qi::copyToLocal(imageFile, tempFilePath);

      // We now have a local copy of the remote file,
      // so we don't need the remote access anymore.
      imageFile.reset();

      // Now we can work with the local file.
      storeFileInDatabase(name, tempFilePath);
    }

    qi::FilePtr getImage(std::string name)
    {
      const std::string fileLocation = findFileLocation(name);

      // Now we can open it and provide it to the user for reading.
      return qi::openLocalFile(fileLocation);
    }

  private:
    typedef std::map<std::string, std::string> FileRegistry;
    std::map<std::string, std::string> _fileRegistry;

    std::string generateTemporaryFilePath()
    {
      static int idx = 0;
      static const std::string PATH_PREFIX = "./temp_image_file_";
      static const std::string PATH_SUFFIX = ".data";

      std::stringstream newPath;
      newPath << PATH_PREFIX << idx << PATH_SUFFIX;
      ++idx;
      return newPath.str();
    }

    void storeFileInDatabase(const std::string& name, const std::string& path)
    {
      //...fake it
      _fileRegistry[name] = path;
    }

    std::string findFileLocation(const std::string& name)
    {
      return _fileRegistry[name];
    }
  };
  QI_REGISTER_OBJECT(ImageStore, storeImage, getImage);
  QI_REGISTER_IMPLEMENTATION(ImageStore, ImageStoreImpl);

  ImageStorePtr getImageStore()
  {
    static ImageStorePtr imageStore = boost::make_shared<ImageStoreImpl>();
    return imageStore;
  }
}