#include "All_Declarations.h"

class Rectangle
{
public:
  int width;
  int height;
  int area() const
  {
	return width * height;
  }
  Rectangle(int width, int height) { this->width = width; this->height = height; }

  friend bool operator==(Rectangle a, Rectangle b);
};
class Problem
{
public:
  // initial sequence of rectangles or packing sequence
  std::vector<Rectangle> list;
  // rectangles sorted by width (decreasing). Often used
  std::vector<Rectangle> listwidth;
  // items on the strip which are initially placed and fixed
  Solution fixed;
  // strip width
  int Width;

  Problem()
  {
	Width = 0;
  }
  Problem(const std::vector<Rectangle>& list, int Width)
  {
	this->list = list;
	this->listwidth = list;
	this->Width = Width;
	sort(listwidth.begin(), listwidth.end(), compwidth);
  }
  Problem(const std::vector<Rectangle>& list, int Width, const Solution& fixed)
  {
	this->list = list;
	this->listwidth = list;
	this->Width = Width;
	sort(listwidth.begin(), listwidth.end(), compwidth);
	this->fixed = fixed;
  }
};
class Timer
{
  // point of time in which timer was set
  std::chrono::steady_clock::time_point start;
  // given duration of time for timer to work
  std::chrono::duration<double> duration;
public:
  // set without duration
  Timer()
  {
	start = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration<double>(0);
  }
  // set with duration
  Timer(double time_seconds)
  {
	start = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration<double>(time_seconds);
  }
  // check if time is up
  bool run()
  {
	return std::chrono::high_resolution_clock::now() - start < duration;
  }
  // how much time passed since the timer was set
  double last()
  {
	return std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start).count();
  }
  // how much time is left until timer will stop
  double left()
  {
	return std::chrono::duration<double>(duration - (std::chrono::high_resolution_clock::now() - start)).count();
  }
};

bool compwidth(Rectangle a, Rectangle b)
{
  if (a.width == b.width)
	return a.height >= b.height;
  return a.width > b.width;
}
bool compheight(Rectangle a, Rectangle b)
{
  if (a.height == b.height)
	return a.width >= b.width;
  return a.height > b.height;
}
bool comparea(Rectangle a, Rectangle b)
{
  return a.area() >= b.area();
}
bool compsolwidth(item_t a, item_t b)
{
  if (a[2] == b[2])
	return a[3] > b[3];
  return a[2] > b[2];
}
bool compsol(item_t a, item_t b)
{
  if (a[1] == b[1])
  {
	if (a[0] == b[0])
	{
	  if (a[3] == b[3])
		return a[2] <= b[2];
	  return a[3] < b[3];
	}
	return a[0] < b[0];
  }
  return a[1] < b[1];
}
bool comprevsol(item_t a, item_t b)
{
  /*
  * compsol, but y-coord is decreasing
  */
  if (a[1] == b[1])
  {
	if (a[0] == b[0])
	{
	  if (a[3] == b[3])
		return a[2] <= b[2];
	  return a[3] < b[3];
	}
	return a[0] < b[0];
  }
  return a[1] > b[1];
}
bool operator==(Rectangle a, Rectangle b)
{
  if ((a.width == b.width) && (a.height == b.height))
	return true;
  else
	return false;
}
bool dont_interfere(item_t a, item_t b)
{
  if (a[0] >= b[0] + b[2] || b[0] >= a[0] + a[2] || a[1] >= b[1] + b[3] || b[1] >= a[1] + a[3])
	return true;
  else
	return false;
}
bool dont_interfere(item_t a, item_t b, int& touch)
{
  /*
  * touch - initially 0, length of common side if two items are touching each other
  */
  if (a[0] > b[0] + b[2] || b[0] > a[0] + a[2] || a[1] > b[1] + b[3] || b[1] > a[1] + a[3])
	return true;
  if (a[0] == b[0] + b[2] || b[0] == a[0] + a[2])
  {
	touch += std::max(0, std::min(a[1] + a[3], b[1] + b[3]) - std::max(a[1], b[1]));
	return true;
  }
  if (a[1] == b[1] + b[3] || b[1] == a[1] + a[3])
  {
	touch += std::max(0, std::min(a[0] + a[2], b[0] + b[2]) - std::max(a[0], b[0]));
	return true;
  }
  return false;
}
inline double power(double x, double y)
{
  return exp(y * log(x));
}

// INPUT - OUTPUT
Problem Input()
{
  int W, w, h;
  std::vector<Rectangle> v;
  std::cout << "Enter the strip width" << std::endl;
  std::cin >> W;
  std::cout << "Enter the rectangles' width and height, ending the input with 0 0" << std::endl;
  std::cin >> w >> h;
  while (w || h)
  {
	if ((w > 0) && (h > 0) && (w <= W))
	  v.push_back(Rectangle(w, h));
	else
	  std::cout << "Please enter correct values" << std::endl;
	std::cin >> w >> h;
  }
  return Problem(v, W);
}
Problem Input(std::string filename)
{
  /*
  * If name of the file is passed without extension, it is automatically considered as TXT
  * 1st line is useless text: "Enter the strip width"
  * 2nd line is Strip Width
  * 3rd line is useless text "Enter the rectangles' width and height"
  * Every next line are width and height of free rectangles,
  *	  separated by one space, no brackets, no commas or other symbols
  * Then there is useless text: "Enter FIXED rectangles' width and height"
  * Last rows have fixed items: (x, y, w, h),
  *	  separated by one space, no brackets, no commas or other symbols
  */
  if (filename.size() < 4 || filename.substr(filename.size() - 4, 4) != ".txt")
	filename += ".txt";
  filename = FILES_LOCAL_PATH + filename;
  std::ifstream file(filename);
  std::string line;
  int W, w, h, i, x, y;
  std::vector<Rectangle> v;

  getline(file, line);
  getline(file, line);
  W = stoi(line);
  getline(file, line);

  while (getline(file, line) && line.size() && line[0] >= '0' && line[0] <= '9')
  {
	w = 0; h = 0; i = 0;
	while (line[i] != ' ')
	{
	  w = w * 10 + line[i] - '0';
	  i++;
	}
	while (line[i] == ' ')
	  i++;
	while (i < line.size() && line[i] != ' ')
	{
	  h = h * 10 + line[i] - '0';
	  i++;
	}

	if ((w > 0) && (h > 0) && (w <= W))
	  v.push_back(Rectangle(w, h));
	else
	  std::cout << "Please enter correct values" << std::endl;
  }

  Problem p(v, W);

  // read fixed items
  while (getline(file, line) && line.size() && line[0] >= '0' && line[0] <= '9')
  {
	x = 0; y = 0; w = 0; h = 0; i = 0;
	while (line[i] != ' ')
	{
	  x = x * 10 + line[i] - '0';
	  i++;
	}
	while (line[i] == ' ')
	  i++;

	while (line[i] != ' ')
	{
	  y = y * 10 + line[i] - '0';
	  i++;
	}
	while (line[i] == ' ')
	  i++;

	while (line[i] != ' ')
	{
	  w = w * 10 + line[i] - '0';
	  i++;
	}
	while (line[i] == ' ')
	  i++;

	while (i < line.size() && line[i] != ' ')
	{
	  h = h * 10 + line[i] - '0';
	  i++;
	}

	if ((x >= 0) && (y >= 0) && (w > 0) && (h > 0) && (x + w <= W))
	  p.fixed.push_back({ x, y, w, h });
	else
	  std::cout << "Please enter correct values" << std::endl;
  }

  file.close();
  return p;
}
Problem RestoreProblem(std::string filename, int Width, int fixed)
{
  /*
  * Restore Problem from Solution text file
  *
  * If name of the file is passed without extension, it is automatically considered as TXT
  * Solution can be written as (coordinates only) or (coordinates and sides),
  *	which is written in the first line "The solution has n rectangles (...)"
  * All next lines are items, they are represented in the file in different ways
  * First items are fixed items, there are "fixed" of them (passed as an argument)
  * (coordinates only) - items are stored as coordinates of
  *   bottom-left and top-right corners: (x, y, x+w, y+h)
  *	  numbers in the file are represented in a following way:
  *	  (x, y) (x+w, y+h)
  * (coordinates and sides) - items are stored as coordinates of bottom-left corner and
  *   width and height: (x, y, w, h)
  *	  numbers in the file are represented in a following way:
  *	  (x, y), w, h
  */
  if (filename.size() < 4 || filename.substr(filename.size() - 4, 4) != ".txt")
	filename += ".txt";
  filename = FILES_LOCAL_PATH + filename;
  std::ifstream file(filename);
  std::string line;
  Problem p;
  Solution sol;
  std::vector<Rectangle> list;
  item_t item;
  int i, W = 0;

  getline(file, line);
  // if (coordinates only)
  if (line[line.size() - 2] == 'y')
	while (getline(file, line) && line.size() && line[0] == '(')
	{
	  item = { 0,0,0,0 };
	  i = 1;
	  while (line[i] <= '9' && line[i] >= '0')
		item[0] = item[0] * 10 + line[i++] - '0';
	  while (line[i] > '9' || line[i] < '0')
		i++;
	  while (line[i] <= '9' && line[i] >= '0')
		item[1] = item[1] * 10 + line[i++] - '0';
	  while (line[i] > '9' || line[i] < '0')
		i++;
	  while (line[i] <= '9' && line[i] >= '0')
		item[2] = item[2] * 10 + line[i++] - '0';
	  while (line[i] > '9' || line[i] < '0')
		i++;
	  while (line[i] <= '9' && line[i] >= '0')
		item[3] = item[3] * 10 + line[i++] - '0';
	  if (W < item[2])
		W = item[2];
	  item[2] -= item[0];
	  item[3] -= item[1];
	  sol.push_back(item);
	}
  // if (coordinates and sides)
  else
	while (getline(file, line) && line.size() && line[0] == '(')
	{
	  item = { 0,0,0,0 };
	  i = 1;
	  while (line[i] <= '9' && line[i] >= '0')
		item[0] = item[0] * 10 + line[i++] - '0';
	  while (line[i] > '9' || line[i] < '0')
		i++;
	  while (line[i] <= '9' && line[i] >= '0')
		item[1] = item[1] * 10 + line[i++] - '0';
	  while (line[i] > '9' || line[i] < '0')
		i++;
	  while (line[i] <= '9' && line[i] >= '0')
		item[2] = item[2] * 10 + line[i++] - '0';
	  while (line[i] > '9' || line[i] < '0')
		i++;
	  while (i < line.size() && line[i] <= '9' && line[i] >= '0')
		item[3] = item[3] * 10 + line[i++] - '0';
	  if (W < item[0] + item[2])
		W = item[0] + item[2];
	  sol.push_back(item);
	}
  i = sol.size() - 1;
  while (i >= fixed)
  {
	list.push_back(Rectangle(sol[i][2], sol[i][3]));
	sol.pop_back();
	i--;
  }
  p = Problem(list, std::max(W, Width));
  p.fixed = sol;
  return p;
}
Problem RestoreProblem(const Solution& solution, int Width, int fixed)
{
  /*
  * Restore Problem from Solution object
  *
  * First items are fixed items, there are "fixed" of them (passed as an argument)
  */
  int i, W = 0;
  std::vector<Rectangle> list;
  Problem p;

  for (const auto& item : solution)
	if (W < item[0] + item[2])
	  W = item[0] + item[2];
  i = solution.size() - 1;
  while (i >= fixed)
  {
	list.push_back(Rectangle(solution[i][2], solution[i][3]));
	i--;
  }
  p = Problem(list, std::max(W, Width));
  p.fixed = Solution(solution.begin(), solution.begin() + fixed);
  return p;
}
Solution InputSolution(std::string filename)
{
  /*
  * Input Solution from text file
  *
  * If name of the file is passed without extension, it is automatically considered as TXT
  * Solution can be written as (coordinates only) or (coordinates and sides),
  *	which is written in the first line "The solution has n rectangles (...)"
  * All next lines are items, they are represented in the file in different ways
  * (coordinates only) - items are stored as coordinates of
  *   bottom-left and top-right corners: (x, y, x+w, y+h)
  *	  numbers in the file are represented in a following way:
  *	  (x, y) (x+w, y+h)
  * (coordinates and sides) - items are stored as coordinates of bottom-left corner and
  *   width and height: (x, y, w, h)
  *	  numbers in the file are represented in a following way:
  *	  (x, y), w, h
  */
  if (filename.size() < 4 || filename.substr(filename.size() - 4, 4) != ".txt")
	filename += ".txt";
  filename = FILES_LOCAL_PATH + filename;
  std::ifstream file(filename);
  std::string line;
  Solution sol;
  item_t item;
  int i;

  getline(file, line);
  // if (coordinates only)
  if (line[line.size() - 2] == 'y')
	while (getline(file, line) && line.size() && line[0] == '(')
	{
	  item = { 0,0,0,0 };
	  i = 1;
	  while (line[i] <= '9' && line[i] >= '0')
		item[0] = item[0] * 10 + line[i++] - '0';
	  while (line[i] > '9' || line[i] < '0')
		i++;
	  while (line[i] <= '9' && line[i] >= '0')
		item[1] = item[1] * 10 + line[i++] - '0';
	  while (line[i] > '9' || line[i] < '0')
		i++;
	  while (line[i] <= '9' && line[i] >= '0')
		item[2] = item[2] * 10 + line[i++] - '0';
	  while (line[i] > '9' || line[i] < '0')
		i++;
	  while (line[i] <= '9' && line[i] >= '0')
		item[3] = item[3] * 10 + line[i++] - '0';
	  item[2] -= item[0];
	  item[3] -= item[1];
	  sol.push_back(item);
	}
  // if (coordinates and sides)
  else
	while (getline(file, line) && line.size() && line[0] == '(')
	{
	  item = { 0,0,0,0 };
	  i = 1;
	  while (line[i] <= '9' && line[i] >= '0')
		item[0] = item[0] * 10 + line[i++] - '0';
	  while (line[i] > '9' || line[i] < '0')
		i++;
	  while (line[i] <= '9' && line[i] >= '0')
		item[1] = item[1] * 10 + line[i++] - '0';
	  while (line[i] > '9' || line[i] < '0')
		i++;
	  while (line[i] <= '9' && line[i] >= '0')
		item[2] = item[2] * 10 + line[i++] - '0';
	  while (line[i] > '9' || line[i] < '0')
		i++;
	  while (i < line.size() && line[i] <= '9' && line[i] >= '0')
		item[3] = item[3] * 10 + line[i++] - '0';
	  sol.push_back(item);
	}
  return sol;
}
void PrintProblem(const Problem& p, std::string filename)
{
  /*
  * If name of the file is passed without extension, it is automatically considered as TXT
  */
  if (filename.size() < 4 || filename.substr(filename.size() - 4, 4) != ".txt")
	filename += ".txt";
  filename = FILES_LOCAL_PATH + filename;
  std::ofstream file(filename);
  file << "Enter the strip width" << std::endl;
  file << p.Width << std::endl;
  file << "Enter the rectangles' width and height" << std::endl;
  for (const auto& rec : p.listwidth)
	file << rec.width << " " << rec.height << std::endl;
  if (p.fixed.size())
	file << "Enter FIXED rectangles' width and height" << std::endl;
  for (int i = 0; i < p.fixed.size(); i++)
	file << p.fixed[i][0] << " " << p.fixed[i][1] << " " << p.fixed[i][2] << " " << p.fixed[i][3] << std::endl;
  return;
}
void PrintSolution(const Solution& sol, std::string filename, bool Coordinates_Only)
{
  /*
  * If name of the file is passed without extension, it is automatically considered as TXT
  * Solution can be written as (coordinates only) or (coordinates and sides),
  *	which is written in the first line "The solution has n rectangles (...)"
  * All next lines are items, they are represented in the file in different ways
  * First items are fixed items, there are "fixed" of them (passed as an argument)
  * (coordinates only) - items are stored as coordinates of
  *   bottom-left and top-right corners: (x, y, x+w, y+h)
  *	  numbers in the file are represented in a following way:
  *	  (x, y) (x+w, y+h)
  * (coordinates and sides) - items are stored as coordinates of bottom-left corner and
  *   width and height: (x, y, w, h)
  *	  numbers in the file are represented in a following way:
  *	  (x, y), w, h
  */
  if (filename.size() < 4 || filename.substr(filename.size() - 4, 4) != ".txt")
	filename += ".txt";
  filename = FILES_LOCAL_PATH + filename;
  std::ofstream file(filename);
  file << "The solution has " << sol.size() << " rectangles " << (Coordinates_Only ? "(coordinates only)" : "(coordinates and sides)") << std::endl;
  if (Coordinates_Only)
	for (int i = 0; i < sol.size(); i++)
	  file << "(" << sol[i][0] << ", " << sol[i][1] << ") (" << sol[i][0] + sol[i][2] << ", " << sol[i][1] + sol[i][3] << ")" << std::endl;
  else
	for (int i = 0; i < sol.size(); i++)
	  file << "(" << sol[i][0] << ", " << sol[i][1] << "), " << sol[i][2] << ", " << sol[i][3] << std::endl;
  return;
}
void Draw(std::string filename)
{
  /*
  * If name of the file is passed without extension, it is automatically considered as TXT
  * Call python draw.py to draw Solution in a new window
  * passing there text file with a Solution
  */
  filename = FILES_LOCAL_PATH + filename;
  system(("python draw.py " + filename).c_str());
}
void Draw(const Solution& sol, bool save, std::string imagename)
{
  /*
  * imagename has to be passed without the extension, cause there will be TXT and PNG
  * Call python draw.py to draw Solution in a new window
  * passing there a Solution object
  *
  * bool save is responsible for saving the image and Solution file
  * if save==true and imagename=="", files get name
  *	  "temporary_solution_file_" with random 8-digit hexadecimal number after it
  * if save==false files are to be deleted no matter what value imagename has
  */
  std::string s;
  if (imagename != "")
  {
	s = imagename;
	if (s[s.size() - 4] == '.')
	  s = s.substr(0, s.size() - 4);
  }
  else
  {
	std::random_device rd;
	std::default_random_engine rng{ rd() };
	unsigned int randnum = rng();
	std::stringstream stream;
	stream << std::hex << randnum;
	s = "temporary_solution_file_" + stream.str();
  }
  PrintSolution(sol, s);
  s = FILES_LOCAL_PATH + s;
  system(("python draw.py " + s).c_str());
  if (!save)
  {
	system(("del " + s + ".txt").c_str());
	system(("del " + s + ".png").c_str());
  }
}
std::vector<Problem> InputTest(int foldernum, int& Hopt)
{
  /*
  * Get 100 test problems based on number of folder (foldernum)
  * Hopt gets optimal height for all 100 problems
  * foldernum stands for:
  * 0 - guillotine cutting generated problems, each problem has 25 rectangles, Hopt=24
  * 1 - guillotine cutting generated problems, each problem has 50 rectangles, Hopt=30
  * 2 - guillotine cutting generated problems, each problem has 100 rectangles, Hopt=60
  * 3 - nonguillotine cutting generated problems, each problem has 50 rectangles, Hopt=30
  * 4 - nonguillotine cutting generated problems, each problem has 100 rectangles, Hopt=60
  */
  std::string path, numtxt, line;
  switch (foldernum)
  {
  case 0:
	path = "generated\\guil25\\test_guil_25_";
	Hopt = 24;
	break;
  case 1:
	path = "generated\\guil50\\test_guil_50_";
	Hopt = 30;
	break;
  case 2:
	path = "generated\\guil100\\test_guil_100_";
	Hopt = 60;
	break;
  case 3:
	path = "generated\\nonguil50\\test_nonguil_50_";
	Hopt = 30;
	break;
  case 4:
	path = "generated\\nonguil100\\test_nonguil_100_";
	Hopt = 60;
	break;
  }
  std::vector<Problem> problems;
  std::vector<Rectangle> list;
  std::ifstream file;
  problems.reserve(100);
  int W, n, i, w, h, j;
  for (i = 0; i < 100; i++)
  {
	numtxt = std::to_string(i) + ".txt";
	if (i < 10)
	  numtxt = "0" + numtxt;
	file.open(path + numtxt);
	file >> W >> n;
	for (j = 0; j < n; j++)
	{
	  file >> w >> h;
	  list.push_back(Rectangle(w, h));
	}
	problems.push_back(Problem(list, W));
	getline(file, line);
	if (getline(file, line))
	{
	  std::cout << "ITS NOT OVER !!!" << line.size() << "\n";
	}
	list.clear();
	file.close();
  }
  return problems;
}

