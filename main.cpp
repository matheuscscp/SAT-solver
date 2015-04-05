/*
 * main.cpp
 *
 *  Created on: Apr 1, 2015
 *      Author: Pimenta
 */

#include <cstdio>
#include <string>
#include <sstream>

using namespace std;

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
      while (scanf("%d", &literals[nbliterals]) == 1 && literals[nbliterals]) {
        nbliterals++;
      }
      clauses[i] = nbliterals;
    }
  }
  
  // basic backtracking solver
  bool solve() {
    Formula& simple = simplifications[++stack_size];
    int sign[2] = {1, -1}; // true, false
    for (int i = 0; i < 2; i++) {
      literal = sign[i]*literals[0];
      switch (simplify()) {
        case 1: // smaller formula
          if (simple.solve()) {
            return true;
          }
          break;
        case 2: // satisfiable
          return true;
        case 3: // evaluated false
          break;
      }
    }
    stack_size--;
    return false;
  }
  
  // simplify this formula according to the last variable assignment
  int simplify() {
    Formula& simple = simplifications[stack_size];
    simple.nbclauses = 0;
    simple.nbliterals = 0;
    for (int i = 1; i <= nbclauses; i++) {
      bool found = false;
      int cur_nbliterals = 0;
      for (int j = clauses[i - 1]; j < clauses[i] && !found; j++) {
        if (literals[j] == literal) {
          found = true;
        }
        else if (literals[j] != -literal) {
          simple.literals[simple.nbliterals + (cur_nbliterals++)] = literals[j];
        }
      }
      if (cur_nbliterals == 0 && !found) {
        return 3; // evaluated false
      }
      if (!found) {
        simple.nbliterals += cur_nbliterals;
        simple.clauses[++simple.nbclauses] = simple.nbliterals;
      }
    }
    if (simple.nbclauses == 0) { // satisfiable
      return 2;
    }
    return 1; // smaller formula
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
