#ifndef COMMENTPARSER_H
#define COMMENTPARSER_H

class QString;

class CommentParser {

public:
  static QString RearrangeOneStringComments(const QString &comment);
  static void IdentifyCommentTypes(const QString &file);
};

#endif // COMMENTPARSER_H
