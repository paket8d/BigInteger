#include <iostream>
#include <algorithm>
#include <cstring>
#include "big_integer.h"

BigIntegerOverflow::BigIntegerOverflow() : std::runtime_error("BigIntegerOverflow") {

}

BigIntegerDivisionByZero::BigIntegerDivisionByZero() : std::runtime_error("BigIntegerDivisionByZero") {

}

BigInteger::BigInteger() : number{}, negative_flag(false) {

}

BigInteger::BigInteger(const int64_t integer) : number{}, negative_flag(false) {
	negative_flag = (integer < 0);
	uint64_t temp = negative_flag ? -integer : integer;
	if (temp == 0) {
		number.emplace_back(0);
	}
	else {
		while (temp != 0) {
			number.emplace_back(temp % p);
			temp /= p;
		}
	}
	KillLeadingZeros(*this);
}

BigInteger::BigInteger(const char* c_string) : number{}, negative_flag(false) {
	if (c_string[0] == '-') {
		negative_flag = true;
	}
	std::string digit;
	for (size_t i = std::strlen(c_string) - 1; i > 0; --i) {
		if (digit.size() == 6) {
			number.emplace_back(std::stoi(digit));
			digit = c_string[i];
		}
		else {
			digit = c_string[i] + digit;
		}
	}
	if (digit.size() == 6) {
		number.emplace_back(std::stoi(digit));
		if (c_string[0] != '-') {
			number.emplace_back(static_cast<int>(c_string[0] - '0'));
		}
	}
	else if (c_string[0] != '-') {
		digit = c_string[0] + digit;
		number.emplace_back(std::stoi(digit));
	}
	else {
		number.emplace_back(std::stoi(digit));
	}
	KillLeadingZeros(*this);
	if (number.size() > 30'000 / 6) {
		throw BigIntegerOverflow{};
	}
}

BigInteger::BigInteger(const int& integer) : BigInteger(static_cast<int64_t>(integer)) {

}

BigInteger::BigInteger(const BigInteger& big_integer) = default;


bool BigInteger::IsNegative() const {
	return negative_flag;
}

BigInteger& BigInteger::operator=(const BigInteger& other) {
	this->negative_flag = other.IsNegative();
	this->number.resize(other.number.size());
	for (size_t i = 0; i < other.number.size(); ++i) {
		this->number[i] = other.number[i];
	}
	if (number.size() > 30'000 / 6) {
		throw BigIntegerOverflow{};
	}
	return *this;
}

BigInteger BigInteger::operator+() const {
	return *this;
}
BigInteger BigInteger::operator-() const {
	BigInteger negative = *this;
	if (*this != BigInteger(0)) {
		negative.negative_flag = !IsNegative();
	}
	return negative;
}

