//
// Created by Derawn Kelly on 9/4/24.
//

#include <iostream>
#include <map>
#include <set>
#include <list>
#include <cmath>
#include <ctime>
#include <deque>
#include <queue>
#include <stack>
#include <limits>
#include <string>
#include <vector>
#include <numeric>
#include <algorithm>
#include <unordered_map>
#include <memory>
#include <variant>
#include <optional>
#include <tuple>
#include <format>


using namespace std;

enum class OrderType 
{
    GoodTllCancelled,
    FillAndKill
};

enum class Side 
{
    Buy,
    Sell
};

using Price = std::int32_t;
using Quantity = std::uint32_t;
using OrderId = std::uint64_t;

struct LevelInfo
{
    Price price_;
    Quantity quantity_;
};

using LevelInfos = std::vector<LevelInfo>;

class OrderbookLevelInfos 
{
    public:
    OrderbookLevelInfos(const LevelInfos& bids, const LevelInfos& asks)
    : bids_{bids}
    , asks_{asks}
    {}

    const LevelInfos& GetBids() const {return bids_;}
    const LevelInfos& GetAsks() const {return asks_;}

    private:
    LevelInfos bids_;
    LevelInfos asks_;

};

class Order 
{
    public:
    Order(OrderType orderType, OrderId orderId, Side side, Price price, Quantity quantity)
    : orderType_{orderType}
    , orderId_{orderId}
    , side_{side}
    , price_{price}
    , initialQuantity_{quantity}
    , remainingQuantity_{quantity}
    {}

    OrderId GetOrderId() const {return orderId_;}
    Side GetSide() const {return side_;}
    Price GetPrice() const {return price_;}
    OrderType GetOrderType() const {return orderType_;}
    Quantity GetInitialQuantity() const {return initialQuantity_;}
    Quantity GetRemainingQuantity() const {return remainingQuantity_;}
    Quantity GetFilledQuantity() const {return GetInitialQuantity() - GetRemainingQuantity();}
    bool IsFilled() const {return GetRemainingQuantity() == 0;}
    void Fill(Quantity quantity) 
    {

    }

private:
OrderType orderType_;
OrderId orderId_;
Side side_;
Price price_;
Quantity initialQuantity_;
Quantity remainingQuantity_;
};

using OrderPointer = shared_ptr<Order>;
using OrderPointers = list<OrderPointer>;

class OrderModify 
{
    public:
    OrderModify(OrderId orderId, Side side, Price price, Quantity quantity)
    : orderId_{orderId}
    , side_{side}
    , price_{price}
    , quantity_{quantity}
    {}

    OrderId GetOrderId() const {return orderId_;}
    Side GetSide() const {return side_;}
    Price GetPrice() const {return price_;}
    Quantity GetQuantity() const {return quantity_;}

    OrderPointer ToOrderPointer(OrderType type) const
    {
        return make_shared<Order>(type, GetOrderId(), GetSide(), GetPrice(), GetQuantity());
    }

    private:
    OrderId orderId_;
    Side side_;
    Price price_;
    Quantity quantity_;
};

struct TradeInfo 
{
    OrderId orderId_;
    Price price_;
    Quantity quantity_;
};

class Trade 
{
    public:
    Trade(const TradeInfo& bidTrade, const TradeInfo& askTrade)
    : bidTrade_{bidTrade}
    , askTrade_{askTrade}
    {}

    const TradeInfo& GetBidTrade() const {return bidTrade_;}
    const TradeInfo& GetAskTrade() const {return askTrade_;}

    private:
    TradeInfo bidTrade_;
    TradeInfo askTrade_;
};

using Trades = vector<Trade>;

class Orderbook
{
    private:
    struct OrderEntry
    {
        OrderPointer order_{nullptr};
        OrderPointers::iterator location_;
    };

    map<Price, OrderPointers, greater<Price>> bids_;
    map<Price, OrderPointers, less<Price>> asks_;
    unordered_map<OrderId, OrderEntry> orders_;

    bool CanMatch(Side side, Price price) const
    {
        if (side == Side::Buy)
        {
            if (asks_.empty())
            return false;

            const auto& [bestAsk, _] = *asks_.begin();
            return price >= bestAsk;
        }
        else
        {
            if (bids_.empty())
            return false;

            const auto& [bestBid, _] = *bids_.begin();
            return price >= bestBid;

        }
    }

    Trades MatchOrders()
    {
        Trades trades;
        trades.reserve(orders_.size());

        while (true)
        {
            if (bids_.empty() || asks_.empty())
            break;

            auto& [bidPrice, bids] = *bids_.begin();
            auto& [askPrice, asks] = *asks_.begin();

            if (bidPrice < askPrice)
            break;

            while (bids.size() && asks.size())
            {
                auto& bid = bids.front();
                auto& ask = asks.front();

                Quantity quantity = min(bid->GetRemainingQuantity(), ask->GetRemainingQuantity());

                bid->Fill(quantity);
                ask->Fill(quantity);

                if (bid->IsFilled())
                {
                    bids.pop_front();
                    orders_.erase(bid->GetOrderId());
                }

                if (ask->IsFilled())
                {
                    asks.pop_front();
                    orders_.erase(ask->GetOrderId());
                }

            }
        }
    }
};

int main()
{

return 0;
};





/* Code for error message. Needs to be reworked.
if (quantity > GetRemainingQuantity())
        throw logic_error(format("Order ({}) cannot be filled for than its remaining quantity.", GetOrderId() ));

        remainingQuantity_ -= quantity;
*/