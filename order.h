#include "pch.h"

enum class Side{
    BUY,
    SELL
};

enum class OrderType {
    LIMIT,
    MARKET
};

struct Order{
    int id;
    Side side;
    OrderType type;
    int quantity;
    double price;
    long long timestamp; 
};