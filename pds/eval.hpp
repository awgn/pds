#include <cstdint>
#include <set>

namespace pds
{
	inline bool eval(bool n) { return n;}
	inline char eval(char n) { return n;}
	inline unsigned char eval(unsigned char n) { return n;}

	inline short int eval(short int n) { return n; }
	inline short unsigned int eval(short unsigned int n) { return n; }
	inline int eval(int n) { return n; }
	inline unsigned int eval(unsigned int n) { return n; }
	inline long int eval(long int n) { return n; }
	inline unsigned long int eval(unsigned long int n) { return n; }
	inline long long int eval(long long int n) { return n; }
	inline unsigned long long int eval(unsigned long long int n) { return n; }

	inline float eval(float n) { return n; }
	inline double eval(double n) { return n; }
	inline long double eval(long double n) { return n; }
	
	template <typename T>
	auto eval(std::set<T> const &value)
	{
	    return value.size();
	}

	template <typename T>
	auto eval(T const &value)
	{
	    return value.eval();
	}
}
