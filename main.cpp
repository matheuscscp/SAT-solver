/*
 * main.cpp
 *
 *  Created on: Apr 4, 2015
 *      Author: Pimenta
 */

#include <cstdio>

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
  // above) + 1
  int clauses[MAX_FORMULA_SIZE];
  int nbclauses;
  
  // the number of variables
  static int nbvar;
  
  // solver stack
  static Formula simplifications[MAX_VARIABLES + 1];
  static int assignments[MAX_VARIABLES + 1];
  static int stack_size;
  
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
    for (int i = 0; i < nbclauses; i++) {
      while (scanf("%d", &literals[nbliterals]) == 1 && literals[nbliterals]) {
        nbliterals++;
      }
      clauses[i] = nbliterals;
    }
  }
  
  // DPLL solver
  bool solve() {
    Formula& simple = simplifications[++stack_size];
    assignments[stack_size] = literals[0];
    switch (simplify()) {
      case 1: // possibly smaller formula
        if (simple.solve()) {
          return true;
        }
        break;
      case 2: // satisfiable
        return true;
      case 3: // evaluated false
        break;
    }
    assignments[stack_size] = -literals[0];
    switch (simplify()) {
      case 1: // possibly smaller formula
        if (simple.solve()) {
          return true;
        }
        break;
      case 2: // satisfiable
        return true;
      case 3: // evaluated false
        break;
    }
    stack_size--;
    return false;
  }
  
  // simplify this formula according to the last variable assignment
  int simplify() {
    Formula& simple = simplifications[stack_size];
    simple.nbclauses = 0;
    simple.nbliterals = 0;
    int literal = assignments[stack_size];
    bool found = false;
    int cur_nbliterals = 0;
    for (int j = 0; j < clauses[0] && !found; j++) {
      if (literals[j] == literal) {
        found = true;
      }
      else if (literals[j] != -literal) {
        simple.literals[cur_nbliterals++] = literals[j];
      }
    }
    if (cur_nbliterals == 0 && !found) {
      return 3; // evaluated false
    }
    if (!found) {
      simple.nbliterals += cur_nbliterals;
      simple.clauses[simple.nbclauses++] = simple.nbliterals;
    }
    for (int i = 1; i < nbclauses; i++) {
      found = false;
      cur_nbliterals = 0;
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
        simple.clauses[simple.nbclauses++] = simple.nbliterals;
      }
    }
    if (simple.nbclauses > 0) {
      return 1; // possibly smaller formula
    }
    return 2; // satisfiable
  }
  
  // prettyprint formula
  void show() {
    if (nbliterals == 0) {
      printf("\n");
      return;
    }
    if (literals[0] > 0) {
      printf("(X%d", literals[0]);
    }
    else {
      printf("(~X%d", -literals[0]);
    }
    for (int j = 1; j < clauses[0]; j++) {
      if (literals[j] > 0) {
        printf(" V X%d", literals[j]);
      }
      else {
        printf(" V ~X%d", -literals[j]);
      }
    }
    printf(")");
    for (int i = 1; i < nbclauses; i++) {
      if (literals[clauses[i - 1]] > 0) {
        printf("^(X%d", literals[clauses[i - 1]]);
      }
      else {
        printf("^(~X%d", -literals[clauses[i - 1]]);
      }
      for (int j = clauses[i - 1] + 1; j < clauses[i]; j++) {
        if (literals[j] > 0) {
          printf(" V X%d", literals[j]);
        }
        else {
          printf(" V ~X%d", -literals[j]);
        }
      }
      printf(")");
    }
    printf("\n");
  }
};

// static Formula members
int Formula::nbvar;
Formula Formula::simplifications[MAX_VARIABLES + 1];
int Formula::assignments[MAX_VARIABLES + 1];
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
