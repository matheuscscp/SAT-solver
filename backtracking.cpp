/*
 * backtracking.cpp
 *
 *  Created on: Apr 1, 2015
 *      Author: Pimenta
 */

#include <cstdio>

// the size of a formula is the sum of the number of literals in each clause
#define MAX_FORMULA_SIZE  1000

// maximum number of variables
#define MAX_VARIABLES     30

// represents a propositional formula
struct Formula {
  // i means the i-th variable, -i means i-th variable negated
  int literals[MAX_FORMULA_SIZE];
  int n_literals;
  
  // index of the last literal of the clause (in the array of literals
  // above) + 1.
  int clauses[MAX_FORMULA_SIZE];
  int n_clauses;
  
  // solver stack
  static bool X[MAX_VARIABLES + 1];
  static Formula simplifications[MAX_VARIABLES + 1];
  static int cur_variable;
  
  // try to read an instance from stdin
  bool read() {
    if (scanf("%d", &n_clauses) != 1 || (!n_clauses)) return false;
    n_literals = 0;
    for (int i = 0; i < n_clauses; i++) {
      for (char c = 0; c != '\n'; c = getchar()) {
        scanf("%d", &literals[n_literals++]);
      }
      clauses[i] = n_literals;
    }
    return true;
  }
  
  // prettyprint formula
  void show() {
    if (n_literals == 0) {
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
    for (int i = 1; i < n_clauses; i++) {
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
  
  // simplify this formula according to the last variable assignment
  int simplify() {
    Formula& simple = simplifications[cur_variable];
    simple.n_clauses = 0;
    simple.n_literals = 0;
    bool var = X[cur_variable];
    bool found = false;
    int s_n_literals = 0;
    for (int j = 0; j < clauses[0] && !found; j++) {
      if ((var && literals[j] == cur_variable) ||
          (!var && literals[j] == -cur_variable)) {
        found = true;
      }
      else if (literals[j] != cur_variable && literals[j] != -cur_variable) {
        simple.literals[s_n_literals++] = literals[j];
      }
    }
    if (s_n_literals == 0 && !found) {
      return 3; // evaluated false
    }
    if (!found) {
      simple.n_literals += s_n_literals;
      simple.clauses[simple.n_clauses++] = simple.n_literals;
    }
    for (int i = 1; i < n_clauses; i++) {
      found = false;
      s_n_literals = 0;
      for (int j = clauses[i - 1]; j < clauses[i] && !found; j++) {
        if ((var && literals[j] == cur_variable) ||
            (!var && literals[j] == -cur_variable)) {
          found = true;
        }
        else if (literals[j] != cur_variable && literals[j] != -cur_variable) {
          simple.literals[simple.n_literals + (s_n_literals++)] = literals[j];
        }
      }
      if (s_n_literals == 0 && !found) {
        return 3; // evaluated false
      }
      if (!found) {
        simple.n_literals += s_n_literals;
        simple.clauses[simple.n_clauses++] = simple.n_literals;
      }
    }
    if (simple.n_clauses > 0) {
      return 1; // possibly smaller formula
    }
    return 2; // satisfiable
  }
  
  // basic backtracking solver
  bool solve() {
    Formula& simple = simplifications[++cur_variable];
    X[cur_variable] = true;
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
    X[cur_variable] = false;
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
    cur_variable--;
    return false;
  }
};

// static Formula members
bool Formula::X[MAX_VARIABLES + 1];
Formula Formula::simplifications[MAX_VARIABLES + 1];
int Formula::cur_variable;

int main() {
  
  bool printed = false;
  for (Formula& formula = Formula::simplifications[0]; formula.read();) {
    if (printed) {
      printf("\n");
    }
    else {
      printed = true;
    }
    formula.show();
    Formula::cur_variable = 0;
    if (formula.solve()) {
      printf("satisfiable\n");
    }
    else {
      printf("NOT satisfiable\n");
    }
  }
  
  return 0;
}
