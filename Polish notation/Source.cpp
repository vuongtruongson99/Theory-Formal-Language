// if = ~
// then = `
// else = #
// else if = _
// while = !
// for = '
// do = @
// then do = :

#include <iostream>
#include <stack>
#include <vector>
#include <string.h>
#include <string>
#include <set>
#include <map>
#include <algorithm>
using namespace std;

int no_func;
string function_name[100];
bool checkCondition;
bool checkLoop;
vector<int> unconditional_tran;
vector<int> point_trans;
vector<pair<int, int>> conditional_tran;
int out_length;
int first_expression_length;

bool isFunction(string x) {
	for (int i = 0; i < no_func; ++i) {
		if (x == function_name[i]) {
			return true;
		}
	}
	return false;
}

bool isOperator(char x) {
	if (x == '+' || x == '-' || x == '*' || x == '/') {
		return true;
	}
	return false;
}

bool isBoolDoubleOperator(char c1, char c2) {
	string op = "";
	op += c1;
	op += c2;
	if (op == "&&" || op == "||" || op == ">=" || op == "<=" || op == "!=" || op == "==") {
		return true;
	}
	return false;
}

bool isBoolOperator(string c1) {
	if (c1 == "<" || c1 == ">") {
		return true;
	}
	return false;
}

bool isNormalDoubleOperator(char c1, char c2) {
	string op = "";
	op += c1;
	op += c2;
	if (op == "+=" || op == "-=" || op == "*=" || op == "/=") {
		return true;
	}
	return false;
}

bool isOperand(string x) {
	if ((x >= "0" && x <= "9") || (x >= "a" && x <= "z") || x == ".") {
		return true;
	}
	return false;
}

float ApplyOperator(float a, float b, string op) {
	if (op == "+") return a + b;
	else if (op == "-") return a - b;
	else if (op == "*") return a * b;
	else if (op == "/") return a / b;
	else if (op == ">") {
		if (a > b) return 1;
		return 0;
	}
	else if (op == "<") {
		if (a < b) return 1;
		return 0;
	}
}

int ApplyDoubleOperator(float a, float b, string op) {
	if (op == "==") {
		if (a == b) return 1;
		else return 0;
	}
	else if (op == "!=") {
		if (a != b) return 1;
		else return 0;
	}
	else if (op == ">=") {
		if (a >= b) return 1;
		else return 0;
	}
	else if (op == "<=") {
		if (a <= b) return 1;
		else return 0;
	}
	else if (op == "&&") {
		if (a == 0 || b == 0) return 0;
		else return 1;
	}
	else {
		if (a > 0 || b > 0) return 1;
		else return 0;
	}
}

int precedence(string x) {
	if (isFunction(x)) {
		return 6;
	}
	else if (x == "*" || x == "/") {
		return 5;
	}
	else if (x == "+" || x == "-") {
		return 4;
	}
	else if (x == ">" || x == ">=" || x == "<" || x == "<=") {
		return 3;
	}
	else if (x == "==" || x == "!=") {
		return 2;
	}
	else if (x == "&&" || x == "||") {
		return 1;
	}
	else if (x == "=" || x == "+=" || x == "-=" || x == "*=" || x == "/=") {
		return 0;
	}
	else {
		return -1;
	}
}

string ParseUnary(string infix) {
	string new_infix = infix;
	new_infix.erase(remove(new_infix.begin(), new_infix.end(), ' '), new_infix.end());
	int cnt = 0;
	for (int i = 0; i < (int)infix.length(); ++i) {
		if (infix[i] == '-') {
			if (i == 0 || infix[i - 1] == '(' || isOperator(infix[i - 1]) || infix[i-1] == '=') {
				int j = 0;
				if (infix[i + 1] == '(') {
					int cntBalance = 1;
					for (j = i + 2; j < infix.length(); j++) {
						if (infix[j] == '(') cntBalance++;
						else if (infix[i] == ')') cntBalance--;

						if (cntBalance == 0) break;
					}
				}
				
				if (j) {
					new_infix.insert(i + cnt * 3, "(");
					new_infix.insert(i + cnt * 3 + 1, "0");
					new_infix.insert(j + cnt * 3 + 1, ")");
				}
				else {
					new_infix.insert(i + cnt * 3, "(");
					new_infix.insert(i + cnt * 3 + 1, "0");
					new_infix.insert(i + cnt * 3 + 4, ")");
				}
				cnt++;
			}
		}
	}
	return new_infix;
}

