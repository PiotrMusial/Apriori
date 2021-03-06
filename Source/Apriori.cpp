#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <iterator>
#include <time.h>
#include <cstdlib>

#define MAP_SI map<string, int>
#define MAP_SI_IT map<string, int>::iterator
#define VEC_S vector<string>
#define VEC_I vector<int>
#define VEC_SI vector<pair<string, int> >
#define VEC_SF vector<pair<string, float> >
#define VEC_S_2D vector<vector<string> >

#include "StringTokenizer.hpp";

using namespace std;

clock_t start, stop;
double exeTime;

const float MIN_SUPP = 0.12;
int numberOfTransactions = 0;
int numberOfItemsInSet = 0;
VEC_S candidates, large;
VEC_SF frequentPatterns;
bool enda, enda2;

void display_vec_si(VEC_SI v) {
	for (int i = 0; i < v.size(); i++)
		cout << v[i].first << "      " << v[i].second << endl;
	cout << endl;
}

void display_vec_sf(VEC_SF v) {
	for (int i = 0; i < v.size(); i++)
		cout << v[i].first << "      " << v[i].second << endl;
	cout << endl;
}

void display_vec_s(VEC_S v) {
	for (int i = 0; i < v.size(); i++)
		cout << v[i] << " | ";
	cout << endl;
}

/*****
***** 1 krok - generacja jednoelementowych zbiorów częstych
*****/

VEC_SI count_items_in_DB(ifstream &dataBaseFile) {
	MAP_SI itemCounted;
	MAP_SI_IT it;
	string item;
	string transaction;

	dataBaseFile.clear();
	dataBaseFile.seekg(0, ios::beg);

	while (getline(dataBaseFile, transaction)) {
		StringTokenizer st(transaction);
		VEC_S vecTransaction = st.getTokens();

		//dodawanie przedmiotow do mapy
		for (int i = 0; i < vecTransaction.size(); i++) {
			it = itemCounted.find(vecTransaction[i]);
			if (it != itemCounted.end())
				it->second++; // jeœli wystêpuje w mapie
			else
				itemCounted.insert(make_pair(vecTransaction[i], 1)); // jeœli nie wystêpuje w mapie
		}
		numberOfTransactions++;
	}

	VEC_SI vecItemsCounted;
	copy(itemCounted.begin(), itemCounted.end(), back_inserter(vecItemsCounted));

	//for (int i = 0; i < itemCounted.size(); i++)
		//cout << vecItemsCounted[i].first << " " << vecItemsCounted[i].second << endl;

	return vecItemsCounted;
}

void erase_patterns_below_minsupp(VEC_SI itemsCounted) {
	VEC_S frequentOneElementSets;

	for (int i = 0; i < itemsCounted.size();)
		if ((float)itemsCounted[i].second / numberOfTransactions < MIN_SUPP) // jeœli supp < MINSUPP
			itemsCounted.erase(itemsCounted.begin() + i); // usuniêcie przedmiotu z wektora
		else {
			large.push_back(itemsCounted[i].first);
			i++;
		}
}

void generate_one_element_frequent_patterns(ifstream &dataBaseFile) {
	VEC_SI itemsCounted = count_items_in_DB(dataBaseFile);
	erase_patterns_below_minsupp(itemsCounted);
	numberOfItemsInSet++;

	//cout << "LARGE" << endl;
	//display_vec_s(large);
}

/*****
***** 2 krok - generacja n + 1-elementowych zbiorów kandydujących
*****/

string trim(const string& str) {
	size_t first = str.find_first_not_of(' ');
	if (string::npos == first)
		return str;
	size_t last = str.find_last_not_of(' ');
	return str.substr(first, (last - first + 1));
}

void generate_two_elements_patterns() {
	VEC_S tmpLarge = large;
	large.erase(large.begin(), large.end());

	for (int i = 0; i < tmpLarge.size() - 1; i++)
		for (int j = i + 1; j < tmpLarge.size(); j++) {
			string newPattern = tmpLarge[i] + " ";
			newPattern += tmpLarge[j];
			candidates.push_back(newPattern);
		}
}

