#ifndef COMMENTPARSER_H
#define COMMENTPARSER_H

class QString;
class QRegExp;
class QStringList;

class CommentParser {
 public:
  static QString RewriteCommentsAccordingToCodeStyle(const QString &file_text);

 private:
  static QString RemoveCommentFromTheMiddleOfLine(
      const QString &line_with_comment);
  static QString RearrangeMultipleStringComments(const QString &comment);
  static QString RearrangeDoxyGenComments(const QString &comment);
  static QString FixProperlyMarkedComments(const QString &comment);

  static QString CleanCommentsClutter(const QString &comment);

  static QStringList SplitStringKeepingSeparartor(const QString &string,
                                                  const QRegExp &separator);
  static QStringList FindCommentsMatchingRegexp(QString edited_file_text,
                                                QString reg_exp_text,
                                                bool is_minimal_regrexp = true);

  static QString RemoveDecorationsFromStartOfString(const QString &comment);

  static QString RearrangeCommentsFound(
      const QString &regex_string,
      const QString &text,
      QString (*rearrangement_method)(const QString &),
      bool is_minimal_regexp = true);
  static void ParseDoxyGenStyleComments(QString &edited_comment,
                                        const QString &join_token);
  static bool IsCommentEndingInPunctuation(const QString &edited_comment);

  static const QString kPossiblePunctuation;
  static void RemoveStarFromDoxyGenParametersNames(QString &comment_string);
};

#endif  // COMMENTPARSER_H
