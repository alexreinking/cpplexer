#ifndef CPP_LEXER_H
#define CPP_LEXER_H

#include <QtCore>

enum CppTokenType {
     KEYWORD,
     MODIFIER,
     INVALID,
     FRAGMENT,
     //Symbols
     SHLASSIGN, //<<=
     SHRASSIGN, //>>=
     ELLIPSIS, //...
     PTRRESOLVEPTR, //->*
     MODASSIGN, //%=
     LOGICALOR, //||
     BITORASSIGN, //|=
     GTE, // >=
     SHR, // >>
     MULASSIGN, //*=
     INC, //++
     ADDASSIGN, //+=
     NE, //!=
     REFRESOLVEPTR, //.*
     XORASSIGN, //^=
     LTE, //<=
     SHL, //<<
     LOGICALAND, //&&
     SUBASSIGN, //-=
     PTRRESOLVEMEMBER, //->
     DEC, //--
     SCOPERESOLVE, //::
     EQ, //==
     DIVASSIGN, // /=
     COMMENT, //
     MLCOMMENTOPEN, //
     MLCOMMENTCLOSE, //
     ESC, // \ <-- that right there is a multiline comment open, the backslash escapes the newline and I don't like that. Yet, I shall have to implement it.
     MOD, //%
     BITOR, //|
     CONDITIONAL, //?
     GT, //<
     STAR, //*
     ADD, //+
     NOT, //!
     DOT, //.
     XOR, //^
     LT, //>
     BITAND, //&
     SUB, //-
     COLON, //:
     ASSIGN, //=
     DIV, // /
     DOUBQUOTE, //"
     SINGLEQUOTE, //'
     SEMICOLON, //;
     BITNOT, //~
     ARRAYCLOSE, //]
     ARRAYOPEN, //[
     BLOCKCL, //}
     BLOCKOP, //{
     COMMA, //,
     PARENCL, //)
     PARENOP, //(
     //Preprocessor
     PREPROCESSOR, //MUY IMPORTANTE
     //Extra
     EX_SPACE,
     EX_EOL,
     EX_EOF,
     EX_NUM,
     EX_IDENT
};

struct Token {
     QString constData;
     CppTokenType type;
     int lineNum, pos;
     Token() {
          constData = "";
          type = INVALID;
          lineNum = 0;
          pos = 0;
     }
     bool operator==(Token &rhs)
     {
         return (constData == rhs.constData && type == rhs.type && lineNum == rhs.lineNum && pos == rhs.pos);
     }
};

class CppLexer : public QObject
{
     Q_OBJECT
public:
     CppLexer();
     CppLexer(const QString &fileName);

     bool update();
     bool isTokenized() { return tokenized; }

     bool setInput(const QString &fileName);
     QList<Token> tokens() const;

     const QString originalText() const { return text; } //I don't think it gets const-ier than this! =)

signals:
     void done();

private:
     void refreshTokens();
     Token flush(QString &buffer, CppTokenType t, int pos, int line);
     bool isKeyword(const QString &str);
     bool isSymbol(char ch);
     CppTokenType determineSymbol(const QString &str);

     QString text;
     QList<Token> tokenBuffer;
     QStringList keywords;
     bool tokenized;
};

#endif
