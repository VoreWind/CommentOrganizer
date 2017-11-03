#ifndef COMMENTPARSER_H
#define COMMENTPARSER_H

class QString;
class QRegExp;
class QStringList;

class CommentParser {
 public:
  static QString RewrieCommentsAccordingToCodeStyle(const QString &file_text);

 private:
  static QString RearrangeMultipleStringComments(const QString &comment);
  static QString RearrangeDoxyGenComments(const QString &comment);
  static QString CleanCommentsClutter(const QString &comment);

  static QStringList FindCommentsMatchingRegexp(QString edited_file_text,
                                                QString reg_exp_text);
};

#endif  // COMMENTPARSER_H