void generate_rest() {
	VEC_S tmpCandidates;
	enda = true;

	for (int i = 0; i < candidates.size() - 1; i++) {
		StringTokenizer st1(candidates[i]);
		VEC_S vecSt1 = st1.getTokens();

		for (int j = i + 1; j < candidates.size(); j++) {
			StringTokenizer st2(candidates[j]);
			VEC_S vecSt2 = st2.getTokens();
			string str1 = "";
			string str2 = "";

			for (int k = 0; k < numberOfItemsInSet - 1; k++) {
				str1 += vecSt1[k] + " ";
				str2 += vecSt2[k] + " ";
			}

			//cout << str1 << " " << str2 << endl;

			if (str2.compare(str1) == 0) {
				//cout << str1 + vecSt1[vecSt1.size() - 1] + " " + vecSt2[vecSt2.size() - 1] << endl;
				tmpCandidates.push_back(str1 + vecSt1[vecSt1.size() - 1] + " " + vecSt2[vecSt2.size() - 1]);
				enda = false;
			}
			else
				break;
		}
	}

	if (!enda) {
		candidates.erase(candidates.begin(), candidates.end());
		candidates = tmpCandidates;
		tmpCandidates.erase(tmpCandidates.begin(), tmpCandidates.end());
	}
}

void apriori_gen() {
	if (numberOfItemsInSet == 1)
		generate_two_elements_patterns();
	else
		generate_rest();
	numberOfItemsInSet++;
}

/*****
***** 3 krok - liczenie wsparcia kandydatów i usuniecię tych z supp < MIN_SUPP
*****/

void add_large_to_vector(VEC_I counter) {
	for (int i = 0; i < candidates.size(); i++) {
		float supp = (float)counter[i] / (float)numberOfTransactions;
		if (supp >= MIN_SUPP) {
			large.push_back(candidates[i]);
			frequentPatterns.push_back(make_pair(candidates[i], supp));
		}
	}
}

void check_large_size() {
	if (large.size() > 0) {
		candidates.erase(candidates.begin(), candidates.end());
		candidates = large;
		large.erase(large.begin(), large.end());
	}
	else {
		cout << "No more large sets!" << endl;
		enda2 = true;
		// flaga - przerwanie glownej petli programu!
	}
}

void count_support_of_pattern(ifstream &dataBaseFile) {
	dataBaseFile.clear();
	dataBaseFile.seekg(0, ios::beg);

	string transaction;
	bool match;
	VEC_I counter(candidates.size());

	while (getline(dataBaseFile, transaction)) {
		StringTokenizer st1(transaction);
		VEC_S vecTransaction = st1.getTokens();
		VEC_S notFound;

		cout << "Transaction: " << endl;
		display_vec_s(vecTransaction);

		for (int i = 0; i < candidates.size(); i++) {
			StringTokenizer st2(candidates[i]);
			VEC_S vecCandidate = st2.getTokens();

			//cout << i+1 << " pattern - ";

			for (int j = 0; j < vecCandidate.size(); j++) {
				match = false;

				if (find(notFound.begin(), notFound.end(), vecCandidate[j]) != notFound.end())
					break;

				//cout << "TOKEN: " << vecCandidate[j] << " ";

				if (find(vecTransaction.begin(), vecTransaction.end(), vecCandidate[j]) != vecTransaction.end())
					match = true;
				else {
					match = false;
					notFound.push_back(vecCandidate[j]);
					break;
				}
			}

			if (match) {
				//cout << "found" << endl;
				counter[i]++;
			}
			//else
				//cout << "not found" << endl;
		}

		//cout << "NOT FOUND: ";
		//display_vec_s(notFound);
		notFound.erase(notFound.begin(), notFound.end());
	}

	add_large_to_vector(counter);
	check_large_size();
}

/*****
***** 4 krok - generacja wszystkich reguł asocjacyjnych
*****/

// zliczanie przedmiotów w zbiorze
int count_number_of_items_in_pattern(string pattern) {
	int counter = 0;
	istringstream ss(pattern);
	int num;
	while (ss >> num)
		counter++;
	return counter;
}

// generazja wszystkich podzbiorów zbioru
VEC_S_2D generate_all_subsets(string oneSet) {
	StringTokenizer st(oneSet);
	VEC_S pattern = st.getTokens();
	int t = pattern.size();
	int n = t;
	int numberOfItemsInSet = count(oneSet.begin(), oneSet.end(), ' ') + 1;
	VEC_S_2D subsets(numberOfItemsInSet - 1);
	string str;

	for (int i = 1; i < (1 << n) - 1; i++) {
		str = "";
		for (int j = 0; j < n; j++) {
			if (i & (1 << j))
				str = str + pattern[j] + " ";
		}
		int length = count_number_of_items_in_pattern(str);
		subsets[length - 1].push_back(str);
	}
	return subsets;
}

// sprawdza czy dany pozdzbiór występuje w drugim podzbiorze
bool is_present(string str1, string str2) {
	StringTokenizer st1(str1);
	StringTokenizer st2(str2);
	VEC_S tmp1 = st1.getTokens();
	VEC_S tmp2 = st2.getTokens();
	bool found;

	for (int i = 0; i < tmp1.size(); i++)
		if (find(tmp2.begin(), tmp2.end(), tmp1[i]) != tmp2.end())
			return true;
		else
			found = false;

	return found;
}