BigInteger BigInteger::operator+(const BigInteger& other) const {
	BigInteger result = BigInteger();
	if (!(IsNegative() ^ other.IsNegative())) {
		result.negative_flag = IsNegative();
		uint64_t adding = 0;
		size_t i = 0;
		for (; i < number.size() && i < other.number.size(); ++i) {
			result.number.emplace_back((number[i] + other.number[i] + adding) % p);
			adding = (number[i] + other.number[i] + adding) / p;
		}
		for (; i < other.number.size(); ++i) {
			result.number.emplace_back((other.number[i] + adding) % p);
			adding = (other.number[i] + adding) / p;
		}
		for (; i < number.size(); ++i) {
			result.number.emplace_back((number[i] + adding) % p);
			adding = (number[i] + adding) / p;
		}
		if (adding != 0) {
			result.number.push_back(adding);
		}
		KillLeadingZeros(result);
		if (number.size() > 30'000 / 6) {
			throw BigIntegerOverflow{};
		}
		return result;
	}

	if (*this >= other && *this >= -other) {
		return *this - (-other);
	}
	if (*this <= other && *this <= -other) {
		return -((-*this) - other);
	}
	if (*this > other && *this < -other) {
		return (-other) - *this;
	}
	return other - (-*this);
}
BigInteger BigInteger::operator-(const BigInteger& other) const {
	BigInteger result = BigInteger();
	if (IsNegative() ^ other.IsNegative()) {
		return *this + (-other);
	}
	if (IsNegative()) {
		return -(-*this - (-other));
	}
	if (*this < other) {
		return -(other - *this);
	}
	uint64_t taking = 0;
	uint64_t took = 0;
	size_t i = 0;
	for (; i < number.size() && i < other.number.size(); ++i) {
		taking = number[i] - took < other.number[i] ? 1 : 0;
		result.number.emplace_back(number[i] + taking * p - other.number[i] - took);
		took = taking;
	}
	taking = 0;
	for (; i < number.size(); ++i) {
		result.number.emplace_back(number[i] + taking * p - took);
		took = taking;
	}
	KillLeadingZeros(result);
	if (number.size() > 30'000 / 6) {
		throw BigIntegerOverflow{};
	}
	return result;
}
BigInteger BigInteger::operator*(const BigInteger& other) const {
	if (*this == BigInteger(0) || other == BigInteger(0)) {
		return BigInteger(0);  // NOLINT
	}
	BigInteger result = BigInteger(0);
	result.number.assign(2 * std::max(number.size(), other.number.size()), 0);
	result.negative_flag = IsNegative() ^ other.IsNegative();
	for (size_t j = 0; j < other.number.size(); ++j) {
		for (size_t i = 0; i < number.size(); ++i) {
			uint64_t temp_result = number[i] * other.number[j];
			result.number[i + j] += temp_result;
		}
		for (size_t k = 0; k < result.number.size() - 1; ++k) {
			result.number[k + 1] += result.number[k] / p;
			result.number[k] %= p;
		}
	}
	KillLeadingZeros(result);
	if (result.number.size() > 30'000 / 6) {
		throw BigIntegerOverflow{};
	}
	return result;
}
BigInteger BigInteger::operator/(const BigInteger& other) const {
	BigInteger numerator = *this;
	BigInteger denominator = other;
	numerator.negative_flag = false;
	denominator.negative_flag = false;

	if (denominator == BigInteger(2)) {
		BigInteger result = numerator;
		for (size_t i = result.number.size(); i > 0; --i) {
			uint64_t digit = result.number[i - 1];
			result.number[i - 1] /= 2;
			if (i > 1) {
				result.number[i - 2] += (digit % 2) * p;
			}
		}
		result.negative_flag = (IsNegative() ^ other.IsNegative());
		KillLeadingZeros(result);
		return result;
	}

	if (other == BigInteger(0)) {
		throw BigIntegerDivisionByZero{};
	}

	BigInteger left(0);
	BigInteger right(numerator + 1);
	while (right - left > BigInteger(1)) {
		BigInteger mid = (right + left) / 2;
		if (mid * denominator > numerator) {
			right = mid;
		}
		else {
			left = mid;
		}
	}
	if (left != BigInteger(0)) {
		left.negative_flag = (IsNegative() ^ other.IsNegative());
	}
	KillLeadingZeros(left);
	return left;
}
BigInteger BigInteger::operator%(const BigInteger& other) const {
	return *this - (*this / other) * other;
}

BigInteger& BigInteger::operator+=(const BigInteger& other) {
	*this = *this + other;
	return *this;
}
BigInteger& BigInteger::operator-=(const BigInteger& other) {
	*this = *this - other;
	return *this;
}
BigInteger& BigInteger::operator*=(const BigInteger& other) {
	*this = *this * other;
	return *this;
}
BigInteger& BigInteger::operator/=(const BigInteger& other) {
	*this = *this / other;
	return *this;
}
BigInteger& BigInteger::operator%=(const BigInteger& other) {
	*this = *this % other;
	return *this;
}

BigInteger BigInteger::operator+(const int64_t& other) const {
	return *this + BigInteger(other);
}
BigInteger BigInteger::operator-(const int64_t& other) const {
	return *this - BigInteger(other);
}
BigInteger BigInteger::operator*(const int64_t& other) const {
	return *this * BigInteger(other);
}
BigInteger BigInteger::operator/(const int64_t& other) const {
	return *this / BigInteger(other);
}
BigInteger BigInteger::operator%(const int64_t& other) const {
	return *this % BigInteger(other);
}

BigInteger& BigInteger::operator+=(const int64_t& other) {
	*this = *this + BigInteger(other);
	return *this;
}
BigInteger& BigInteger::operator-=(const int64_t& other) {
	*this = *this - BigInteger(other);
	return *this;
}
BigInteger& BigInteger::operator*=(const int64_t& other) {
	*this = *this * other;
	return *this;
}
BigInteger& BigInteger::operator/=(const int64_t& other) {
	*this = *this / BigInteger(other);
	return *this;
}
BigInteger& BigInteger::operator%=(const int64_t& other) {
	*this = *this % other;
	return *this;
}

