#ifndef CPPHTMLHIGHLIGHTER_H
#define CPPHTMLHIGHLIGHTER_H

#include <QObject>
#include "lexer.h"
class QFile;

class CppHTMLHighlighter : public QObject
{
public:
    CppHTMLHighlighter(const CppLexer &l);
    bool writeToFile(QFile &file);

private:
    QList<Token> chunks;
    QString sourceText;
};

#endif // CPPHTMLHIGHLIGHTER_H
