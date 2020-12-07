#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
using namespace std;

typedef map<char, vector<string>> AugumentedGrammar;
typedef map<string, int> GotoMap;

int no_nt;
char non_terminals[100];
int no_t;
char terminals[100];
vector<string> init_grammar;

// Struct để biểu diễn Augumented grammar production
struct AugumentedProduction {
	char lhs;
	string rhs;

	AugumentedProduction() { }
	AugumentedProduction(char _lhs, string _rhs) : lhs(_lhs), rhs(_rhs) {}
};

// Final state in DFA LR
class DFA_LR {
private:
	// List of production. Ex: S->.A; A->A.+E
	vector<AugumentedProduction*> productions;

public:
	// Danh sách các state có thể đến được khi có char
	map<char, int> gotos;
	bool final_state;
	bool half_state;

	DFA_LR() {
		final_state = false;
		half_state = false;
	}

	void push(AugumentedProduction *p) {
		productions.push_back(p);
	}

	// Get size of production in LRItem
	int size() {
		return productions.size();
	}

	// Kiểm tra items "production" có ở trong class này không
	bool contain(string production) {
		for (auto it : productions) {
			string tmp = string(1, it->lhs) + "->" + it->rhs;
			if (strcmp(production.c_str(), tmp.c_str()) == 0) {
				return true;
			}
		}
		return false;
	}

	// Overload operator []
	AugumentedProduction* operator[] (const int index) {
		return productions[index];
	}
};

void input_grammar(AugumentedGrammar& grammar, vector<DFA_LR>& state_LR) {
	string lhs, rhs;
	string delim = "->";

	cout << "Input number of non-terminals: ";
	cin >> no_nt;
	cout << "Input one by one non-terminals: \n";
	for (int i = 0; i < no_nt; ++i) {
		cin >> non_terminals[i];
	}
	cout << "Input number of terminals: ";
	cin >> no_t;
	cout << "Input one by one terminals: \n";
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

		init_grammar.push_back(production);
		grammar[lhs[0]].push_back(rhs);
		state_LR[0].push(new AugumentedProduction(lhs[0], "." + rhs));
	}
}

void add_closure(char lookahead, DFA_LR &state, AugumentedGrammar& grammar) {
	// Chỉ xét non-terminal
	if (!isupper(lookahead)) return;

	string lhs = string(&lookahead, 1);

	for (int i = 0; i < grammar[lookahead].size(); ++i) {
		string rhs = "." + grammar[lookahead][i];

		if (!state.contain(lhs + "->" + rhs)) {
			state.push(new AugumentedProduction(lookahead, rhs));
		}
	}
}

void get_state(vector<DFA_LR>& LR_state, AugumentedGrammar& grammar, int state, GotoMap& globalGoto) {
	cout << "State " << state + 1 << ": \n";

	for (int i = 0; i < LR_state[state].size(); ++i) {
		string rhs = LR_state[state][i]->rhs;
		char lookahead = rhs[rhs.find('.') + 1];
		add_closure(lookahead, LR_state[state], grammar);
	}

	int nextpos;
	char lookahead, lhs;
	string rhs;
	AugumentedProduction* prod;

	// Lặp lại qua state này 
	for (int i = 0; i < LR_state[state].size(); ++i) {
		// Production hiện tại
		lhs = LR_state[state][i]->lhs;
		rhs = LR_state[state][i]->rhs;
		string production = string(&lhs, 1) + "->" + rhs;

		lookahead = rhs[rhs.find('.') + 1];
		
		// Check final state
		if (lookahead == '\0') {
			printf("\t%-20s", &production[0]);
			string new_production = string(&lhs, 1) + "->" + rhs.substr(0, rhs.find('.')) + rhs.substr(rhs.find('.') + 1);
			cout << "(" << rhs.find('.');
			for (int z = 0; z < init_grammar.size(); ++z) {
				if (strcmp(new_production.c_str(), init_grammar[z].c_str()) == 0) {
					cout << z;
				}
			}
			cout << ")";
			cout << endl;
			LR_state[state].final_state = true;
			if (LR_state[state].size() > 1) LR_state[state].half_state = true;
			continue;
		}


		// Kiểm tra từ state này có thể đi đến các state khác thông qua terminal or non-terminal nào
		if (LR_state[state].gotos.find(lookahead) == LR_state[state].gotos.end()) {
			if (globalGoto.find(production) == globalGoto.end()) {
				LR_state.push_back(DFA_LR());

				// Di chuyển dấu "." sang bên phải 1
				string new_rhs = rhs;
				auto pos = rhs.find('.');
				swap(new_rhs[pos], new_rhs[pos + 1]);
				
				// Thêm items vào state và gotos
				LR_state.back().push(new AugumentedProduction(lhs, new_rhs));
				LR_state[state].gotos[lookahead] = LR_state.size() - 1;
				globalGoto[production] = LR_state.size() - 1;
			}
			else {
				LR_state[state].gotos[lookahead] = globalGoto[production];
			}
		}
		else {
			auto pos = rhs.find('.');
			swap(rhs[pos], rhs[pos + 1]);
			
			int next_state = LR_state[state].gotos[lookahead];
			if (!LR_state[next_state].contain(string(&lhs, 1) + "->" + rhs)) {
				LR_state[next_state].push(new AugumentedProduction(lhs, rhs));
			}

			swap(rhs[pos], rhs[pos + 1]);
		}
		printf("\t%-20s", &production[0]);
		string new_production = string(&lhs, 1) + "->" + rhs.substr(0, rhs.find('.')) + rhs.substr(rhs.find('.') + 1);
		cout << "(" << rhs.find('.');
		for (int z = 0; z < init_grammar.size(); ++z) {
			if (strcmp(new_production.c_str(), init_grammar[z].c_str()) == 0) {
				cout << z;
			}
		}
		cout << ")";
		cout << endl;
	}
}

