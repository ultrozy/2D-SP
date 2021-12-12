#pragma once

#include <iostream>
#include <vector>
#include <array>
#include <utility>
#include <algorithm>
#include <numeric>
#include <fstream>
#include <string>
#include <ctime>
#include <random>
#include <cassert>
#include <thread>
#include "ilcplex\ilocplex.h"

#define DEBUG
constexpr auto MAX_THREADS = 40;
constexpr auto RNG_MAX = 4294967295;
constexpr auto UL_MAX = 4294967296;
const std::string FILES_LOCAL_PATH = "files\\";

//using std::cout;

class Rectangle;
// Problem object contains vector of rectangles and strip width
class Problem;
// Timer class uses chrono library to easily set timer and get spent time
class Timer;


// item_t - (x, y, w, h), where
// (x, y) - coordinates of bottom-left rectangle corner,
// (w, h) - width and height of the PLACED rectangle
typedef std::array<int, 4> item_t;
// vector of placed rectangles
typedef std::vector<item_t> Solution;
// vector of rectangle pairs
// needed to restore the solution after solving modified equivalent problem
typedef std::vector<std::pair<Rectangle, Rectangle>> Rectangle_Pair_Vector;


// COMPARATORS
// comparison of rectangles to sort by various parameters
// dec = decreasing, inc = increasing
// sort by width (dec), then height (dec)
bool compwidth(Rectangle a, Rectangle b);
// sort by height (dec), then width (dec)
bool compheight(Rectangle a, Rectangle b);
// sort by area (dec)
bool comparea(Rectangle a, Rectangle b);

// comparison of rectangles to sort by various parameters
// sort by items' width (dec), then height (dec)
bool compsolwidth(item_t a, item_t b);
// sort by y-coord (inc), then x-coord (inc), then height (inc), then width (inc)
bool compsol(item_t a, item_t b);
// same, but y-coord decreasing
bool comprevsol(item_t a, item_t b);


bool operator==(Rectangle a, Rectangle b);
// check if items overlap
bool dont_interfere(item_t a, item_t b);
bool dont_interfere(item_t a, item_t b, int& touch);
// mathematical power function
inline double power(double x, double y);

// INPUT - OUTPUT
// input Problem from console or text file
Problem Input();
Problem Input(std::string filename);
// input Problem from Solution object or text file
Problem RestoreProblem(std::string filename, int Width = 0, int fixed = 0);
Problem RestoreProblem(const Solution& solution, int Width = 0, int fixed = 0);
// input Solution from Solution text file
Solution InputSolution(std::string filename);
void PrintProblem(const Problem& p, std::string filename);
void PrintSolution(const Solution& sol, std::string filename, bool Coordinates_Only = true);
// call draw.py to draw Solution using graphics.py library from Solution object or text file
void Draw(std::string filename);
void Draw(const Solution& sol, bool save = false, std::string imagename = "");
// input vector of test Problems from given folder
// 5 test folders, foldernum from 0 to 4 inclusively
std::vector<Problem> InputTest(int foldernum, int& Hopt);
// Generate random problem using "Guillotine Cutting" with given weights of parameters
Solution GenerateProblem(Problem& p, int W, int H, int free, int fixed = 0, bool normdist = true, char orient = 2, int ks = 100, int kw = 0, int kh = 0);

// check if Problem or Solution to the Problem are correctly defined
bool ProblemCorrectness(const Problem& p);
int SolutionCorrectness(const Problem& problem, const Solution& solution);

// REDUCTIONS - simplifying Problem
// find rectangles that can be initially placed without changing optimal solution
void Prepack(Problem& p, int& H, Solution& sol);
// decreasing strip width without changing optimal solution
Problem ModifyStripWidth(const Problem& p);
// increasing width of given item without changing optimal solution
Problem ModifyItemWidth(const Problem& p, int num, Rectangle_Pair_Vector* rpv_ptr = nullptr);
// apply ModifyItemWidth for every rectangle in decreasing width order
Problem ModifyAllItemsWidths(const Problem& p, Rectangle_Pair_Vector* rpv_ptr = nullptr);
// apply all the reductions: Prepack, Modification of strip and items' widths
Problem Reduction(const Problem& p, int& H, Solution& sol, Rectangle_Pair_Vector* rpv_ptr = nullptr);
// restoring solution to an original Problem after solving reduced one
void UnmodifySolution(const Problem& original_problem, Solution& solution,
  int Prepack_Height, Solution Prepack_Solution, const Rectangle_Pair_Vector& rpv);

