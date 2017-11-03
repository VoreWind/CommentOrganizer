#include "commentparser.h"

#include <QDebug>
#include <QRegExp>
#include <QString>
#include <QStringList>

QString CommentParser::RewrieCommentsAccordingToCodeStyle(
    const QString &file_text) {
  QString edited_file_text = file_text;
  QRegExp multi_line_comment_reg_exp("\\/\\*.*\\*\\/");
  multi_line_comment_reg_exp.setMinimal(true);
  int first_index = multi_line_comment_reg_exp.indexIn(edited_file_text);
  QStringList captured_comments;

  while (first_index != -1) {
    QStringList locally_captured_list =
        multi_line_comment_reg_exp.capturedTexts();
    first_index =
        multi_line_comment_reg_exp.indexIn(edited_file_text, first_index + 1);

    captured_comments.append(locally_captured_list);
  }
  qDebug() << captured_comments.count();

  for (const auto &captured_comment : captured_comments) {
    QString edited_comment = RearrangeMultipleStringComments(captured_comment);
    edited_file_text.replace(captured_comment, edited_comment);
  }
  return edited_file_text;
}

QString CommentParser::RearrangeMultipleStringComments(const QString &comment) {
  QString edited_comment = comment;
  edited_comment.remove("/*");
  edited_comment.remove("*/");

  edited_comment = edited_comment.trimmed();

  QRegExp line_breaker_regexp("\n[ \\*]*");

  edited_comment.remove(line_breaker_regexp);

  if (!edited_comment.endsWith(".")) {
    edited_comment.append(".");
  }
  QString first_letter = edited_comment.left(1);
  edited_comment.replace(0, 1, first_letter.toUpper());
  edited_comment.prepend("  // ");
  qDebug() << comment << edited_comment;
  return edited_comment;
}

QString CommentParser::RearrangeDoxyGenComments(const QString &comment) {}
