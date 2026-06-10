#include "pch.h"
#include "order.h"

struct OrderLocation{
    double price;
    Side side;
    std::list<Order>::iterator it;
};

class OrderBook{
    private:
    std::map<double,std::list<Order>>bids;
    std::map<double,std::list<Order>>asks;
    std::unordered_map<int,OrderLocation>cancel_order;

    void match(Order& order);

    public:
    void addLimitOrder(int id,Side side, int quantity,double price);
    void addMarketOrder(int id,Side side,int quantity);
    void cancelOrder(int id);
    void printBook();
    bool hasBid();
    bool hasAsk();
    double getBestBid();
    double getBestAsk();
};