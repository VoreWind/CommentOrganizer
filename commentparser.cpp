#include "commentparser.h"

#include <QDebug>
#include <QRegExp>
#include <QString>
#include <QStringList>

const QString CommentParser::kPossiblePunctuation = ".,;:!?{}[]()";

QStringList CommentParser::FindCommentsMatchingRegexp(QString edited_file_text,
                                                      QString reg_exp_text,
                                                      bool is_minimal_regrexp) {
  QRegExp reg_exp(reg_exp_text);
  reg_exp.setMinimal(is_minimal_regrexp);
  int first_index = reg_exp.indexIn(edited_file_text);

  QStringList captured_comments;

  while (first_index != -1) {
    QString locally_captured_comment = reg_exp.cap(0);
    first_index = reg_exp.indexIn(
        edited_file_text, first_index + locally_captured_comment.count());

    captured_comments.append(locally_captured_comment);
  }

  return captured_comments;
}

QString
CommentParser::RewriteCommentsAccordingToCodeStyle(const QString &file_text) {
  QString edited_file_text = file_text;

  edited_file_text.replace(QRegExp("\n *\\/\\/ *\n"), "\n");

  FixExternCBracket(edited_file_text);

  edited_file_text =
      RearrangeCommentsFound("\n[ \t]+\\w+[^\n]+[ \t]+\\/\\/[ \t]+[^\n]+",
                             edited_file_text, *FixSideComments, false);
  edited_file_text =
      RearrangeCommentsFound("(\n *\\/\\/[^\n]+)+\n", edited_file_text,
                             *FixProperlyMarkedComments, false);

  edited_file_text = RearrangeCommentsFound(
      "[[\\w \\(\\)]*\\/\\*[^\n]*\\*\\/[^\\n]+", edited_file_text,
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
    const QString &regex_string, const QString &text,
    QString (*rearrangement_method)(const QString &), bool is_minimal_regexp) {
  QString edited_text = text;
  QStringList inline_comments =
      FindCommentsMatchingRegexp(edited_text, regex_string, is_minimal_regexp);

  for (const auto &captured_comment : inline_comments) {
    QString edited_comment = rearrangement_method(captured_comment);
    edited_text.replace(captured_comment, edited_comment);
  }
  return edited_text;
}

QString CommentParser::RearrangeMultipleStringComments(const QString &comment) {
  QString edited_comment = comment;

  edited_comment = CleanCommentsClutter(edited_comment);
  if (!edited_comment.contains("extern \"C\" {") &&
      !edited_comment.contains("namespace ")) {
    QRegExp line_breaker_regexp("\n[ \\*]*");

    edited_comment.replace(line_breaker_regexp, " ");
    RemoveHashSymbolsFromComment(edited_comment);

    if (!IsCommentEndingInPunctuation(edited_comment)) {
      edited_comment.append(".");
    }

    QString first_letter = edited_comment.left(1);
    edited_comment.replace(0, 1, first_letter.toUpper());
  }

  edited_comment.prepend("// ");
  return edited_comment;
}

void CommentParser::ParseDoxyGenStyleComments(QString &edited_comment,
                                              const QString &join_token) {
  QString split_up_token;
  if (edited_comment.contains("@")) {
    split_up_token = "@";
  } else {
    split_up_token = "\\\\";
  }

  QRegExp single_character_token(split_up_token + ".[^\\s]");
  single_character_token.setMinimal(true);
  QStringList comment_strings =
      SplitStringKeepingSeparartor(edited_comment, single_character_token);

  edited_comment.clear();
  for (auto comment_string : comment_strings) {
    comment_string = comment_string.trimmed();
    if (!comment_string.isEmpty()) {
      RemoveStarFromDoxyGenParametersNames(comment_string);

      QString first_letter = comment_string.left(1);
      comment_string.replace(0, 1, first_letter.toUpper());
      comment_string.replace("\n", " ");
      comment_string.prepend(join_token + " ");

      if (!IsCommentEndingInPunctuation(comment_string)) {
        comment_string.append(".");
      }

      edited_comment.append(comment_string + "\n");
    }
  }
  edited_comment.chop(1);
}

void CommentParser::RemoveStarFromDoxyGenParametersNames(
    QString &comment_string) {
  QRegExp parameter_with_stars("^ *[\\\\@][\\w]+ +\\*+");
  if (comment_string.contains(parameter_with_stars)) {
    QRegExp star_regexp("\\*+");
    int star_position = star_regexp.indexIn(comment_string);
    QString star_bit = star_regexp.cap(0);
    comment_string.replace(star_position, star_bit.count(), "");
  }
}

void CommentParser::RemoveHashSymbolsFromComment(QString &comment_string) {
  if (comment_string.contains("##")) {
    while (comment_string.contains("##")) {
      comment_string.remove("##");
    }

    while (comment_string.contains("  ")) {
      comment_string.replace("  ", " ");
    }
  }
}

void CommentParser::FixExternCBracket(QString &comment_string) {
  int closing_bracket_position = FindExternCBracket(comment_string);
  if (closing_bracket_position == -1) {
    return;
  }

  QString substring =
      comment_string.right(comment_string.count() - closing_bracket_position);
  QString bracket_string = substring.section("\n", 1, 3).trimmed();
  const QString proper_bracket_string = "}  // extern "
                                        "\"C\""
                                        "\n  #endif  // __cplusplus";

  if (bracket_string != proper_bracket_string) {
    comment_string.replace(bracket_string, proper_bracket_string);
  }
}

int CommentParser::FindExternCBracket(const QString &comment_string) {
  return comment_string.indexOf(QRegExp("#ifdef __cplusplus\n *\\}"));
}

bool CommentParser::IsCommentEndingInPunctuation(
    const QString &edited_comment) {
  QString last_character = edited_comment.right(1);
  return kPossiblePunctuation.contains(last_character);
}

QString CommentParser::RearrangeDoxyGenComments(const QString &comment) {
  QString edited_comment = comment;
  edited_comment = CleanCommentsClutter(edited_comment);

  edited_comment = RemoveDecorationsFromStartOfString(edited_comment);
  RemoveHashSymbolsFromComment(edited_comment);

  ParseDoxyGenStyleComments(edited_comment, "///");
  return edited_comment;
}

QString CommentParser::FixProperlyMarkedComments(const QString &comment) {
  QString edited_comment = comment;
  QRegExp token_finder_regexp("\\/+");
  token_finder_regexp.indexIn(edited_comment);
  QString join_token = token_finder_regexp.cap(0);
  QRegExp splitter("\\/[\\/]+\\s*");

  edited_comment =
      edited_comment.split(splitter, QString::SkipEmptyParts).join(" ");
  RemoveHashSymbolsFromComment(edited_comment);
  ParseDoxyGenStyleComments(edited_comment, join_token);
  edited_comment.append("\n");
  edited_comment.prepend("\n");
  return edited_comment;
}

QString CommentParser::FixSideComments(const QString &string_with_comment) {
  const QString comment_splitter = "//";
  int comment_position = string_with_comment.indexOf(comment_splitter);
  QString proper_comment =
      string_with_comment.right(string_with_comment.count() - comment_position);

  QRegExp comment_clutter_regexp("\\/+ +");
  comment_clutter_regexp.indexIn(proper_comment);
  QString comment_clutter = comment_clutter_regexp.cap(0);
  proper_comment = proper_comment.trimmed();
  RemoveHashSymbolsFromComment(proper_comment);

  const QString old_comment = proper_comment;
  QString first_letter = proper_comment.mid(comment_clutter.count(), 1);
  proper_comment.replace(comment_clutter.count(), 1, first_letter.toUpper());

  if (!IsCommentEndingInPunctuation(proper_comment)) {
    proper_comment.append(".");
  }

  QString edited_string_with_comment = string_with_comment;
  edited_string_with_comment.replace(old_comment, proper_comment);
  return edited_string_with_comment;
}

QString
CommentParser::RemoveDecorationsFromStartOfString(const QString &comment) {
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

QStringList
CommentParser::SplitStringKeepingSeparartor(const QString &string,
                                            const QRegExp &separator) {
  QStringList split_strings;
  int separator_count = string.count(separator);
  for (int i = 0; i <= separator_count; ++i) {
    split_strings.append(
        string.section(separator, i, i, QString::SectionIncludeLeadingSep));
  }

  return split_strings;
}
