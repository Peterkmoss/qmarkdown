#include "resgen.h"
#include "helpers.h"

#if __has_include(<filesystem>)
#include <filesystem>
#define FILESYSTEM filesystem
#elif __has_include(<experimental/filesystem>)
#include <experimental/filesystem>
#define FILESYSTEM std::experimental::filesystem
#endif

#include <QString>
#include <fstream>

using namespace std;

void read_directory(ofstream *outfile, QString path, bool *status) {
  for (auto it = FILESYSTEM::recursive_directory_iterator(path.toStdString(), FILESYSTEM::directory_options::skip_permission_denied); it != FILESYSTEM::recursive_directory_iterator(); ++it) {
    if (it.depth() > 3) continue;
    const auto entry = *it;
    const QString e_path = entry.path().c_str();
    const QString image_types[] = {".png", ".jpg", ".jpeg", ".gif"};
    for (QString type : image_types) {
      const size_t p_length = e_path.size();
      const size_t t_length = type.size();
      if (p_length >= t_length) {
        if (e_path.endsWith(type)) {
          *status = true;
          *outfile << "<file>" << e_path.toStdString() << "</file>" << endl;
        }
      }
    }
  }
}

void res_gen() {
  ofstream outfile("images.qrc");

  outfile << "<!DOCTYPE RCC><RCC version='1.0'>" << endl;
  outfile << "<qresource>" << endl;

  bool image_exists = false;
  read_directory(&outfile, ".", &image_exists);

  outfile << "</qresource>" << endl;
  outfile << "</RCC>" << endl;

  outfile.close();

  // Clean up
  if (image_exists)
    system(("rcc -binary images.qrc -o " + QRC_FILE).c_str());
  system("rm -f images.qrc");
}
