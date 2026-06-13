#include "pch.h"
#include "orderbook.h"

int main(){
    OrderBook book;
    std::string cmd;

    while(std::cin >> cmd){
        if(cmd == "LIMIT"){
            std::string sideStr;
            int id, qty;
            double price;
            std::cin >> sideStr >> id >> price >> qty;
            Side side = (sideStr == "BUY") ? Side::BUY : Side::SELL;
            book.addLimitOrder(id, side, qty, price);
        }
        else if(cmd == "MARKET"){
            std::string sideStr;
            int id, qty;
            std::cin >> sideStr >> id >> qty;
            Side side = (sideStr == "BUY") ? Side::BUY : Side::SELL;
            book.addMarketOrder(id, side, qty);
        }
        else if(cmd == "CANCEL"){
            int id;
            std::cin >> id;
            book.cancelOrder(id);
        }
        else if(cmd == "PRINT"){
            book.printBook();
        }
        else{
            std::cout << "Unknown command: " << cmd << "\n";
        }
    }

    return 0;
}