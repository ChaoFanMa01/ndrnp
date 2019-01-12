#include <iostream>
#include <random>
#include <ctime>

#include "../src/header.h"
#include "../src/node.h"
#include "../src/graph.h"
#include "../src/graph_misc.h"
#include "../src/mysql_api.h"

std::uniform_real_distribution<double> d(0.0, 100.0);
std::uniform_real_distribution<double> p(0.0, 10.0);
std::default_random_engine e(std::time(0));

ndrnp::id_type id = 0;

ndrnp::Node*
random_node(ndrnp::NodeType t) {
    switch (t) {
        case ndrnp::NodeType::SENSOR:
            return new ndrnp::Sensor(ndrnp::Coordinate(d(e), d(e), 0.0), 15.0, 10, id++);
        case ndrnp::NodeType::CDL:
            return new ndrnp::CDL(ndrnp::Coordinate(d(e), d(e), 0.0), 15.0, 10, id++);
        case ndrnp::NodeType::SINK:
            return new ndrnp::Sink(ndrnp::Coordinate(d(e), d(e), 0.0), 15.0, 10, id++);
    }
    return nullptr;
}

int main() {
    ndrnp::Nodes nodes;
    char user[] = "root";
    char db[] = "cpp";
    char passwd[] = "0000000027";

    ndrnp::MySQLdb mysql(user, passwd, db);

    mysql.query("DELETE FROM graph");

    for (int i = 0; i < 400; ++i)
        if (i < 10)
            nodes.push_back(random_node(ndrnp::NodeType::SINK));
        else if (i < 100)
            nodes.push_back(random_node(ndrnp::NodeType::SENSOR));
        else
            nodes.push_back(random_node(ndrnp::NodeType::CDL));

    ndrnp::AdjacencyList<ndrnp::Node*> al(nodes.begin(), nodes.end());
    std::cout << al << std::endl;

//    if (!mysql.write_adjacency_list(ndrnp::breadth_first_tree(al)))
    if (!mysql.write_adjacency_list(ndrnp::dijkstra_spt(al, 0, std::vector<ndrnp::size_type>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}), "graph"))
        std::cout << "mysql  error!" << std::endl;
    else
        std::cout << "mysql done." << std::endl;

    return 0;
}