// GENERATOR
Solution GenerateProblem(Problem& p, int W, int H, int free, int fixed,
  bool normdist, char orient, int ks, int kw, int kh)
{
  /*
  * Generate random problem with
  *	W - strip width
  *	H - optimal height
  *	N = free + fixed - total number of rectangles
  *	  free - must be decided to place
  *	  fixed - have predetermined position
  * Problem is generated by Guillotine Cutting i.e.
  *	Rectangle with W width and H height is created.
  *	Then the rectangle is sliced in two in random position.
  *	From existing rectangles we choose random one and slice it
  *	process continues until there are N rectangles
  *
  * if normdist==true slicing position is chosen by normal distribution
  *	N(mu, sigma), where mu = Length / 2, sigma = Length / 4
  *	Length is either width or height whichever side we slice
  * if normdist=false slicing position is chosen by uniform distribution
  *
  * orient is 0, 1 or 2. It is responsible for
  * how we choose side to slice (vertically or horizontally)
  *   orient = 0: slicing side is chosen randomly (50% - vert, 50% - horiz)
  *	orient = 1: probability of chosen side is proportional to Length-1, i.e.
  *	  P(vert ) = (width  - 1) / (width + height - 2)
  *	  P(horiz) = (height - 1) / (width + height - 2)
  *
  * ks, kw, kh are responsible for choosing a rectangle to slice,
  *	ks, kw, kh are weights of final value of the rectangle
  * the more ks - the more probability of choosing rectangle with higher area
  * the more kw - the more probability of choosing rectangle with higher (width / height)
  * the more ks - the more probability of choosing rectangle with higher (height / width)
  * the more value - the higher probability of choosing the rectangle
  */
#ifdef DEBUG
  assert(ks >= 0 && kw >= 0 && kh >= 0 && orient >= 0 && orient <= 2 && free && fixed >= 0 && W * H >= std::max(2, free + fixed));
#endif
  std::vector<Rectangle> list;
  Solution sol; //emerging solution
  std::vector<int> value;
  std::random_device rd;
  std::default_random_engine rng{ rd() };
  std::normal_distribution<double> norm;
  int div, another, val, w, h; // div - width/height of left/bottom rectangle, another - of the right/top one
  list.reserve(free);
  sol.reserve(free + fixed);
  value.reserve(free + fixed);
  sol.push_back({ 0, 0, W, H });
  value.push_back(std::max(1, ks * W * H + (int)(kw * W / H) + (int)(kh * H / W)));
  while (sol.size() < free + fixed)
  {
	unsigned long long a = rng(), b = rng();
	val = (a * UL_MAX + b) % accumulate(value.begin(), value.end(), 0);
	int i = 0, sumval = value[0];
	while (sumval <= val)
	{
	  i++;
	  sumval += value[i];
	}
	bool vert; // if vert - vertical slice, else - horizontal slice
	switch (orient)
	{
	case 0:
	  vert = rng() % 2;
	  break;
	case 1:
	  vert = (rng() % (sol[i][2] + sol[i][3] - 2) < sol[i][2] - 1);
	  break;
	case 2:
	  vert = (sol[i][2] == sol[i][3]) ? (rng() % 2) : (sol[i][2] > sol[i][3]);
	  break;
	}
	if (vert && sol[i][2] == 1 || !vert && sol[i][3] == 1)
	  vert = !vert;

	// Vertical slice
	if (vert)
	{
	  if (normdist)
	  {
		norm = std::normal_distribution<double>(0.5 * sol[i][2], 0.25 * sol[i][2]);
		div = (int)(norm(rng) + 0.5);
		while (div < 1 || div >= sol[i][2])
		  div = (int)(norm(rng) + 0.5);
	  }
	  else
		div = rng() % (sol[i][2] - 1) + 1;
	  another = sol[i][2] - div;
	  sol[i][2] = div;
	  sol.push_back({ sol[i][0] + div, sol[i][1], another, sol[i][3] });
	  w = sol[i][2]; h = sol[i][3];
	  value[i] = (w * h == 1) ? 0 : std::max(1, ks * w * h + (int)(kw * w / h) + (int)(kh * h / w));
	  w = sol.back()[2]; h = sol.back()[3];
	  value.push_back((w * h == 1) ? 0 : std::max(1, ks * w * h + (int)(kw * w / h) + (int)(kh * h / w)));
	}
	// Horizontal slice
	else
	{
	  if (normdist)
	  {
		norm = std::normal_distribution<double>(0.5 * sol[i][3], 0.25 * sol[i][3]);
		div = (int)(norm(rng) + 0.5);
		while (div < 1 || div >= sol[i][3])
		  div = (int)(norm(rng) + 0.5);
	  }
	  else
		div = rng() % (sol[i][3] - 1) + 1;
	  another = sol[i][3] - div;
	  sol[i][3] = div;
	  sol.push_back({ sol[i][0], sol[i][1] + div, sol[i][2], another });
	  w = sol[i][2]; h = sol[i][3];
	  value[i] = (w * h == 1) ? 0 : std::max(1, ks * w * h + (int)(kw * w / h) + (int)(kh * h / w));
	  w = sol.back()[2]; h = sol.back()[3];
	  value.push_back((w * h == 1) ? 0 : std::max(1, ks * w * h + (int)(kw * w / h) + (int)(kh * h / w)));
	}
  }
  shuffle(sol.begin(), sol.end(), rng);
  for (int i = fixed; i < free + fixed; i++)
	list.push_back(Rectangle(sol[i][2], sol[i][3]));
  p = Problem(list, W);
  p.fixed = Solution(sol.begin(), sol.begin() + fixed);
  return Solution(sol);
}
// CHECK
bool ProblemCorrectness(const Problem& p)
{
  if ((p.Width <= 0) || (!p.listwidth.size()))
	return false;
  if (p.listwidth.size() != p.list.size())
	return false;
  const int n = p.listwidth.size(), W = p.Width, m = p.fixed.size();
  int i, j;
  for (i = 0; i < n - 1; i++)
	if (!compwidth(p.listwidth[i], p.listwidth[i + 1]))
	  return false;

  // All the rectangles in p.listwidth and p.list must coincide
  std::vector<Rectangle> list1 = p.listwidth, list2 = p.list;
  sort(list2.begin(), list2.end(), compwidth);
  for (i = 0; i < n; i++)
	if (!(list1[i] == list2[i]))
	  return false;
  // width and height must be feasible
  for (i = 0; i < n; i++)
	if ((list1[i].width <= 0) || (list1[i].width > W) || (list1[i].height <= 0))
	  return false;

  // fixed rectangles must be within the strip
  for (i = 0; i < m; i++)
	if ((p.fixed[i][2] <= 0) || (p.fixed[i][2] > W) || (p.fixed[i][3] <= 0) || (p.fixed[i][0] < 0) || (p.fixed[i][1] < 0) || (p.fixed[i][0] + p.fixed[i][2] > W))
	  return false;
  // fixed rectangles must not overlap
  for (i = 0; i < m - 1; i++)
	for (j = i + 1; j < m; j++)
	  if (!((p.fixed[i][0] + p.fixed[i][2] <= p.fixed[j][0]) || (p.fixed[j][0] + p.fixed[j][2] <= p.fixed[i][0]) || (p.fixed[i][1] + p.fixed[i][3] <= p.fixed[j][1]) || (p.fixed[j][1] + p.fixed[j][3] <= p.fixed[i][1])))
		return false;
  return true;
}
int SolutionCorrectness(const Problem& problem, const Solution& solution)
{
  if (problem.listwidth.size() + problem.fixed.size() != solution.size())
  {
#ifdef DEBUG
	std::cout << "size don't match, orig = " << problem.listwidth.size() << ", solsize = " << solution.size() << std::endl;
#endif
	return 0;
  }

  const int W = problem.Width;
  Solution sol = solution; // copy of solution, will be changed
  int i, j = sol.size(), n = problem.listwidth.size(), H = 0;

  // find all fixed rectangles in "problem" and deleting from "sol"
  for (const auto& item : problem.fixed)
  {
	H = std::max(H, item[1] + item[3]);
	j = sol.size();
	for (i = 0; i < sol.size(); i++)
	  if (item[0] == sol[i][0] && item[1] == sol[i][1] && item[2] == sol[i][2] && item[3] == sol[i][3])
	  {
		j = i;
		break;
	  }
	if (j == sol.size())
	{
#ifdef DEBUG
	  std::cout << "fixed don't match" << std::endl;
#endif
	  return 0;
	}
	else
	  sol.erase(sol.begin() + j);
  }

  // comparing sol and p.listwidth
  sort(sol.begin(), sol.end(), compsolwidth);

#ifndef DEBUG
  for (i = 0; i < n; i++)
	if ((sol[i][0] >= 0) && (sol[i][1] >= 0) && (sol[i][0] + sol[i][2] <= W) && (sol[i][2] == problem.listwidth[i].width) && (sol[i][3] == problem.listwidth[i].height))
	  H = std::max(H, sol[i][1] + sol[i][3]);
	else
	  return 0;
#else
  for (i = 0; i < n; i++)
  {
	if (sol[i][0] < 0)
	{
	  std::cout << "item is off left" << std::endl;
	  return 0;
	}
	if (sol[i][1] < 0)
	{
	  std::cout << "item is off down" << std::endl;
	  return 0;
	}
	if (sol[i][0] + sol[i][2] > W)
	{
	  std::cout << "item is off right" << std::endl;
	  return 0;
	}
	if (sol[i][2] != problem.listwidth[i].width)
	{
	  std::cout << "item width don't match" << std::endl;
	  return 0;
	}
	if (sol[i][3] != problem.listwidth[i].height)
	{
	  std::cout << "item height don't match" << std::endl;
	  return 0;
	}
	H = std::max(H, sol[i][1] + sol[i][3]);
  }
#endif

  // check if items overlap
  for (i = 0; i < n - 1; i++)
	for (j = i + 1; j < n; j++)
	  if (!((sol[i][0] + sol[i][2] <= sol[j][0]) || (sol[j][0] + sol[j][2] <= sol[i][0]) || (sol[i][1] + sol[i][3] <= sol[j][1]) || (sol[j][1] + sol[j][3] <= sol[i][1])))
	  {
#ifdef DEBUG
		std::cout << "conflicting objects " << i << ": (" << sol[i][0] << ", " << sol[i][1] << ", (" << sol[i][2] << ", " << sol[i][3] << ")) and " << j << ": (" << sol[j][0] << ", " << sol[j][1] << ", (" << sol[j][2] << ", " << sol[j][3] << "))" << std::endl;
#endif
		return 0;
	  }

  return H;
}
// PROBLEM MODIFIERS
Problem Prepack(const Problem& p, int& H, Solution& sol)
{
  /*
  *	sol contains initially placed rectangles, H - height of occupied strip
  * first we place all rectangles with width > problem.Width / 2 (set WIDE),
  *	  then try to place all narrow rectangles (set NARROW)
  *	  (width lesser than problem.Width - min(WIDE.width))
  *	  placing algorithm is either ExactSolution (if NARROW.size < 10) or PBF
  *	  ExactSolution can technically solve bigger cases, but they're much slower
  *	if NARROW's occupied height <= WIDE's occupied height,
  *	  then we succeeded, return sol and H
  *	else - delete narrowest rectangles from WIDE, repeat process
  * when WIDE is empty - Prepack went unsuccessful, return empty solution and H=0
  */
  H = 0;
  sol = Solution();
  // if there are fixed rectangles, Prepack reduction seems hard to make
  if (p.fixed.size())
	return p;

  const int W = p.Width, n = p.listwidth.size();
  // wcons - width of narrowest in WIDE set
  // iwide - index of narrowest in WIDE set
  // inarr - index of widest in NARROW set
  int wcons, iwide = 0, inarr = n, i, j, LB, UB;
  const std::vector<Rectangle> list = p.listwidth;
  std::vector<Rectangle> narrlist; // NARROW rectangles
  Problem interm; // problem with NARROW rectangles
  Solution intsol; // solution of interm Problem
  item_t item;
  // packed - if NARROW packed successfully
  // dopbf - is it necessary to execute PBF (ExactSolution failed)
  bool packed, dopbf;

  while (iwide < n && p.listwidth[iwide].width > W / 2)
	iwide++;
  if (!iwide)
	return p;
  wcons = p.listwidth[iwide - 1].width;
  for (i = 0; i < iwide; i++)
  {
	sol.push_back({ 0, H, p.listwidth[i].width, p.listwidth[i].height });
	H += p.listwidth[i].height;
  }
  while (p.listwidth[inarr - 1].width <= W - wcons)
	inarr--;
  if (inarr == n)
  {
	// WIDE is completely packed, NARROW is empty
	narrlist = std::vector<Rectangle>(list.begin() + iwide, list.end());
	return Problem(narrlist, W);
  }

  while (true)
  {
	narrlist = std::vector<Rectangle>(list.begin() + inarr, list.end());
	interm = Problem(narrlist, W - wcons);
	i = 0;
	// Add to "interm" all WIDE with width > wcons (or else this rectangle is pointless)
	while (list[i].width > wcons)
	{
	  interm.fixed.push_back({ 0, sol[i][1], list[i].width - wcons, list[i].height });
	  i++;
	}
#ifdef DEBUG
	assert(ProblemCorrectness(interm));
#endif
	// trying to pack NARROW
	LB = LB_easy(interm);
	dopbf = false;
	if (LB <= H)
	{
	  if (interm.fixed.size() + narrlist.size() <= 9)
	  {
		// interm size is good to solve exactly
		// if LB==UB - it's exact optimal height
		intsol = ExactSolution(interm, LB, UB);
		// It is necessary to check correctness, because CPLEX in ExactSolution sometimes bugs
		// if "intsol" is correct, UB will be the same, else UB == 0
		UB = SolutionCorrectness(interm, intsol);
		if (UB)
		{
		  // ExactSolution solved correctly
		  if (LB <= H)
		  {
			// if ExactSolution didn't find exact solution, do PBF
			if (UB > H)
			  dopbf = true;
			// success
			else
			  packed = true;
		  }
		  // executing PBF is poinless
		  else
			packed = false;
		}
		// ExactSolution solved incorrectly
		else
		  dopbf = true;
	  }
	  // problem is too big for ExactSolution, easier to solve by PBF
	  else
		dopbf = true;
	}
	// LB > H, packing makes no sense
	else
	  packed = false;

	// executing PBF
	if (dopbf)
	{
	  intsol = PBF(interm, UB, H, 0.1);
#ifdef DEBUG
	  assert(UB == SolutionCorrectness(interm, intsol));
#endif
	  packed = (UB <= H);
	}

	if (packed)
	{
	  // success - add NARROW to "sol"
	  for (i = 0; i < intsol.size(); i++)
	  {
		// Check if this rectangle is in WIDE (already in "sol")
		bool isfixed = false;
		for (j = 0; j < interm.fixed.size(); j++)
		  if (intsol[i] == interm.fixed[j])
		  {
			isfixed = true;
			break;
		  }
		// rectangle is in NARROW, need to add
		if (!isfixed)
		  sol.push_back({ intsol[i][0] + wcons, intsol[i][1], intsol[i][2], intsol[i][3] });
	  }
	  narrlist = std::vector<Rectangle>(list.begin() + iwide, list.begin() + inarr);
	  // return reduced problem
	  return Problem(narrlist, W);
	}
	else
	{
	  // fail - delete narrowest from WIDE, i.e. rectangles with width==wcons,
	  // then update wcons with new narrowest rectangle in WIDE
	  while (iwide && list[iwide - 1].width == wcons)
	  {
		H -= sol.back()[3];
		sol.pop_back();
		iwide--;
	  }
	  // WIDE is empty, Prepack went unsuccessful
	  if (!iwide)
		return p;
	  // update wcons
	  wcons = list[iwide - 1].width;
	  // update NARROW, because wcons is different
	  while (inarr < n && list[inarr].width > W - wcons)
		inarr++;
	  // if NARROW is empty, just place WIDE and return
	  if (inarr == n)
	  {
		narrlist = std::vector<Rectangle>(list.begin() + iwide, list.end());
		return Problem(narrlist, W);
	  }
	}
  }
}
Problem ModifyStripWidth(const Problem& p)
{
  /*
  * decrease Strip Width down to reachable x-coord by rectangles
  * (based on Principle of Normal Patterns)
  * reachable x-coords are determined by dynamic programming like this:
  *	  reachable x-coords by first 0 rectangles are {0} and
  *		right corners of all fixed rectangles
  *	  reachable x-coords by first k rectangles are
  *	  all reachable x-coords by first (k-1) rectangles plus width of k-th rectangle
  * new Strip Width is max(reachable x-coords by all rectangles)
  */
  Problem newp = p;
  std::vector<bool> poss; // possible - bools for each x-coord if it's reachable
  std::vector<int> poswidths; // possible widths - list of reachable x-coords
  std::vector<int> newwidths; // new reachable x-coords
  int W = p.Width, maxW = 0; // maxW - maximum reachable x-coord
  poss.reserve(W + 1);
  poswidths.reserve(W + 1);
  poswidths.push_back(0);
  poss.push_back(true);
  for (int i = 1; i < W + 1; i++)
	poss.push_back(false);

  // add all right corners of fixed items
  for (int i = 0; i < p.fixed.size(); i++)
  {
	int cons = p.fixed[i][0] + p.fixed[i][2];
	if (!poss[cons])
	{
	  poss[cons] = true;
	  poswidths.push_back(cons);
	  if (cons > maxW)
		maxW = cons;
	}
  }

  for (const auto& rec : p.listwidth)
  {
	// for every "rec" make newwidths (new x-coords)
	for (const auto& width : poswidths)
	{
	  int cons = width + rec.width;
	  if ((cons <= W) && (!poss[cons]))
	  {
		poss[cons] = true;
		newwidths.push_back(cons);
		if (cons > maxW)
		  maxW = cons;
	  }
	}
	// add newwidths to poswidths
	poswidths.insert(poswidths.end(), newwidths.begin(), newwidths.end());
	newwidths.clear();
	if (maxW == W)
	  break;
  }
  newp.Width = maxW;
  return newp;
}
Problem ModifyItemWidth(const Problem& p, int num, Rectangle_Pair_Vector* rpv_ptr)
{
  /*
  * increase given item's width up to reachable x-coord by other rectangles
  * (based on Principle of Normal Patterns)
  * reachable x-coords are determined by dynamic programming like this:
  *	  reachable x-coords by first 0 rectangles are {p.listwidth[num].width}
  *		and right corners of all fixed rectangles
  *	  reachable x-coords by first k rectangles (not including
  *	  "num" rectangle) are all reachable x-coords by first k-1 rectangles
  *	  (not including "num rectangle") plus width of k-th rectangle
  * new item width is
  *	  p.listwidth[num].width + (p.Width - max(reachable x-coords by all rectangles))
  */
  std::vector<Rectangle> list = p.listwidth;
  std::vector<bool> poss; // possible - bools for each x-coord if it's reachable
  std::vector<int> poswidths; // possible widths - list of reachable x-coords
  std::vector<int> newwidths; // new reachable x-coords
  int W = p.Width, maxW = list[num].width;  // maxW - maximum reachable x-coord
  poss.reserve(W + 1);
  poswidths.reserve(W + 1);
  poswidths.push_back(list[num].width);
  for (int i = 0; i < W + 1; i++)
	poss.push_back(false);
  poss[list[num].width] = true;

  // add all right corners of fixed items
  for (int i = 0; i < p.fixed.size(); i++)
  {
	int cons = p.fixed[i][0] + p.fixed[i][2] + list[num].width;
	if ((cons <= W) && (!poss[cons]))
	{
	  poss[cons] = true;
	  poswidths.push_back(cons);
	  if (cons > maxW)
		maxW = cons;
	}
  }
  // delete "num" rectangle
  list.erase(list.begin() + num);

  for (const auto& rec : list)
  {
	// for every "rec" make newwidths (new x-coords)
	for (const auto& width : poswidths)
	{
	  int cons = width + rec.width;
	  if ((cons <= W) && (!poss[cons]))
	  {
		poss[cons] = true;
		newwidths.push_back(cons);
		if (cons > maxW)
		  maxW = cons;
	  }
	}
	// add newwidths to poswidths
	poswidths.insert(poswidths.end(), newwidths.begin(), newwidths.end());
	newwidths.clear();
	if (maxW == W)
	  break;
  }
  if (maxW == W)
	return p;
  else
  {
	// if rectangle is widened, add this information to rpv_ptr if necessary to restore solution after
	if (rpv_ptr)
	{
	  (*rpv_ptr).push_back(std::pair<Rectangle, Rectangle>(Rectangle(p.listwidth[num].width, p.listwidth[num].height), Rectangle(p.listwidth[num].width + W - maxW, p.listwidth[num].height)));
	}
	list.push_back(Rectangle(p.listwidth[num].width + W - maxW, p.listwidth[num].height));
	return Problem(list, W, p.fixed);
  }
}
Problem ModifyAllItemsWidths(const Problem& p, Rectangle_Pair_Vector* rpv_ptr)
{
  /*
  * Perform ModifyItemWidth for every rectangle by decreasing width
  */
  Problem pp = p;
  for (int i = 0; i < p.listwidth.size(); i++)
	pp = ModifyItemWidth(pp, i, rpv_ptr);
  return pp;
}
Problem Reduction(const Problem& p, int& H, Solution& sol, Rectangle_Pair_Vector* rpv_ptr)
{
  /*
  * Apply all the reductions
  */
  Problem pp = Prepack(p, H, sol);
  pp = ModifyStripWidth(pp);
  for (int i = 0; i < pp.listwidth.size(); i++)
	pp = ModifyItemWidth(pp, i, rpv_ptr);
  return pp;
}
void UnmodifySolution(const Problem& original_problem, Solution& solution,
  int Prepack_Height, Solution Prepack_Solution, const Rectangle_Pair_Vector& rpv)
{
  /*
  * Get solution to an original problem after solving modified problem
  * Reductions must be made IN RIGHT ORDER:
  *	  FIRST: Prepack
  *	  AFTER: Modification of strip and item widths
  * When solution is found this function first restores original widths
  *	using rpv_ptr, which was constructed during modifying item widths
  * rpv[i].first - original rectangle, rpv[i].second - modified
  */

  // returning old widths
  for (int i = rpv.size() - 1; i >= 0; i--)
  {
	int h = rpv[i].first.height;
	int w1 = rpv[i].first.width;
	int w2 = rpv[i].second.width;
	bool found = false; // if we found a rectangle from the pair
	int j = 0;
	while (j < solution.size() && !found)
	{
	  // suppose we found
	  found = true;
	  if (solution[j][2] == w2 && solution[j][3] == h)
	  {
		// we found, but it could be fixed rectangle. Fixed cannot be changed
		for (const item_t& item : original_problem.fixed)
		{
		  if (item[0] == solution[j][0] && item[1] == solution[j][1])
		  {
			found = false;
			break;
		  }
		}
	  }
	  else
		found = false;
	  j++;
	}
#ifdef DEBUG
	assert(found);
#endif
	// at the end of cycle we jumped forward
	j--;
	solution[j][2] = w1;
  }

  // adding prepacked items
  for (auto& item : solution)
	item[1] += Prepack_Height;
  solution.insert(solution.end(), Prepack_Solution.begin(), Prepack_Solution.end());
}