string ParseConditional(string infix) {
	string tmp_inf = infix;
	tmp_inf.erase(remove(tmp_inf.begin(), tmp_inf.end(), ' '), tmp_inf.end());

	string cElse_if = "elseif";
	string cIf = "if";
	string cThen = "then";
	string cElse = "else";

	while (tmp_inf.find(cElse_if) != string::npos) {
		checkCondition = true;
		auto pos = tmp_inf.find(cElse_if);
		tmp_inf.replace(pos, cElse_if.length(), "_");
	}

	while (tmp_inf.find(cIf) != string::npos) {
		checkCondition = true;
		auto pos = tmp_inf.find(cIf);
		tmp_inf.replace(pos, cIf.length(), "~");
	}

	while (tmp_inf.find(cThen) != string::npos) {
		checkCondition = true;
		auto pos = tmp_inf.find(cThen);
		tmp_inf.replace(pos, cThen.length(), "`");
	}

	while (tmp_inf.find(cElse) != string::npos) {
		checkCondition = true;
		auto pos = tmp_inf.find(cElse);
		tmp_inf.replace(pos, cElse.length(), "#");
	}
	return tmp_inf;
}

string ParseLoop(string infix) {
	first_expression_length = 0;
	string tmp_inf = infix;
	tmp_inf.erase(remove(tmp_inf.begin(), tmp_inf.end(), ' '), tmp_inf.end());

	string cWhile = "while";
	string cFor = "for";
	string cThenDo = "thendo";
	string cDo = "do";
	
	while (tmp_inf.find(cWhile) != string::npos) {
		checkLoop = true;
		auto pos = tmp_inf.find(cWhile);
		tmp_inf.replace(pos, cWhile.length(), "!");
	}

	while (tmp_inf.find(cFor) != string::npos) {
		checkLoop = true;
		auto pos = tmp_inf.find(cFor);
		tmp_inf.replace(pos, cFor.length(), "'");
	}

	while (tmp_inf.find(cThenDo) != string::npos) {
		checkLoop = true;
		auto pos = tmp_inf.find(cThenDo);
		tmp_inf.replace(pos, cThenDo.length(), ":");
	}

	while (tmp_inf.find(cDo) != string::npos) {
		checkLoop = true;
		auto pos = tmp_inf.find(cDo);
		tmp_inf.replace(pos, cDo.length(), "@");
	}

	if (tmp_inf.find("@") != string::npos) {
		int pos_do = tmp_inf.find("@");
		int pos_while = tmp_inf.find("!");
		string first_exp = tmp_inf.substr(pos_do + 1, pos_while - pos_do - 1);
		first_expression_length = first_exp.length();
		tmp_inf.erase(pos_do, 1);
		tmp_inf += ":";
		tmp_inf += first_exp;
	}

	if (tmp_inf.find("'") != string::npos) {
		vector<int>pos1, pos2;
		for (int z = tmp_inf.find("'"); z < tmp_inf.length(); ++z) {
			if (tmp_inf[z] == ')') {
				pos2.push_back(z + tmp_inf.find("'"));
				break;
			}
			else if (tmp_inf[z] == ';') {
				pos1.push_back(z + tmp_inf.find("'"));
				continue;
			}
		}
		string new_inf = "";
		new_inf += tmp_inf.substr(tmp_inf.find("'") + 2, pos1[0] - tmp_inf.find("'") - 2);
		first_expression_length = pos1[0] - tmp_inf.find("'") - 1;
		new_inf += "!";
		new_inf += tmp_inf.substr(pos1[0] + 1, pos1[1] - pos1[0] - 1);
		new_inf += tmp_inf.substr(pos2[0] + 1);
		new_inf += ",";
		new_inf += tmp_inf.substr(pos1[1] + 1, pos2[0] - pos1[1] - 1);
		return new_inf;
	}

	return tmp_inf;
}

