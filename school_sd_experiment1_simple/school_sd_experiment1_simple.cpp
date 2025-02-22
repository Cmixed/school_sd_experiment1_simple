import std;

bool constexpr  is_debug{ false };
//x^3 -7x^2 +15x -9 = 0;
//1 4	

/**
 * @brief 多项式元素结构体 1: 系数 2: 次数
 */
class Element
{
public:
	int ratio{}, power{};

	Element() = default;

	[[nodiscard]] double value(const double val) const
	{
		if ((this->ratio) == 0) return 0;
		if (val == 0 && (this->power) != 0) return 0;
		if ((this->power) == 0) return (this->ratio);
		return (this->ratio) * std::pow(val, this->power);
	}

	auto operator<=>(const Element other) const
	{
		return this->power <=> other.power;
	}
	double operator*(const double val) const
	{
		return this->value(val);
	}
	friend double operator*(const double val, const Element element)
	{
		return element.value(val);
	}

};	

std::vector<std::string> extract_terms(const std::string& equation);
std::vector<Element> parse_terms(const std::vector<std::string>& terms);
std::vector<double> solve_polynomial(const std::vector<Element>& polynomial,
	const std::pair<double, double>& x_range = std::make_pair(0.0, 10.0));

void test_equation(const std::vector<std::string>& terms, const std::string& expected);
void test_element(const std::vector<Element> polynomial);


int main() {

    std::string equation = "3X^4 +2x^2+3X -4= 5";
	std::pair<double, double> x_range{ 0,0 };

	std::println(R"(Input a polynomial. Example: 3X^4+2x^2+3X-4=5 (q to quit )");
	do {
		if (static bool is_correct{ true }; 
			!is_correct) {
			std::println("Input Error! Try Again!");
		} else {
			is_correct = false;
		}
		equation.clear();
		std::getline(std::cin, equation);
		if (equation[0] == 'q') {
			return 0;
		}
	} while (!equation.contains('x') && !equation.contains('X'));

	std::println(R"(Input x range. Example: 2 3 (' ' to separate )");
	while (!(std::cin >> x_range.first >> x_range.second)) {
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::println("Input Error! Try Again!");
	}

	if constexpr (is_debug) 
		std::println("x_range is: {} {}", x_range.first, x_range.second);

    std::vector<std::string> const terms = extract_terms(equation);
	if constexpr (is_debug) test_equation(terms, equation);

	std::vector<Element> const polynomial{parse_terms(terms)};
	if constexpr (is_debug) test_element(polynomial);

	std::println("In range [{} {}]", x_range.first, x_range.second);
	if (const auto answer{ solve_polynomial(polynomial, x_range) };
		!answer.empty()) {
		std::print("Answer is : ");
		for (auto val : answer) {
			std::print("{} ", val);
		}
		std::println();
	} else {
		std::println("No Answer");
	}

    return 0;
}

/**
 * @brief 提取方程中的各个项
 * @param equation 
 * @return 
 */
std::vector<std::string> extract_terms(const std::string& equation) {

	std::string express{ equation };
	
	std::ranges::transform(express, express.begin(), 
		[](const unsigned char c) {return std::tolower(c); });

    std::vector<std::string> terms;
    std::regex const term_regex(R"(\s*([+-]?(\d*\.?\d*)x(\^\d+)?|\s*[+-]?(\d*\.?\d*)x|\s*[+-]?(\d*\.?\d+))\s*)");

    std::sregex_token_iterator it(express.begin(), express.end(), term_regex, 0);
    std::sregex_token_iterator const end;

    while (it != end) {
        terms.push_back(it->str());
        ++it;
    }

    return terms;
}

/**
 * @brief 解析各项元素,提取成统一多项式
 * @param terms 
 * @return 
 */
std::vector<Element> parse_terms(const std::vector<std::string>& terms)
{
	std::vector<Element> polynomial;
	for (const auto& term : terms) {
		Element element;
		if (term.contains('x')) {
			if (auto const pos = term.find('x');
				term.contains('^')) {
				element.power = std::stoi(term.substr(pos + 2));
				if (term[0] == 'x') {
					element.ratio = 1;
				} else {
					element.ratio = std::stoi(term.substr(0, pos));
				}
			} else {
				element.power = 1;
				if (term[0] == 'x') {
					element.ratio = 1;
				} else {
					element.ratio = std::stoi(term.substr(0, pos));
				}
			}
		}
		else {
			element.power = 0;
			element.ratio = std::stoi(term);
		}
		polynomial.push_back(element);
	}

	if (!polynomial.empty()) {
		polynomial.back().ratio = -polynomial.back().ratio;
	}

	return polynomial;
}

/**
 * @brief 根据范围计算出解
 * @param polynomial 
 * @param x_range default {0, 0} 
 * @return 
 */
std::vector<double> solve_polynomial(const std::vector<Element>& polynomial, 
                                     const std::pair<double, double>& x_range)
{
	std::vector<double> solve_stack;
	double result{}, solve{x_range.first};

	while (solve <= x_range.second) {
		for (auto const ele : polynomial) {
			result += (ele * solve);
		}
		if constexpr (is_debug) std::print("x is {}, result is {}\n", solve, result);
		if (result == 0) {
			if constexpr (is_debug) std::print("Solve {} added!\n", result);
			solve_stack.push_back(solve);
		}
		result = 0;
		solve += 1;
	}

	//if constexpr (is_debug) return std::vector<double>{1.0};
	return solve_stack;
}

// 测试函数
void test_equation(const std::vector<std::string>& terms, const std::string& expected)
{
	std::print("Extracted terms for equation \"{}\":\n", expected);
    for (const auto& term : terms) {
		static int line_number{ 0 }; ++line_number;
		std::print("E{}: {} ", line_number, term);
    }
	std::println();
}


void test_element(const std::vector<Element> polynomial)
{
	std::print("Total get:\n");
	for (const auto& element : polynomial)
	{
		static int line_number{ 0 }; ++line_number;
		std::print("{}x^{} ", element.ratio, element.power);
	}
	std::println();
}