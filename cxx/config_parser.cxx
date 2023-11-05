#include "config_parser.hxx"
#include "date.hxx"
#include "logger.hxx"
#include <string_view>

namespace BT {

std::optional<Date> parse_date_from_env(std::string_view env_name) {
  const char *envValue = std::getenv(env_name.data());
  if (envValue) {
    return Date(std::string(envValue));
  } else {
    return std::nullopt;
  }
}

std::optional<double> parse_double_from_env(std::string_view env_name) {
  const char *envValue = std::getenv(env_name.data());
  if (envValue) {
    try {
      double excutionCost = std::stod(envValue);
      return excutionCost;
    } catch (...) {
      return std::nullopt;
    }
  } else {
    return std::nullopt;
  }
}

std::optional<std::string> parse_string_from_env(std::string_view env_name) {
  const char *envValue = std::getenv(env_name.data());
  if (envValue) {
    return envValue;
  } else {
    return std::nullopt;
  }
}

} // namespace BT