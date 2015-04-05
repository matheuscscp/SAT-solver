/*
 * main.cpp
 *
 *  Created on: Apr 4, 2015
 *      Author: Pimenta
 */

#include <cstdio>
#include <cstring>
#include <string>
#include <sstream>

using namespace std;

#define abs(X)  ((X) > 0 ? (X) : (-(X)))

// the size of a formula is the sum of the number of literals in each clause
#define MAX_FORMULA_SIZE  1000

// maximum number of variables
#define MAX_VARIABLES     50

// represents a propositional formula
struct Formula {
  // i means the i-th variable, -i means the i-th variable negated
  int literals[MAX_FORMULA_SIZE];
  int nbliterals;
  
  // index of the last literal of the clause (in the array of literals
  // above) + 1. starts at 1 instead of 0. clauses[0] is always 0
  int clauses[1 + MAX_FORMULA_SIZE];
  int nbclauses;
  
  // 0 if there is no unit clauses in this formula, or the literal, otherwise
  int unitclause;
  
  // the number of variables
  static int nbvar;
  
  // solver stack
  static Formula simplifications[MAX_VARIABLES + 1];
  static int literal;
  static int stack_size;
  
  // constructor
  Formula() {
    clauses[0] = 0;
  }
  
  // read the instance from stdin
  void read() {
    while (getchar() == 'c') { // ignore comments
      for (char c = getchar(); c != '\n'; c = getchar()); // ignore line
    }
    for (int i = 0; i < 5; i++) { // ignore format
      getchar();
    }
    scanf("%d %d", &nbvar, &nbclauses);
    nbliterals = 0;
    for (int i = 1; i <= nbclauses; i++) {
      int cur_literals = 0;
      while (scanf("%d", &literals[nbliterals]) == 1 && literals[nbliterals]) {
        nbliterals++;
        cur_literals++;
      }
      clauses[i] = nbliterals;
      if (cur_literals == 1 && !unitclause) {
        unitclause = literals[nbliterals - 1];
      }
    }
  }
  
  // DPLL solver
  bool solve() {
    if (nbliterals == nbclauses) { // if this formula is a set of literals
      return consistent_set_of_literals();
    }
    if (contains_empty_clause()) { // unsatisfiable
      return false;
    }
    while (unitclause) { // propagate unit clauses
      unit_propagate();
    }
  }
  
  // if this formula is a set of literals (check before entering this function),
  // this function will tell if it is consistent or not
  bool consistent_set_of_literals() {
    static char signs[MAX_VARIABLES + 1];
    memset(signs, 0, sizeof signs);
    for (int i = 0; i < nbliterals; i++) {
      int var = abs(literals[i]);
      char& sign = signs[var];
      if (!sign) {
        sign = (literals[i] > 0 ? 1 : -1);
        continue;
      }
      if (var*sign != literals[i]) {
        return false; // inconsistent set of literals
      }
    }
    return true; // consistent set of literals
  }
  
  // check if this formula contains an empty clause
  bool contains_empty_clause() {
    for (int i = 1; i <= nbclauses; i++) {
      if (clauses[i] == clauses[i - 1]) {
        return true;
      }
    }
    return false;
  }
  
  // propagate the unit clause
  void unit_propagate() {
    int nbclauses = 0;
    int nbliterals = 0;
    int unitclause = 0;
    for (int i = 1; i <= this->nbclauses; i++) {
      bool found = false;
      int cur_nbliterals = 0;
      for (int j = clauses[i - 1]; j < clauses[i] && !found; j++) {
        if (literals[j] == this->unitclause) {
          found = true;
        }
        else if (literals[j] != -this->unitclause) {
          literals[nbliterals + (cur_nbliterals++)] = literals[j];
        }
      }
      if (!found) {
        nbliterals += cur_nbliterals;
        clauses[++nbclauses] = nbliterals;
        if (cur_nbliterals == 1 && !unitclause) {
          unitclause = literals[nbliterals - 1];
        }
      }
    }
    this->nbclauses = nbclauses;
    this->nbliterals = nbliterals;
    this->unitclause = unitclause;
  }
  
  // prettyprint formula
  void show() {
    if (nbliterals == 0) {
      printf("\n");
      return;
    }
    bool printed = false;
    for (int i = 1; i <= nbclauses; i++) {
      if (printed) {
        printf("^");
      }
      else {
        printed = true;
      }
      printf("(%s", literal_to_str(literals[clauses[i - 1]]).c_str());
      for (int j = clauses[i - 1] + 1; j < clauses[i]; j++) {
        printf(" V %s", literal_to_str(literals[j]).c_str());
      }
      printf(")");
    }
    printf("\n");
  }
  
  // consider the negation to print the variable
  static string literal_to_str(int literal) {
    stringstream ss;
    if (literal > 0) {
      ss << "X" << literal;
    }
    else {
      ss << "~X" << -literal;
    }
    return ss.str();
  }
};

// static Formula members
int Formula::nbvar;
Formula Formula::simplifications[MAX_VARIABLES + 1];
int Formula::literal;
int Formula::stack_size = 0;

int main() {
  
  Formula& formula = Formula::simplifications[0];
  formula.read();
  if (formula.solve()) {
    printf("SATISFIABLE\n");
  }
  else {
    printf("UNSATISFIABLE\n");
  }
  
  return 0;
}