string infix_to_postfix(string infix) {
	// Reset all vector
	unconditional_tran.clear();
	conditional_tran.clear();
	point_trans.clear();

	string out;
	stack<string> st;
	st.push("$");
	int cnt = 1;
	infix = ParseLoop(infix);
	infix = ParseConditional(infix);
	infix = ParseUnary(infix);
	
	for (int i = 0; i < (int)infix.length(); ++i) {
		string ex = "";
		ex += infix[i];

		if (isOperand(ex)) {
			out += ex;
		}
		else if (ex == " ") {
			continue;
		}
		else if (ex == "!") { // while
			while (st.top() != "$") {
				string c = st.top();
				st.pop();
				out += c;
				out += " ";
			}
			st.push("!");
		}
		else if (ex == ":") { // then do
			while (st.top() != "$" && st.top() != "!") {
				string c = st.top();
				st.pop();
				out += " ";
				out += c;
			}
			if (st.top() == "!") {
				st.pop();
			}
			out += " p(";
			out += to_string(cnt);
			out += ") !F ";
			point_trans.push_back(cnt);
			st.push(":");
			cnt++;
		}

		else if (ex == "~") { // if
			st.push("~");
		}
		else if (ex == "`") { // then
			while (st.top() != "$" && st.top() != "~" && st.top() != "_") {
				string c = st.top();
				st.pop();
				out += " ";
				out += c;
			}
			if (st.top() == "~" || st.top() == "_") {
				st.pop();
				out += " p(";
				out += to_string(cnt);
				out += ") !F ";
				point_trans.push_back(cnt);
			}
			cnt++;
		}
		else if (ex == "_") { // else if
			while (st.top() != "$" && st.top() != "`" && st.top() != "_") {
				string c = st.top();
				st.pop();
				out += " ";
				out += c;
			}
			if (st.top() == "`" || st.top() == "_") {
				st.pop();
			}
			out += " p(";
			out += to_string(cnt);
			out += ") !! ";

			int point_tran = point_trans.back();
			point_trans.pop_back();
			conditional_tran.push_back(make_pair(point_tran, out.length()));
			unconditional_tran.push_back(cnt);

			st.push("_");
			cnt++;
		}
		else if (ex == "#") { // else
			while (st.top() != "$" && st.top() != "~" && st.top() != "_" && st.top() != "`") {
				string c = st.top();
				st.pop();
				out += " ";
				out += c;
			}
			if (st.top() == "~" || st.top() == "_" || st.top() == "`") {
				st.pop();
			}
			out += " p(";
			out += to_string(cnt);
			out += ") !! ";
			unconditional_tran.push_back(cnt);
			int point_tran = point_trans.back();
			point_trans.pop_back();
			conditional_tran.push_back(make_pair(point_tran, out.length()));
			cnt++;
		}
		else if (isBoolDoubleOperator(infix[i], infix[i + 1])) {
			out += " ";
			string op;
			op += infix[i];
			op += infix[i + 1];

			while (st.top() != "$" && precedence(op) <= precedence(st.top())) {
				string c = st.top();
				st.pop();
				out += c;
				out += " ";
			}
			st.push(op);
			++i;
		}
		else if (isBoolOperator(ex)) {
			out += " ";
			while (st.top() != "$" && precedence(ex) <= precedence(st.top())) {
				string c = st.top();
				st.pop();
				out += c;
				out += " ";
			}
			st.push(ex);
		}
		else if (ex == "=") {
			out += " ";
			while (st.top() != "$" && precedence(ex) < precedence(st.top())) {
				string c = st.top();
				st.pop();
				out += c;
				out += " ";
			}
			st.push(ex);
		}
		else if (isNormalDoubleOperator(infix[i], infix[i+1])) {
			out += " ";

			for (int z = i-1; z >= 0; --z) {
				string operand;
				operand += infix[z];
				if (isOperand(operand)) {
					out += operand;
					out += " ";
					break;
				}
			}
			string op;
			op += infix[i];
			op += infix[i + 1];
			while (st.top() != "$" && precedence(op) < precedence(st.top())) {
				string c = st.top();
				st.pop();
				out += c;
				out += " ";
			}
			st.push(op);
			++i;
		}
		else if (isOperator(infix[i])) {
			out += " ";
			while (st.top() != "$" && precedence(ex) <= precedence(st.top())) {
				string c = st.top();
				st.pop();
				out += c;
				out += " ";
			}
			st.push(ex);	
		}
		else if (ex == "(") {
			st.push("(");
		}
		else if (ex == ")") {
			while (st.top() != "$" && st.top() != "(") {
				string c = st.top();
				st.pop();
				out += " ";
				out += c;
			}
			if (st.top() == "(") {
				string c = st.top();
				st.pop();
			}
		}
		else if (isFunction(ex)) {
			st.push(ex);
		}
		else if (ex == "," || ex == ";") {
			out += " ";
			while (st.top() != "$" && st.top() != "(" && st.top() != ":") {
				string c = st.top();
				st.pop();
				out += c;
				out += " ";
			}
		}
	}

	while (st.top() != "$") {
		if (st.top() == ":") {
			st.pop();
			out += " p(";
			out += to_string(cnt);
			out += ") !! ";
			conditional_tran.push_back(make_pair(cnt, first_expression_length));
			int point_tran = point_trans.back();
			point_trans.pop_back();
			conditional_tran.push_back(make_pair(point_tran, out.length()));
		}
		else {
			string c = st.top();
			st.pop();
			out += " ";
			out += c;
		}
	}
	out_length = out.length();
	return out;
}

