#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
using namespace std;

string sanitizeInput(string &s);

//These three functions call each other to evaluate a simple mathematical expression. Pos is shared between them.
double evalExpression(string s, int *pos);
double evalFactor(string s, int *pos);
double evalTerm(string s, int *pos);

int main()
{
	string s;
	char ans;
	int pos;
	do {
		pos = 0;
		cout << "Enter Expression: ";
		getline(cin, s);
		cout << "Result: " << evalExpression(sanitizeInput(s), &pos) << endl;
		cout << "Again? (y/n) ";
		cin >> ans; cin.ignore();
	} while (ans == 'Y' || ans == 'y');
	return 0;
}

//Input: Reference to User input.
//Return Value: User input stripped of all non-math characters
string sanitizeInput(string &s)
{
	string ret;
	for(int i = 0; i < s.length(); i++) {
		switch(s[i]) {
			case '+':
			case '-':
			case '*':
			case '/':
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '(':
			case ')':
				ret += s[i];
				break;
			default:
				break;
		}
	}
	return ret;
}

//Converts strings to other data types. In the context of this program, numbers.
//Inputs: Reference to desired type, reference to data in string format.
//Writes into: T& t
//Return Value: Success as a boolean
template <class T>
bool from_string(T& t, string& s)
{
	istringstream iss(s);
	return !(iss >> t).fail();
}

//Evaluates an expression. Directly handles addition and subtraction.
//Inputs: Expression in string, pointer to position in string to start from
//Writes into: pos
//Return Value: Evaluated Expression
double evalExpression(string s, int *pos)
{
	double result = evalTerm(s, pos);
	while(*pos < s.length()) {
		char op = s[*pos];
		if(op != '+' && op != '-')
			return result;
		++(*pos);

		double term = evalTerm(s, pos);
		if(op == '+') {
			result += term;
		}
		else if(op == '-') {
			result -= term;
		}
	}
	return result;
}

//Evaluates a term. Directly handles multiplication and division.
//Inputs: Expression in string, pointer to position in string to start from
//Writes into: pos
//Return Value: Evaluated Piece of Expression
double evalTerm(string s, int *pos)
{
	double result = evalFactor(s, pos);
	while(*pos < s.length()) {
		char op = s[*pos];
		if(op != '*' && op != '/' && op != '(')
			return result;

		if(op != '(') ++(*pos);

		double factor = evalFactor(s, pos);
		if(op == '*' || op == '(')
			result *= factor;
		else if(op == '/' && factor != 0)
			result /= factor;
		else if(factor == 0) {
			cout << "Divide by zero!!" << endl;
			exit(1);
		}
	}
	return result;
}

//Evaluates a factor. Directly handles parenthesis, and casting individual numbers.
//Inputs: Expression in string, pointer to position in string to start from
//Writes into: pos
//Return Value: Evaluated Piece of Expression
double evalFactor(string s, int *pos)
{
	double result;
	bool negative = false;
	
	if(s[*pos] == '-') {
		negative = true;
		++(*pos);
	}
	if(s[*pos] == '(') {
		++(*pos);
		result = evalExpression(s, pos);
		if(s[*pos] != ')') {
			cout << "Unmatched parenthesis!" << endl;
			exit(1);
		}
		++(*pos);
	} else {
		string tmp;
		while((s[*pos] >= '0' && s[*pos] <= '9') || s[*pos] == '.')
		{
			tmp += s[*pos];
			++(*pos);
		}
		from_string<double>(result, tmp); 
	}
	if(negative) {
		result *= -1.0;
	}
	
	return result;
}
