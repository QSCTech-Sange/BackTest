#include "data_handler.hxx"
#include "logger.hxx"


std::optional<BT::Price> dongcai_close_price(std::string_view path) {
  if (!std::filesystem::exists(path)) {
    // LOG(critical, "data path {} doesn't exists", path);
    return std::nullopt;
  }
  csv::CSVReader reader(path);
  BT::Price closePrice;
  for (csv::CSVRow &row : reader) {
    closePrice[row["代码"].get<>()] = row["收盘"].get<double>();
  }
  return closePrice;
}

std::optional<BT::Price> dongcai_mid_price(std::string_view path) {
  if (!std::filesystem::exists(path)) {
    // LOG(critical, "data path {} doesn't exists", path);
    return std::nullopt;
  }
  csv::CSVReader reader(path);
  BT::Price midPrice;
  for (csv::CSVRow &row : reader) {
    midPrice[row["代码"].get<>()] = row["收盘"].get<double>() - row["涨跌额"].get<double>() / 2;
  }
  return midPrice;
}

std::optional<BT::Target> read_buckets(std::string_view path) {
  if (!std::filesystem::exists(path)) {
    // LOG(critical, "strategy path {} doesn't exists", path);
    return std::nullopt;
  }
  csv::CSVReader reader(path);
  BT::Target ratio;
  for (csv::CSVRow &row : reader) {
    ratio[row[0].get<>()] = row[1].get<double>();
  }
  return ratio;
}