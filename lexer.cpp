#include <QtCore>
#include <QtWidgets/QtWidgets>
#include <QtDebug>
#include "lexer.h"

//I don't handle trigraphs or digraphs, because they suck.

CppLexer::CppLexer()
{
    text = "";
    keywords << "asm" << "auto" << "bool" << "break" << "case" << "catch" << "char" << "class" << "const" << "const_cast" << "continue" << "default" << "delete" << "do" << "double"
            << "dynamic_cast" << "else" << "enum" << "explicit" << "export" << "extern" << "false" << "float" << "for" << "friend" << "goto" << "if" << "inline" << "int" << "long"
            << "mutable" << "namespace" << "new" << "operator" << "private" << "protected" << "public" << "register" << "reinterpret_cast" << "return" << "short" << "signed"
            << "sizeof" <<"static" << "static_cast" << "struct" << "switch" << "template" << "this" << "throw" << "true" << "try" << "typedef" << "typeid" << "typename" << "union"
            << "unsigned" << "using" << "virtual" << "void" << "volatile"  <<"wchar_t" << "while"; //The actual meanings of all of these keywords are determined by the parser's interpretation.
}

CppLexer::CppLexer(const QString &fileName)
{
    setInput(fileName);
}

bool CppLexer::update()
{
    refreshTokens();
    return tokenized;
}

bool CppLexer::setInput(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    QTextStream in(&file);
    text = in.readAll();
    return true;
}

QList<Token> CppLexer::tokens() const
{
    if(tokenized) return tokenBuffer;
    return QList<Token>();
}

inline bool CppLexer::isKeyword(const QString &str)
{
    return keywords.contains(str);
}

inline bool isSpace(char c)
{
    return (c == ' ' || c == '\t');
}

bool isNum(QString &str)
{
    QRegExpValidator flt(QRegExp("[0123456789]*\\.[0123456789]+f?"), 0);
    QRegExpValidator hex(QRegExp("0[xX][0123456789abcdefABCDEF]+[ulUL]*"), 0);
    QRegExpValidator oct(QRegExp("0[01234567]+"), 0);
    QRegExpValidator dec(QRegExp("[0123456789]+[ulUL]*"), 0);
    int pos = 0;
    if(flt.validate(str, pos) == QValidator::Acceptable)
        return true;
    else if(hex.validate(str, pos) == QValidator::Acceptable)
        return true;
    else if(oct.validate(str, pos) == QValidator::Acceptable)
        return true;
    else if(dec.validate(str, pos) == QValidator::Acceptable)
        return true;
    return false;
}

CppTokenType CppLexer::determineSymbol(const QString &str)
{
    char c, c1, c2, i = 0;
    c = c1 = c2 = 0; //Null bytes, FTW
    while(i < str.size() && c2 == 0)
    {
        switch (i) {
               case 0: c = str.at(i).toLatin1(); break;
               case 1: c1 = str.at(i).toLatin1(); break;
               case 2: c2 = str.at(i).toLatin1(); break;
               default: break;
               }
        i++;
    }
    //Now on to the long haul...
    if(c == '<' && c1 == '<' && c2 == '=')
        return SHLASSIGN;
    else if(c == '>' && c1 == '>' && c2 == '=')
        return SHRASSIGN;
    else if(c == '.' && c1 == '.' && c2 == ',')
        return ELLIPSIS;
    else if(c == '-' && c1 == '>' && c2 == '*')
        return PTRRESOLVEPTR;
    else if(c == '%' && c1 == '=' && c2 == 0)
        return MODASSIGN;
    else if(c == '|' && c1 == '|' && c2 == 0)
        return LOGICALOR;
    else if(c == '|' && c1 == '=' && c2 == 0)
        return BITORASSIGN;
    else if(c == '>' && c1 == '=' && c2 == 0)
        return GTE;
    else if(c == '>' && c1 == '>' && c2 == 0)
        return SHR;
    else if(c == '*' && c1 == '=' && c2 == 0)
        return MULASSIGN;
    else if(c == '+' && c1 == '+' && c2 == 0)
        return INC;
    else if(c == '+' && c1 == '=' && c2 == 0)
        return ADDASSIGN;
    else if(c == '!' && c1 == '=' && c2 == 0)
        return NE;
    else if(c == '.' && c1 == '*' && c2 == 0)
        return REFRESOLVEPTR;
    else if(c == '^' && c1 == '=' && c2 == 0)
        return XORASSIGN;
    else if(c == '<' && c1 == '=' && c2 == 0)
        return LTE;
    else if(c == '<' && c1 == '<' && c2 == 0)
        return SHL; //Commonly overloaded ((cout))
    else if(c == '&' && c1 == '&' && c2 == 0)
        return LOGICALAND;
    else if(c == '-' && c1 == '=' && c2 == 0)
        return SUBASSIGN;
    else if(c == '-' && c1 == '>' && c2 == 0)
        return PTRRESOLVEMEMBER;
    else if(c == '-' && c1 == '-' && c2 == 0)
        return DEC;
    else if(c == ':' && c1 == ':' && c2 == 0)
        return SCOPERESOLVE;
    else if(c == '=' && c1 == '=' && c2 == 0)
        return EQ;
    else if(c == '/' && c1 == '=' && c2 == 0)
        return DIVASSIGN;
    else if(c == '/' && c1 == '/' && c2 == 0)
        return COMMENT;
    else if(c == '/' && c1 == '*' && c2 == 0)
        return MLCOMMENTOPEN;
    else if(c == '*' && c1 == '/' && c2 == 0)
        return MLCOMMENTCLOSE;
    else if(c == '\\' && c1 == 0 && c2 == 0)
        return ESC;
    else if(c == '%' && c1 == 0 && c2 == 0)
        return MOD;
    else if(c == '|' && c1 == 0 && c2 == 0)
        return BITOR;
    else if(c == '?' && c1 == 0 && c2 == 0)
        return CONDITIONAL;
    else if(c == '>' && c1 == 0 && c2 == 0)
        return GT;
    else if(c == '*' && c1 == 0 && c2 == 0)
        return STAR;
    else if(c == '+' && c1 == 0 && c2 == 0)
        return ADD;
    else if(c == '!' && c1 == 0 && c2 == 0)
        return NOT;
    else if(c == '.' && c1 == 0 && c2 == 0)
        return DOT;
    else if(c == '^' && c1 == 0 && c2 == 0)
        return XOR;
    else if(c == '<' && c1 == 0 && c2 == 0)
        return LT;
    else if(c == '&' && c1 == 0 && c2 == 0)
        return BITAND; //Also reference
    else if(c == '-' && c1 == 0 && c2 == 0)
        return SUB;
    else if(c == ':' && c1 == 0 && c2 == 0)
        return COLON; //Multipurpose, to the max
    else if(c == '=' && c1 == 0 && c2 == 0)
        return ASSIGN;
    else if(c == '/' && c1 == 0 && c2 == 0)
        return DIV;
    else if(c == '\"' && c1 == 0 && c2 == 0)
        return DOUBQUOTE;
    else if(c == '\'' && c1 == 0 && c2 == 0)
        return SINGLEQUOTE;
    else if(c == ';' && c1 == 0 && c2 == 0)
        return SEMICOLON;
    else if(c == '~' && c1 == 0 && c2 == 0)
        return BITNOT;
    else if(c == ']' && c1 == 0 && c2 == 0)
        return ARRAYCLOSE;
    else if(c == '[' && c1 == 0 && c2 == 0)
        return ARRAYOPEN;
    else if(c == '}' && c1 == 0 && c2 == 0)
        return BLOCKCL;
    else if(c == '{' && c1 == 0 && c2 == 0)
        return BLOCKOP;
    else if(c == ',' && c1 == 0 && c2 == 0)
        return COMMA;
    else if(c == ')' && c1 == 0 && c2 == 0)
        return PARENCL;
    else if(c == '(' && c1 == 0 && c2 == 0)
        return PARENOP;
    else if(c == '#' && c1 == 0 && c2 == 0)
        return PREPROCESSOR;
    return INVALID;
}

