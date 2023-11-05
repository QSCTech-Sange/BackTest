#include "backtester.hxx"
#include "config_parser.hxx"
#include "data_handler.hxx"
#include "date.hxx"
#include "logger.hxx"
#include "types.hxx"
#include <chrono>
#include <cstddef>
#include <filesystem>
#include <functional>
#include <optional>
#include <string_view>
#include <unordered_map>


std::optional<BackTester> makeBT() {
  double excutionCost =
      BT::parse_double_from_env("BACKTEST_EXECUTION_COST").value_or(0.00045);
  LOG(info, "Execution Cost: {}", excutionCost);

  double initialCash =
      BT::parse_double_from_env("BACKTEST_INITIAL_CASH").value_or(100000000);
  LOG(info, "Initial Cash: {}", initialCash);
  
  double riskFreeRate =
      BT::parse_double_from_env("BACKTEST_RISK_FREE_RATE").value_or(0.0015);
  LOG(info, "Risk Free Rate: {}", riskFreeRate);

  BT::Date startDate =
      BT::parse_date_from_env("BACKTEST_START").value_or(BT::Date("2022-11-04"));
  LOG(info, "Start Date: {}", startDate.toString());

  BT::Date endDate =
      BT::parse_date_from_env("BACKTEST_END").value_or(BT::Date("2023-11-03"));
  LOG(info, "End Date: {}", endDate.toString());

  std::string strategyPathStr =
      BT::parse_string_from_env("BACKTEST_STRATEGY")
          .value_or("/home/strategy/TEST001");
  LOG(info, "Strategy: {}", strategyPathStr);
  BT::Path strategyPath = strategyPathStr;
  if (!std::filesystem::exists(strategyPath)) {
    LOG(critical, "Strategy Path doesn't exists!");
    return std::nullopt;
  }
  std::string dataPathStr =
      BT::parse_string_from_env("BACKTEST_DATA").value_or("/home/data");
  LOG(info, "Data Path: {}", dataPathStr);
  BT::Path dataPath = dataPathStr;
  if (!std::filesystem::exists(dataPath)) {
    LOG(critical, "Data Path doesn't exists!");
    return std::nullopt;
  }

  std::string dataSource =
      BT::parse_string_from_env("BACKTEST_DATA_SOURCE").value_or("DongCai");
  LOG(info, "Data Source: {}", dataSource);
  std::function<std::optional<BT::Price>(std::string_view)> closePriceFunc;
  if (dataSource == "DongCai") {
    closePriceFunc = dongcai_close_price;
  } else {
    LOG(critical, "Data Parser for {} is missing.", dataSource);
    return std::nullopt;
  }

  std::string changePrice =
      BT::parse_string_from_env("BACKTEST_CHANGE_PRICE").value_or("DongCaiMid");
  LOG(info, "Change Price: {}", changePrice);
  std::function<std::optional<BT::Price>(std::string_view)> changePriceFunc;
  if (changePrice == "DongCaiMid") {
    changePriceFunc = dongcai_mid_price;
  } else {
    LOG(critical, "Data Parser for {} is missing.", changePrice);
    return std::nullopt;
  }

  std::string outputPathStr =
      BT::parse_string_from_env("BACKTEST_OUTPUT").value_or("/home/data");
  LOG(info, "Output Path: {}", outputPathStr);
  BT::Path outputPath = outputPathStr;
  if (!std::filesystem::exists(outputPath)) {
    LOG(critical, "Output Path doesn't exists!");
    return std::nullopt;
  }

  return BackTester {initialCash, startDate, endDate, dataPath, strategyPath, closePriceFunc, changePriceFunc, excutionCost, outputPath, riskFreeRate};
}


int main() {
  LOG(info, "BackTest Starts");
  std::optional<BackTester> bt = makeBT();
  if (!bt.has_value()) {
    LOG(critical, "Cannot create backtest, exiting");
    return 1;
  }
  LOG(info, "BackTest Ready");
  auto t0 = std::chrono::high_resolution_clock::now();
  bt->simulate();
  auto t1 = std::chrono::high_resolution_clock::now();
  LOG(info, "Time is {} milli seconds", std::chrono::duration_cast<std::chrono::milliseconds>(t1-t0).count());
}