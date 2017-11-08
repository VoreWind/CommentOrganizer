#include "commentparser.h"

#include <QDebug>
#include <QRegExp>
#include <QString>
#include <QStringList>

QStringList CommentParser::FindCommentsMatchingRegexp(QString edited_file_text,
                                                      QString reg_exp_text) {
  QRegExp reg_exp(reg_exp_text);
  reg_exp.setMinimal(true);
  int first_index = reg_exp.indexIn(edited_file_text);

  QStringList captured_comments;

  while (first_index != -1) {
    QStringList locally_captured_list = reg_exp.capturedTexts();
    first_index = reg_exp.indexIn(edited_file_text, first_index + 1);

    captured_comments.append(locally_captured_list);
  }

  return captured_comments;
}

QString CommentParser::RewriteCommentsAccordingToCodeStyle(
    const QString &file_text) {
  QString edited_file_text = file_text;

  edited_file_text = RearrangeCommentsFound(
      "[[\\w \\(\\)]*\\/\\*.*\\*\\/[\\w \\(\\)]+", edited_file_text,
      *RemoveCommentFromTheMiddleOfLine);

  edited_file_text = RearrangeCommentsFound(
      "\\/\\*[*!].*\\*\\/", edited_file_text, *RearrangeDoxyGenComments);

  edited_file_text = RearrangeCommentsFound("\\/\\*.*\\*\\/", edited_file_text,
                                            *RearrangeMultipleStringComments);

  return edited_file_text;
}

QString CommentParser::RemoveCommentFromTheMiddleOfLine(
    const QString &line_with_comment) {
  QString edited_comment = line_with_comment;
  QRegExp actual_inline_comment_regexp("\\/\\*.*\\*\\/");
  int comment_position =
      actual_inline_comment_regexp.indexIn(line_with_comment);
  QString comment = actual_inline_comment_regexp.capturedTexts().at(0);
  edited_comment.remove(comment);
  return edited_comment;
}

QString CommentParser::RearrangeCommentsFound(
    const QString &regex_string,
    const QString &text,
    QString (*rearrangement_method)(const QString &)) {
  QString edited_text = text;
  QStringList inline_comments =
      FindCommentsMatchingRegexp(edited_text, regex_string);

  for (const auto &captured_comment : inline_comments) {
    QString edited_comment = rearrangement_method(captured_comment);
    edited_text.replace(captured_comment, edited_comment);
  }
  return edited_text;
}

QString CommentParser::RearrangeMultipleStringComments(const QString &comment) {
  QString edited_comment = comment;

  edited_comment = CleanCommentsClutter(edited_comment);

  QRegExp line_breaker_regexp("\n[ \\*]*");

  edited_comment.replace(line_breaker_regexp, " ");

  if (!edited_comment.endsWith(".")) {
    edited_comment.append(".");
  }

  QString first_letter = edited_comment.left(1);
  edited_comment.replace(0, 1, first_letter.toUpper());

  edited_comment.prepend("// ");
  return edited_comment;
}

QString CommentParser::RearrangeDoxyGenComments(const QString &comment) {
  QString edited_comment = comment;
  edited_comment = CleanCommentsClutter(edited_comment);

  edited_comment = RemoveDecorationsFromStartOfString(edited_comment);

  QString split_up_token;
  if (edited_comment.contains("@")) {
    split_up_token = "@";
  } else {
    split_up_token = "\\";
  }

  QStringList comment_strings =
      SplitStringKeepingSeparartor(edited_comment, split_up_token);

  edited_comment.clear();
  for (auto comment_string : comment_strings) {
    comment_string = comment_string.trimmed();
    if (!comment_string.isEmpty()) {
      QString first_letter = comment_string.left(1);
      comment_string.replace(0, 1, first_letter.toUpper());
      comment_string.replace("\n", " ");
      comment_string.prepend("/// ");

      if (!comment_string.endsWith(".")) {
        comment_string.append(".");
      }
      edited_comment.append(comment_string + "\n");
    }
  }
  edited_comment.chop(1);
  return edited_comment;
}

QString CommentParser::RemoveDecorationsFromStartOfString(
    const QString &comment) {
  QStringList comment_lines = comment.split("\n");
  QString edited_comment;
  for (auto comment_line : comment_lines) {
    QRegExp space_regexp("[ \\*!]+");
    int first_occurence = space_regexp.indexIn(comment_line);
    comment_line.replace(first_occurence, space_regexp.matchedLength(), " ");
    edited_comment.append(comment_line + "\n");
  }
  edited_comment.chop(1);

  return edited_comment;
}

QString CommentParser::CleanCommentsClutter(const QString &comment) {
  QString cleaned_comment = comment;
  cleaned_comment.remove("/*");
  cleaned_comment.remove("*/");

  return cleaned_comment.trimmed();
}

QStringList CommentParser::SplitStringKeepingSeparartor(
    const QString &string, const QString &separator) {
  QStringList split_strings;
  int separator_count = string.count(separator);
  for (int i = 0; i <= separator_count; ++i) {
    split_strings.append(
        string.section(separator, i, i, QString::SectionIncludeLeadingSep));
  }

  return split_strings;
}
