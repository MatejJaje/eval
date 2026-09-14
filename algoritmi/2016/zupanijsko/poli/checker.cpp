// Checker to be used by CMS evaluator.
//
// Usage: [checker] [input] [official_output] [contestant_output]
//
// Score (real between 0.0 and 1.0) written on stdout.
// Textual description of the result written on stderr.

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <set>

using namespace std;

#define FOR(i, a, b) for (int i = (a); i < int(b); ++i)
#define REP(i, n) FOR(i, 0, n)
#define TRACE(x) cout << #x << " = " << x << endl
#define _ << " _ " <<

typedef long long llint;
typedef pair<llint, llint> P;

const llint INF = 1LL<<31;

enum cmd_type {MUL, ADD};

struct by_second {
  int operator() (const P &a, const P &b) {
    if (a.second != b.second)
      return a.second > b.second;
    return a.first < b.first;
  }
};

struct cmd {
  cmd_type t;
  int a;
  int b;
  int operator== (const cmd &x) {
    return a == x.a && b == x.b && t == x.t;
  }
};

int parse_command(const string &line, cmd &c) {
  int a, b;
  if (sscanf(line.c_str(), " mul d%d d%d", &a, &b) == 2) {
    c = {MUL, a, b};
    if (a < 0 || a > 9 || b < 0 || b > 9)
      return 0;
    return 1;
  }
  if (sscanf(line.c_str(), " add d%d d%d", &a, &b) == 2) {
    c = cmd{ADD, a, b};
    if (a < 0 || a > 9 || b < 0 || b > 9)
      return 0;
    return 1;
  }
  return 0;
}

void test_parse_command() {
  cmd c;
  assert(parse_command("mul d0 d1", c));
  assert(c == (cmd{MUL, 0, 1}));
  assert(parse_command("mul d9 d2", c));
  assert(c == (cmd{MUL, 9, 2}));
  assert(parse_command("add d9 d2", c));
  assert(c == (cmd{ADD, 9, 2}));
  assert(parse_command(" add d9 d2", c));
  assert(c == (cmd{ADD, 9, 2}));
  assert(parse_command(" add   d9  d2\n", c));
  assert(c == (cmd{ADD, 9, 2}));
  assert(parse_command("add d9d2", c));
  assert(c == (cmd{ADD, 9, 2}));
  assert(!parse_command("addm d9 d2\n", c));
  assert(!parse_command("add mull d9 d2\n", c));
  assert(!parse_command("ADD d9 d2", c));
  assert(!parse_command("add d10 d2", c));
  assert(!parse_command("add d0 d10", c));
  cerr << "test_parse_command pass" << endl;
}

int parse_poli(const string &line, vector<P> &poli) {
  enum {BEFORE, A, POT, B} state = BEFORE;
  poli.clear();
  int a = 0;
  int b = 0;
  for (int i=0; i<(int)line.size(); i++)
    if (state == BEFORE) {
      if (line[i] == 'x')
        a = 1, b = 1, state = POT;
      else if (isdigit(line[i]))
        a = line[i]-'0', state = A, b = 1;
      else
        return 0;
    } else if (state == A) {
      if (line[i] == 'x')
        state = POT;
      else if (isdigit(line[i]))
        a = a*10 + line[i]-'0';
      else
        return 0;
    } else if (state == POT) {
      if (line[i] == '^')
        b = 0, state = B;
      else if (line[i] == '+')
        poli.push_back({a, b}), state=BEFORE;
      else
        return 0;
    } else if (state == B) {
      if (isdigit(line[i]))
        b = b*10 + line[i]-'0';
      else if (line[i] == '+')
        poli.push_back({a, b}), state=BEFORE;
      else
        return 0;
    }
  if (!(state == POT || state == B))
    return 0;
  poli.push_back({a, b});
  return 1;
}

void test_parse_poli() {
  vector<P> poli;
  assert(!parse_poli("", poli));
  assert(!parse_poli("xx", poli));
  assert(!parse_poli("x+", poli));
  assert(!parse_poli("2", poli));
  assert(!parse_poli("2", poli));
  assert(parse_poli("x", poli));
  assert(poli == (vector<P>{{1, 1}}));
  assert(parse_poli("x^2", poli));
  assert(poli == (vector<P>{{1, 2}}));
  assert(parse_poli("x^23", poli));
  assert(poli == (vector<P>{{1, 23}}));
  assert(parse_poli("12x^23", poli));
  assert(poli == (vector<P>{{12, 23}}));
  assert(parse_poli("12x^23+x", poli));
  assert(poli == (vector<P>{{12, 23}, {1, 1}}));
  assert(parse_poli("12x^23+x^34+12x", poli));
  assert(poli == (vector<P>{{12, 23}, {1, 34}, {12, 1}}));
  assert(parse_poli("12x^23+x^34+12x+x+32x^2", poli));
  assert(poli == (vector<P>{{12, 23}, {1, 34}, {12, 1}, {1, 1}, {32, 2}}));
  cerr << "test_parse_poli pass" << endl;
}

