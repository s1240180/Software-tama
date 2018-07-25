#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <regex>

using namespace std;

#define NODATA -1

struct DATA {
  int user;
  int book;
  double eval;
};

struct RES {
  int num;
  double score;
};

struct MODE {
  string command;
  int T_user;
  int T_book;
  double T_eval;
};

void read_data(istream&, ostream&, DATA&, vector<DATA>&);
void calculate(MODE, const DATA, vector<DATA>&);
bool compare(const RES, const RES);
void read_command(istream&, ostream&, const DATA, MODE&);

typedef vector<DATA>::size_type idx;

int main() {
  DATA max;
  vector<DATA> data;
  MODE mode;

  read_data(cin, cout, max, data);
  read_command(cin, cout, max, mode);
  calculate(mode, max, data);

  return 0;
}

bool compare(const RES d1, const RES d2) {
  return d1.score > d2.score;
}

void read_command(istream &in, ostream &out, const DATA max, MODE &mode) {
  int u = NODATA, b = NODATA;
  double e = NODATA;
  bool correct = true;
  string line;
  string::size_type s;

  while (1) {
    correct = true;
    out << "input the command" << endl;
    getline(in, line);

    //read command
    if (line.size() > 0) {
      if (regex_search(line, regex(" [0-9]"))) {
	s = line.find(" ");
	mode.command = line.substr(0, s);
	line.erase(0, s + 1);
      }
      else {
	correct = false;
	line.clear();
      }
    }

    //read u
    if (line.size() > 0) {
      if (regex_search(line, regex(" [0-9]"))) {
	s = line.find(" ");
	u = stoi(line.substr(0, s));
	line.erase(0, s + 1);
      }
      else {
	u = stoi(line);
	line.clear();
      }
    }

    //read b
    if (line.size() > 0) {
      if (regex_search(line, regex(" [0-9]"))) {
	s = line.find(" ");
	b = stoi(line.substr(0, s));
	line.erase(0, s + 1);
      }
      else {
	b = stoi(line);
	line.clear();
      }
    }

    //read e
    if (line.size() > 0)
      for (string::size_type i = 0; i < line.size(); i++)
	if (!(('0' <= line[i] && line[i] <= '9') || line[i] == '.') || i == line.size() - 1)
	  e = stod(line.substr(0, i + 1));

    if (mode.command != "rec" && mode.command != "eval") {
      correct = false;
      out << "comand is not found" << endl;
    }

    if (!(1 <= u && u <= max.user) && correct) {
      correct = false;
      out << "user is not found" << endl;
    }

    if (b != NODATA && !(1 <= b && b <= max.book) && correct) {
      correct = false;
      out << "book is not found" << endl;
    }

    if (e != NODATA && !(0 <= e && e <= 5) && correct) {
      correct = false;
      out << "evaluation value is not correct" << endl;
    }

    if (correct)
      break;

    out << "please input again" << endl;
    in.clear();
  }

  mode.T_user = u;
  mode.T_book = b;
  mode.T_eval = e;
}

void calculate(MODE mode, const DATA max, vector<DATA> &data) {
  vector<DATA> target;
  vector<RES> result;
  vector<double> score(max.user);
  vector<double> dist(max.user, 0);
  vector<double> recomm(max.book, 0);
  vector<double> S(max.book, 0);

  if (mode.command == "rec") {
    for (idx i = 0; i < data.size(); i++)
      if (data[i].user == mode.T_user)
	target.push_back(data[i]);

    for (idx i = 0; i < data.size(); i++) {
      if (data[i].user != mode.T_user) {
	for (idx j = 0; j < target.size(); j++)
	  if (target[j].book == data[i].book)
	    dist[data[i].user - 1] += pow(target[j].eval - data[i].eval, 2);
      }
    }

    for (vector<double>::size_type i = 0; i < max.user; i++) {
      if (i + 1 == mode.T_user)
	continue;
      else
	score[i] = 1 / (sqrt(dist[i]) + 1);
    }

    for (idx i = 0; i < data.size(); i++) {
      if (data[i].user != mode.T_user) {
	recomm[data[i].book - 1] += score[data[i].user - 1] * data[i].eval;
	S[data[i].book - 1] += score[data[i].user - 1];
      }
    }

    if (mode.T_book == NODATA) {
      for (int i = 0; i < max.book; i++) {
	RES tmp;
	bool insert = true;
	for (idx j = 0; j < target.size(); j++) {
	  if (i + 1 == target[j].book) {
	    insert = false;
	    break;
	  }
	}

	if (insert) {
	  tmp.num = i + 1;
	  tmp.score = recomm[i] / S[i];
	  result.push_back(tmp);
	}
      }

      sort(result.begin(), result.end(), compare);
    }
    else {
      RES tmp;
      tmp.num = mode.T_book;
      tmp.score = recomm[mode.T_book - 1] / S[mode.T_book - 1];
      result.push_back(tmp);
    }

    for (vector<RES>::size_type i = 0; i < result.size(); i++)
      cout << result[i].num << ":" << result[i].score << endl;
  }
  else if (mode.command == "eval") {
    bool change = false;
    for (idx i = 0; i < data.size(); i++) {
      if (data[i].user == mode.T_user && data[i].book == mode.T_book) {
	data[i].eval == mode.T_eval;
	change = true;
	cout << "change the data" << endl;
      }
    }

    if (!change) {
      DATA NEW;
      NEW.book = mode.T_book;
      NEW.eval = mode.T_eval;
      NEW.user = mode.T_user;
      data.push_back(NEW);
      cout << "insert new data" << endl;
    }
  }

}

void read_data(istream &in, ostream &out, DATA &max, vector<DATA> &data) {
  out << "input the number of users, books, evaluation" << endl;
  in >> max.user >> max.book >> max.eval;

  out << "input each data(user number,book,evaluation)" << endl;
  for (int i = 0; i < max.eval; i++) {
    DATA tmp;
    bool u = false, b = false, e = false;

    in >> tmp.user >> tmp.book >> tmp.eval;
    u = 1 <= tmp.user && tmp.user <= max.user;
    b = 1 <= tmp.book && tmp.book <= max.book;
    e = 0 <= tmp.eval && tmp.eval <= 5;

    if (u && b && e)
      data.push_back(tmp);
    else {
      out << "input data is not correct" << endl;
      out << "please input again" << endl;
      i--;
    }
  }
}
