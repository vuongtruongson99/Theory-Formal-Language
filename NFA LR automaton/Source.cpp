#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <string.h>
using namespace std;

struct Grammar {
	string lhs;
	string rhs;

	Grammar() {}
	Grammar(string _lhs, string _rhs) : lhs(_lhs), rhs(_rhs) {}
};

class Items {
public:
	int position_dot;
	int position_grammar;
	struct Grammar production;
	
	vector<string> trans_non_terminal[30];
	vector<string> trans_terminal[30];
	vector<string> trans_closure;


	Items() {}

	bool operator==(const Items I1) {
		if (this->position_dot == I1.position_dot && this->position_grammar == I1.position_grammar) {
			return true;
		}
		return false;
	}
};

vector<Grammar> grammar;
int no_nt;
char non_terminals[100];
int no_t;
char terminals[100];


void input_grammar() {
	string lhs;
	string rhs;
	string delim = "->";

	cout << "input number of non-terminals: ";
	cin >> no_nt;
	cout << "input one by one non-terminals: \n";
	for (int i = 0; i < no_nt; ++i) {
		cin >> non_terminals[i];
	}
	cout << "input number of terminals: ";
	cin >> no_t;
	cout << "input one by one terminals: \n";
	for (int i = 0; i < no_t; ++i) {
		cin >> terminals[i];
	}
	cin.ignore(1);

	cout << "input grammar (S->ABC): \n";
	string production;
	while (1) {
		getline(cin, production);

		if (production.length() < 1) return;

		auto pos = production.find(delim);
		if (pos != string::npos) {
			lhs = production.substr(0, pos);
			rhs = production.substr(pos + delim.length(), string::npos);
		}

		grammar.push_back(Grammar(lhs, rhs));
	}
}

bool is_non_terminal(char a) {
	if (a >= 'A' && a <= 'Z') {
		return true;
	}
	return false;
}

int main() {
	input_grammar();
	int no_state = 0;
	set<string> final_state;
	Items I[50];
	queue<Items> q;

	for (int i = 0; i < grammar.size(); ++i) {
		I[i].position_grammar = i;
		I[i].position_dot = 0;
		I[i].production = Grammar(grammar[i].lhs, "." + grammar[i].rhs);
		q.push(I[i]);
		no_state++;

		string final_s = to_string(0) + to_string(i);
		final_state.insert(final_s);
	}

	while (!q.empty()) {
		Items state = q.front();
		q.pop();

		Items *tmp_state = new Items();
		for (int i = 0; i < no_state; ++i) {
			if (I[i] == state) {
				tmp_state = &I[i];
			}
		}

		//cout << tmp_state->production.lhs << " -> " << tmp_state->production.rhs  << ": " << tmp_state->position_dot << tmp_state->position_grammar << endl;

		char lookahead = tmp_state->production.rhs[state.production.rhs.find('.') + 1];

		// Thêm các state mà có thể đến được từ "state" thông qua non_terminal
		for (int i = 0; i < no_nt; ++i) {
			if (non_terminals[i] == lookahead) {
				string next_state = to_string(tmp_state->position_dot + 1) + to_string(tmp_state->position_grammar);
				tmp_state->trans_non_terminal[i].push_back(next_state);
				
				if (tmp_state->position_dot + 1 <= grammar[tmp_state->position_grammar].rhs.length() + 1) {
					final_state.insert(next_state);
				}
			}
		}

		// Thêm các state mà có thể đến được từ "state" thông qua terminal
		for (int i = 0; i < no_t; ++i) {
			if (terminals[i] == lookahead) {
				string next_state = to_string(tmp_state->position_dot + 1) + to_string(tmp_state->position_grammar);
				tmp_state->trans_terminal[i].push_back(next_state);

				if (tmp_state->position_dot + 1 <= grammar[tmp_state->position_grammar].rhs.length() + 1) {
					final_state.insert(next_state);
				}
			}
		}

		// Thêm các state mà có thể đến được từ "state" thông qua closure
		for (int i = 0; i < grammar.size(); ++i) {
			if (grammar[i].lhs == string(1, lookahead)) {
				string next_state = to_string(0) + to_string(i);
				tmp_state->trans_closure.push_back(next_state);
				final_state.insert(next_state);
			}
		}

		if (lookahead == '\0') {
			continue;
		}

		// Thêm new Item
		I[no_state].position_dot = tmp_state->position_dot + 1;
		I[no_state].position_grammar = tmp_state->position_grammar;

		string rhs = grammar[tmp_state->position_grammar].rhs;
		int pos;
		for (pos = tmp_state->position_dot; pos < rhs.length(); ++pos) {
			if (rhs[pos] == lookahead) {
				break;
			}
		}
		string tmp = rhs.substr(0, pos + 1);
		tmp += '.';
		tmp += rhs.substr(pos + 1, string::npos);
		rhs = tmp;
		//cout << grammar[state.position_grammar].lhs << " -> " << rhs << endl;

		I[no_state].production = Grammar(grammar[state.position_grammar].lhs, rhs);
		q.push(I[no_state]);
		no_state++;
	}


	cout << "\n\n-------------------------TABLE OF NON-DETERMINISTIC AUTOMATON-------------------------\n";
	cout << setw(15) << "STATE/INPUT  |";

	for (int i = 0; i < no_t; ++i) {
		cout << setw(9) << terminals[i] <<  "|";
	}
	for (int i = 0; i < no_nt; ++i) {
		cout << setw(9) << non_terminals[i] << "|";
	}
	cout << setw(10) << "^  |";
	cout << endl;

	for (int i = 0; i < no_state; ++i) {
		//cout << I[i].position_dot << I[i].position_grammar << ": ";
		//cout << I[i].production.lhs << " -> " << I[i].production.rhs << endl;
		string tmp_st = to_string(I[i].position_dot) + to_string(I[i].position_grammar);
		if (final_state.count(tmp_st)) {
			cout << setw(15) << tmp_st + "  |";
			// In ra tất cả những state có thể đạt được khi dùng terminal
			for (int j = 0; j < no_t; ++j) {
				if (I[i].trans_terminal[j].size() != 0) {
					cout << setw(8);
					for (int k = 0; k < I[i].trans_terminal[j].size(); ++k) {
						cout << I[i].trans_terminal[j][k] << " ";
					}
					cout << "|";
				}
				else {
					cout << setw(10) << "  |";
				}
			}

			// In ra tất cả những state có thể đạt được khi dùng non_terminal
			for (int j = 0; j < no_nt; ++j) {
				if (I[i].trans_non_terminal[j].size() != 0) {
					cout << setw(8);
					for (int k = 0; k < I[i].trans_non_terminal[j].size(); ++k) {
						cout << I[i].trans_non_terminal[j][k] << " ";
					}
					cout << "|";
				}
				else {
					cout << setw(10) << "  |";
				}
			}

			// In ra tất cả những state có thể đạt được khi dùng closure
			if (I[i].trans_closure.size() > 0) {
				cout << setw(8);
				for (int j = 0; j < I[i].trans_closure.size(); ++j) {
					cout << I[i].trans_closure[j] << " ";
				}
				cout << "|";
			}
			else {
				cout << setw(10) << "  |";
			}
			cout << endl;
			
		}

	}

	system("pause");
	return 0;
}