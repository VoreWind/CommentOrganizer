#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = 0);
  static int Factorial(int number);
  ~MainWindow();

 private slots:
  void CleanComments();

 private:
  Ui::MainWindow *ui;
};

#endif  // MAINWINDOW_H
