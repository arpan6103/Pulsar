#include "pch.h"
#include "orderbook.h"

void OrderBook::addLimitOrder(int id, Side side, int quantity, double price){
    long long timestamp = std::chrono::system_clock::now().time_since_epoch().count();
    Order order={id,side,OrderType::LIMIT,quantity,price,timestamp};

    match(order);

    if(order.quantity>0){
        if(side==Side::BUY){
            bids[price].push(order);
            cancel_order[id]=&bids[price].back();
        }
        else{
            asks[price].push(order);
            cancel_order[id]=&asks[price].back();
        }
    }
}

void OrderBook::addMarketOrder(int id,Side side,int quantity){
    long long timestamp = std::chrono::system_clock::now().time_since_epoch().count();
    Order order={id,side,OrderType::MARKET,quantity,0.0,timestamp};

    match(order);
}