P add(const P &x, const P &y) {
  assert(x.second == y.second);
  return {min(INF, x.first+y.first), x.second};
}

P mul(const P &x, const P &y) {
  return {min(INF, x.first*y.first), min(INF, x.second+y.second)};
}

void normalize(vector<P> &poli) {
  sort(poli.begin(), poli.end(), by_second());
  for (int i=0; i<(int)poli.size()-1; i++)
    if (poli[i].second == poli[i+1].second) {
      poli[i+1] = add(poli[i+1], poli[i]);
      poli[i].first = poli[i].second = 0;
    }
  sort(poli.begin(), poli.end(), by_second());
  poli.erase(find(poli.begin(), poli.end(), P{0, 0}), poli.end());
}

void test_normalize() {
  vector<P> poli = {{3, 4}, {1, 2}, {3, 4}, {10, 2}, {34, 1}};
  normalize(poli);
  assert(poli == (vector<P>{{6, 4}, {11, 2}, {34, 1}}));
  normalize(poli);
  assert(poli == (vector<P>{{6, 4}, {11, 2}, {34, 1}}));
  poli = {{1, 1}};
  normalize(poli);
  assert(poli == (vector<P>{{1, 1}}));
  poli = {{1, 1}, {1, 1}};
  normalize(poli);
  assert(poli == (vector<P>{{2, 1}}));
  cerr << "test_normalize pass" << endl;
}

void add(vector<P> &a, const vector<P> &b) {
  int n = (int)b.size();
  for (int i=0; i<n; i++)
    a.push_back(b[i]);
  normalize(a);
}

void mul(vector<P> &a, const vector<P> &b) {
  vector<P> aa = a;
  vector<P> bb = b;
  a.clear();
  for (auto d : aa)
    for (auto c : bb)
      a.push_back(mul(d, c));
  normalize(a);
}

string to_string(const vector<P> &poli) {
  if ((int)poli.size() == 0)
    return "0";
  string result;
  for (int i=0; i<(int)poli.size(); i++) {
    if (i > 0)
      result += "+";
    if (poli[i].first > 1)
      result += to_string(poli[i].first);
    result += "x";
    if (poli[i].second > 1)
      result += "^" + to_string(poli[i].second);
  }
  return result;
}


vector<P> run(const vector<cmd> &cmds) {
  vector<P> reg[10];
  reg[0].push_back({1, 1});
  for (auto c : cmds) {
    if (c.t == ADD)
      add(reg[c.a], reg[c.b]);
    else
      mul(reg[c.a], reg[c.b]);
  }
  return reg[0];
}


/**
 * @param p fraction of points awarded to the contestant.
 * @pararm m error message displayed to the contestant.
 */
void finish(double p, const string& m);

/**
 * The main checking function.
 * @param fin official input
 * @param foff official output
 * @param fout contestant's output
 */
void checker(ifstream& fin, ifstream& foff, ifstream& fout)
{
  const string WRONG_OUTPUT_FORMAT = "Krivo formatiran izlaz.";
  const string TEST_DATA_ERROR = "Greska u sluzbenom ulazu ili izlazu.";
  const string WRONG = "Netocno.";
  const string CORRECT = "Tocno.";
  const string PARTIAL_FIRST = "Prvi red je tocan, drugi nije.";

  // Read official input
  string polinom_str;
  if (!(fin >> polinom_str)) finish(0, TEST_DATA_ERROR);
  vector<P> polinom;
  if (!(parse_poli(polinom_str, polinom))) finish(0, TEST_DATA_ERROR);
  // Read official output
  // Nothing to read

  // Read contestant's output
  vector<cmd> cmds;
  int line_number = 0;
  string line;
  while (getline(fout, line)) {
    line_number++;
    // Skip empty lines
    if ((int)line.size() == 0)
      continue ;
    cmd c;
    if (!parse_command(line, c))
      finish(0, WRONG_OUTPUT_FORMAT +
             " Greska u redu " +
             to_string(line_number) +
             ".");
    cmds.push_back(c);
  }

  // Check
  if ((int)cmds.size() > 300)
    finish(0, "Prevelik broj naredbi (" + to_string(cmds.size()) + ").");
  vector<P> result=run(cmds);
  if (result != polinom)
    finish(0, WRONG +
           "(ocekivao " + polinom_str +
           ". dobio " + to_string(result) + ")");
  finish(1, CORRECT + " (" + to_string(cmds.size()) + " naredbi)");

  // The function MUST terminate before this line via finish()!
}

void finish(double p, const string& m) {
  cout << p << endl;
  cerr << m << endl;
  exit(0);
}

int main(int argc, char *argv[])
{
  if (argc != 4) {
    test_parse_command();
    test_parse_poli();
    test_normalize();
  }

  assert(argc == 4);

  ifstream fin(argv[1]);
  ifstream foff(argv[2]);
  ifstream fout(argv[3]);

  assert(!fin.fail() && !fout.fail());
  checker(fin, foff, fout);
  assert(false); // checker must terminate via finish() before exiting!

  return 0;
}
