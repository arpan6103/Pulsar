#include "pch.h"
#include "order.h"

class OrderBook{
    private:
    std::map<double,std::queue<Order>>bids;
    std::map<double,std::queue<Order>>asks;
    std::unordered_map<int,Order*>cancel_order;

    void match(Order& order);

    public:
    void addLimitOrder(int id,Side side, int quantity,double price);
    void addMarketOrder(int id,Side side,int quantity);
    void cancelOrder(int id);
};