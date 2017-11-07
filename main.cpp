#include <QApplication>
#include <QTextCodec>

#include "mainwindow.h"

#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  doctest::Context context;
  int res = context.run();  // run

  if (context.shouldExit()) {
    return res;
  }

  auto cyrillic_codec = QTextCodec::codecForName("UTF-8");
//  QTextCodec::setCodecForTr(cyrillic_codec);
//  QTextCodec::setCodecForLocale(cyrillic_codec);
//  QTextCodec::setCodecForCStrings(cyrillic_codec);

  MainWindow w;
  w.show();

  return a.exec();
}
