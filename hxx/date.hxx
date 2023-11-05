#pragma once


#include <chrono>
#include <string>
#include <vector>

namespace BT {
class Date {
public:
  Date(int year, int month, int day);
  Date(const std::string &dateStr);

  int getYear() const;
  int getMonth() const;
  int getDay() const;

  std::string toString() const;
  std::vector<Date> toDates(const Date &endDate) const;
  Date nextDate() const;
  Date previousDate() const;
  bool operator<=(const Date &rhs) const;
  operator std::string() const { return toString(); }
  static std::string nowStr();

private:
  int year;
  int month;
  int day;
};
} // namespace BT