// LOWER BOUNDS
// Based on items' areas (also known as Continuous Lower Bound)
int LB_Area(const Problem& p);
// LB_Area of Problems, modified by predetermined Dual Feasible Functions
int LB_DFF(const Problem& p);
// Lower Bounds based on heights: LBH, LBH2, LBH3
int LB_H(const Problem& p);
// All easy computing Lower Bounds
int LB_easy(const Problem& p);
// Hard computing F1 Lower Bound, based on relaxed Problem
int LB_F1(const Problem& p);
// Lower Bound postprocessing #1: using Principle of Normal Patterns
int LB_post1(const Problem& p, const int LB);
// Lower Bound postprocessing #1: using Lower Bound of rotated problem
int LB_post2(const Problem& p, const int LB, const int UB, bool better = true);

// SKYLINE FUNCTIONS
// modify skyline by placing given item
void skyline_add_item(std::vector<int>& wskyline, std::vector<int>& hskyline, const item_t& item, const int W);
// remove given niche from skyline (when no item can fit in there)
void skyline_remove_niche(std::vector<int>& wskyline, std::vector<int>& hskyline, const int indniche, const int W);
// check if item and skyline overlap
bool good_to_add_to_skyline(const std::vector<int>& wskyline, const std::vector<int>& hskyline, const item_t& item);

// SOLVERS
// Solution postprocessing: using Solutions of rotated Problem
Solution UB_post(const Problem& p, const Solution& oldsol, int& UB, const int LB, const double time_sec, bool better = true);
// HEURISTIC ALGORITHM
// used in PBF to determine which unplaced rectangle is better to place
int placement_criteria(const std::vector<int>& wsk, const std::vector<int>& hsk, const std::vector<Rectangle>& list, int& jtallest, const int criteria, const int W, const int sumareas, int& indniche);
// Priority Best Fit - heuristic algorithm
// places rectangles sequentially, one by one, using placement_criteria
Solution PBF(const Problem& p, int& UB, int LB = 0, const double time_sec = 0.0);
// Exact Solution for small cases using IBM's CPLEX for n <= 12
// gives incorrect Solutions with some frequency (about 0.05 or less)
Solution ExactSolution(const Problem& p, int& LB, int& UB, const unsigned int time_sec = 200);
// METAHEURISTIC ALGORITHM
// for the next algorithms solution is stored in Problem by sequence of rectangles to pack
// get Solution by placing given sequence of rectangles
Solution PackSequence(const Problem& p, int& H, const bool accurate = false);
// get Solution by placing rectangles sequentially but
// instead of placement criteria there are given weights in eval
Solution HeuristicPacking(Problem& p, int& H, const std::array<double, 8> eval = { 1.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0 }, const bool accurate = false);
// get Solution by calling HeuristicPacking with random evals
Solution FindHeuristicSequence(Problem& p, int& H, int LB = 0, const double time_sec = 200.0);
// improve Solution by swapping two rectangles in the sequence
Solution LocalSearch(Problem& p, int& H, int LB, const double time_sec = 200.0);
// perform pure Simulated Annealing
Solution SimulatedAnnealing(Problem& p, int& H, int LB = 0, const double T0 = 0.5, const double alpha = 0.93, const int L = 50, const double time_sec = 0.1, const double smarttime = 0.0);
// perform Simulated Annealing several times
Solution IntelligentSimulatedAnnealing(Problem& p, int& H, const int LB, const double time_sec);

// SEARCH FOR GOOD VALUE OF L HYPERPARAMETER IN SIMULATED ANNEALING
// used in Test_Sim_Anneal, generates vector of problems and finds initial heuristic solutions
void Thread_GenProblem(std::vector<Problem>& pvec, std::vector<int>& Hvec, int ibegin, int iend, int n);
// used in Test_Sim_Anneal, performs SA to vector of problems
void Thread_SimAnneal(const std::vector<Problem>& pvec, const std::vector<int>& Hvec, std::vector<int>& Hsum, int index, int L, double t, int N, int LB);
// calls SA with different L values and prints results
void Test_Sim_Anneal(int RectNum, int ProbNum, double Time, int Lstart, int Lend, int Lstep, bool Multiply);