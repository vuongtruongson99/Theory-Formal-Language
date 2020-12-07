// Ý tưởng:
// 1 - Đầu tiên, ta sẽ chia các state của DFA ra làm 2 phần (class): final state và normal state
//			+ Ví dụ: final state = {4}
//					 normal state = {0, 1, 2, 3}
// 
// 2 - Sau đó, dần dần tách cách class thành cách class bé hơn. Các state giống nhau (distinguishable)
// sẽ được cho vào 1 class chung
//			+ Ví dụ: final state chỉ có 1 phần từ => không cần tách
//					 normal state có 4 phần tử => cố gắng tách sao cho các state giống nhau được gộp vào 1 class
//
// 3 - Lặp lại bước 2 đến khi nào không thể tách được thành các class bé hơn nữa (số class sau == số class trước)
// => Hoàn thành

// Các hàm:
// int ind(int x) - Trả về class mà state x đang thuộc về trước khi thực hiện tách class


#include <iostream>
#include <vector>
#include <algorithm>
#include <assert.h>
using namespace std;

const int N = 109;

int transition[N][N]; // transition table
bool finalState[N]; // final states
vector<vector<int> > equalClass[2]; // set of equivalence classes - equalClass[0]: class trước khi tách
									//							  - equalClass[1]: class sau khi tách

// n is no of states, m is no of input symbols, nfs is no of final states
int n, m, nfs;

// Xem state X thuộc class nào: 
int idx(int x) {
	for (int i = 0; i < equalClass[0].size(); ++i) {
		for (int j = 0; j < equalClass[0][i].size(); ++j) {
			if (equalClass[0][i][j] == x) {
				return i;
			}
		}
	}
}

// Prints the minimised DFA
void print_minDfa(bool printed[]) {
	cout << "\n Minimised DFA Table:\n";
	cout << "---------------------\n";
	cout << "State\t";
	for (int j = 0; j < m; j++) cout << char(j + 'a') << "\t"; cout << endl << endl;

	for (int i = 0; i < n; ++i) {
		if (printed[i]) continue;

		int ind = idx(i);
		cout << "[";
		for (int k = 0; k < equalClass[0][ind].size(); ++k) {
			cout << char(equalClass[0][ind][k] + 'A');
			printed[equalClass[0][ind][k]] = 1;
		}
		cout << "]\t";
		for (int j = 0; j < m; j++) {
			ind = idx(transition[i][j]);
			cout << "[";
			for (int k = 0; k < equalClass[0][ind].size(); k++) {
				cout << char(equalClass[0][ind][k] + 'A');
			}
			cout << "]\t";
		}
		cout << endl;
	}
	cout << endl;
}

int main() {
	cout << "************************* DFA Minimization *************************" << endl << endl;
	cout << "Enter total number of states in DFA : ";
	cin >> n;
	cout << "Enter number of elements in alphabet : ";
	cin >> m;
	cout << "Enter number of final state in DFA: ";
	cin >> nfs;
	cout << "Enter " << nfs << " final state in DFA: ";
	for (int i = 0; i < nfs; ++i) {
		char final_state;
		cin >> final_state;
		// Đánh dấu final state trong DFA
		finalState[final_state - 'A'] = 1;
	}

	cout << "\n\n";
	// Input transition table of DFA:
	for (int i = 0; i < n; i++) {
		cout << "For state " << char(i + 'A') << endl;
		char a = 'a';
		for (int j = 0; j < m; j++) {
			cout << "Enter output states for input " << a++ << ": ";
			char nextState;
			cin >> nextState;
			transition[i][j] = nextState - 'A';
		}
		cout << endl;
	}

	// Khởi tạo 2 class ban đầu: final state và normal state
	vector<int> initialClass[2];
	for (int i = 0; i < n; ++i) {
		initialClass[finalState[i]].push_back(i);
	}

	equalClass[0].push_back(initialClass[0]); // equalClass[0][0] = initialClass[0];
	equalClass[0].push_back(initialClass[1]); // equalClass[0][1] = v[1];
	equalClass[1] = equalClass[0];

	// Lặp 100 lần để tách class
	int iters = 100;
	while (iters--) {
		equalClass[0] = equalClass[1];

		for (int i = 0; i < equalClass[1].size(); ++i) {
			for (int j = 1; j < equalClass[1][i].size(); ++j) {
				// Xem rằng equalClass[1][i][j] sẽ thuộc class nào
				bool placed = 0;
				for (int k = 0; k < j; ++k) {
					bool check = 1;
					for (int l = 0; l < m; ++l) {
						if (idx(transition[equalClass[1][i][j]][l]) != idx(transition[equalClass[1][i][k]][l])) {
							check = 0;
							break;
						}
					}

					// equalClass[1][i][j] vẫn ở nguyên class cũ
					if (check) {
						placed = 1;
						break;
					}
				}

				if (placed) continue;

				// Kiểm tra equalClass[1][i][j] có thuộc class mới nào không
				for (int x = equalClass[0].size(); x < equalClass[1].size(); ++x) {
					for (int k = 0; k < equalClass[1][x].size(); ++k) {
						// Kiểm tra equalClass[1][i][j] với equalClass[1][x][k]
						bool check = 1;
						for (int l = 0; l < m; ++l) {
							if (idx(transition[equalClass[1][i][j]][l]) != idx(transition[equalClass[1][x][k]][l])) {
								check = 0;
								break;
							}
						}

						// Nếu equalClass[1][i][j] thuộc class mới trong equalClass[1]
						if (check) {
							// Xóa equalClass[1][i][j] khỏi class cũ, chèn vào class mới equalClass[1][x]
							equalClass[1][x].push_back(equalClass[1][i][j]);
							equalClass[1][i].erase(equalClass[1][i].begin() + j);
							placed = 1;
							break;
						}
					}
				}

				if (placed) continue;

				// Tạo 1 class mới nếu equalClass[1][i][j] nếu không thuộc class cũ nào
				vector<int> newClass;
				newClass.push_back(equalClass[1][i][j]);
				equalClass[1].push_back(newClass);
				equalClass[1][i].erase(equalClass[1][i].begin() + j);
			}
		}

		// So sánh equalClass[0] và equalClass[1]
		for (int i = 0; i < equalClass[0].size(); ++i) {
			sort(equalClass[0][i].begin(), equalClass[0][i].end());
		}
		sort(equalClass[0].begin(), equalClass[0].end());

		for (int i = 0; i < equalClass[1].size(); ++i) {
			sort(equalClass[1][i].begin(), equalClass[1][i].end());
		}
		sort(equalClass[1].begin(), equalClass[1].end());

		if (equalClass[0] == equalClass[1]) break;
	}

	bool printed[N];
	for (int i = 0; i < n; ++i) {
		printed[i] = false;
	}

	print_minDfa(printed);

	system("pause");
	return 0;
}