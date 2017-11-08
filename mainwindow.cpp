#include <mainwindow.h>
#include <ui_mainwindow.h>

#include <QDebug>
#include <QDirIterator>
#include <QFileDialog>
#include <QProcess>

#include <commentparser.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  connect(ui->clean_button, SIGNAL(pressed()), SLOT(CleanComments()));

  connect(ui->source_directory_button, SIGNAL(pressed()),
          SLOT(SelectSourceFolder()));
  connect(ui->destination_directory_button, SIGNAL(pressed()),
          SLOT(SelectDestinationFolder()));
  connect(ui->clean_all_button, SIGNAL(pressed()),
          SLOT(CleanAllTextInFolder()));
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::CleanComments() {
  QString text = ui->text_edit->toPlainText();
  text = CommentParser::RewriteCommentsAccordingToCodeStyle(text);

  ui->text_edit->setPlainText(text);
}

void MainWindow::SelectSourceFolder() {
  QString dir = QFileDialog::getExistingDirectory(
      this, tr("Source Directory"), "/home",
      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  ui->source_directory_line->setText(dir);
  ui->destination_directory_line->setText(dir);
}

void MainWindow::SelectDestinationFolder() {
  QString dir = QFileDialog::getExistingDirectory(
      this, tr("Destination Directory"), "/home/",
      QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  ui->destination_directory_line->setText(dir);
}

void MainWindow::CleanAllTextInFolder() {
  QDirIterator::IteratorFlag flag = QDirIterator::NoIteratorFlags;
  if (ui->is_recursive_check->isChecked()) {
    flag = QDirIterator::Subdirectories;
  }

  QDirIterator it(ui->source_directory_line->text(), QStringList() << "*.h"
                                                                   << "*.c",
                  QDir::Files, flag);

  while (it.hasNext()) {
    QFile source_file(it.next());
    source_file.open(QIODevice::ReadOnly);
    QString file_contents = source_file.readAll();
    source_file.close();

    QFileInfo source_file_info(source_file.fileName());

    QString subdirectory_path = source_file_info.dir().path().mid(
        ui->source_directory_line->text().count());

    QString parsed_file;
    parsed_file = CleanCommentsFromString(file_contents);

    if (parsed_file.isEmpty()) {
      qDebug() << source_file.fileName();
      continue;
    }

    QString destination_file_name = ui->destination_directory_line->text() +
                                    subdirectory_path + "/" +
                                    source_file_info.fileName();
    QFile destination_file(destination_file_name);

    destination_file.open(QIODevice::WriteOnly);

    QTextStream out(&destination_file);
    out << parsed_file;
    destination_file.close();

    QProcess::startDetached("clang-format-3.8", {"-i", destination_file_name});
  }
}

QString MainWindow::CleanCommentsFromString(const QString &text) {
  return CommentParser::RewriteCommentsAccordingToCodeStyle(text);
}
