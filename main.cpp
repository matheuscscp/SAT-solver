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
  
  // the number of variables
  static int nbvar;
  
  // solver stack
  static Formula simplifications[MAX_VARIABLES + 1];
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
    }
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
      printf("(%s", literal2str(literals[clauses[i - 1]]).c_str());
      for (int j = clauses[i - 1] + 1; j < clauses[i]; j++) {
        printf(" V %s", literal2str(literals[j]).c_str());
      }
      printf(")");
    }
    printf("\n");
  }
  
  // consider the negation to print the variable
  static string literal2str(int literal) {
    stringstream ss;
    if (literal > 0) {
      ss << "X" << literal;
    }
    else {
      ss << "~X" << -literal;
    }
    return ss.str();
  }
  
  // DPLL solver
  bool solve() {
    return false;
  }
};

// static Formula members
int Formula::nbvar;
Formula Formula::simplifications[MAX_VARIABLES + 1];
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
