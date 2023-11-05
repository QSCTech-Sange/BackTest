#pragma once

#include <optional>
#include <string>
#include <chrono>
#include "date.hxx"
#include "types.hxx"

namespace BT {

std::optional<Date> parse_date_from_env(std::string_view env_name);
std::optional<double> parse_double_from_env(std::string_view env_name);
std::optional<std::string> parse_string_from_env(std::string_view env_name);
}