// generuje reguly asocjacyjne z dwuelementowych zbiorów
void generate_of_two(string pattern, float support, int &counter) {
	StringTokenizer st(pattern);
	VEC_S vecPattern = st.getTokens();

	cout << vecPattern[0] << " => " << vecPattern[1] << "  supp: " << support << endl;
	cout << vecPattern[1] << " => " << vecPattern[0] << "  supp: " << support << endl;
	counter += 2;
}

// generuje reguły asocjacyjne ze zbiorów o parzystej liczbie przedmiotów (oprócz ziorów dwuelementowych)
void generate_of_even(string pattern, float support, int &counter) {
	VEC_S_2D allSubsets = generate_all_subsets(pattern);

	for (int i = 0; i < allSubsets.size(); i++) {
		if (i == allSubsets.size() - 1)
			break;

		if (i != allSubsets.size() / 2) {
			VEC_S first = allSubsets[i];
			VEC_S last = allSubsets[allSubsets.size() - 1 - i];

			for (int j = 0; j < first.size(); j++) {
				for (int k = 0; k < last.size(); k++) {
					if (!is_present(first[j], last[k])) {
						cout << first[j] << " => " << last[k] << " supp: " << support << endl;
						cout << last[k] << " => " << first[j] << " supp: " << support << endl;
						counter += 2;
					}
				}
			}
		}
		else {
			VEC_S mid = allSubsets[i];

			for (int i = 0; i < mid.size() - 1; i++) {
				for (int j = i + 1; j < mid.size(); j++) {
					if (!is_present(mid[i], mid[j])) {
						cout << mid[i] << " => " << mid[j] << " supp: " << support << endl;
						cout << mid[j] << " => " << mid[i] << " supp: " << support << endl;
						counter += 2;
					}
				}
			}
		}
	}
}

// generuje reguły asocjacyjne ze zbiorów o nieparzystej liczbie przedmiotów
void generate_of_odd(string pattern, float support, int &counter) {
	VEC_S_2D allSubsets = generate_all_subsets(pattern);

	for (int i = 0; i < allSubsets.size(); i++) {
		VEC_S first = allSubsets[i];
		VEC_S last = allSubsets[allSubsets.size() - 1 - i];

		if (i == allSubsets.size() - i)
			break;

		for (int j = 0; j < first.size(); j++) {
			for (int k = 0; k < last.size(); k++) {
				if (!is_present(first[j], last[k])) {
					cout << first[j] << " => " << last[k] << " supp: " << support << endl;
					cout << last[k] << " => " << first[j] << " supp: " << support << endl;
					counter += 2;
				}
			}
		}
	}
}

// głowna funkcja do generowania reguł asocjacyjnych
void generate_all_association_rules() {
	int counter = 0;

	for (int i = 0; i < frequentPatterns.size(); i++) {
		string pattern = frequentPatterns[i].first;
		float support = frequentPatterns[i].second;
		int numberOfItems = count_number_of_items_in_pattern(pattern);

		if (numberOfItems == 2) {
			generate_of_two(pattern, support, counter);
		}
		else if (numberOfItems % 2 == 0) {
			generate_of_even(pattern, support, counter);
		}
		else if (numberOfItems % 2 == 1) {
			generate_of_odd(pattern, support, counter);
		}
	}

	cout << "Lczba wszystkich zbiorow czestych: " << frequentPatterns.size() << endl;
	cout << "Liczba wszystkich regul asocjacyjnych: " << counter << endl;
}

void apriori(ifstream &dataBaseFile) {
	start = clock();

	generate_one_element_frequent_patterns(dataBaseFile);
	//cout << "LARGE" << endl;
	//display_vec_s(large);

	while (true) {
		apriori_gen();
		//cout << "CANDIDATES" << endl;
		//display_vec_s(candidates);

		if (enda)
			break;

		count_support_of_pattern(dataBaseFile);

		if(enda2)
			break;

		//cout << "LARGE" << endl;
		//display_vec_s(candidates);
	}

	display_vec_sf(frequentPatterns);

	generate_all_association_rules();

	stop = clock();
	exeTime = (double)(stop - start) / CLOCKS_PER_SEC;
	cout << "Execution time: " << exeTime << " seconds." << endl;
}

int main() {
	ifstream dataBaseFile;
	dataBaseFile.open("F:/Moje Dokumenty/E/Praca licencjacka/BAZY/TABELA/db1.txt", ios::in);
	if (!dataBaseFile.good()) {
		cout << "Failure while opening file";
		exit(1);
	}

	apriori(dataBaseFile);

	dataBaseFile.close();
}
