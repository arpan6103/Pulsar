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

void OrderBook::match(Order& order){
    if(order.side == Side::BUY){
        while(!asks.empty() && order.quantity>0){
            if(order.price>=asks.begin()->first || order.type==OrderType::MARKET){
                int tradeQuantity=std::min(order.quantity,asks.begin()->second.front().quantity);
                std::cout<<"TRADE: "<< tradeQuantity<<" @ "<<asks.begin()->first<<"\n";
                order.quantity-=tradeQuantity;
                asks.begin()->second.front().quantity-=tradeQuantity;
                if(asks.begin()->second.front().quantity==0){
                    asks.begin()->second.pop();
                    if(asks.begin()->second.empty()){
                        asks.erase(asks.begin());
                    }
                }
            }
            else{
                break;
            }
        }
    }
    else{
        while(!bids.empty() && order.quantity>0){
            if(order.price<=bids.rbegin()->first || order.type==OrderType::MARKET){
                int tradeQuantity=std::min(order.quantity,bids.rbegin()->second.front().quantity);
                std::cout<<"TRADE: "<< tradeQuantity<<" @ "<<bids.rbegin()->first<<"\n";
                order.quantity-=tradeQuantity;
                bids.rbegin()->second.front().quantity-=tradeQuantity;
                if(bids.rbegin()->second.front().quantity==0){
                    bids.rbegin()->second.pop();
                    if(bids.rbegin()->second.empty()){
                        bids.erase(--bids.end());
                    }
                }
            }
            else{
                break;
            }
        }
    }
}