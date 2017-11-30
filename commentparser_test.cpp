#include <QString>

#include <doctest.h>

#include <commentparser.h>

SCENARIO("Rewriting single comments according to code style") {
  GIVEN("Simple single-line code enclosed in /*...*/") {
    QString wrong_source_code = "/*mycool test comment*/";

    WHEN("Run the wrong code through comment parser") {
      auto parsed_source_code =
          CommentParser::RewriteCommentsAccordingToCodeStyle(wrong_source_code);
      THEN(
          "Parsed source code will have comments with tags /*...*/ replaced "
          "by comments starting with // ") {
        QString right_source_code = "// Mycool test comment.";
        REQUIRE(parsed_source_code.toStdString() ==
                right_source_code.toStdString());
      }
    }
  }

  GIVEN("Simple multi-line code enclosed in /*...*/") {
    QString wrong_source_code = "/*mycool\n * test comment*/";

    WHEN("Run the wrong code through comment parser") {
      auto parsed_source_code =
          CommentParser::RewriteCommentsAccordingToCodeStyle(wrong_source_code);

      THEN(
          "Parsed source code will have comments with tags /*...*/ replaced "
          "by comments starting with // ") {
        QString right_source_code = "// Mycool test comment.";
        REQUIRE(parsed_source_code.toStdString() ==
                right_source_code.toStdString());
      }
    }
  }

  GIVEN("Comment enclosed in /*...*/ in the middle of valid code line") {
    QString wrong_source_code = "int FunShit(/*cool comment */ blah);";

    WHEN("Run the wrong code through comment parser") {
      auto parsed_source_code =
          CommentParser::RewriteCommentsAccordingToCodeStyle(wrong_source_code);
      THEN("Parsed source code will have comments with tags /*...*/ removed ") {
        QString right_source_code = "int FunShit( blah);";
        REQUIRE(parsed_source_code.toStdString() ==
                right_source_code.toStdString());
      }
    }
  }

  GIVEN("Comment structured like /*!...*/ ") {
    QString wrong_source_code = "/*!mycool test !comment*/";

    WHEN("Run the wrong code through comment parser") {
      auto parsed_source_code =
          CommentParser::RewriteCommentsAccordingToCodeStyle(wrong_source_code);
      THEN(
          "Parsed source code will have /// at the start (IDE colors them "
          "blue)") {
        QString right_source_code = "/// Mycool test !comment.";
        REQUIRE(parsed_source_code.toStdString() ==
                right_source_code.toStdString());
      }
    }
  }

  GIVEN(
      "Doxygen styled multiline comments structured like /*! ... \n * ... "
      "*/ ") {
    QString wrong_source_code =
        "/*!\\param   *comment1\n * \\param   comment2*/";

    WHEN("Run the wrong code through comment parser") {
      auto parsed_source_code =
          CommentParser::RewriteCommentsAccordingToCodeStyle(wrong_source_code);
      THEN(
          "Parsed source code will have /// at the start (IDE colors them and "
          "the stars in the middle will be gone") {
        QString right_source_code =
            "/// \\param   *comment1.\n/// \\param   comment2.";
        REQUIRE(parsed_source_code.toStdString() ==
                right_source_code.toStdString());
      }
    }
  }

  GIVEN(
      "Doxygen styled multiline comments structured like /*! ... \n * ... "
      "*/ not starting with \\smth block") {
    QString wrong_source_code = "/*!param   *comment1\n * \\param   comment2*/";

    WHEN("Run the wrong code through comment parser") {
      auto parsed_source_code =
          CommentParser::RewriteCommentsAccordingToCodeStyle(wrong_source_code);
      THEN(
          "Parsed source code will have /// at the start (IDE colors them and "
          "the stars in the middle will be gone") {
        QString right_source_code =
            "/// Param   *comment1.\n/// \\param   comment2.";
        REQUIRE(parsed_source_code.toStdString() ==
                right_source_code.toStdString());
      }
    }
  }

  GIVEN(
      "Doxygen styled multiline comments structured like /** @param... \n * "
      "@param... */ ") {
    QString wrong_source_code =
        "/*! @param   *comment1\n * @param   comment2*/";

    WHEN("Run the wrong code through comment parser") {
      auto parsed_source_code =
          CommentParser::RewriteCommentsAccordingToCodeStyle(wrong_source_code);
      THEN(
          "Parsed source code will have /// at the start (IDE colors them and "
          "the stars in the middle will be gone") {
        QString right_source_code =
            "/// @param   *comment1.\n/// @param   comment2.";
        REQUIRE(parsed_source_code.toStdString() ==
                right_source_code.toStdString());
      }
    }
  }

  GIVEN(
      "Doxygen comment with a mixture of multi-lettered and single-lettered "
      "tags") {
    QString wrong_source_code =
        "/*!param   *comment1\n * \\param   comment2 \\a thingy*/";

    WHEN("Run the wrong code through comment parser") {
      auto parsed_source_code =
          CommentParser::RewriteCommentsAccordingToCodeStyle(wrong_source_code);
      THEN("The comment gets split only at multi-lined tags") {
        QString right_source_code =
            "/// Param   *comment1.\n/// \\param   comment2 \\a thingy.";
        REQUIRE(parsed_source_code.toStdString() ==
                right_source_code.toStdString());
      }
    }
  }

  GIVEN("extern C comment at the end of pure C block") {
    QString wrong_source_code =
        "#ifdef __cplusplus\n} /* extern \"C\" { */\n#endif";

    WHEN("Run the wrong code through comment parser") {
      auto parsed_source_code =
          CommentParser::RewriteCommentsAccordingToCodeStyle(wrong_source_code);
      THEN("This text in the comments should not be affected by parser") {
        QString right_source_code =
            "#ifdef __cplusplus\n} // extern \"C\" {\n#endif";
        REQUIRE(parsed_source_code.toStdString() ==
                right_source_code.toStdString());
      }
    }
  }
}

// TODO add tags wit ha single letter
