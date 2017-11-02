#include <QApplication>
#include <QTextCodec>

#include "mainwindow.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  auto cyrillic_codec = QTextCodec::codecForName("UTF-8");
  QTextCodec::setCodecForTr(cyrillic_codec);
  QTextCodec::setCodecForLocale(cyrillic_codec);
  QTextCodec::setCodecForCStrings(cyrillic_codec);

  MainWindow w;
  w.show();

  return a.exec();
}