// LOWER BOUNDS
int LB_Area(const Problem& p)
{
  /*
  * Most intuitive Lower Bound for 2D-SP. A.k.a. Continuous Lower Bound.
  * Sum of all rectangles' areas cannot exceed occupied area of optimal solution
  */
  if (!p.list.size())
	return 0;
  int S = 0; // Sum of areas
  // Sum of fixed
  for (int j = 0; j < p.fixed.size(); j++)
	S += p.fixed[j][2] * p.fixed[j][3];
  // Sum of free
  for (const auto& rec : p.list)
	S += rec.area();
  // Common implementation of ceiling function
  return (S - 1) / p.Width + 1;
}
int LB_DFF(const Problem& p)
{
  /*
  * Modification of the problem (specifically - rectangles' widths)
  * algorithm implements 4 types of functions (each of them has a parameter)
  *	  to every rectangles' width.
  * Parameter of functions are in range:
  *	  1st function - Omega1
  *	  2nd, 3rd, 4th - Omega234
  * Algorithm implements for every beta in Omega234 the 2nd function, and
  *	  then k-th function (k=1,2,3,4) for every alpha in Omegak.
  *	  then candidate Lower Bound is taken
  * Functions are called Dual Feasible Functions, for more information -
  *	  see diploma
  */
  if (!p.list.size())
	return 0;
  std::vector<Rectangle> list = p.listwidth;
  list.reserve(list.size() + p.fixed.size());
  // addH - total height of all rectangles, that have "p.Width" width
  int W = p.Width, i, j, w, h, addH = 0;
  for (j = 0; j < p.fixed.size(); j++)
  {
	// Just add all fixed
	w = p.fixed[j][2];
	h = p.fixed[j][3];
	i = 0;
	while ((i < list.size()) && (list[i].width > w))
	  i++;
	list.insert(list.begin() + i, Rectangle(w, h));
  }

  i = 0;
  while (list[i].width == W) // these rectangles will mess the algorithm up
  {
	addH += list[i].height;
	i++;
  }
  list.erase(list.begin(), list.begin() + i); // delete all wide rectangles

  int maxLB = 0, Omega234size = 0; // Best Lower Bound
  std::vector<int> Omega1, Omega234, M, I;
  std::vector<bool> Omega234bool;
  // make Omega1, Omega234, M, I
  // I(alpha) - all the width, that are more or equal than alpha
  //	(initially I have all the widths)
  // M(w, alpha) - maximum number of objects from I(alpha), that fit in w
  //	(M is constructed for every alpha in fourth loop of main algorithm with function 4)
  Omega1.reserve(W); Omega234bool.reserve(W / 2 + 1); M.reserve(W + 1); I.reserve(list.size());
  for (int k = 0; k <= W / 2; k++)
	Omega234bool.push_back(false);
  for (i = 1; i <= W; i++)
	Omega1.push_back(i);
  for (const auto& rec : list)
  {
	I.push_back(rec.width);
	if ((rec.width > W / 2) && !Omega234bool[W - rec.width])
	{
	  Omega234bool[W - rec.width] = true;
	  Omega234size++;
	}
	if ((rec.width <= W / 2) && !Omega234bool[rec.width])
	{
	  Omega234bool[rec.width] = true;
	  Omega234size++;
	}
  }
#ifdef DEBUG
  assert(!Omega234bool[0]);
#endif
  Omega234.reserve(Omega234size);
  for (int k = 1; k <= W / 2; k++)
	if (Omega234bool[k])
	  Omega234.push_back(k);

  // function 1 and function 2
  for (const auto& alpha : Omega1)
  {
	for (const auto& beta : Omega234)
	{
	  // sum of all rectangle areas of modified problem
	  int sum = 0;
	  for (const auto& rec : list)
	  {
		int buffW; // modified width
		// func 2 beta
		if (rec.width > W - beta)
		  buffW = W;
		else
		{
		  if (rec.width < beta)
			buffW = 0;
		  else
			buffW = rec.width;
		}
		// func 1 alpha
		if (((alpha + 1) * buffW) % W != 0)
		  buffW = (((alpha + 1) * buffW) / W) * W;
		else
		  buffW *= alpha;
		sum += buffW * rec.height;
	  }
	  // candidate for Lower Bound
	  int candLB = (sum - 1) / (W * alpha) + 1;
	  if (candLB > maxLB)
		maxLB = candLB;
	}
  }
  // function 2 and function 2
  for (const auto& alpha : Omega234)
  {
	for (const auto& beta : Omega234)
	{
	  // sum of all rectangle areas of modified problem
	  int sum = 0;
	  for (const auto& rec : list)
	  {
		int buffW; // modified width
		// func 2 beta
		if (rec.width > W - beta)
		  buffW = W;
		else
		{
		  if (rec.width < beta)
			buffW = 0;
		  else
			buffW = rec.width;
		}
		// func 2 alpha
		if (buffW > W - alpha)
		  buffW = W;
		else
		{
		  if (buffW < alpha)
			buffW = 0;
		}
		sum += buffW * rec.height;
	  }
	  // candidate for Lower Bound
	  int candLB = (sum - 1) / W + 1;
	  if (candLB > maxLB)
		maxLB = candLB;
	}
  }
  // function 3 and function 2
  for (const auto& alpha : Omega234)
  {
	for (const auto& beta : Omega234)
	{
	  // sum of all rectangle areas of modified problem
	  int sum = 0;
	  for (const auto& rec : list)
	  {
		int buffW; // modified width
		// func 2 beta
		if (rec.width > W - beta)
		  buffW = W;
		else
		{
		  if (rec.width < beta)
			buffW = 0;
		  else
			buffW = rec.width;
		}
		// func 3 alpha
		if (2 * buffW > W)
		  buffW = 2 * (W / alpha - (W - buffW) / alpha);
		else
		{
		  if (2 * buffW == W)
			buffW = W / alpha;
		  else
			buffW = 2 * (buffW / alpha);
		}
		sum += buffW * rec.height;
	  }
	  // candidate for Lower Bound
	  int candLB = (sum - 1) / (2 * (W / alpha)) + 1;
	  if (candLB > maxLB)
		maxLB = candLB;
	}
  }
  // function 4 and function 2
  for (const auto& alpha : Omega234)
  {
	// initially set I had all widths, now since alpha is growing,
	// set I is about to shrink for every alpha
	while (I.back() < alpha)
	  I.pop_back();
	// emerging set M
	j = 0;
	i = I.size() - 1;
	w = 0;
	int sumw = 0;
	while ((w <= W) && (i >= 0))
	{
	  sumw += I[i];
	  i--;
	  while ((w < sumw) && (w <= W))
	  {
		M.push_back(j);
		w++;
	  }
	  j++;
	}
	while (w <= W)
	{
	  M.push_back(j);
	  w++;
	}
	// done emerging M
	// main loop for 4th function
	for (const auto& beta : Omega234)
	{
	  // sum of all rectangle areas of modified problem
	  int sum = 0;
	  for (const auto& rec : list)
	  {
		int buffW; // modified width
		// func 2 beta
		if (rec.width > W - beta)
		  buffW = W;
		else
		{
		  if (rec.width < beta)
			buffW = 0;
		  else
			buffW = rec.width;
		}
		// func 4 alpha
		if (2 * buffW > W)
		  buffW = M[W] - M[W - buffW];
		else
		{
		  if (buffW < alpha)
			buffW = 0;
		  else
			buffW = 1;
		}
		sum += buffW * rec.height;
	  }
	  // candidate for Lower Bound
	  int candLB = (sum - 1) / M[W] + 1;
	  if (candLB > maxLB)
		maxLB = candLB;
	}
	// clear M to emerge it once again
	M.clear();
  }
  return maxLB + addH;
}
int LB_H(const Problem& p)
{
  /*
  * This Lower Bound is the best Lower Bound between LBH, LBH2 and LBH3
  * There is also LBH1, but LBH2 and LBH3 are the better versions of it
  * 
  * LBH = maximum height of all rectangles
  * 
  * Lay is the intermediate variable needed to calculate LBH1, LBH2, LBH3
  *	  Lay = ceiling(sum(widths) / Width), i.e. number of layers
  *	  (widths - set of all rectangles' widths, Width - strip width)
  * 
  * LBH1 is the Lower Bound, assuming, that every layer has height of 
  *	  one of the first Lay minimum heights
  * 
  * LBH2 - first Lay-1 layers has first Lay-1 minimum heights (Ifull2 set),
  *	  last top layer has some residual width, which is larger than wtop,
  *	  wtop = (sum(widths) - 1) % Width + 1 (it's  actually sum(widths) % Width
  *		but if sum(widths) % Width == 0, then wtop = Width)
  *	  this last layer can be filled with remaining minimum height rectangles
  *	  (Itop2 set), sum(widths of Itop2) >= wtop
  *	  LBH2 = sum(minimum heights of Ifull2) + max(minimum heights of Itop2)
  * 
  * LBH3 is analogous to LBH2, but the top layer is filled first,
  *	  then other Lay-1 layers are filled
  *	  wtop is calculated as before, Itop3 - first minimum height objects, that
  *	  sum(widths of Itop3) >= wtop
  *	  remaining objects make Ifull3 set: first Lay-1 minimum height objects
  *	  LBH3 = sum(minimum heights of Ifull3) + max(minimum heights of Itop3)
  */
  if (!p.list.size())
  {
	int H = 0;
	for (int k = 0; k < p.fixed.size(); k++)
	  if (H < p.fixed[k][1] + p.fixed[k][3])
		H = p.fixed[k][1] + p.fixed[k][3];
	return H;
  }
  std::vector<Rectangle> list = p.list;
  list.reserve(list.size() + p.fixed.size()); // just add all fixed to the list
  int Lay = 0, W = p.Width, LB2 = 0, LB3 = 0, LBH = 0, i, j, wtop, sumw, w, h, n = list.size();
  //int LB1;
  for (j = 0; j < p.fixed.size(); j++)
  {
	// by the way, determine LBH from fixed
	if (LBH < p.fixed[j][1] + p.fixed[j][3])
	  LBH = p.fixed[j][1] + p.fixed[j][3];
	list.push_back(Rectangle(p.fixed[j][2], p.fixed[j][3]));
  }
  sort(list.begin(), list.end(), compheight);
  if (LBH < list[0].height)
	LBH = list[0].height; // LBH is known

  // assigning sum(widths) to Lay (it's not actual Lay)
  for (const auto& rec : list)
	Lay += rec.width;

  wtop = (Lay - 1) % W + 1; // wtop is calculated
  Lay = (Lay - 1) / W + 1; // Lay is calculated (implemented ceiling)

  //calculating LBH2
  // sum Ifull2
  i = n - 1;
  while (i + Lay > n)
  {
	LB2 += list[i].height;
	i--;
  }
  //LB1 = LB2 + list[i].height;

  // max Itop2
  sumw = list[i].width;
  i--;
  while ((sumw < wtop) && (i >= 0))
  {
	sumw += list[i].width;
	i--;
  }
  LB2 += list[i + 1].height;

  // calculating LBH3
  // max Itop3
  sumw = list[n - 1].width;
  i = n - 2;
  while ((sumw < wtop) && (i >= 0))
  {
	sumw += list[i].width;
	i--;
  }
  LB3 = list[i + 1].height;

  // sum Ifull3
  j = i - Lay + 1;
  while ((i > j) && (i >= 0))
  {
	LB3 += list[i].height;
	i--;
  }
  if (LBH < LB3)
	LBH = LB3;
  if (LBH < LB2)
	LBH = LB2;
  //if (LBH < LB1) LBH = LB1;
  return LBH;
}
int LB_easy(const Problem& p)
{
  /*
  * All the easy computing (not time consuming) Lower Bounds
  */
  int dff = LB_DFF(p), lbh = LB_H(p), lb = LB_Area(p);
  if (lb < lbh)
	lb = lbh;
  if (lb < dff)
	lb = dff;
  return lb;
}
int LB_F1(const Problem& p) // NOT WORKING
{
  /*
  * This Lower Bound is based on solving relaxed problem
  * First the relaxed problem is formulated for CPLEX and then solved
  * Because the author had Community Edition of CPLEX, there were limitations
  *	  on number of equations, and CPLEX does not solve n >= 17 problems
  *	  (n - number of rectangles)
  * Because of these circumstances, the author finds debugging this function
  *	  pointless, since most real problems are way larger
  */
  std::vector<Rectangle> list = p.listwidth, origlist = p.listwidth, redlist;
  std::vector<std::vector<int>> X;
  std::vector<std::vector<std::vector<bool>>> alpha;
  std::vector<bool> poss, startposs, Xbool, falses;
  std::vector<int> newwidths, startwidths, Xall;
  const int n = list.size(), W = p.Width;
  int i = n, j, k, cons, F1 = 0, z;

  falses.reserve(W);
  for (j = 0; j < W; j++)
	falses.push_back(false);
  Xbool.reserve(W + 1);
  Xall.reserve(W);
  list.reserve(n + p.fixed.size());
  X.reserve(list.capacity());
  startposs.reserve(W + 1);

  for (j = 0; j <= W; j++)
  {
	startposs.push_back(false);
	Xbool.push_back(false);
  }
  Xbool[0] = true;
  startposs[0] = true;
  startwidths.push_back(0);
  for (j = 0; j < X.capacity(); j++)
	X.push_back(std::vector<int>());
  for (j = 0; j < p.fixed.size(); j++)
  {
	list.push_back(Rectangle(p.fixed[j][2], p.fixed[j][3]));
	X[i].push_back(p.fixed[j][0]);
	Xbool[p.fixed[j][0]] = true;
	Xbool[p.fixed[j][0] + p.fixed[j][2]] = true;
	startposs[p.fixed[j][0] + p.fixed[j][2]] = true;
	startwidths.push_back(p.fixed[j][0] + p.fixed[j][2]);
	i++;
  }
  sort(startwidths.begin(), startwidths.end());
  for (j = n; j < X.size(); j++)
	startposs[X[j][0]] = true;

  for (i = 0; i < n; i++)
  {
	poss = startposs;
	X[i] = startwidths;
	redlist = origlist;
	redlist.erase(redlist.begin() + i);
	for (j = W - list[i].width + 1; j <= W; j++)
	  poss[j] = false;
	while (X[i].back() > W - list[i].width)
	  X[i].pop_back();

	for (const auto& rec : redlist)
	{
	  for (const auto& width : X[i])
	  {
		cons = width + rec.width;
		if ((cons <= W - list[i].width) && (!poss[cons]))
		{
		  poss[cons] = true;
		  Xbool[cons] = true;
		  newwidths.push_back(cons);
		}
	  }
	  X[i].insert(X[i].end(), newwidths.begin(), newwidths.end());
	  newwidths.clear();
	}
  }
  for (i = 0; i < W; i++)
	if (Xbool[i])
	  Xall.push_back(i);

  alpha.reserve(list.size());
  for (i = 0; i < alpha.capacity(); i++)
  {
	alpha.push_back(std::vector<std::vector<bool>>());
	alpha[i].reserve(W);
	for (int x = 0; x < W; x++)
	{
	  alpha[i].push_back(std::vector<bool>());
	  if (Xbool[x])
	  {
		alpha[i][x] = falses;
		for (const auto& xbar : X[i])
		  if ((x >= xbar) && (xbar >= 0) && (xbar >= x - list[i].width + 1))
			alpha[i][x][xbar] = true;
	  }
	}
  }

  IloEnv env;
  try
  {
	IloModel model(env);
	IloIntVar h(env);
	IloExpr expr;
	IloArray<IloIntVarArray> t(env, list.size());
	for (i = 0; i < list.size(); i++)
	  t[i] = IloIntVarArray(env, X[i].size(), 0, 1);
	for (i = 0; i < list.size(); i++)
	{
	  expr = IloExpr(env);
	  for (j = 0; j < X[i].size(); j++)
		expr += t[i][j];
	  model.add(expr == 1);
	}
	for (const auto& x : Xall)
	{
	  expr = IloExpr(env);
	  for (i = 0; i < list.size(); i++)
		for (j = 0; j < X[i].size(); j++)
		  if (alpha[i][x][X[i][j]])
			expr += list[i].height * t[i][j];
	  model.add(expr <= h);
	}

	IloCplex solver(model);
	solver.setParam(IloCplex::TiLim, 600);
	solver.solve();
	F1 = solver.getBestObjValue();
	std::cout << "UB = " << solver.getObjValue() << std::endl;
  }
  catch (IloException& e)
  {
	std::cerr << "Concert exception caught: " << e << std::endl;
  }
  catch (...)
  {
	std::cerr << "Unknown exception caught" << std::endl;
  }
  return F1;
}
int LB_post1(const Problem& p, const int LB)
{
  /*
  * This is postprocessing of the Lower Bound based on Principle of Normal Patterns
  * Lower Bound is increased to the closest reachable y-coord
  * reachable y-coords are calculated using dynamic programming:
  *	  reachable y-coords are initially {0} and top corners of fixed items
  *	  reachable y-coords by first k rectangles are all reachable y-coords
  *		by first (k-1) rectangles plus height of k-th rectangle
  */
  std::vector<bool> poss; // bools if y-coord is reachable
  // vectors of reachable y-coords and newly found reachable y-coords
  std::vector<int> posheights, newheights;
  int i, cons, newLB = LB, maxheight = 0;
  bool changed = false;

  poss.reserve(LB);
  posheights.reserve(LB);
  poss.push_back(true);
  posheights.push_back(0);
  for (i = 1; i < LB; i++)
	poss.push_back(false);

  for (i = 0; i < p.fixed.size(); i++)
  {
	cons = p.fixed[i][1] + p.fixed[i][3];
	if (cons < LB && !poss[cons])
	{
	  poss[cons] = true;
	  posheights.push_back(cons);
	}

	if (newLB <= p.fixed[i][1] + p.fixed[i][3])
	{
	  newLB = p.fixed[i][1] + p.fixed[i][3];
	  changed = true;
	}
  }
  // if by some nonsens fixed items were not considered, procedure returns
  // max y-coord of fixed items, because current LB is insufficienly small
  // for the next operations
  if (changed)
	return newLB;
  for (const auto& rec : p.list)
	if (maxheight < rec.height)
	  maxheight = rec.height;
  newLB = LB + maxheight - 1; // new LB can't be higher

  for (const auto& rec : p.list)
  {
	// for every rectangle calculate new reachable y-coords
	for (const auto& height : posheights)
	{
	  cons = height + rec.height;
	  if (cons < LB)
	  {
		if (!poss[cons])
		{
		  poss[cons] = true;
		  newheights.push_back(cons);
		}
	  }
	  // lower newLB to new reachable y-coord
	  else
		if (newLB > cons)
		  newLB = cons;
	}
	posheights.insert(posheights.end(), newheights.begin(), newheights.end());
	newheights.clear();
	// if LB is already a reachable coord, further postprocessing is pointless
	if (newLB == LB)
	  break;
  }
  return newLB;
}
int LB_post2(const Problem& p, const int LB, const int UB, bool better)
{
  /*
  * This postprocessing of Lower Bound requires Upper Bound and is based on
  *	  Lower Bound of rotated problem.
  * Choose h from [LB, UB). If Lower Bound LBW of rotated problem with
  *	  width h is LBW <= p.Width, then h must be lower, else h is insufficient
  *	  to pack rotated rectangles in (p.Width, h), and we can update Lower
  *	  Bound: newLB = h+1
  * Here binary search is implemented. So log2(UB-LB) are checked
  */
  int i, newLB = LB, upp = UB, low = LB, mid, rotLB, reducH;
  // checking for nonsenses
#ifdef DEBUG
  assert(upp - low >= 0);
#endif
  if (upp == low)
	return LB;

  bool changed = false;
  std::vector<Rectangle> list;
  Solution fixed, trashsol;

  list.reserve(p.list.size());
  for (const auto& rec : p.list)
	list.push_back(Rectangle(rec.height, rec.width));
  for (i = 0; i < p.fixed.size(); i++)
  {
	fixed.push_back({ p.fixed[i][1], p.fixed[i][0], p.fixed[i][3], p.fixed[i][2] });
	if (newLB < p.fixed[i][1] + p.fixed[i][3])
	{
	  newLB = p.fixed[i][1] + p.fixed[i][3];
	  changed = true;
	}
  }
  if (changed)
	return newLB;

  // origrotp - original rotated problem, rotp - reduced problem
  Problem origrotp{ list, LB, fixed }, rotp;

  // binary search
  while (upp > low)
  {
	mid = (upp + low) / 2;
	rotp = origrotp;
	rotp.Width = mid;

	reducH = 0;
	// if better, Reduction will be performed for the cost of time
	if (better)
	  rotp = Reduction(rotp, reducH, trashsol);

	rotLB = LB_easy(rotp);
	rotLB = LB_post1(rotp, rotLB);
	if (rotLB + reducH > p.Width)
	{
	  newLB = mid + 1;
	  low = newLB;
	}
	else
	  upp = mid;
  }
  return newLB;
}
// SKYLINE
void skyline_add_item(std::vector<int>& wskyline, std::vector<int>& hskyline,
  const item_t& item, const int W)
{
  /*
  * Skyline is the simplified data structure for rectangles, where it is
  *	  considered that any empty space under any rectangle is occupied
  * In other words skyline is a set of platforms with width and height
  *	  that occupy all the space between strip bottom and the platform
  * 
  *	Adding an item to skyline is done by determining which platforms 
  *	  does the item cover and deleting them from the structure, and
  *	  then if necessary, add and update platforms on the edges
  */
#ifdef DEBUG
  assert(wskyline.size() == hskyline.size());
  assert(accumulate(wskyline.begin(), wskyline.end(), 0) == W);
#endif
  // wsk - widths of the platforms
  // hsk - heights of the platforms
  std::vector<int> wsk, hsk;

  // calculating jbegin, jend, xbegin, xend
  //  jbegin - number of leftmost platform, that item covers (even partially)
  //  jend - number of rightmost platform, that item covers (even partially)
  //  xbegin - x-coord of left corner of jbegin's platform
  //  xend - x-coord of right corner of jend's platform
  int w, h = item[1] + item[3], jbegin = 0, jend, xbegin, xend = 0;
  while (xend <= item[0])
  {
	xend += wskyline[jbegin];
	jbegin++;
  }
  jbegin--;
  xbegin = xend - wskyline[jbegin];
  jend = jbegin;
  while (xend < item[0] + item[2])
  {
	jend++;
	xend += wskyline[jend];
  }
  // jbegin, jend, xbegin, xend are calculated

  // considering all the cases
  if (jbegin == jend)
  {
	// item covers only one platform
	if (h > hskyline[jend])
	{
	  // item is above the platform
	  if (item[0] > xbegin)
	  {
		// need to create new platform on the left
		w = item[0] - xbegin;
		wskyline[jend] -= w;
		wskyline.insert(wskyline.begin() + jend, w);
		hskyline.insert(hskyline.begin() + jend, hskyline[jend]);
		jend++;
	  }
	  if (item[0] + item[2] < xend)
	  {
		// need to create new platform on the right
		w = xend - item[0] - item[2];
		wskyline[jend] -= w;
		wskyline.insert(wskyline.begin() + jend + 1, w);
		hskyline.insert(hskyline.begin() + jend + 1, hskyline[jend]);
	  }
	  // existing platform is updated
	  hskyline[jend] = h;
	}
	// else - do nothing, because item does not cover anything
  }
  else
  {
	// item covers more than 1 platform
	if (h > hskyline[jend])
	{
	  // item is higher than rightmost platform
	  if (item[0] + item[2] < xend)
	  {
		// need to create new platform on the right
		w = xend - item[0] - item[2];
		wskyline[jend] -= w;
		wskyline.insert(wskyline.begin() + jend + 1, w);
		hskyline.insert(hskyline.begin() + jend + 1, hskyline[jend]);
	  }
	  // update existing platform's height
	  hskyline[jend] = h;
	}
	// shrinking range for updating inner platforms after
	jend--;

	if (h > hskyline[jbegin])
	{
	  // item is higher than leftmost platform
	  if (item[0] > xbegin)
	  {
		// need to create new platform on the left
		w = item[0] - xbegin;
		wskyline[jbegin] -= w;
		wskyline.insert(wskyline.begin() + jbegin, w);
		hskyline.insert(hskyline.begin() + jbegin, hskyline[jbegin]);
		jbegin++;
		jend++;
	  }
	  // update existing platform's height
	  hskyline[jbegin] = h;
	}
	// shrinking range for updating inner platforms after
	jbegin++;

	// update inner platforms' height
	for (w = jbegin; w <= jend; w++)
	  if (hskyline[w] < h)
		hskyline[w] = h;
  }

  // now we have to combine neighbouring platforms with equal height
  // using wsk and hsk as buffers
  wsk = wskyline;
  hsk = hskyline;
  wskyline.clear();
  hskyline.clear();
  h = -1;
  // combining platforms
  for (w = 0; w < wsk.size(); w++)
  {
	if (hsk[w] == h)
	  wskyline.back() += wsk[w];
	else
	{
	  h = hsk[w];
	  wskyline.push_back(wsk[w]);
	  hskyline.push_back(h);
	}
  }

  return;
}
void skyline_remove_niche(std::vector<int>& wskyline,
  std::vector<int>& hskyline, const int indniche, const int W)
{
  /*
  * Skyline is the simplified data structure for rectangles, where it is
  *	  considered that any empty space under any rectangle is occupied
  * In other words skyline is a set of platforms with width and height
  *	  that occupy all the space between strip bottom and the platform
  *
  *	Niche is the lowest leftmost platform by the definition
  * Here the function only checks niche to be lower than its neighbours
  * Removing niche means lifting this platform up to one of the neighbours
  *	  with nearest (lowest height)
  * Removing niche is necessary if no rectangle can fit in it
  * 
  * Removing is implemented by considering all the cases, because every one
  *	  of them changes skyline in a different way:
  * 1) left neighbour is a strip edge
  * 2) right neighbour is a strip edge
  *	  if cases 1 and 2 were not encountered
  * 3) left neighbour is higher than the right one
  * 4) left neighbour is lower than th right one
  *	  if cases 3 and 4 were not encountered
  * 5) left neighbour is the same height as th right one
  */
#ifdef DEBUG
  assert(wskyline.size() == hskyline.size());
  // no niches when there is only one platform
  assert(wskyline.size() > 1);
  assert(accumulate(wskyline.begin(), wskyline.end(), 0) == W);
  // check if indniche (index of niche) is actually a niche
  assert(indniche == 0 && hskyline[1] > hskyline[0] || indniche == wskyline.size() - 1 && hskyline[indniche - 1] > hskyline[indniche]
	|| hskyline[indniche - 1] > hskyline[indniche] && hskyline[indniche + 1] > hskyline[indniche]);
#endif
  int w;
  if (indniche == 0)
  {
	// case 1
	// 0 platform is combined with 1
	hskyline.erase(hskyline.begin());
	w = wskyline[0];
	wskyline.erase(wskyline.begin());
	wskyline[0] += w;
  }
  else if (indniche == wskyline.size() - 1)
  {
	// case 2
	// last platform is combined with last-1 platform
	hskyline.erase(hskyline.begin() + indniche);
	w = wskyline[indniche];
	wskyline.erase(wskyline.begin() + indniche);
	wskyline[indniche - 1] += w;
  }
  else if (hskyline[indniche - 1] > hskyline[indniche + 1])
  {
	// case 3
	// indniche is combined with right neighbour
	hskyline.erase(hskyline.begin() + indniche);
	w = wskyline[indniche];
	wskyline.erase(wskyline.begin() + indniche);
	wskyline[indniche] += w;
  }
  else if (hskyline[indniche - 1] < hskyline[indniche + 1])
  {
	// case 4
	// indniche is combined with left neighbour
	hskyline.erase(hskyline.begin() + indniche);
	w = wskyline[indniche];
	wskyline.erase(wskyline.begin() + indniche);
	wskyline[indniche - 1] += w;
  }
  else
  {
	// case 5
	// left and right neighbours and niche are all combined into one platform
	hskyline.erase(hskyline.begin() + indniche);
	hskyline.erase(hskyline.begin() + indniche);
	w = wskyline[indniche] + wskyline[indniche + 1];
	wskyline.erase(wskyline.begin() + indniche);
	wskyline.erase(wskyline.begin() + indniche);
	wskyline[indniche - 1] += w;
  }
  return;
}
bool good_to_add_to_skyline(const std::vector<int>& wskyline,
  const std::vector<int>& hskyline, const item_t& item)
{
  /*
  * Check if item item overlaps with the skyline
  * Just check if all covered platforms are lower or equal than item's y-coord
  */

  // x - current x-coord
  // j - current platform

  // now x is the right x-coord of j-th platform
  int x = wskyline[0], j = 0;
  while (x <= item[0])
  {
	j++;
	x += wskyline[j];
  }
  x -= wskyline[j];
  // now x is the left x-coord of j-th platform
  // checking all covered platforms
  while (x < item[0] + item[2])
  {
	if (item[1] > hskyline[j])
	  return false;
	x += wskyline[j];
	j++;
  }
  return true;
}
// SOLVERS / UPPER BOUNDS
Solution UB_post(const Problem& p, const Solution& oldsol, int& UB,
  const int LB, const double time_sec, bool better)
{
  /*
  * Postprocessing of the Upper Bound to improve solution by solving rotated
  *	  problems
  * Choose h from [LB, UB)
  * If rotated problem can be packed in strip with width h, and height p.Width,
  *	  i.e. UBW <= p.Width, then UB and solution can be updated: UB = h
  * Else: search for values h in [h+1, UB)
  * 
  *	Binary search is implemented
  * 
  * Also becuase unlike LB_post2, packing is way more time consuming, basic
  *	  time management is implemented, giving for every packing algorithm
  *	  such time, that every packing will get about equal time (used avcalls)
  */
  int i, newUB = UB, upp = UB, low = LB, mid, rotUB, reducH;
#ifdef DEBUG
  assert(upp - low >= 0);
  assert(upp == SolutionCorrectness(p, oldsol));
#endif
  if (upp == low)
	return oldsol;
  // outer - global Timer
  // inner - Timer in the cycle
  Timer outer(time_sec), inner;
  // avcalls - average number of calls of packing algorthm for given upp-low
  std::vector<double> avcalls;
  avcalls.reserve(upp - low + 1);
  avcalls.push_back(0.0);
  for (i = 1; i <= upp - low; i++)
  {
	if (i % 2)
	  avcalls.push_back(1 + avcalls[i / 2]);
	else
	  avcalls.push_back(1 + 0.5 * (avcalls[i / 2] + avcalls[i / 2 - 1]));
  }

  std::vector<Rectangle> list;
  // rotfixed - rotated p.fixed
  // reducsol - solution after reduction (prepack)
  // finalsol - improved best solution
  // rawsol - candidate for best solution
  Solution rotfixed, reducsol, finalsol, rawsol;
  
  list.reserve(p.list.size());
  for (const auto& rec : p.list)
	list.push_back(Rectangle(rec.height, rec.width));
  for (i = 0; i < p.fixed.size(); i++)
	rotfixed.push_back({ p.fixed[i][1], p.fixed[i][0], p.fixed[i][3], p.fixed[i][2] });
  // origrotp - original rotated problem
  // rotp - reduced origrotp
  Problem origrotp{ list, low, rotfixed }, rotp;

  // checking if using PBF is adequate (PBF is better than FindHeuristicSequence,
  // but PBF only works correct, when there are no fixed items or fixed items
  // do not cover any empty space)
  // area_real - real area, that fixed items occupy
  // area_skyline - area, that fixed items occupy as a skyline
  // if area_real < area_skyline then empty space is covered and PBF is not used
  int area_real = 0, area_skyline = 0;
  std::vector<int> wsk{ low }, hsk{ 0 };
  for (const item_t& item : origrotp.fixed)
  {
	skyline_add_item(wsk, hsk, item, low);
	area_real += item[2] * item[3];
  }
  for (i = 0; i < wsk.size(); i++)
	area_skyline += wsk[i] * hsk[i];
  bool CanUsePBF = (area_real == area_skyline);
  Rectangle_Pair_Vector rpv;

  // binary search
  while (upp > low)
  {
	// setting inner Timer for this iteration's packing
	// remaining time is distributed by average packing function calls
	inner = Timer(outer.left() / avcalls[upp - low]);
	mid = (upp + low) / 2;
	origrotp.Width = mid;
	rotp = origrotp;
	rpv.clear();

	reducH = 0;
	// for the cost of time, Reduction can be called
	if (better)
	  rotp = Reduction(rotp, reducH, reducsol, &rpv);
	if (CanUsePBF)
	  rawsol = PBF(rotp, rotUB, p.Width - reducH, std::max(inner.left(), 0.1));
	else
	  rawsol = FindHeuristicSequence(rotp, rotUB, p.Width - reducH, std::max(inner.left(), 0.1));
	rotUB += reducH;

	// update UB and finalsol if packed successfully
	if (rotUB <= p.Width)
	{
	  newUB = mid;
	  upp = mid;
	  finalsol = rawsol;
	  UnmodifySolution(origrotp, finalsol, reducH, reducsol, rpv);
	}
	else
	  low = mid + 1;
  }
  if (newUB == UB)
  {
	return oldsol;
  }
  else
  {
	// rotate solution back
	UB = newUB;
	for (item_t& item : finalsol)
	{
	  std::swap(item[0], item[1]);
	  std::swap(item[2], item[3]);
	}
	return finalsol;
  }
}
int placement_criteria(const std::vector<int>& wsk, const std::vector<int>& hsk,
  const std::vector<Rectangle>& list, int& jtallest, const int criteria,
  const int W, const int sumareas, int& indniche)
{
  /*
  * Used in PBF to determine which of free rectangles to pack in niche
  * There are 4 hard and 6 weak criterias (small)
  * Actual criteria (big) is a combination of three of aforementioned
  *	  small criterias
  * There are 20 useful combinations, that are used
  * 
  * Function first calculates all the small criterias, then determines
  *	  necessary combination
  * All the small criterias are calculated to break ties between some
  *	  rectangles with equally passing small criterias
  * All the criterias for all rectangles construct value vector. Chosen
  *	  small criterias in the combination contribute most impact
  *	  (20 points). Other small criterias contribute 1 point
  */
  const int n = list.size();
  // value - final values to determine chosen rectangle
  // w4val - niche width, which must be maximized when placed along with
  //	objects with same height
  // w5val - niche width, which must be maximized when placed along with
  //	objects even with different heights
  std::vector<int> widths, heights, value, w4val, w5val;
  // bools for every rectangle whether they pass small criterias
  std::vector<bool> h1, h2, h3, h4, w1, w2, w3, w4, w5, w6;
  // needed for w6
  std::vector<double> density;
  
  // h1 - object's width == niche width
  h1.reserve(n);
  // h2 - object's heifht == one of the neighbouring platforms' height
  h2.reserve(n);
  // h3 - object's heifht == left platform's height
  // if strip edge is on the left then object is the highest
  h3.reserve(n);
  // object fills the niche with other rectangles (even with different heights)
  h4.reserve(n);
  // object is the highest
  w1.reserve(n);
  // object is the widest
  w2.reserve(n);
  // object has maximum area out of other
  w3.reserve(n);
  // object maximizes niche width when packed with same height objects
  w4.reserve(n);
  // object maximizes niche width when packed with different height objects
  w5.reserve(n);
  // object maximizes density of the emerging solution
  w6.reserve(n);
  widths.reserve(n);
  heights.reserve(n);
  value.reserve(n);
  w4val.reserve(n);
  w5val.reserve(n);
  density.reserve(n);

  // filling all vectors
  for (const auto& rec : list)
  {
	widths.push_back(rec.width);
	heights.push_back(rec.height);
	value.push_back(-1);
	w4val.push_back(0);
	density.push_back(0.0);
	h1.push_back(false);
  }
  w5val = w4val;
  h2 = h3 = h4 = w1 = w2 = w3 = w4 = w5 = w6 = h1;

  int maxw4 = 0, maxw5 = 0;
  double maxdens = 0.0;
  // indniche - leftmost lowest platform
  indniche = min_element(hsk.begin(), hsk.end()) - hsk.begin();
  // width and height of the niche
  int wniche = wsk[indniche], hniche = hsk[indniche];
  // H - height of the emerging solution
  int H = *max_element(hsk.begin(), hsk.end());
  // height height from niche to the left and right neighbours (0 if it's strip edge)
  int hleft = (indniche ? hsk[indniche - 1] - hniche : 0), hright = (indniche < wsk.size() - 1 ? hsk[indniche + 1] - hniche : 0);
  // max widths, heights and areas out of all elements, that fit in niche
  int maxheight = 0, maxwidth = 0, maxarea = 0;
  for (int i = 0; i < n; i++)
	if (widths[i] <= wniche)
	{
	  value[i] = 0;
	  if (maxarea < list[i].area())
		maxarea = list[i].area();
	  if (maxwidth < widths[i])
		maxwidth = widths[i];
	  if (maxheight < heights[i])
		maxheight = heights[i];
	}

  // main cycle to determine small criterias for all rectangles
  for (int i = 0; i < n; i++)
  {
	// rectangles, that don't fit in niche get value -1 (value is initialized by -1)
	if (widths[i] > wniche)
	  continue;
	int w = wniche - widths[i];
	if (!w)
	{
	  h1[i] = true;
	  // when niche is completely filled by an object,
	  // w4val and w5val are calculated trivially
	  if (hleft == hright)
	  {
		if (wsk.size() == 1)
		  w4val[i] = w5val[i] = W;
		else if (heights[i] < hleft)
		  w4val[i] = w5val[i] = wniche;
		else if (heights[i] == hleft)
		  w4val[i] = w5val[i] = hsk[indniche - 1] + wniche + hsk[indniche + 1];
		else if (wsk[indniche - 1] < wsk[indniche + 1])
		  w4val[i] = w5val[i] = wsk[indniche - 1];
		else
		  w4val[i] = w5val[i] = wsk[indniche + 1];
	  }
	  else if (!hleft || (hleft > hright))
	  {
		if (hright < heights[i])
		  w4val[i] = w5val[i] = wsk[indniche + 1];
		else if (hright == heights[i])
		  w4val[i] = w5val[i] = wniche + wsk[indniche + 1];
		else
		  w4val[i] = w5val[i] = wniche;
	  }
	  else
	  {
		if (hleft < heights[i])
		  w4val[i] = w5val[i] = wsk[indniche - 1];
		else if (hleft == heights[i])
		  w4val[i] = w5val[i] = wniche + wsk[indniche - 1];
		else
		  w4val[i] = w5val[i] = wniche;
	  }
	}
	// if object doesn't fill niche completely, assign
	// w4val and w5val leftover width for now
	else
	  w4val[i] = w5val[i] = w;

	if ((heights[i] == hleft) || (heights[i] == hright))
	  h2[i] = true;
	if ((heights[i] == hleft) || (!hleft) && (heights[i] == maxheight))
	  h3[i] = true;
	// calculate h4, w4, w5
	for (int j = 0; j < n; j++)
	{
	  if ((i != j) && (widths[j] == w))
	  {
		if ((heights[i] == hleft) && (heights[j] == hleft))
		  h4[i] = true;
		if ((heights[i] == hright) && (heights[j] == hright))
		  h4[i] = true;

		if (heights[j] == heights[i])
		{
		  // calculate w4val and w5val
		  if (hleft == hright)
		  {
			if (wsk.size() == 1)
			{
			  w4val[i] = W;
			  w5val[i] = W;
			}
			else if (heights[i] < hleft)
			{
			  if (w4val[i] < wniche)
				w4val[i] = wniche;
			  if (w5val[i] < wniche)
				w5val[i] = wniche;
			}
			else if (heights[i] == hleft)
			{
			  if (w4val[i] < hsk[indniche - 1] + wniche + hsk[indniche + 1])
				w4val[i] = hsk[indniche - 1] + wniche + hsk[indniche + 1];
			  if (w5val[i] < hsk[indniche - 1] + wniche + hsk[indniche + 1])
				w5val[i] = hsk[indniche - 1] + wniche + hsk[indniche + 1];
			}
			else if (wsk[indniche - 1] < wsk[indniche + 1])
			{
			  if (w4val[i] < wsk[indniche - 1])
				w4val[i] = wsk[indniche - 1];
			  if (w5val[i] < wsk[indniche - 1])
				w5val[i] = wsk[indniche - 1];
			}
			else
			{
			  if (w4val[i] < wsk[indniche + 1])
				w4val[i] = wsk[indniche + 1];
			  if (w5val[i] < wsk[indniche + 1])
				w5val[i] = wsk[indniche + 1];
			}
		  }
		  else if (!hleft || (hleft > hright))
		  {
			if (hright < heights[i])
			{
			  if (w4val[i] < wsk[indniche + 1])
				w4val[i] = wsk[indniche + 1];
			  if (w5val[i] < wsk[indniche + 1])
				w5val[i] = wsk[indniche + 1];
			}
			else if (hright == heights[i])
			{
			  if (w4val[i] < wniche + wsk[indniche + 1])
				w4val[i] = wniche + wsk[indniche + 1];
			  if (w5val[i] < wniche + wsk[indniche + 1])
				w5val[i] = wniche + wsk[indniche + 1];
			}
			else
			{
			  if (w4val[i] < wniche)
				w4val[i] = wniche;
			  if (w5val[i] < wniche)
				w5val[i] = wniche;
			}
		  }
		  else
		  {
			if (hleft < heights[i])
			{
			  if (w4val[i] < wsk[indniche - 1])
				w4val[i] = wsk[indniche - 1];
			  if (w5val[i] < wsk[indniche - 1])
				w5val[i] = wsk[indniche - 1];
			}
			else if (hleft == heights[i])
			{
			  if (w4val[i] < wniche + wsk[indniche - 1])
				w4val[i] = wniche + wsk[indniche - 1];
			  if (w5val[i] < wniche + wsk[indniche - 1])
				w5val[i] = wniche + wsk[indniche - 1];
			}
			else
			{
			  if (w4val[i] < wniche)
				w4val[i] = wniche;
			  if (w5val[i] < wniche)
				w5val[i] = wniche;
			}
		  }
		}
		else
		{
		  // calculate w5val
		  if (hleft == hright)
		  {
			if (wsk.size() == 1)
			  if (heights[j] < heights[i])
				w5val[i] = std::max(w5val[i], widths[j]);
			  else
				w5val[i] = std::max(w5val[i], widths[i]);
			else if (heights[j] < heights[i])
			{
			  if (heights[j] < hleft)
				w5val[i] = std::max(w5val[i], widths[j]);
			  else if (heights[j] == hleft)
				w5val[i] = std::max(w5val[i], std::max(std::min(wsk[indniche - 1] + widths[j], wsk[indniche + 1]), std::min(wsk[indniche - 1], wsk[indniche + 1] + widths[j])));
			  else
				w5val[i] = std::max(w5val[i], std::min(wsk[indniche - 1], wsk[indniche + 1]));
			}
			else
			{
			  if (heights[i] < hleft)
				w5val[i] = std::max(w5val[i], widths[i]);
			  else if (heights[i] == hleft)
				w5val[i] = std::max(w5val[i], std::max(std::min(wsk[indniche - 1] + widths[i], wsk[indniche + 1]), std::min(wsk[indniche - 1], wsk[indniche + 1] + widths[i])));
			  else
				w5val[i] = std::max(w5val[i], std::min(wsk[indniche - 1], wsk[indniche + 1]));
			}
		  }
		  else
		  {
			int win, hin, wout, hout;
			if (!hleft)
			{
			  hout = hsk[indniche + 1];
			  wout = wsk[indniche + 1];
			}
			else if (!hright)
			{
			  hout = hsk[indniche - 1];
			  wout = wsk[indniche - 1];
			}
			else if (hsk[indniche - 1] < hsk[indniche + 1])
			{
			  hout = hsk[indniche - 1];
			  wout = wsk[indniche - 1];
			}
			else
			{
			  hout = hsk[indniche + 1];
			  wout = wsk[indniche + 1];
			}
			if (heights[j] < heights[i])
			{
			  hin = heights[j];
			  win = widths[j];
			}
			else
			{
			  hin = heights[i];
			  win = widths[i];
			}
			if (hin < hout)
			  w5val[i] = std::max(w5val[i], win);
			else if (hin == hout)
			  w5val[i] = std::max(w5val[i], win + wout);
			else
			  w5val[i] = std::max(w5val[i], wout);
		  }
		}
	  }
	}

	if (heights[i] == maxheight)
	  w1[i] = true;
	if (widths[i] == maxwidth)
	  w2[i] = true;
	if (list[i].area() == maxarea)
	  w3[i] = true;
	// the reasons why sumareas has to be passed (skyline doesn't count empty spaces)
	density[i] = (sumareas + list[i].area()) / (double)(std::max(H, hniche + heights[i]));
	maxw4 = std::max(maxw4, w4val[i]);
	maxw5 = std::max(maxw5, w5val[i]);
	maxdens = std::max(maxdens, density[i]);
  }
  // determine w4, w5, w6
  for (int i = 0; i < n; i++)
  {
	if (w4val[i] == maxw4)
	  w4[i] = true;
	if (w5val[i] == maxw5)
	  w5[i] = true;
	if ((density[i] - maxdens) < 0.00001)
	  w6[i] = true;
  }

  // CALCULATE PASSED BIG CRITERIA (all the combinations are described in diploma)
  switch (criteria + 1)
  {
  case 1:
	for (int i = 0; i < n; i++)
	  if (value[i] != -1)
	  {
		value[i] = h1[i] + h2[i] + h3[i] + h4[i] + w1[i] + w2[i] + w3[i] + w4[i] + w5[i] + w6[i];
		if (h3[i])
		{
		  value[i] += 20;
		  if (h1[i])
		  {
			value[i] += 20;
			if (w1[i])
			  value[i] += 20;
		  }
		}
	  }
	break;
  case 2:
	for (int i = 0; i < n; i++)
	  if (value[i] != -1)
	  {
		value[i] = h1[i] + h2[i] + h3[i] + h4[i] + w1[i] + w2[i] + w3[i] + w4[i] + w5[i] + w6[i];
		if (h1[i])
		{
		  value[i] += 20;
		  if (h3[i])
		  {
			value[i] += 20;
			if (w1[i])
			  value[i] += 20;
		  }
		}
	  }
	break;
  case 3:
	for (int i = 0; i < n; i++)
	  if (value[i] != -1)
	  {
		value[i] = h1[i] + h2[i] + h3[i] + h4[i] + w1[i] + w2[i] + w3[i] + w4[i] + w5[i] + w6[i];
		if (h3[i])
		{
		  value[i] += 20;
		  if (w4[i])
		  {
			value[i] += 20;
			if (w6[i])
			  value[i] += 20;
		  }
		}
	  }
	break;
  case 4:
	for (int i = 0; i < n; i++)
	  if (value[i] != -1)
	  {
		value[i] = h1[i] + h2[i] + h3[i] + h4[i] + w1[i] + w2[i] + w3[i] + w4[i] + w5[i] + w6[i];
		if (h3[i])
		{
		  value[i] += 20;
		  if (w4[i])
		  {
			value[i] += 20;
			if (w1[i])
			  value[i] += 20;
		  }
		}
	  }
	break;
  case 5:
	for (int i = 0; i < n; i++)
	  if (value[i] != -1)
	  {
		value[i] = h1[i] + h2[i] + h3[i] + h4[i] + w1[i] + w2[i] + w3[i] + w4[i] + w5[i] + w6[i];
		if (h1[i])
		{
		  value[i] += 20;
		  if (h2[i])
		  {
			value[i] += 20;
			if (w2[i])
			  value[i] += 20;
		  }
		}
	  }
	break;
  case 6:
	for (int i = 0; i < n; i++)
	  if (value[i] != -1)
	  {
		value[i] = h1[i] + h2[i] + h3[i] + h4[i] + w1[i] + w2[i] + w3[i] + w4[i] + w5[i] + w6[i];
		if (h1[i])
		{
		  value[i] += 20;
		  if (h2[i])
		  {
			value[i] += 20;
			if (w3[i])
			  value[i] += 20;
		  }
		}
	  }
	break;
  case 7:
	for (int i = 0; i < n; i++)
	  if (value[i] != -1)
	  {
		value[i] = h1[i] + h2[i] + h3[i] + h4[i] + w1[i] + w2[i] + w3[i] + w4[i] + w5[i] + w6[i];
		if (h3[i])
		{
		  value[i] += 20;
		  if (w3[i])
			value[i] += 20;
		}
	  }
	break;
  case 8:
	for (int i = 0; i < n; i++)
	  if (value[i] != -1)
	  {
		value[i] = h1[i] + h2[i] + h3[i] + h4[i] + w1[i] + w2[i] + w3[i] + w4[i] + w5[i] + w6[i];
		if (h1[i])
		{
		  value[i] += 20;
		  if (w1[i])
			value[i] += 20;
		}
	  }
	break;
  case 9:
	for (int i = 0; i < n; i++)
	  if (value[i] != -1)
	  {
		value[i] = h1[i] + h2[i] + h3[i] + h4[i] + w1[i] + w2[i] + w3[i] + w4[i] + w5[i] + w6[i];
		if (h1[i])
		{
		  value[i] += 20;
		  if (w3[i])
			value[i] += 20;
		}
	  }
	break;
  case 10:
	for (int i = 0; i < n; i++)
	  if (value[i] != -1)
	  {
		value[i] = h1[i] + h2[i] + h3[i] + h4[i] + w1[i] + w2[i] + w3[i] + w4[i] + w5[i] + w6[i];
		if (h1[i])
		{
		  value[i] += 20;
		  if (w4[i])
		  {
			value[i] += 20;
			if (w2[i])
			  value[i] += 20;
		  }
		}
	  }
	break;
  case 11:
	for (int i = 0; i < n; i++)
	  if (value[i] != -1)
	  {
		value[i] = h1[i] + h2[i] + h3[i] + h4[i] + w1[i] + w2[i] + w3[i] + w4[i] + w5[i] + w6[i];
		if (w4[i])
		{
		  value[i] += 20;
		  if (h3[i])
		  {
			value[i] += 20;
			if (w2[i])
			  value[i] += 20;
		  }
		}
	  }
	break;
  case 12:
	for (int i = 0; i < n; i++)
	  if (value[i] != -1)
	  {
		value[i] = h1[i] + h2[i] + h3[i] + h4[i] + w1[i] + w2[i] + w3[i] + w4[i] + w5[i] + w6[i];
		if (w4[i])
		{
		  value[i] += 20;
		  if (w1[i])
			value[i] += 20;
		}
	  }
	break;
  case 13:
	for (int i = 0; i < n; i++)
	  if (value[i] != -1)
	  {
		value[i] = h1[i] + h2[i] + h3[i] + h4[i] + w1[i] + w2[i] + w3[i] + w4[i] + w5[i] + w6[i];
		if (h2[i])
		{
		  value[i] += 20;
		  if (w3[i])
			value[i] += 20;
		}
	  }
	break;
  case 14:
	for (int i = 0; i < n; i++)
	  if (value[i] != -1)
	  {
		value[i] = h1[i] + h2[i] + h3[i] + h4[i] + w1[i] + w2[i] + w3[i] + w4[i] + w5[i] + w6[i];
		if (h3[i])
		{
		  value[i] += 20;
		  if (h1[i])
		  {
			value[i] += 20;
			if (w6[i])
			  value[i] += 20;
		  }
		}
	  }
	break;
  case 15:
	for (int i = 0; i < n; i++)
	  if (value[i] != -1)
	  {
		value[i] = h1[i] + h2[i] + h3[i] + h4[i] + w1[i] + w2[i] + w3[i] + w4[i] + w5[i] + w6[i];
		if (h2[i])
		{
		  value[i] += 20;
		  if (w4[i])
		  {
			value[i] += 20;
			if (w2[i])
			  value[i] += 20;
		  }
		}
	  }
	break;
  case 16:
	for (int i = 0; i < n; i++)
	  if (value[i] != -1)
	  {
		value[i] = h1[i] + h2[i] + h3[i] + h4[i] + w1[i] + w2[i] + w3[i] + w4[i] + w5[i] + w6[i];
		if (h3[i])
		{
		  value[i] += 20;
		  if (h4[i])
		  {
			value[i] += 20;
			if (w3[i])
			  value[i] += 20;
		  }
		}
	  }
	break;
  case 17:
	for (int i = 0; i < n; i++)
	  if (value[i] != -1)
	  {
		value[i] = h1[i] + h2[i] + h3[i] + h4[i] + w1[i] + w2[i] + w3[i] + w4[i] + w5[i] + w6[i];
		if (h1[i])
		{
		  value[i] += 20;
		  if (h4[i])
		  {
			value[i] += 20;
			if (w3[i])
			  value[i] += 20;
		  }
		}
	  }
	break;
  case 18:
	for (int i = 0; i < n; i++)
	  if (value[i] != -1)
	  {
		value[i] = h1[i] + h2[i] + h3[i] + h4[i] + w1[i] + w2[i] + w3[i] + w4[i] + w5[i] + w6[i];
		if (w4[i])
		{
		  value[i] += 20;
		  if (h4[i])
		  {
			value[i] += 20;
			if (w3[i])
			  value[i] += 20;
		  }
		}
	  }
	break;
  case 19:
	for (int i = 0; i < n; i++)
	  if (value[i] != -1)
	  {
		value[i] = h1[i] + h2[i] + h3[i] + h4[i] + w1[i] + w2[i] + w3[i] + w4[i] + w5[i] + w6[i];
		if (h2[i])
		{
		  value[i] += 20;
		  if (h4[i])
		  {
			value[i] += 20;
			if (w1[i])
			  value[i] += 20;
		  }
		}
	  }
	break;
  case 20:
	for (int i = 0; i < n; i++)
	  if (value[i] != -1)
	  {
		value[i] = h1[i] + h2[i] + h3[i] + h4[i] + w1[i] + w2[i] + w3[i] + w4[i] + w5[i] + w6[i];
		if (w5[i])
		{
		  value[i] += 20;
		  if (h1[i])
		  {
			value[i] += 20;
			if (w3[i])
			  value[i] += 20;
		  }
		}
	  }
	break;
  }

  // determine chosen rectangle
  int maxvalue = -1, imax = -1;
  // index of the tallest rectangle, that fit in niche
  // necessary to perform Look-Ahead procedure in PBF
  jtallest = 0;
  for (int i = 0; i < n; i++)
  {
	if (value[i] > maxvalue)
	{
	  imax = i;
	  maxvalue = value[i];
	}
	if (w1[i] && (value[i] > value[jtallest]))
	  jtallest = i;
  }
  // if imax == -1, niche is too narrow, need to be deleted
  return imax;
}
Solution PBF(const Problem& p, int& UB, int LB, const double time_sec)
{
  /*
  * Priority Best Fit heuristic. Solution is emerging by placing rectangles
  *	  one by one. Every time rectangle to place is determined by placement
  *	  criteria (plus Look-Ahead procedure)
  * PBF packs several times with different conditions:
  *	  1) using different combinations of small criterias for placement_criteria
  *	  2) using different arrangements of strip bottom got by Warm Start
  * First Warm Start gives valid arrangement of strip bottom,
  *	  then using this arrangement PBF packs 20 times using all criterias
  * PBF packs until time is up or if time is not passed - numit times:
  *	  numit = 40000000 / (n^2) + 20 (each iteration - packing with determined
  *	  combination of criterias)
  * Warm Start procedure - bottom of the strip is packed randomly, maximizing
  *	  covered width. Done by dynamic programming.
  * Look-Ahead procedure - pure PBF could pack inefficiently, remaining tall
  *	  items for last placements, vastly worsening the solution.
  *	  This procedure makes PBF sometimes place tall items, instead of
  *	  chosen by placement_criteria
  */
  if (!p.list.size())
  {
	UB = 0;
	for (int ii = 0; ii < p.fixed.size(); ii++)
	  if (UB < p.fixed[ii][1] + p.fixed[ii][3])
		UB = p.fixed[ii][1] + p.fixed[ii][3];
	return p.fixed;
  }
  Timer timer(time_sec);
#ifdef DEBUG
  assert(ProblemCorrectness(p));
#endif
  UB = 0;
  // dynlist - dynamically changing vector of free rectangles
  // warmstart - vector used to build warmlist
  // warmlist - vector of objects, that are placed in the bottom
  std::vector<Rectangle> list = p.listwidth, dynlist, warmstart, warmlist;
  const int W = p.Width, n = list.size();
  int startsumareas = 0, warmsumareas = 0, sumareas;
  // if time is not passed, numit is used to count number of packs
  int numit = 40000000 / (list.size() * list.size()) + 20, it = 0;

  // ?startskyline - skyline of fixed items
  // ?skyline - skyline after warm start
  // ?sk - skyline used throughout the packing
  std::vector<int> wskyline{ W }, hskyline{ 0 }, wstartskyline, hstartskyline, wsk, hsk;

  // used in warmstart
  std::vector<std::vector<bool>> poss;
  std::vector<int> posswidths, newwidths;
  poss.reserve(n + 1);
  posswidths.reserve(W + 1);

  Solution startsol = p.fixed, warmsol, sol, bestsol;
  std::random_device rd;
  std::default_random_engine rng{ rd() };

  // there are 20 criterias, at the begining criteria increases, so
  // first iteration will be with 0 criteria
  // Also only when (criteria == 0) Warm Start is performed
  int criteria = 19;
  for (int i = 0; i <= n; i++)
  {
	poss.push_back(std::vector<bool>());
	poss[i].reserve(W + 1);
  }
  for (int i = 0; i <= W; i++)
	poss[0].push_back(false);

  int jbegin, jend, xbegin, xend;
  for (int i = 0; i < p.fixed.size(); i++)
  {
	startsumareas += p.fixed[i][2] * p.fixed[i][3];
	skyline_add_item(wskyline, hskyline, p.fixed[i], W);
  }

  wstartskyline = wskyline; hstartskyline = hskyline;
  int wstart = W - wskyline.back(), cons;
  poss[0][wstart] = true;
  // AE, AM - used for Look-Ahead
  int AE, AM, H, tempH, indniche, toplace;
  char position;

  while (!time_sec && it < numit || time_sec && timer.run())
  {
	it++;
	criteria = (criteria + 1) % 20;
	// Warm Start is performed only before 0-th criteria
	if (!criteria)
	{
	  // WARM START
	  // Warm start is based on dynamic programming just like in ModifyItemWidth
	  // first we SHUFFLE dynlist (full list of free rectangles) for the
	  //	Warm Start to give random placement of strip bottom
	  // Then sequentially reachable x-coords are calculated
	  //	Once during calculating x-coords right strip edge was reached
	  //	calculating x-coords stops, then rectangles to place at bottom
	  //	are determined by restoring solution by using table got from
	  //	dynamic programming:
	  //	  Suppose x0 coord is reached (and n+1, n+2,... objects are
	  //		decided whether to place them or not to place on warm start)
	  //	  If (x0 - dynlist[n].width) coord is reached then place
	  //		n-th object, x0 -= dynlist[n].width
	  //	  Else do not place n-th object, n -= 1
	  rng = std::default_random_engine(rd());
	  dynlist = list; wskyline = wstartskyline; hskyline = hstartskyline; warmsol = startsol; warmsumareas = startsumareas;
	  shuffle(dynlist.begin(), dynlist.end(), rng);
	  warmlist = dynlist;

	  warmstart.clear();
	  posswidths.clear();
	  posswidths.push_back(wstart);
	  int i = 0;
	  for (const auto& rec : dynlist)
	  {
		poss[i + 1] = poss[i];
		for (const auto& width : posswidths)
		{
		  cons = width + rec.width;
		  if ((cons <= W) && (!poss[i + 1][cons]))
		  {
			poss[i + 1][cons] = true;
			newwidths.push_back(cons);
		  }
		}
		i++;
		posswidths.insert(posswidths.end(), newwidths.begin(), newwidths.end());
		newwidths.clear();
		if (poss[i][W])
		  break;
	  }
	  int j = W;
	  while (!poss[i][j])
		j--;
	  while (j > wstart)
	  {
		i--;
		if ((j >= dynlist[i].width) && (poss[i][j - dynlist[i].width]))
		{
		  j -= dynlist[i].width;
		  warmstart.push_back(dynlist[i]);
		  warmlist.erase(warmlist.begin() + i);
		}
	  }

	  // Converting Warm Start objects to skyline
	  i = wstart;
	  for (const auto& rec : warmstart)
	  {
		wskyline.insert(wskyline.end() - 1, rec.width);
		hskyline.insert(hskyline.end() - 1, rec.height + hskyline.back());
		wskyline.back() -= rec.width;
		warmsol.push_back({ i, hstartskyline.back(), rec.width, rec.height });
		warmsumareas += rec.area();
		i += rec.width;
	  }
	  if (!wskyline.back())
	  {
		wskyline.pop_back();
		hskyline.pop_back();
	  }
	  i = 0;
	  while (i + 1 < wskyline.size())
	  {
		if (hskyline[i] == hskyline[i + 1])
		{
		  int w = wskyline[i] + wskyline[i + 1];
		  wskyline.erase(wskyline.begin() + i);
		  hskyline.erase(hskyline.begin() + i);
		  wskyline[i] = w;
		  continue;
		}
		i++;
	  }
	}
	// Main part of PBF
	wsk = wskyline; hsk = hskyline; sol = warmsol; dynlist = warmlist; sumareas = warmsumareas;
	H = *max_element(hsk.begin(), hsk.end());
	while (dynlist.size())
	{
	  int j; // tallest object that fit in niche
	  int i = placement_criteria(wsk, hsk, dynlist, j, criteria, W, sumareas, indniche);
	  if (i >= 0)
	  {
		// if niche is wide enough to fit at least one object

		// toplace - chosen object to place after Look-Ahead
		if (i == j) // chosen rectangle is already is tallest
		  toplace = i;
		else
		{
		  // perform Look-Ahead
		  // Look-Ahead is performed keeping track of density of emerging solution
		  // AE - sumarea of empty space after placing tallest object
		  // AM - sumarea of other free rectangles
		  tempH = std::max(H, hsk[indniche] + dynlist[j].height);
		  AE = W * (tempH - hsk[indniche]);
		  for (int k = 0; k < sol.size(); k++)
			AE -= sol[k][2] * (std::max(hsk[indniche], sol[k][1] + sol[k][3]) - std::max(hsk[indniche], sol[k][1]));
		  AM = 0;
		  for (int k = 0; k < dynlist.size(); k++)
			if (k != j)
			  AM += dynlist[k].area();
		  if (AE > AM)
			toplace = j;
		  else
		  {
			tempH = std::max(H, hsk[indniche] + dynlist[i].height);
			AE = W * (tempH - hsk[indniche]);
			for (int k = 0; k < sol.size(); k++)
			  AE -= sol[k][2] * (std::max(hsk[indniche], sol[k][1] + sol[k][3]) - std::max(hsk[indniche], sol[k][1]));
			AM = 0;
			for (int k = 0; k < dynlist.size(); k++)
			  if (k != i)
				AM += dynlist[k].area();
			if (AE > AM)
			  toplace = j;
			else
			  toplace = i;
		  }
		}

		// placing toplace object
		// position == 0 - object is aligned to the left
		// position == 1 - object fits perfectly in niche
		// position == 2 - object is aligned to the right
		sumareas += dynlist[toplace].area();
		int h = dynlist[toplace].height + hsk[indniche];
		H = std::max(H, h);
		int x = accumulate(wsk.begin(), wsk.begin() + indniche, 0);
		int y = hsk[indniche];
		if (dynlist[toplace].width == wsk[indniche])
		  position = 1;
		else if (indniche == hsk.size() - 1)
		{
		  if ((indniche == 0) || (h == hsk[indniche - 1]))
			position = 0;
		  else
			position = 2;
		}
		else if (indniche == 0)
		{
		  if (h == hsk[indniche + 1])
			position = 2;
		  else
			position = 0;
		}
		else if (hsk[indniche - 1] == hsk[indniche + 1])
		{
		  if (x + x + wsk[indniche] > W)
			position = 2;
		  else
			position = 0;
		}
		else
		{
		  if (h == hsk[indniche - 1])
			position = 0;
		  else if (h == hsk[indniche + 1])
			position = 2;
		  else if (hsk[indniche - 1] > hsk[indniche + 1])
			position = 0;
		  else
			position = 2;
		}

		switch (position)
		{
		case 0:
		  hsk.insert(hsk.begin() + indniche, h);
		  wsk[indniche] -= dynlist[toplace].width;
		  wsk.insert(wsk.begin() + indniche, dynlist[toplace].width);
		  break;
		case 1:
		  hsk[indniche] = h;
		  break;
		case 2:
		  x = x + wsk[indniche] - dynlist[toplace].width;
		  hsk.insert(hsk.begin() + indniche + 1, h);
		  wsk[indniche] -= dynlist[toplace].width;
		  wsk.insert(wsk.begin() + indniche + 1, dynlist[toplace].width);
		  break;
		}
		int k = std::max(0, indniche - 1);
		j = std::min(indniche + 2, (int)wsk.size() - 1);
		while (k < j)
		{
		  if (hsk[j - 1] == hsk[j])
		  {
			hsk.erase(hsk.begin() + j);
			wsk[j - 1] += wsk[j];
			wsk.erase(wsk.begin() + j);
		  }
		  j--;
		}
		sol.push_back({ x, y, dynlist[toplace].width, dynlist[toplace].height });
		dynlist.erase(dynlist.begin() + toplace);
	  }
	  else
		skyline_remove_niche(wsk, hsk, indniche, W);
	}

	// update UB
	if (!UB || UB > H)
	{
	  UB = H;
	  bestsol = sol;
	}
	// LB - valid Lower Bound. If it's reached, then PBF can't do better
	if ((LB) && (UB <= LB))
	{
	  return bestsol;
	}
  }
  return bestsol;
}
Solution ExactSolution(const Problem& p, int& LB, int& UB, const unsigned int time_sec)
{
  /*
  * Formulates problem in it's pure form in CPLEX language, then solves it
  *	  using CPLEX
  * Because the author has CPLEX Community Edition, solves only n <= 12
  *	  where n - number of rectangles
  * Sometimes solves wrong because of CPLEX's bug (some constraints are violated)
  *	  Occures with about 0.05 frequency or lower
  * Very useful in Prepack reduction along with PBF
  */
  if (!p.list.size())
  {
	UB = 0;
	for (int ii = 0; ii < p.fixed.size(); ii++)
	  if (UB < p.fixed[ii][1] + p.fixed[ii][3])
		UB = p.fixed[ii][1] + p.fixed[ii][3];
	return p.fixed;
  }
#ifdef DEBUG
  assert(ProblemCorrectness(p));
#endif
  int i = 0, j;
  Solution sol;
  IloEnv env;
  try
  {
	IloModel model(env); // model with all constraints
	// variables: x and y coords of rectangles' left-bottom corner
	IloIntVarArray x(env), y(env);
	// Height to minimize
	IloIntVar H(env);

	model.add(IloMinimize(env, H));
	for (const auto& rec : p.listwidth)
	{
	  // adding variables with ranges
	  x.add(IloIntVar(env, 0, p.Width - rec.width));
	  y.add(IloIntVar(env));
	  model.add(0 <= y[i]);
	  model.add(y[i] + rec.height <= H);
	  i++;
	}
	// add all fixed items asoociated constraints
	for (i = 0; i < p.fixed.size(); i++)
	{
	  for (j = 0; j < p.listwidth.size(); j++)
		model.add(p.fixed[i][0] >= x[j] + p.listwidth[j].width || x[j] >= p.fixed[i][0] + p.fixed[i][2] ||
		  p.fixed[i][1] >= y[j] + p.listwidth[j].height || y[j] >= p.fixed[i][1] + p.fixed[i][3]);
	}
	// constraints between free rectangles
	for (i = 0; i < p.listwidth.size() - 1; i++)
	{
	  for (j = i + 1; j < p.listwidth.size(); j++)
	  {
		model.add(x[i] >= x[j] + p.listwidth[j].width || x[j] >= x[i] + p.listwidth[i].width ||
		  y[i] >= y[j] + p.listwidth[j].height || y[j] >= y[i] + p.listwidth[i].height);
	  }
	}
	for (i = 0; i < p.listwidth.size() - 1; i++)
	{
	  for (j = i + 1; j < p.listwidth.size(); j++)
	  {
		model.add(x[i] >= x[j] + p.listwidth[j].width || x[j] >= x[i] + p.listwidth[i].width ||
		  y[i] >= y[j] + p.listwidth[j].height || y[j] >= y[i] + p.listwidth[i].height);
	  }
	}

	// solver
	IloCplex solver(model);
	solver.setParam(IloCplex::TiLim, time_sec);
	solver.setOut(env.getNullStream());
	solver.solve();

	sol = p.fixed;

	// solution building
	for (int i = 0; i < p.listwidth.size(); i++)
	  sol.push_back({ (int)solver.getValue(x[i]), (int)solver.getValue(y[i]), p.listwidth[i].width, p.listwidth[i].height });
	// getObjValue returns best Upper Bound, getBestObjValue - best Lower Bound
	UB = solver.getObjValue();
	LB = solver.getBestObjValue();
  }
  catch (IloException& e)
  {
#ifdef DEBUG
	std::cerr << "Concert exception caught: " << e << std::endl;
#endif
  }
  catch (...)
  {
#ifdef DEBUG
	std::cerr << "Unknown exception caught" << std::endl;
#endif
  }
  return sol;
}
Solution PackSequence(const Problem& p, int& H, const bool accurate)
{
  /*
  * pack given rectangle sequence, stored in p.list
  * if accurate == true - algorithm nullifies skyline's flaw
  *	  of forgetting empty spaces for the cost of time
  */
  if (!p.list.size())
  {
	H = 0;
	for (int ii = 0; ii < p.fixed.size(); ii++)
	  if (H < p.fixed[ii][1] + p.fixed[ii][3])
		H = p.fixed[ii][1] + p.fixed[ii][3];
	return p.fixed;
  }
#ifdef DEBUG
  assert(ProblemCorrectness(p));
#endif
  Solution sol = p.fixed;
  // flying - rectangles that are not consumed into skyline
  //	(becuase they will leave empty space)
  Solution flying = p.fixed;
  std::vector<int> wsk{ p.Width }, hsk{ 0 }, wskres, hskres;
  int i, indniche;
  item_t candidat;
  bool goodplace;

  sort(flying.begin(), flying.end(), comprevsol);
  H = 0;
  for (const auto& item : flying)
	if (H < item[1] + item[3])
	  H = item[1] + item[3];
  while (flying.size() && flying.back()[1] == 0)
  {
	skyline_add_item(wsk, hsk, flying.back(), p.Width);
	flying.pop_back();
  }

  for (const auto& rec : p.list)
  {
	wskres = wsk; hskres = hsk;
	goodplace = false;
	while (!goodplace)
	{
	  indniche = min_element(hsk.begin(), hsk.end()) - hsk.begin();
	  while (wsk[indniche] < rec.width)
	  {
		skyline_remove_niche(wsk, hsk, indniche, p.Width);
		indniche = min_element(hsk.begin(), hsk.end()) - hsk.begin();
	  }
	  candidat = { accumulate(wsk.begin(), wsk.begin() + indniche, 0), hsk[indniche], rec.width, rec.height };
	  goodplace = true;

	  for (i = flying.size() - 1; i >= 0; i--)
		if (!dont_interfere(flying[i], candidat))
		{
		  goodplace = false;
		  skyline_add_item(wsk, hsk, flying[i], p.Width);
		  break;
		}
	}

	wsk = wskres; hsk = hskres;
	sol.push_back(candidat);
	if (H < candidat[1] + candidat[3])
	  H = candidat[1] + candidat[3];
	if (accurate)
	{
	  if (good_to_add_to_skyline(wsk, hsk, candidat))
		skyline_add_item(wsk, hsk, candidat, p.Width);
	  else
		flying.push_back(candidat);
	}
	else
	{
	  skyline_add_item(wsk, hsk, candidat, p.Width);
	  i = 0;
	  while (i < flying.size())
	  {
		if (good_to_add_to_skyline(wsk, hsk, flying[i]))
		{
		  skyline_add_item(wsk, hsk, flying[i], p.Width);
		  flying.erase(flying.begin() + i);
		}
		else
		  i++;
	  }
	}
  }
  return sol;
}
Solution HeuristicPacking(Problem& p, int& H, const std::array<double, 8> eval, const bool accurate)
{
  /*
  * generate packing Sequence of rectangles using eval
  *	  (analogue to placement_criteria)
  * if accurate == true - algorithm nullifies skyline's flaw
  *	  of forgetting empty spaces for the cost of time
  */
  if (!p.list.size())
  {
	H = 0;
	for (int ii = 0; ii < p.fixed.size(); ii++)
	  if (H < p.fixed[ii][1] + p.fixed[ii][3])
		H = p.fixed[ii][1] + p.fixed[ii][3];
	return p.fixed;
  }
  std::vector<Rectangle> sequence;
  Solution sol = p.fixed;
  // flying - rectangles that are not consumed into skyline
  //	(becuase they will leave empty space)
  Solution flying = p.fixed, items;
  std::vector<int> wskyline{ p.Width }, hskyline{ 0 }, wsk, hsk;
  std::vector<double> value;
  std::vector<std::array<double, 8>> values;
  int i, j, k, indniche, imax, xln, xrn, perimeter, hmax, wmax, smax, pmax, goodarea, initarea, currarea, randmax;
  bool goodplace, rand = !accumulate(eval.begin() + 1, eval.end(), 0);
  std::random_device rd;
  std::default_random_engine rng{ rd() };

  sort(flying.begin(), flying.end(), comprevsol);
  H = 0;
  for (const auto& item : flying)
	if (H < item[1] + item[3])
	  H = item[1] + item[3];
  while (flying.size() && flying.back()[1] == 0)
  {
	skyline_add_item(wskyline, hskyline, flying.back(), p.Width);
	flying.pop_back();
  }

  while (p.listwidth.size())
  {
	// choose best rectangle
	values.assign(p.listwidth.size(), { 0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0 });
	value.assign(p.listwidth.size(), 0.0);
	items.assign(p.listwidth.size(), { 0,0,0,0 });
	initarea = 0;
	wmax = p.listwidth[0].width;
	hmax = pmax = smax = 0;
	for (i = 0; i < wskyline.size(); i++)
	  initarea += wskyline[i] * hskyline[i];

	for (i = 0; i < p.listwidth.size(); i++)
	{
	  wsk = wskyline; hsk = hskyline;
	  goodplace = false;
	  goodarea = p.listwidth[i].area();
	  while (!goodplace)
	  {
		indniche = min_element(hsk.begin(), hsk.end()) - hsk.begin();
		while (wsk[indniche] < p.listwidth[i].width)
		{
		  skyline_remove_niche(wsk, hsk, indniche, p.Width);
		  indniche = min_element(hsk.begin(), hsk.end()) - hsk.begin();
		}
		items[i] = { accumulate(wsk.begin(), wsk.begin() + indniche, 0), hsk[indniche], p.listwidth[i].width, p.listwidth[i].height };
		goodplace = true;
		perimeter = 0;
		for (j = flying.size() - 1; j >= 0; j--)
		  if (!dont_interfere(flying[j], items[i], perimeter))
		  {
			goodplace = false;
			skyline_add_item(wsk, hsk, flying[j], p.Width);
			goodarea += flying[j][2] * flying[j][3];
			break;
		  }
	  }
	  currarea = 0;
	  for (j = 0; j < wsk.size(); j++)
		currarea += wsk[j] * hsk[j];
	  values[i][0] = (double)goodarea / (currarea - initarea);
	  if (values[i][0] > 1.0)
		values[i][0] = 0.9 + rng() / 85899345920.0;
	  if (p.listwidth[i].width == wsk[indniche])
	  {
		values[i][1] = 1.0;
		if (indniche == wsk.size() - 1)
		{
		  values[i][3] = 1.0;
		  perimeter += p.listwidth[i].height;
		}
		else
		{
		  if (p.listwidth[i].height == hsk[indniche + 1] - hsk[indniche])
			values[i][3] = 1.0;
		  else
			values[i][3] = 0.0;
		  perimeter += std::min(p.listwidth[i].height, hsk[indniche + 1] - hsk[indniche]);
		}
	  }
	  perimeter += p.listwidth[i].width;
	  xln = accumulate(wsk.begin(), wsk.begin() + indniche, 0);
	  xrn = xln + wsk[indniche];
	  if (indniche == 0)
	  {
		values[i][2] = 1.0;
		perimeter += p.listwidth[i].height;
	  }
	  else
	  {
		if (p.listwidth[i].height == hsk[indniche - 1] - hsk[indniche])
		  values[i][2] = 1.0;
		else
		  values[i][2] = 0.0;
		perimeter += std::min(p.listwidth[i].height, hsk[indniche - 1] - hsk[indniche]);
	  }
	  value[i] = (rand ? 1.0 : eval[0]) * values[i][0] + eval[1] * values[i][1] + eval[2] * values[i][2] + eval[3] * values[i][3];
	  values[i][4] = p.listwidth[i].width;
	  values[i][5] = p.listwidth[i].height;
	  values[i][6] = perimeter;
	  values[i][7] = p.listwidth[i].area();
	  if (hmax < p.listwidth[i].height)
		hmax = p.listwidth[i].height;
	  if (pmax < perimeter)
		pmax = perimeter;
	  if (smax < p.listwidth[i].area())
		smax = p.listwidth[i].area();
	}

	imax = 0;
	randmax = 0;
	for (i = 0; i < p.listwidth.size(); i++)
	{
	  value[i] += values[i][4] * eval[4] / wmax + values[i][5] * eval[5] / hmax + values[i][6] * eval[6] / pmax + values[i][7] * eval[7] / smax;
	  if (values[imax] < values[i])
		imax = i;
	  if (rand && value[i] > 0.99)
		randmax++;
	}
	if (rand)
	{
	  if (!randmax)
		imax = rng() % p.listwidth.size();
	  else
	  {
		k = rng() % randmax;
		imax = 0;
		i = -1;
		while (i != k)
		{
		  if (value[imax] > 0.99)
			i++;
		  imax++;
		}
		imax--;
	  }
	}
#ifdef DEBUG
	assert(imax < p.listwidth.size());
#endif
	sequence.push_back(p.listwidth[imax]);
	if (H < items[imax][1] + items[imax][3])
	  H = items[imax][1] + items[imax][3];
	sol.push_back(items[imax]);
	if (accurate)
	{
	  if (good_to_add_to_skyline(wskyline, hskyline, items[imax]))
		skyline_add_item(wskyline, hskyline, items[imax], p.Width);
	  else
		flying.push_back(items[imax]);
	}
	else
	{
	  skyline_add_item(wskyline, hskyline, items[imax], p.Width);
	  k = 0;
	  while (k < flying.size())
		if (good_to_add_to_skyline(wskyline, hskyline, flying[k]))
		{
		  skyline_add_item(wskyline, hskyline, flying[k], p.Width);
		  flying.erase(flying.begin() + k);
		}
		else
		  k++;
	}
	p.listwidth.erase(p.listwidth.begin() + imax);
  }
  p = Problem(sequence, p.Width, p.fixed);
  return sol;
}
Solution FindHeuristicSequence(Problem& p, int& H, int LB, const double time_sec)
{
  /*
  * Find best eval to generate packing Sequence for HeuristicPacking
  */
  if (!p.list.size())
  {
	H = 0;
	for (int ii = 0; ii < p.fixed.size(); ii++)
	  if (H < p.fixed[ii][1] + p.fixed[ii][3])
		H = p.fixed[ii][1] + p.fixed[ii][3];
	return p.fixed;
  }
#ifdef DEBUG
  assert(ProblemCorrectness(p));
#endif
  Problem pp{ p };
  std::random_device rd;
  std::default_random_engine rng{ rd() };
  std::array<double, 8> eval;
  Solution bestsol, sol;
  int h;
  Timer timer(time_sec);

  if (!LB)
	LB = LB_easy(p);
  bestsol = HeuristicPacking(p, H);
  while (timer.run() && LB < H)
  {
	for (auto& num : eval)
	  num = exp(10.0 * rng() / RNG_MAX);
	sol = HeuristicPacking(pp, h, eval);
	if (H > h)
	{
	  H = h;
	  p = pp;
	  bestsol = sol;
	}
  }
  return bestsol;
}
Solution LocalSearch(Problem& p, int& H, int LB, const double time_sec)
{
  /*
  * Given a pack Sequence. By swapping to rectangles in it LocalSearch
  *	  finds better sequence. All swaps are checked
  */
  if (!p.list.size())
  {
	H = 0;
	for (int ii = 0; ii < p.fixed.size(); ii++)
	  if (H < p.fixed[ii][1] + p.fixed[ii][3])
		H = p.fixed[ii][1] + p.fixed[ii][3];
	return p.fixed;
  }
  Problem pp{ p };
  Solution bestsol, sol;
  int h;
  unsigned int i = 0, j = 1, n = p.list.size();
  Timer timer(time_sec);

  bestsol = PackSequence(p, H);
  std::swap(pp.list[0], pp.list[1]);
  sol = PackSequence(pp, h);
  if (H > h)
  {
	p = pp;
	H = h;
	bestsol = sol;
  }
  if (!LB)
	LB = LB_easy(p);
  while (timer.run() && LB < H)
  {
	std::swap(pp.list[i], pp.list[j]);
	if (j == n - 1)
	{
	  if (i == n - 2)
		break;
	  i++;
	  j = i + 1;
	}
	else
	  j++;
	std::swap(pp.list[i], pp.list[j]);
	sol = PackSequence(pp, h);
	if (H > h)
	{
	  p = pp;
	  H = h;
	  bestsol = sol;
	}
  }
  return bestsol;
}
Solution SimulatedAnnealing(Problem& p, int& H, int LB, const double T0,
  const double alpha, const int L, const double time_sec, const double smarttime)
{
  /*
  * Simulated Annealing is a metaheuristic. Finds best pack Sequence.
  * Search is implemented by choosing to random rectangles to swap.
  * If swap gives better solution, then swap is performed to current Sequence
  * Else: swap is performed with determined probability.
  *	  The worse swap is - the lower probability
  * It is done to avoid local minimums
  * Probability = exp((H - newH) / T), where T - temperature
  * Temperature exponentially decreases to zero:
  *	  Every L swap tries, T decreases by alpha: T *= alpha
  * smarttime - time to wait for improving the solution
  * If during smarttime solution was not improved - algorithm stops
  */
  if (!p.list.size())
  {
	H = 0;
	for (int ii = 0; ii < p.fixed.size(); ii++)
	  if (H < p.fixed[ii][1] + p.fixed[ii][3])
		H = p.fixed[ii][1] + p.fixed[ii][3];
	return p.fixed;
  }
#ifdef DEBUG
  assert(smarttime >= 0.0 && time_sec > smarttime);
#endif
  Timer timer(time_sec), smarthold(smarttime);
  int h, nexth, i, j, k, n = p.list.size();
  double UBupdate = time_sec;
  bool work = true;
  Problem pp{ p };
  Solution sol, bestsol, nextsol;
  std::random_device rd;
  std::default_random_engine rng{ rd() };
  if (!LB)
	LB = LB_easy(p);
  bestsol = PackSequence(p, H);
  h = H; sol = bestsol;
  double T = T0;
  Timer update;
  while (work && LB < H)
  {
	for (i = 0; i < L; i++)
	{
	  j = rng() % n; k = rng() % n;
	  while (j == k)
	  {
		j = rng() % n; k = rng() % n;
	  }
	  std::swap(pp.list[j], pp.list[k]);
	  nextsol = PackSequence(pp, nexth);
	  if (H > nexth)
	  {
		H = h = nexth;
		bestsol = sol = nextsol;
		UBupdate = update.last();
		update = Timer();
	  }
	  else if (exp((h - nexth) / T) >= rng() / RNG_MAX)
	  {
		h = nexth;
		sol = nextsol;
	  }
	  else
		std::swap(pp.list[j], pp.list[k]);
	  if (!timer.run() || !smarthold.run() && update.last() > 2 * UBupdate)
	  {
		work = false;
		break;
	  }
	}
	T *= alpha;
  }
  return bestsol;
}
Solution IntelligentSimulatedAnnealing(Problem& p, int& H, const int LB, const double time_sec)
{
  /*
  * perform FindHeuristicSequence, LocalSearch, SimulatedAnnealing several times
  *	  because SA often fall for local minimum
  * time given to these algorithms to work need to be optimized (currently 0.1)
  */
  Timer timer(time_sec);
  Solution sol, bestsol;
  Problem pp;
  int h;
  FindHeuristicSequence(p, H, LB, 0.1);
  LocalSearch(p, H, LB, 0.1);
  bestsol = SimulatedAnnealing(p, H, LB, 0.5, 0.93, 50, 0.5, 0.1);
  while (timer.run() && LB < H)
  {
	pp = p;
	FindHeuristicSequence(pp, h, LB, 0.1);
	LocalSearch(pp, h, LB, 0.1);
	sol = SimulatedAnnealing(pp, h, LB, 0.5, 0.93, 50, 0.5, 0.1);
	if (H > h)
	{
	  H = h;
	  p = pp;
	  bestsol = sol;
	}
  }
  return bestsol;
}

