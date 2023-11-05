#pragma once

#include "csv.hxx"
#include "config_parser.hxx"
#include <filesystem>
#include <optional>

std::optional<BT::Price> dongcai_close_price(std::string_view path);
std::optional<BT::Target> read_buckets(std::string_view path);
std::optional<BT::Price> dongcai_mid_price(std::string_view path);