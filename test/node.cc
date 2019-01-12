#include <iostream>
#include <random>
#include <ctime>

#include "../src/node.h"

int
main() {
    std::default_random_engine e(std::time(0));
    std::uniform_real_distribution<double> d(0, 100);
    ndrnp::Nodes nodes;

    for (int i = 0; i < 400; ++i) {
        if (i < 10)
            nodes.push_back(new ndrnp::Sink(ndrnp::Coordinate(d(e), d(e), d(e)), 15, 6, i));
        else if (i < 40)
            nodes.push_back(new ndrnp::Sensor(ndrnp::Coordinate(d(e), d(e), d(e)), 14, 5, i));
        else
            nodes.push_back(new ndrnp::CDL(ndrnp::Coordinate(d(e), d(e), d(e)), 17, 19, i));
    }

    for (auto &pn : nodes)
        std::cout << *pn << std::endl;
    return 0;
}