BigInteger& BigInteger::operator++() {
	return *this += 1;
}
BigInteger& BigInteger::operator--() {
	return *this -= 1;
}

BigInteger BigInteger::operator++(int) {
	BigInteger temp = *this;
	*this += static_cast<int64_t>(1);
	return temp;
}
BigInteger BigInteger::operator--(int) {
	BigInteger temp = *this;
	*this -= static_cast<int64_t>(1);
	return temp;
}

BigInteger::operator bool() const {
	return !(*this == BigInteger(0));
}


bool BigInteger::operator==(const BigInteger& other) const {
	if (number.size() != other.number.size() || (IsNegative() ^ other.IsNegative())) {
		return false;
	}
	for (size_t i = 0; i < number.size(); ++i) {
		if (number[i] != other.number[i]) {
			return false;
		}
	}
	return true;
}
bool BigInteger::operator>(const BigInteger& other) const {
	bool abs_greater = false;
	bool abs_equal = true;
	if (number.size() > other.number.size()) {
		abs_greater = true;
		abs_equal = false;
	}
	else if (number.size() < other.number.size()) {
		abs_greater = false;
		abs_equal = false;
	}
	else {
		bool finish = false;
		for (size_t i = number.size() - 1; i > 0 && !finish; --i) {
			if (number[i] > other.number[i]) {
				finish = true;
				abs_greater = true;
				abs_equal = false;
			}
			else if (number[i] < other.number[i]) {
				finish = true;
				abs_greater = false;
				abs_equal = false;
			}
		}
		if (!finish) {
			if (number[0] > other.number[0]) {
				finish = true;
				abs_greater = true;
				abs_equal = false;
			}
			else if (number[0] < other.number[0]) {
				finish = true;
				abs_greater = false;
				abs_equal = false;
			}
		}
	}
	if (!IsNegative() && other.IsNegative()) {
		return true;
	}
	if (IsNegative() && !other.IsNegative()) {
		return false;
	}
	if (IsNegative() && other.IsNegative()) {
		return !abs_greater && !abs_equal;
	}
	return abs_greater;
}
bool BigInteger::operator<(const BigInteger& other) const {
	return !(*this > other || other == *this);
}
bool BigInteger::operator>=(const BigInteger& other) const {
	return (*this > other || *this == other);
}
bool BigInteger::operator<=(const BigInteger& other) const {
	return (*this < other || *this == other);
}
bool BigInteger::operator!=(const BigInteger& other) const {
	return !(*this == other);
}

std::istream& operator>>(std::istream& is, BigInteger& big_integer) {
	std::string input;
	is >> input;
	big_integer = BigInteger(input.c_str());
	return is;
}
std::ostream& operator<<(std::ostream& os, const BigInteger& big_integer) {
	if (big_integer.negative_flag) {
		os << '-';
	}
	bool is_leading_zeros = true;
	for (size_t i = big_integer.number.size() - 1; i > 0; --i) {
		if (is_leading_zeros && big_integer.number[i] == 0) {
			continue;
		}
		if (is_leading_zeros) {
			is_leading_zeros = false;
			os << static_cast<uint64_t>(big_integer.number[i]);
		}
		else {
			std::string output = std::to_string(big_integer.number[i]);
			while (output.size() < 6) {
				output = '0' + output;
			}
			os << output;
		}
	}
	if (is_leading_zeros) {
		is_leading_zeros = false;
		os << static_cast<uint64_t>(big_integer.number[0]);
	}
	else {
		std::string output = std::to_string(big_integer.number[0]);
		while (output.size() < 6) {
			output = '0' + output;
		}
		os << output;
	}
	return os;
}

BigInteger operator+(const int64_t& first, const BigInteger& second) {
	return BigInteger(first) + second;
}
BigInteger operator-(const int64_t& first, const BigInteger& second) {
	return BigInteger(first) - second;
}
BigInteger operator*(const int64_t& first, const BigInteger& second) {
	return BigInteger(first) * second;
}
BigInteger operator/(const int64_t& first, const BigInteger& second) {
	return BigInteger(first) / second;
}
BigInteger operator%(const int64_t& first, const BigInteger& second) {
	return BigInteger(first) % second;
}

void KillLeadingZeros(BigInteger& big_integer) {
	for (size_t i = big_integer.number.size(); i > 0 && big_integer.number.back() == 0; --i) {
		if (i > 1) {
			big_integer.number.pop_back();
		}
	}
}