Token CppLexer::flush(QString &buffer, CppTokenType t = INVALID, int pos = 0, int line = 0)
{
    if(buffer == "") return Token(); //Don't even bother..

    Token tk;
    tk.constData = buffer;
    tk.type = t;
    tk.lineNum = line;
    tk.pos = pos;

    if(isNum(buffer)) {
        tk.type = EX_NUM;
    }
    tokenBuffer << tk;
    buffer = "";
    return tk;
}

bool CppLexer::isSymbol(char ch)
{
    QString str = QString("~!#%^&*()-+={[}]|\\:;\"\'<,>.?/");
    if(str.indexOf(QChar(ch)) != -1)
        return true;
    return false;
}

void CppLexer::refreshTokens()
{
    tokenized = false;
    QString buffer = "";
    int i = -1, ln = 1, lastNewline = 0;
    while(++i < text.size()) {
        char ch = text.at(i).toLatin1();
        if(isSpace(ch) || ch == '\n') {
            if(buffer != "") {
                if(isKeyword(buffer)) {
                    flush(buffer, KEYWORD, i - buffer.size() - lastNewline, ln);
                } else {
                    flush(buffer, EX_IDENT, i - buffer.size() - lastNewline, ln);
                }
            }
            if(ch == '\n')
            {
                QString bufftmp = "\n"; //Why do we need anything for end of line?
                flush(bufftmp, EX_EOL, i - lastNewline, ln);
                lastNewline = i+1;
                ln++;
            } else {
                do
                {
                    ch = text.at(i).toLatin1();
                } while(isSpace(ch) && ++i < text.size());
                i--;
            }
            buffer = "";
        } else if(isSymbol(ch)) {
            if(buffer != "") {
                if(isKeyword(buffer)) {
                    flush(buffer, KEYWORD, i - buffer.size() - lastNewline, ln);
                } else {
                    flush(buffer, EX_IDENT, i - buffer.size() - lastNewline, ln);
                }
            }
            int i2 = i;
            while(isSymbol(ch) && i2 < text.size())
            {
                buffer += ch;
                ch = text.at(++i2).toLatin1();
            }
            while(determineSymbol(buffer) == INVALID && buffer.size() > 0)
            {
                buffer.chop(1);
            }
            i += buffer.size();
            flush(buffer, determineSymbol(buffer), i - buffer.size() - lastNewline, ln);
            i--;
            buffer = "";
        } else if((ch == '_' || ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' || ch <= 'z'))) && ch != '\n') { //identifier-allowed characters
            buffer += ch;
        } else if(ch >= '0' && ch <= '9') {
            QString tmp = QString(buffer.at(0));
            if(isNum(tmp)) {
                buffer += ch;
            } else if (determineSymbol(buffer) == INVALID) {
                buffer += ch;
            }
        }
    }
    tokenized = true;
    if(buffer != "")
        qWarning("Error parsing token: [%s]", qPrintable(buffer));
}