void Thread_GenProblem(std::vector<Problem>& pvec, std::vector<int>& Hvec,
  int ibegin, int iend, int n)
{
  /*
  * generate in pvec vector of (iend-ibegin) problems
  */
  Problem p;
  int H, LB = n * 10, i;
  for (i = ibegin; i < iend; i++)
  {
	GenerateProblem(p, n * 10, n * 10, n);
	FindHeuristicSequence(p, H, LB, 1);
	LocalSearch(p, H, LB, 1);
	pvec[i] = p;
	Hvec[i] = H;
  }
  return;
}
void Thread_SimAnneal(const std::vector<Problem>& pvec, const std::vector<int>& Hvec,
  std::vector<int>& Hsum, int index, int L, double t, int N, int LB)
{
  /*
  * perform SA for given L
  */
  int HS = 0, i, H;
  Problem p;
  for (i = 0; i < N; i++)
  {
	p = pvec[i];
	SimulatedAnnealing(p, H, LB, 0.5, 0.93, L, t);
	HS += (Hvec[i] - H ? 1 : 0);
  }
  Hsum[index] = HS;
  return;
}
void Test_Sim_Anneal(int RectNum, int ProbNum, double Time, int Lstart, int Lend, int Lstep, bool Multiply)
{
  /*
  * test a range of L's using multithreading. Results are printed on the screen
  */
  int PPT = (ProbNum - 1) / MAX_THREADS + 1;
  std::vector<Problem> pvec;
  std::vector<int> Hvec;
  int LB = RectNum * 10, L, index = 0, numL = 0;
  std::vector<int> Hsum;
  Solution sol;
  std::vector<std::thread> threads;
  for (L = Lstart; L <= Lend; (Multiply ? L *= Lstep : L += Lstep))
  {
	Hsum.push_back(0);
	numL++;
  }
  pvec.assign(ProbNum, Problem());
  Hvec.assign(ProbNum, 0);

  int ibegin = 0, iend = PPT;
  while (ibegin < ProbNum)
  {
	threads.push_back(std::thread(Thread_GenProblem, ref(pvec), ref(Hvec), ibegin, iend, RectNum));
	ibegin = iend;
	iend = std::min(iend + PPT, ProbNum);
  }
  for (auto& th : threads)
	th.join();
  threads.clear();

  std::cout << "Problems are made, n = " << RectNum << ", t = " << Time << ", N = " << ProbNum << std::endl << std::endl;
  std::cout << "Wait for " << ProbNum * Time << " seconds (" << numL << " cases)" << std::endl << std::endl << std::endl;

  for (L = Lstart; L <= Lend; (Multiply ? L *= Lstep : L += Lstep))
  {
	threads.push_back(std::thread(Thread_SimAnneal, pvec, Hvec, ref(Hsum), index, L, Time, ProbNum, LB));
	index++;
  }
  for (auto& th : threads)
	th.join();
  index = 0;
  for (L = Lstart; L <= Lend; (Multiply ? L *= Lstep : L += Lstep))
  {
	std::cout << "For L = " << L << "\tHsum = " << Hsum[index] << std::endl << std::endl;
	index++;
  }

  std::cout << "\a";
}

