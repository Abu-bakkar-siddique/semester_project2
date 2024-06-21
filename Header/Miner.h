#pragma once
# include <string>
# include "Node.h"

class Miner : public Node {
public:
    Miner(string un, string pubk, string prik, double w, int dif, string iden);

    string mineBlock();
};