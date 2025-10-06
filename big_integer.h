#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H
#define BIG_INTEGER_DIVISION_IMPLEMENTED
#include <vector>
#include <cstdint>
#include <string>
#include <stdexcept>

class BigIntegerOverflow : public std::runtime_error {
public:
	BigIntegerOverflow();
};

class BigIntegerDivisionByZero : public std::runtime_error {
public:
	BigIntegerDivisionByZero();
};

class BigInteger {
public:
	const uint64_t p = 1'000'000;

	std::vector<uint64_t> number;
	bool negative_flag;

	BigInteger();
	BigInteger(const int64_t integer);  // NOLINT
	BigInteger(const char* c_string);  // NOLINT
	BigInteger(const int& integer);  // NOLINT
	BigInteger(const BigInteger& big_integer);

	bool IsNegative() const;

	BigInteger& operator=(const BigInteger& other);

	BigInteger operator+() const;
	BigInteger operator-() const;

	BigInteger operator+(const BigInteger& other) const;
	BigInteger operator-(const BigInteger& other) const;
	BigInteger operator*(const BigInteger& other) const;
	BigInteger operator/(const BigInteger& other) const;
	BigInteger operator%(const BigInteger& other) const;

	BigInteger& operator+=(const BigInteger& other);
	BigInteger& operator-=(const BigInteger& other);
	BigInteger& operator*=(const BigInteger& other);
	BigInteger& operator/=(const BigInteger& other);
	BigInteger& operator%=(const BigInteger& other);

	BigInteger operator+(const int64_t& other) const;
	BigInteger operator-(const int64_t& other) const;
	BigInteger operator*(const int64_t& other) const;
	BigInteger operator/(const int64_t& other) const;
	BigInteger operator%(const int64_t& other) const;

	BigInteger& operator+=(const int64_t& other);
	BigInteger& operator-=(const int64_t& other);
	BigInteger& operator*=(const int64_t& other);
	BigInteger& operator/=(const int64_t& other);
	BigInteger& operator%=(const int64_t& other);

	BigInteger& operator++();
	BigInteger& operator--();

	BigInteger operator++(int);
	BigInteger operator--(int);

	explicit operator bool() const;

	bool operator==(const BigInteger& other) const;
	bool operator>(const BigInteger& other) const;
	bool operator<(const BigInteger& other) const;
	bool operator>=(const BigInteger& other) const;
	bool operator<=(const BigInteger& other) const;
	bool operator!=(const BigInteger& other) const;
};

std::istream& operator>>(std::istream& is, BigInteger& big_integer);
std::ostream& operator<<(std::ostream& os, const BigInteger& big_integer);

BigInteger operator+(const int64_t& first, const BigInteger& second);
BigInteger operator-(const int64_t& first, const BigInteger& second);
BigInteger operator*(const int64_t& first, const BigInteger& second);
BigInteger operator/(const int64_t& first, const BigInteger& second);
BigInteger operator%(const int64_t& first, const BigInteger& second);

void KillLeadingZeros(BigInteger& big_integer);
#endif