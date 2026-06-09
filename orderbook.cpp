#include "pch.h"
#include "orderbook.h"

void OrderBook::addLimitOrder(int id, Side side, int quantity, double price){
    long long timestamp = std::chrono::system_clock::now().time_since_epoch().count();
    Order order=Order{id,side,OrderType::LIMIT,quantity,price,timestamp};

    match(order);

    if(order.quantity>0){
        if(side==Side::BUY){
            bids[price].push_back(order);
            cancel_order[id]={price,side,std::prev(bids[price].end())};
        }
        else{
            asks[price].push_back(order);
            cancel_order[id]={price,side,std::prev(asks[price].end())};
        }
    }
}

void OrderBook::addMarketOrder(int id,Side side,int quantity){
    long long timestamp = std::chrono::system_clock::now().time_since_epoch().count();
    Order order=Order{id,side,OrderType::MARKET,quantity,0.0,timestamp};
    match(order);
}

void OrderBook::match(Order& order){
    if(order.side == Side::BUY){
        while(!asks.empty() && order.quantity>0){
            auto level=asks.begin();
            if(order.type==OrderType::LIMIT && level->first>order.price){
                break;
            }
            else{
                Order& resting=level->second.front();
                int tradeQuantity=std::min(order.quantity,resting.quantity);
                std::cout<<"TRADE: "<<tradeQuantity<<" @ "<<level->first<<" Buyer: "<<order.id<<" Seller: "<<resting.id<<"\n";
                order.quantity-=tradeQuantity;
                resting.quantity-=tradeQuantity;
                if(resting.quantity==0){
                    cancel_order.erase(resting.id);
                    level->second.pop_front();
                    if(level->second.empty()){
                        asks.erase(level);
                    }
                }
            }
        }
    }
    else{
        while(!bids.empty() && order.quantity>0){
            auto level=prev(bids.end());
            if(order.price>level->first && order.type==OrderType::LIMIT){
                break;
            }
            else{
                Order& resting=level->second.front();
                int tradeQuantity=std::min(order.quantity,resting.quantity);
                std::cout<<"TRADE: "<<tradeQuantity<<" @ "<<level->first<<" Seller: "<<order.id<<" Buyer: "<<resting.id<<"\n";
                order.quantity-=tradeQuantity;
                resting.quantity-=tradeQuantity;
                if(resting.quantity==0){
                    cancel_order.erase(resting.id);
                    level->second.pop_front();
                    if(level->second.empty()){
                        bids.erase(level);
                    }
                }
            }
        }
    }
}

void OrderBook::cancelOrder(int id){
   auto found=cancel_order.find(id);
    if(found==cancel_order.end()){
        return;
    }
    OrderLocation loc=found->second;
    if(loc.side==Side::BUY){
        auto level=bids.find(loc.price);
        level->second.erase(loc.it);
        if(level->second.empty()){
            bids.erase(level);
        }
    }
    else{
        auto level=asks.find(loc.price);
        level->second.erase(loc.it);
        if(level->second.empty()){
            asks.erase(level);
        }
    }
    cancel_order.erase(found);
}

void OrderBook::printBook(){
    std::cout << "------ ORDER BOOK ------\n";

    std::cout << "ASKS:\n";
    for(auto it=asks.begin();it!=asks.end();it++){
        for(auto i:it->second){
            std::cout<<"id: "<<i.id<<"  "<<i.price<<" -> "<<i.quantity<<"\n";
        }
        std::cout<<"\n";
    }
    std::cout<<"\n";

    std::cout << "BIDS:\n";
    for(auto it=bids.rbegin();it!=bids.rend();it++){
        for(auto i:it->second){
            std::cout<<"id: "<<i.id<<"  "<<i.price<<" -> "<<i.quantity<<"\n";
        }
        std::cout<<"\n";
    }
    std::cout << "------------------------\n";
}