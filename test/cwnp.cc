#include <random>
#include <iostream>
#include <ctime>
#include <vector>

#include "../src/header.h"
#include "../src/cwnp.h"
#include "../src/mysql_api.h"
#include "../src/miscellaneous.h"

std::uniform_real_distribution<double> d(0.0, 100.0);
std::uniform_int_distribution<unsigned> u(0,2);
std::uniform_int_distribution<int> ui(0, 100);
std::uniform_int_distribution<unsigned> delta(10, 20);
std::default_random_engine e(std::time(0));
ndrnp::id_type id = 0;

ndrnp::Node*
random_node(ndrnp::NodeType t) {
    switch(t) {
    case ndrnp::NodeType::SENSOR:
        return new ndrnp::Sensor(ndrnp::Coordinate(d(e), d(e), 0.0), 15.0, 20, id++);
    case ndrnp::NodeType::CDL:
        return new ndrnp::CDL(ndrnp::Coordinate(d(e), d(e), 0.0), 15.0, 9999, id++);
    case ndrnp::NodeType::SINK:
        return new ndrnp::Sink(ndrnp::Coordinate(d(e), d(e), 0.0), 15.0, 9999, id++);
    }
    return nullptr;
}

int c1np_test(void) {
    ndrnp::Nodes nds;
    char user[] = "root";
    char db[] = "cpp";
    char passwd[] = "0000000027";

    ndrnp::MySQLdb mysql(user, passwd, db);

    mysql.query("DELETE FROM graph");
    
    for (int i = 0; i < 400; ++i) {
        if (i < 1) 
            nds.push_back(random_node(ndrnp::NodeType::SINK));
        else if (1 <= i && i < 40)
            nds.push_back(random_node(ndrnp::NodeType::SENSOR));
        else
            nds.push_back(random_node(ndrnp::NodeType::CDL));
    }

    std::vector<ndrnp::Node*> nodes(nds.begin(), nds.end());

    std::set<ndrnp::size_type> y = ndrnp::cwnp(nodes, 4);

    std::cout << "y_hat: ";
    for (auto &e : y)
        std::cout << e << ", ";
    std::cout <<  std::endl;
    std::cout << "size: " << y.size() << std::endl;

    ndrnp::AdjacencyList<ndrnp::Node*> al(nodes.begin(), nodes.end());
    if (!mysql.write_adjacency_list(al))
        std::cout << "mysql error!" << std::endl;
    else
        std::cout << "mysql done." << std::endl;

    std::cout << "c1np over..." << std::endl;
    return 0;
}

int main(void) {
    c1np_test();
    return 0;
}