void Calculate_postfix(string postfix) {
	stack<float> vals;
	int i;
	for (i = 0; i < postfix.length(); ++i) {
		string op1;
		op1 += postfix[i];
		if (postfix[i] == ' ') continue;
		else if (isdigit(postfix[i])) {
			string val;
			while (i != postfix.length() && (isdigit(postfix[i]) || postfix[i] == '.')) {
				val += postfix[i];
				++i;
			}

			float tmp_val = std::stof(val);
			vals.push(tmp_val);
			--i;
		}
		else if (isBoolDoubleOperator(postfix[i], postfix[i + 1])) {
			float val1 = vals.top();
			vals.pop();
			float val2 = vals.top();
			vals.pop();
			std::string op;
			op += postfix[i];
			op += postfix[i + 1];
			int new_val = ApplyDoubleOperator(val2, val1, op);
			vals.push(new_val);
			++i;
		}
		else if (isBoolOperator(op1)) {
			float val1 = vals.top();
			vals.pop();
			float val2 = vals.top();
			vals.pop();
			int new_val = ApplyOperator(val2, val1, op1);
			vals.push(new_val);
		}
		else if (isOperator(postfix[i])) {
			float val1 = vals.top();
			vals.pop();
			float val2 = vals.top();
			vals.pop();
			string op;
			op += postfix[i];
			float new_vals = ApplyOperator(val2, val1, op);
			vals.push(new_vals);
		}
		else if (!isdigit(postfix[i])) {
			cout << "Cannot calculate this expression!";
			return;
		}
	}

	float ans = vals.top();
	cout << "Answer: " << ans << endl;
}

void Convert_To_1D(string arr_name, vector<int> dimension) {
	cout << "Element " << arr_name;
	for (int i = 0; i < dimension.size(); ++i) {
		cout << "[i" << i + 1 << "]";
	}
	cout << " will stored in 1-D array at position: \n";

	cout << "By lines: ";
	no_func = 1;
	function_name[0] = "A";
	string s = "w*(";
	for (int i = 0; i < dimension.size(); ++i) {
		s += ("i" + to_string(i+1));
		for (int j = i + 1; j < dimension.size(); ++j) {
			s += ("*" + to_string(dimension[j]));
		}
		if (i < dimension.size() - 1) s += "+";
	}
	s += ")";
	string infix_arr = "A(" + arr_name + ";" + s + ")";

	cout << infix_to_postfix(infix_arr) << endl;

	cout << "By columns: ";
	s = "w*(";
	for (int i = 0; i < dimension.size(); ++i) {
		s += ("i" + to_string(i + 1));
		for (int j = i - 1; j >= 0; --j) {
			s += ("*" + to_string(dimension[j]));
		}
		if (i < dimension.size() - 1) s += "+";
	}
	s += ")";
	infix_arr = "A(" + arr_name + ";" + s + ")";

	cout << infix_to_postfix(infix_arr) << endl;
}

