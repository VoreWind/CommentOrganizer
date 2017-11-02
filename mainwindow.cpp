#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <commentparser.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  connect(ui->clean_button, SIGNAL(pressed()), SLOT(CleanComments()));
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::CleanComments() {
  QString text = ui->text_edit->toPlainText();
  CommentParser::IdentifyCommentTypes(text);
}
