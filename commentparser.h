#ifndef COMMENTPARSER_H
#define COMMENTPARSER_H

class QString;

class CommentParser {
 public:
  static QString RewrieCommentsAccordingToCodeStyle(const QString &file_text);

 private:
  static QString RearrangeMultipleStringComments(const QString &comment);
};

#endif  // COMMENTPARSER_H