void Access_To_Element(string arr_name) {
	cin.ignore(1);
	string input_expression;
	cout << "Input expression: ";
	getline(cin, input_expression);
	string new_inf = "";

	for (int i = 0; i < input_expression.length(); ++i) {
		if (input_expression[i + 1] == '[') {
			if (string(1, input_expression[i]) == arr_name) {
				new_inf += "A(" + arr_name + "; w*" + input_expression[i + 2] + ")";
				i += 3;
			}
			else {
				string a = "A(" + string(1, input_expression[i]) + ";";
				new_inf += a;
				string s = "w*(";
				string tmp;
				int first_open = i + 1;
				int cnt = 0;
				for (int j = i + 2; j < input_expression.length(); j++) {
					if (input_expression[j] == ']') {
						if (cnt) {
							s += 'n' + to_string(cnt) + '*' + input_expression.substr(first_open + 1, j - first_open - 1);
							cnt++;
						}
						else {
							s += input_expression.substr(first_open + 1, j - first_open - 1);
							cnt++;
						}

						if (input_expression[j + 1] != '[') {
							i = j;
							break;
						}
						else {
							s += "+";
						}
					}
					else if (input_expression[j] == '[') {
						first_open = j;
					}
				}
				s += "))";
				new_inf += s;
			}
		}
		else {
			new_inf += input_expression[i];
		}
	}
	cout << "Postfix: " << infix_to_postfix(new_inf) << endl;
}

void Work_with_arr() {
	string input_arr;
	cout << "Enter first array(name[i1][i2][i3]...[in]): ";
	cin >> input_arr;
	
	string arr_name;
	string tmp_num;
	vector<int> dimension;
	int i;
	for (i = 0; i < input_arr.length(); ++i) {
		if (input_arr[i] == ' ') continue;
		else if (input_arr[i] == '[') break;
		else arr_name += input_arr[i];
	}
	
	for (i = i; i < input_arr.length(); ++i) {
		if (input_arr[i] == ' ' || input_arr[i] == '[') continue;
		else if (input_arr[i] == ']') {
			dimension.push_back(stoi(tmp_num));
			tmp_num = "";
		}
		else {
			tmp_num += input_arr[i];
		}
	}

	cout << "\n\n";
	cout << "------------------Work with array menu------------------\n";
	cout << "[1] - Convert to 1 dimensional array\n";
	cout << "[2] - Access to element of array.\n";
	cout << "[0] - Exit!\n\n";

	int choice = 0;
	do
	{
		cout << "Input your choice: ";
		cin >> choice;

		switch (choice)
		{
		case 1:
			Convert_To_1D(arr_name, dimension);
			cout << "\n";
			break;
		case 2:
			Access_To_Element(arr_name);
			cout << "\n";
			break;
		case 0:
			break;
		default:
			break;
		}
		
	} while (choice != 0);
}

void Normal_expression() {
	cout << "Input number of function in your expression: ";
	cin >> no_func;
	cout << "Input name of function one by one: \n";
	for (int i = 0; i < no_func; ++i) {
		cin >> function_name[i];
	}

	//cin.ignore();
	for (int i = 0; i < 10; ++i) {
		cin.ignore(1);
		cout << "Input infix expression: ";
		string exp;
		getline(cin, exp);
		out_length = 0;
		checkCondition = false;
		checkLoop = false;
		cout << "Postfix notation: " << infix_to_postfix(exp) << endl;

		if (checkCondition || checkLoop) {
			for (auto x : conditional_tran) {
				cout << "p(" << x.first << ") = " << x.second << endl;
			}

			bool abc = false;
			for (auto x : unconditional_tran) {
				cout << "p(" << x << ") = ";
				abc = true;
			}
			if (abc) {
				cout << out_length << endl;
			}

		}

		Calculate_postfix(infix_to_postfix(exp));

		cout << endl;
	}
}

int main() {
	int choice = 0;
	do {
		cout << "-------------------------------MENU-------------------------------\n";
		cout << "[1] - Work with array.\n";
		cout << "[2] - Work with normal expression.\n";
		cout << "[0] - Exit program\n";

		cout << "\n";
		cout << "Input your choice: ";
		cin >> choice;

		switch (choice)
		{
		case 1:
			Work_with_arr();
			cout << "\n\n";
			break;
		case 2:
			Normal_expression();
			break;
		case 0:
			break;
		default:
			break;
		}

	} while (choice != 0);


	system("pause");
	return 0;
}


// if a > 0 then a = a + 1 else a = b + 1
// if a > b + 1 then a = a * b + 2 else if a < b + 1 then a = a / b + 2 else a = b * 2
// while a>0 then do a = a-1
// do a = a + 1 while a < 5
// for (i=0; i<5; i+=1) then do a += 1