int main() {
	int no_state = -1;
	AugumentedGrammar grammar;
	// Push empty state to set of state;
	vector<DFA_LR> state = { DFA_LR() };
	GotoMap globalGoto;
	input_grammar(grammar, state);

	cout << "Items in each state\n";
	cout << "----------------------------------------------\n";
	while (++no_state < int(state.size())) {
		get_state(state, grammar, no_state, globalGoto);
	}

	cout << "\n\n";
	cout << setw(15) << "STATE/INPUT  |";
	for (int i = 0; i < no_t; ++i) {
		cout << setw(10) << terminals[i] << "  |";
	}
	for (int i = 0; i < no_nt; ++i) {
		cout << setw(10) << non_terminals[i] << "  |";
	}
	cout << setw(12) << "FLAG  |";
	cout << endl;

	for (int i = 0; i < state.size(); ++i) {
		//cout << setw(14) << i + 1 << "|";

		// Nếu là final state
		if (state[i].final_state) {
			if (state[i].half_state) {
				cout << setw(14) << i + 1 << "|";
				// In các state có thể đến được với terminals
				for (int j = 0; j < no_t; ++j) {
					bool check = false;
					for (auto it = state[i].gotos.begin(); it != state[i].gotos.end(); ++it) {
						if (it->first == terminals[j]) {
							check = true;
							cout << setw(10) << it->second + 1 << "  |";
						}
					}

					if (!check) {
						cout << setw(13) << "0  |";
					}
				}

				// In các state có thể đến được với non-terminals
				for (int j = 0; j < no_nt; ++j) {
					bool check = false;
					for (auto it = state[i].gotos.begin(); it != state[i].gotos.end(); ++it) {
						if (it->first == non_terminals[j]) {
							check = true;
							cout << setw(10) << it->second + 1 << "  |";
						}
					}

					if (!check) {
						cout << setw(13) << "0  |";
					}
				}

				cout << setw(13) << "0  |\n";
			}

			cout << setw(14) << i + 1 << "|";
			for (int j = 0; j < no_t; ++j) {
				cout << setw(13) << "-  |";
			}
			for (int j = 0; j < no_nt; ++j) {
				cout << setw(13) << "-  |";
			}
			cout << setw(13) << "1  |\n";
			continue;
		}

		cout << setw(14) << i + 1 << "|";
		// In các state có thể đến được với terminals
		for (int j = 0; j < no_t; ++j) {
			bool check = false;
			for (auto it = state[i].gotos.begin(); it != state[i].gotos.end(); ++it) {
				if (it->first == terminals[j]) {
					check = true;
					cout << setw(10) << it->second + 1 << "  |";
				}
			} 

			if (!check) {
				cout << setw(13) << "0  |";
			}
		}

		// In các state có thể đến được với non-terminals
		for (int j = 0; j < no_nt; ++j) {
			bool check = false;
			for (auto it = state[i].gotos.begin(); it != state[i].gotos.end(); ++it) {
				if (it->first == non_terminals[j]) {
					check = true;
					cout << setw(10) << it->second + 1 << "  |";
				}
			}

			if (!check) {
				cout << setw(13) << "0  |";
			}
		}

		cout << setw(13) << "0  |\n";
	}

	system("pause");
	return 0;
}