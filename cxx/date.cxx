#include "date.hxx"
#include <iomanip>
#include <sstream>

namespace BT {

Date::Date(int year, int month, int day) : year(year), month(month), day(day) {}

Date::Date(const std::string &dateStr) {
  std::istringstream ss(dateStr);
  char delimiter;
  ss >> year >> delimiter >> month >> delimiter >> day;
}

int Date::getYear() const { return year; }

int Date::getMonth() const { return month; }

int Date::getDay() const { return day; }

std::string Date::toString() const {
  return std::to_string(year) + "-" + (month < 10 ? "0" : "") +
         std::to_string(month) + "-" + (day < 10 ? "0" : "") +
         std::to_string(day);
}

std::vector<Date> Date::toDates(const Date &endDate) const {
  std::vector<Date> dates;
  Date currentDate = *this;

  while (currentDate <= endDate) {
    dates.push_back(currentDate);
    currentDate = currentDate.nextDate();
  }

  return dates;
}

Date Date::nextDate() const {
  Date result = *this;
  result.day++;

  if (result.day > 31 || (result.month == 2 && result.day > 28) ||
      (result.month == 4 || result.month == 6 || result.month == 9 ||
       result.month == 11) &&
          result.day > 30) {
    result.day = 1;
    result.month++;
  }

  if (result.month > 12) {
    result.month = 1;
    result.year++;
  }

  return result;
}

Date Date::previousDate() const {
  Date result = *this;
  result.day--;

  if (result.day == 0) {
    result.month--;

    if (result.month == 0) {
      result.year--;
      result.month = 12;
    }

    switch (result.month) {
    case 2:
      result.day = 28;
      break;
    case 4:
    case 6:
    case 9:
    case 11:
      result.day = 30;
      break;
    default:
      result.day = 31;
      break;
    }
  }

  return result;
}

bool Date::operator<=(const Date &rhs) const {
  if (this->getYear() < rhs.getYear())
    return true;
  if (this->getYear() > rhs.getYear())
    return false;
  if (this->getMonth() < rhs.getMonth())
    return true;
  if (this->getMonth() > rhs.getMonth())
    return false;
  return this->getDay() <= rhs.getDay();
}

std::string Date::nowStr() {
      // 获取当前时间点
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

    // 转换时间点到time_t（秒级别）
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    // 使用tm结构体将time_t转换为本地时间
    std::tm localTime = *std::localtime(&currentTime);

    // 使用stringstream将本地时间转换为字符串
    std::stringstream timeStringStream;
    timeStringStream << std::put_time(&localTime, "%Y-%m-%d_%H_%M_%S");
    return timeStringStream.str();
}


} // namespace BT