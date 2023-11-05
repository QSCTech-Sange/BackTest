#include "data_handler.hxx"
#include "csv.hxx"
#include "date.hxx"
#include "types.hxx"
#include <functional>
#include <numeric>
#include <optional>

class BackTester {
public:
  BackTester(
      double initialCash, BT::Date startDate, BT::Date endDate,
      BT::Path dataPath, BT::Path strategyPath,
      std::function<std::optional<BT::Price>(std::string_view)> closePriceFunc,
      std::function<std::optional<BT::Price>(std::string_view)> changePriceFunc,
      double excution_cost, BT::Path outputPath, double risk_free_rate)
      : initialCash_(initialCash), dataPath_(dataPath),
        strategyPath_(strategyPath), closePriceFunc_(closePriceFunc),
        changePriceFunc_(changePriceFunc), excutionCost_(excution_cost) , outputPath_(outputPath), riskFreeRate_(risk_free_rate){
    dates_ = startDate.toDates(endDate);
    days_ = dates_.size();
  }

  void simulate();

private:
  double initialCash_;
  double riskFreeRate_;
  double excutionCost_;
  double sharpe_;
  int days_;
  std::vector<BT::Date> dates_;
  std::vector<std::string> valid_dates;
  BT::Path dataPath_;
  BT::Path strategyPath_;
  BT::Path outputPath_;
  BT::Return return_;
  BT::Return IC_;
  std::function<std::optional<BT::Price>(std::string_view)> closePriceFunc_;
  std::function<std::optional<BT::Price>(std::string_view)> changePriceFunc_;

  double calcDailyRet(const BT::Price &old_price, const BT::Price &new_price,
                      const BT::Price &change_price,
                      const BT::Target &old_holds, const BT::Target &new_holds);
  BT::Target calcTargetDiff(const BT::Target &old_t, const BT::Target &new_t);
  double calcAsset(const BT::Target &target, const BT::Price &price);
  double calcExcution(const BT::Target &diff, const BT::Price &changePrice,
                      const BT::Price &closePrice);
  BT::Target calcHolds(const BT::Target &target, const BT::Price &price);
  BT::Target getTargetAmount(const BT::Target &target);
  double calcIC(const BT::Price& old_price, const BT::Price& new_price, const BT::Target& target);
  void saveReturnAsCSV();
};
