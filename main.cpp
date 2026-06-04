#include "pch.h"
#include "orderbook.h"

int main(){
    OrderBook book;

    book.addLimitOrder(1, Side::SELL, 5, 100.0); //
    book.addLimitOrder(2, Side::SELL, 3, 101.0); //
    book.addLimitOrder(3, Side::SELL, 8, 102.0); //3

    book.addLimitOrder(4, Side::BUY, 10, 101.0);

    book.addMarketOrder(5, Side::BUY, 5);
    book.addLimitOrder(6, Side::SELL, 10, 105.0);
    book.cancelOrder(6);
    book.addMarketOrder(7, Side::BUY, 5);

    return 0;
}