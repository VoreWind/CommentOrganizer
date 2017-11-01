#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "doctest.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  connect(ui->clean_button, SIGNAL(pressed()), SLOT(CleanComments()));
}

int MainWindow::Factorial(int number) {
  return number > 1 ? Factorial(number - 1) * number : 1;
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::CleanComments() {
  QString text = ui->text_edit->toPlainText();
}

TEST_CASE("testing the factorial function") {
  CHECK(MainWindow::Factorial(0) == 1);
  CHECK(MainWindow::Factorial(1) == 1);
  CHECK(MainWindow::Factorial(2) == 2);
  CHECK(MainWindow::Factorial(3) == 6);
  CHECK(MainWindow::Factorial(10) == 3628800);
}
