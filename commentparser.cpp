#include "commentparser.h"

#include <QDebug>
#include <QRegExp>
#include <QString>
#include <QStringList>

void CommentParser::IdentifyCommentTypes(const QString &file) {
  QRegExp single_line_comment_reg_exp("\\/\\*[^\n]*\\*\\/");
  single_line_comment_reg_exp.setMinimal(true);
  int first_index = file.indexOf(single_line_comment_reg_exp);
  QStringList captured = single_line_comment_reg_exp.capturedTexts();
  for (auto caught : captured) {
    RearrangeOneStringComments(caught);
  }
}

QString CommentParser::RearrangeOneStringComments(const QString &comment) {
  QString edited_comment = comment;
  edited_comment.remove("/*");
  edited_comment.remove("*/");
  edited_comment = edited_comment.trimmed();
  if (!edited_comment.endsWith(".")) {
    edited_comment.append(".");
  }
  QString first_letter = edited_comment.left(1);
  edited_comment.replace(0, 1, first_letter.toUpper());
  edited_comment.prepend("  // ");
  qDebug() << comment << edited_comment;
  return edited_comment;
}
