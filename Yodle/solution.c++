#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

class Juggler;
class Circuit;

vector<Circuit> cs;
vector<Juggler> js;
int cid = 0;

//////////////////// Juggler Class ////////////////////////
class Juggler {
public:
	string name;
	int h;
	int e;
	int p;
	vector<int> prefs;
	vector<int> prefDot;
	bool matched;
	int matchNum;
	Juggler(string name, int hand, int endurance, int pizzazz, vector<int> preferences) :
		name (name),
		h (hand),
		e (endurance),
		p (pizzazz),
		prefs (preferences),
		matched (false),
		matchNum (-1)
	{ prefDot = vector<int>(); }

	void print() {
		cout << "My name is: " << name << endl;
		cout << "Hand-eye coordination: " << h << " endurance: " << e << " pizzazz: " << p << endl;
		cout << "My preferences are: ";
		int i = 0;
		for(i; i < prefs.size(); ++i)
			cout << prefs[i] << " ";
		cout << endl;
	}
};

//////////////////// Circuit Class ////////////////////////
class Circuit {
public:
	string name;
	int h;
	int e;
	int p;
	int id;
	vector<int> jugglerMatch;
	vector<int> finalJuggs;
	bool filled;
	bool propsedAll;
	int propNum;
	Circuit(string name, int hand, int endurance, int pizzazz, int i) :
		name (name),
		h (hand),
		e (endurance),
		p (pizzazz),
		id (i),
		filled (false),
		propsedAll (false),
		propNum (0)
	{ jugglerMatch = vector<int>();
	  finalJuggs = vector<int>(); }

	void calculateMatches() {
		vector<int> dots = vector<int>();
		int i = 0;
		for(i; i < js.size(); ++i)
			dots.push_back((h * js[i].h) + (e * js[i].e) + (p * js[i].p));
		i = 0;
		for(i; i < dots.size(); ++i)
			js[i].prefDot.push_back(dots[i]);
		int j = 0;
		for(j; j < dots.size(); ++j) {
			i = 1;
			int max = dots[0];
			int index = 0;
			for(i; i < dots.size(); ++i) {
				if(dots[i] > max){
					max = dots[i];
					index = i;
				}
			}
			dots[index] = -1;
			jugglerMatch.push_back(index);
		}
	}
};

int stripString(string s) {
	string num = s.substr(2);
	return stoi(num);
}

void scanInput() {
	string line;
	ifstream inFile("jugglefest.txt");
	while(getline(inFile, line)) {
		if(!line.empty()){
			istringstream iss(line);
			char classification;
			string name, hand, endurance, pizzazz, p;
			iss >> classification >> name >> hand >> endurance >> pizzazz;
			if(classification == 'C') {
				// line is a circuit
				Circuit t = Circuit(name, stripString(hand), stripString(endurance), stripString(pizzazz), cid);
				++cid;
				cs.push_back(t);
			} else if(classification == 'J') {
				// line is a juggler
				iss >> p;
				string delimiter = ",";
				vector<int> pref;
				size_t pos = 0;
				string token;
				while ((pos = p.find(delimiter)) != string::npos) {
				    token = p.substr(1, pos - 1);
				    pref.push_back(stoi(token));
				    p.erase(0, pos + delimiter.length());
				}
				pref.push_back(stoi(p.substr(1)));
				Juggler j = Juggler(name, stripString(hand), stripString(endurance), stripString(pizzazz), pref);
				js.push_back(j);
			} else {
				cout << "There is an error in your input file, try again" << endl;
				exit(1);
			} 
		}

	}
}

int keepItGoin() {
	bool result = false;
	int i = 0;
	for(i; i < cs.size(); ++i)
		if(!cs[i].filled && !cs[i].propsedAll)
			return i;
	return -1;
}

bool prefWho(int w, int m, int f) {
	int i = 0;
	int t;
	for(i; i < js[w].prefs.size(); ++i) {
		t = js[w].prefs[i];
		if(t == m)
			return true;
		if(t == f)
			return false;
	}
	return false;
}

void freeUpDumped(int m, int w) {
	int i = 0;
	for(i; i < cs[m].finalJuggs.size(); ++i) {
		if(cs[m].finalJuggs[i] == w)
			cs[m].finalJuggs.erase(cs[m].finalJuggs.begin() + i);
	}
	cs[m].filled = false;
}

void stableMatching(int maxJs) {
	int i = keepItGoin();
	while((i = keepItGoin()) >= 0) {
		int w = cs[i].jugglerMatch[cs[i].propNum];
		++cs[i].propNum;
		if(cs[i].propNum == js.size())
			cs[i].propsedAll = true;
		if(!js[w].matched) {
			cs[i].finalJuggs.push_back(w);
			if(cs[i].finalJuggs.size() >= maxJs) {
				cs[i].filled = true;
			}
			js[w].matched = true;
			js[w].matchNum = i;
		}
		else if(prefWho(w, i, js[w].matchNum)) {
			freeUpDumped(js[w].matchNum, w);
			cs[i].finalJuggs.push_back(w);
			if(cs[i].finalJuggs.size() == maxJs)
				cs[i].filled = true;
			js[w].matchNum = i;
		}
		else {
			continue;
		}
	}
}

int main() {
	scanInput();
	int i = 0;
	for(i; i < cs.size(); ++i)
		cs[i].calculateMatches();
	int maxJs = js.size() / cs.size();
	stableMatching(maxJs);
	ofstream outFile;
	outFile.open("output.txt");
	i = cs.size() - 1;
	int j, k;
	for(i; i >= 0; --i) {
		outFile << "C" << i << " ";
		for(j = 0; j < cs[i].finalJuggs.size(); ++j) {
			outFile << " J" << cs[i].finalJuggs[j] << " ";
			for(k = 0; k < js[cs[i].finalJuggs[j]].prefs.size(); ++k) {
				if(k == js[cs[i].finalJuggs[j]].prefs.size() - 1 && j != cs[i].finalJuggs.size() - 1)
					outFile << "C" << js[cs[i].finalJuggs[j]].prefs[k] << ":" << js[cs[i].finalJuggs[j]].prefDot[js[cs[i].finalJuggs[j]].prefs[k]] << ", ";
				else
					outFile << "C" << js[cs[i].finalJuggs[j]].prefs[k] << ":" << js[cs[i].finalJuggs[j]].prefDot[js[cs[i].finalJuggs[j]].prefs[k]] << " ";
			}
		}
		outFile << endl;
	}
	outFile.close();
	return 0;
}