void main()
{
  Problem p;
  Solution optsol, pbfsol, extsol, isasol;
  int W = 100, optH = 100, LBS, LBH, LBDFF, n = 12, LB, UB;
  optsol = GenerateProblem(p, W, optH, n, 0, false, 2, 100, 0, 0);
  Draw(optsol, true, "optimal_solution");
  LBS = LB_Area(p);
  LBH = LB_H(p);
  LBDFF = LB_DFF(p);
  std::cout << "Continuous   Lower Bound: " << LBS << "\n";
  std::cout << "Height Based Lower Bound: " << LBS << "\n";
  std::cout << "DFF Based    Lower Bound: " << LBS << "\n";
  extsol = ExactSolution(p, LB, UB, 10);
  std::cout << "Exact Solution Lower Bound: " << LB << "\n";
  std::cout << "Exact Solution Upper Bound: " << UB << "\n";
  PrintSolution(extsol, "exactsolution");

  n = 50;
  optsol = GenerateProblem(p, W, optH, n, 0, false, 2, 100, 0, 0);
  //Draw(optsol, true, "optimal_solution");
  LBS = LB_Area(p);
  LBH = LB_H(p);
  LBDFF = LB_DFF(p);
  std::cout << "\nContinuous   Lower Bound: " << LBS << "\n";
  std::cout << "Height Based Lower Bound: " << LBS << "\n";
  std::cout << "DFF Based    Lower Bound: " << LBS << "\n";
  pbfsol = PBF(p, UB, LBS, 10);
  std::cout << "Priority Best Fit solution: " << UB << "\n";
  PrintSolution(pbfsol, "pbfsolution");

  isasol = IntelligentSimulatedAnnealing(p, UB, LBS, 20);
  std::cout << "Intelligent Simulated Annealing solution: " << UB << "\n";
  PrintSolution(isasol, "isasolution");
}