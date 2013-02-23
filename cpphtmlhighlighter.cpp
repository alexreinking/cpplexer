#include <QtCore>
#include <cmath>
#include <iostream>
#include "cpphtmlhighlighter.h"
using std::cout;
using std::endl;

//Preserves formatting by mapping HTML from tokens to original string.

CppHTMLHighlighter::CppHTMLHighlighter(const CppLexer &l)
{
    chunks = l.tokens();
    sourceText = l.originalText();
    foreach(Token str, chunks)
    {
        int i = str.constData.size();
        QString tmp = "";
        i -= 20;
        i *= -1;
        for(int j = 0; j < i; j++, tmp += " ") ;
        QString printme = str.constData;
        if(printme == "\n") {
            printme = "EOL";
            tmp = "                 ";
        } else {
            printme.prepend("\'");
            printme.append("\'");
        }
        cout << qPrintable(printme) << qPrintable(tmp) << "\t" << str.type << '\t' << str.lineNum << '\t' << str.pos << endl;
    }
    cout << "EOF" << "                 " << endl;
}

inline unsigned short digits(unsigned int num)
{
    return floor(log10((long double)num)) + 1;
}

bool CppHTMLHighlighter::writeToFile(QFile &file)
{
    enum State { None, SingleComment, MultiComment, String, Preprocessor, Char };
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&file);
        State s = None;
        QStringList lines = sourceText.split("\n"); //Each line, ready for injective processing...
        for(int i = 0; i < lines.size(); i++) {
            QString line = lines.at(i);
            QList<Token>::iterator j;
            int addedSpace = 0;

            QString lineNo = QString("%1").arg(i+1);
            if(lineNo.length() < 6) {
                while(lineNo.length() != 6)
                    lineNo.prepend("0");
            }
            lineNo.prepend("<tr><td class=\"bar\">"); //!
            lineNo.append("</td><td class=\"code\">"); //!
            line.prepend(lineNo);

            addedSpace = lineNo.length();

            for(j = chunks.begin(); j != chunks.end(); ++j) {
                Token token = *j;
                ++j;
                Token next; next.lineNum = chunks.back().lineNum+1;
                if(j != chunks.end())
                    next = *j;
                --j;
                if(token.lineNum == i+1)
                {
                    if(token.type == GT) {
                        line.replace(token.pos+addedSpace, 1, "&gt;");
                        addedSpace += 3;
                    } else if(token.type == LT) {
                        line.replace(token.pos+addedSpace, 1, "&lt;");
                        addedSpace += 3;
                    } else if(token.type == BITAND) {
                        line.replace(token.pos+addedSpace, 1, "&amp;");
                        addedSpace += 4;
                    } else if(token.type == SHR) {
                        line.replace(token.pos+addedSpace, 2, "&gt;&gt;");
                        addedSpace += 6;
                    } else if(token.type == SHL) {
                        line.replace(token.pos+addedSpace, 2, "&lt;&lt;");
                        addedSpace += 6;
                    } else if(token.type == GTE) {
                        line.replace(token.pos+addedSpace, 2, "&lt;=");
                        addedSpace += 3;
                    } else if(token.type == LTE) {
                        line.replace(token.pos+addedSpace, 2, "&lt;=");
                        addedSpace += 3;
                    } else if(token.type == LOGICALAND) {
                        line.replace(token.pos+addedSpace, 2, "&amp;&amp;");
                        addedSpace += 8;
                    } else if(token.type == SHLASSIGN) {
                        line.replace(token.pos+addedSpace, 3, "&lt;&lt;=");
                        addedSpace += 6;
                    } else if(token.type == SHRASSIGN) {
                        line.replace(token.pos+addedSpace, 3, "&gt;&gt;=");
                        addedSpace += 6;
                    } else if(token.type == PTRRESOLVEPTR) {
                        line.replace(token.pos+addedSpace, 3, "-&gt;*");
                        addedSpace += 3;
                    }
                    switch(s) {
                    case String:
                        if(token.type == ESC) { //If ESC is the second-to-last token on a line, we will be moved on to EOL next time.
                            j++;
                            break;
                        } else if(token.type == DOUBQUOTE) {
                            line.insert(token.pos+addedSpace+1, QString("</span>"));
                            addedSpace += 7;
                            s = None;
                        }
                        if(next.lineNum != i+1 && token.type != ESC) { //rare case, handles malformatted code
                            line.append(QString("</span>"));
                            lines[i] = line;
                            s = None;
                            goto end; //think of this as a break, but a little further. Yes, I could create a "done" bool,
                        }             //but I think that's excessive just to avoid the four letters of 'goto'
                        break;
                    case Char:
                        if(token.type == ESC) {
                            j++;
                            break;
                        } else if(token.type == SINGLEQUOTE) {
                            line.insert(token.pos+addedSpace+1, QString("</span>"));
                            addedSpace += 7;
                            s = None;
                        }
                        break;
                    case MultiComment:
                        if(token.type == MLCOMMENTCLOSE) {
                            line.insert(token.pos+addedSpace+2, QString("</span>"));
                            addedSpace += 7;
                            s = None;
                        }
                        break;
                    case SingleComment:
                    case Preprocessor:
                        if(token.type == ESC) { //If ESC is the second-to-last token on a line, we will be moved on to EOL next time.
                            j++;
                            break;
                        }
                        if(next.lineNum != i+1 && token.type != ESC) { // Such a lovely terminator
                            line.append(QString("</span>"));
                            lines[i] = line;
                            s = None;
                            goto end; //think of this as a break, but a little further.
                        }
                        break;
                    default:
                        if(token.type == PREPROCESSOR) {
                            line.insert(token.pos+addedSpace, QString("<span class=\"preprocessor\">"));
                            addedSpace += 27;
                            s = Preprocessor;
                        } else if(token.type == DOUBQUOTE) {
                            line.insert(token.pos+addedSpace, QString("<span class=\"string\">"));
                            addedSpace += 21;
                            s = String;
                        } else if(token.type == SINGLEQUOTE) {
                            line.insert(token.pos+addedSpace, QString("<span class=\"string\">"));
                            addedSpace += 21;
                            s = Char;
                        } else if(token.type == MLCOMMENTOPEN) {
                            line.insert(token.pos+addedSpace, QString("<span class=\"comment\">"));
                            addedSpace += 22;
                            s = MultiComment;
                        } else if(token.type == COMMENT) {
                            line.insert(token.pos+addedSpace, QString("<span class=\"comment\">"));
                            addedSpace += 22;
                            s = SingleComment;
                        } else if(token.type == KEYWORD) {
                            line.insert(token.pos+addedSpace, QString("<span class=\"keyword\">"));
                            addedSpace += 22;
                            line.insert(token.pos+addedSpace+token.constData.size(), QString("</span>"));
                            addedSpace += 7;
                        } else if(token.type == EX_NUM) {
                            line.insert(token.pos+addedSpace, QString("<span class=\"number\">"));
                            addedSpace += 21;
                            line.insert(token.pos+addedSpace+token.constData.size(), QString("</span>"));
                            addedSpace += 7;
                        }
                        break;
                    }
                } else {
                    lines[i] = line;
                }
                if(token == chunks.last()) //!
                { //!
                    line.append("</td></tr>"); //!
                    lines[i] = line;
                } //!
            }
            end: ;
        }
        out << "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">" << endl;
        out << "<html>" << endl;
        out << "<head>" << endl;
        out << "<title>Output</title>"<< endl;
        out << "<link rel=\"stylesheet\" type=\"text/css\" href=\"scheme.css\">" << endl;
        out << "<meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\">" << endl;
        out << "</head>" << endl;
        out << "<body>" << endl;
        out << "<table cellspacing=\"0\">" << endl;
        foreach(QString line, lines) {
            out << qPrintable(line) << endl;
        }
        out << "</table>" << endl;
        out << "<p> <a href=\"http://validator.w3.org/check?uri=referer\"><img src=\"http://www.w3.org/Icons/valid-html401\" alt=\"Valid HTML 4.01 Strict\" height=\"31\" width=\"88\"></a></p>" << endl;
        out << "</body>" << endl;
        out << "</html>" << endl;
    }
    file.close();
    return true;
}
