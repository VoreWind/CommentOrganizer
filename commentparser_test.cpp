#include <QString>

#include <doctest.h>

#include <commentparser.h>

SCENARIO("Rewriting single comments according to code style") {
  GIVEN("Source code with comments written with tags /*...*/") {
    QString wrong_source_code =
        "/**"
        "* @brief"
        " * other"
        " * @return source"
        " */"
        " buff_t *meth1() {"
        " }"
        ""
        "/*device function*/"
        " int mount_dev() {"
        "}"
        ""
        "/*lorem"
        "* ang*/"
        ""
        "int readFdev_MU() {"
        "}";

    WHEN("Run the wrong code through comment parser") {
      auto parsed_source_code =
          CommentParser::RewrieCommentsAccordingToCodeStyle(wrong_source_code);

      THEN(
          "Parsed source code will have comments with tags /*...*/ replaced "
          "by comments starting with // ") {
        QString right_source_code =
            "  /// @brief other.\n  "
            "/// @return source. buff_t *meth1() { }  // Device function. int "
            "mount_dev() {}  // Lorem* ang.int readFdev_MU() {}";
        REQUIRE(parsed_source_code.toStdString() ==
                right_source_code.toStdString());
      }
    }
  }

  GIVEN("Simple single-line code enclosed in /*...*/") {
    QString wrong_source_code = "/*mycool test comment*/";

    WHEN("Run the wrong code through comment parser") {
      auto parsed_source_code =
          CommentParser::RewrieCommentsAccordingToCodeStyle(wrong_source_code);
      THEN(
          "Parsed source code will have comments with tags /*...*/ replaced "
          "by comments starting with // ") {
        QString right_source_code = "  // Mycool test comment.";
        REQUIRE(parsed_source_code.toStdString() ==
                right_source_code.toStdString());
      }
    }
  }

  GIVEN("Simple multi-line code enclosed in /*...*/") {
    QString wrong_source_code = "/*mycool\n * test comment*/";

    WHEN("Run the wrong code through comment parser") {
      auto parsed_source_code =
          CommentParser::RewrieCommentsAccordingToCodeStyle(wrong_source_code);
      THEN(
          "Parsed source code will have comments with tags /*...*/ replaced "
          "by comments starting with // ") {
        QString right_source_code = "  // Mycool test comment.";
        REQUIRE(parsed_source_code.toStdString() ==
                right_source_code.toStdString());
      }
    }
  }
}
