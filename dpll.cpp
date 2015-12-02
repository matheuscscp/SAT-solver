#include <cstdio>
#include <set>
#include <map>
#include <string>
#include <algorithm>

using namespace std;

string int2str(int n) {
  char buf[12];
  sprintf(buf,"%d",n);
  return string(buf);
}

struct Clause {
  set<int> lits;
};
struct Variable {
  set<int> pos; // the set of clauses in which this variable occurs positive
  set<int> neg; // the set of clauses in which this variable occurs negative
  set<int>& occurrences(int sign) { return sign > 0 ? pos : neg; }
};
struct Formula {
  map<int,Clause> clauses;
  map<int,Variable> vars;
  
  // default constructor
  Formula() {}
  
  // read an instance from stdin in DIMACS format
  void read() {
    clauses.clear();
    vars.clear();
    
    while (getchar() == 'c') { // ignore comments
      for (char c = getchar(); c != '\n'; c = getchar()); // ignore line
    }
    for (int i = 0; i < 5; i++) { // ignore format
      getchar();
    }
    
    int nbvar, nbclauses;
    scanf("%d %d", &nbvar, &nbclauses);
    for (int i = 1; i <= nbclauses; i++) {
      Clause& c = clauses[i];
      for (int lit; scanf("%d",&lit) > 0 && lit;) {
        c.lits.insert(lit);
        vars[abs(lit)].occurrences(lit).insert(i);
      }
    }
    
    dpll();
  }
  
  // convert formula to human-legible string
  static string lit2str(int lit) {
    return lit > 0 ? "X"+int2str(lit) : "~X"+int2str(-lit);
  }
  string str() const {
    string ret;
    bool printed = false;
    for (auto& c : clauses) {
      if (printed) ret += '^';
      printed = true;
      ret += '(';
      if (c.second.lits.size() > 0) {
        auto it = c.second.lits.begin();
        ret += lit2str(*it);
        for (it++; it != c.second.lits.end(); it++) ret += (" V "+lit2str(*it));
      }
      ret += ')';
    }
    ret += " |";
    for (auto& v : vars) {
      ret += ' ';
      ret += lit2str(v.first)+":";
      ret += " pos={";
      printed = false;
      for (int c : v.second.pos) {
        if (printed) ret += ',';
        printed = true;
        ret += int2str(c);
      }
      ret += "} neg={";
      printed = false;
      for (int c : v.second.neg) {
        if (printed) ret += ',';
        printed = true;
        ret += int2str(c);
      }
      ret += "};";
    }
    return ret;
  }
  
  // SAT decider (the backtracking)
  bool sat() const {
    // trivial cases
    if (clauses.size() == 0) return true; // no clause -> satisfiable
    for (auto& c : clauses) { // empty clause exists -> unsatisfiable
      if (c.second.lits.size() == 0) return false;
    }
    
    bool answer = false;
    int var = vars.begin()->first; // var choice
    for (int sign = 1; sign >= -1 && !answer; sign -= 2) { // split
      answer = Formula(*this, var*sign).sat();
    }
    return answer;
  }
  
  // copy constructor with literal simplification and dpll rules
  Formula(const Formula& other, int lit) : Formula(other) {
    simplify(lit);
    dpll();
  }
  
  // as simple as it sounds: simplify the formula with lit = true
  void simplify(int lit) {
#ifdef DEBUG
    printf("%s",str().c_str());
#endif
    
    auto var = vars.find(abs(lit)); // lit's variable
    
    // erase negative occurrences
    auto& neg = var->second.occurrences(-lit);
    for (int i : neg) clauses[i].lits.erase(-lit);
    
    // handle clauses with positive occurrences
    auto& pos = var->second.occurrences(lit);
    for (int i : pos) {
      // erase occurrences of other literals of this clause from vars map
      auto c = clauses.find(i);
      for (int l : c->second.lits) {
        auto v = vars.find(abs(l));
        if (v->first != var->first) {
          v->second.occurrences(l).erase(i);
          if (v->second.pos.size() == 0 && v->second.neg.size() == 0) {
            vars.erase(v);
          }
        }
      }
      
      clauses.erase(c);
    }
    
    vars.erase(var); // erase lit's variable
    
#ifdef DEBUG
    printf("   ---simplify(%s)--->   %s\n",lit2str(lit).c_str(),str().c_str());
#endif
  }
  
  void dpll() {
    for (bool u = true, p; u || p; u = unit(), p = pure());
  }
  
  // find ONE unit clause with literal lit and call simplify(lit)
  bool unit() {
    int lit = 0;
    for (auto& c : clauses) {
      if (c.second.lits.size() == 1) {
        lit = *c.second.lits.begin();
        break;
      }
    }
    if (lit) {
      simplify(lit);
      return true;
    }
    return false;
  }
  
  // find ONE pure literal lit and call simplify(lit)
  bool pure() {
    int lit = 0;
    for (auto& v : vars) {
      if (v.second.neg.size() == 0) {
        lit = v.first;
        break;
      }
      else if (v.second.pos.size() == 0) {
        lit = -v.first;
        break;
      }
    }
    if (lit) {
      simplify(lit);
      return true;
    }
    return false;
  }
};

int main() {
  Formula phi;
  phi.read();
  if (phi.sat())  printf("SATISFIABLE\n");
  else            printf("UNSATISFIABLE\n");
  return 0;
}
