#include <QCoreApplication>
#include <iostream>
#include "lexer.h"
#include "cpphtmlhighlighter.cpp"
using namespace std;

int main(int argc, char **argv)
{
     QCoreApplication app(argc, argv);
     QStringList args = QCoreApplication::arguments();
     
     if(args.count() != 3)
     {
          cerr << "Usage: cpplexer [c++ file] [output file]" << endl
          	<< "Example:" << endl
                << "     ./cpplexer example.cpp output.html"
          	<< endl;
          
          return 1;
     }
     CppLexer lexer;
     cout << "1st Argument: " << qPrintable(args[1]) << endl;
     if(args[1] != "secrettest") {
          if(!lexer.setInput(args[1]))
          {
               return -1;
          }
     }
     lexer.update();
     if(!lexer.isTokenized())
          return 1;
     else {
         QFile file(args[2]);
         CppHTMLHighlighter high(lexer);
         high.writeToFile(file);
         /*
          foreach(Token str, lexer.tokens())
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
               }
               cout << qPrintable(printme) << qPrintable(tmp) << "\t" << str.type << '\t' << str.lineNum << '\t' << str.pos << endl;
          }
          cout << "EOF" << "                 " << endl;
          */
     }
     return 0;
}
