#ifndef NDRNP_MYSQL_API_H
#define NDRNP_MYSQL_API_H

#include <mysql/mysql.h>   //mysql adaptor.
#include <string>
#include <stdexcept>
#include <cstdlib>

#include "header.h"
#include "node.h"
#include "graph.h"

namespace ndrnp {
    class MySQLdb {
    public:
        class mysql_error: public std::runtime_error {
        public:
            mysql_error(const std::string& what_arg)
            : runtime_error(what_arg) {}
            mysql_error(const char* what_arg)
            : runtime_error(what_arg) {}
        };

/*
 * constructors and destructor.
 */
        MySQLdb() {
            if ((mysql = mysql_init(nullptr)) == nullptr)
                throw mysql_error("Error: cannot open MySQL.");
        }

        MySQLdb(const std::string& user,
                const std::string& passwd,
                const std::string& db
               )
        {
            if ((mysql = mysql_init(nullptr)) == nullptr)
                throw mysql_error("Error: cannot open MySQL.");
            if ((mysql = mysql_real_connect(mysql, nullptr, 
                 user.c_str(), passwd.c_str(), db.c_str(), 
                 0, nullptr, 0)) == nullptr)
                throw mysql_error("Error: cannot connect MySQL.");
        }

        MySQLdb(const char* user, const char* passwd, const char* db)
        {
            if ((mysql = mysql_init(nullptr)) == nullptr)
                throw mysql_error("Error: cannot open MySQL.");
            if ((mysql = mysql_real_connect(mysql, nullptr, 
                 user, passwd, db, 0, nullptr, 0)) == nullptr)
                throw mysql_error("Error: cannot connect MySQL.");
        }

        MySQLdb(const std::string& host, const std::string& user,
                const std::string& passwd, const std::string& db,
                const unsigned int& port, 
                const std::string& socket,
                const unsigned int& flag
                )
        {
            if ((mysql = mysql_init(nullptr)) == nullptr)
                throw mysql_error("Error: cannot open MySQL.");
            if ((mysql = mysql_real_connect(mysql, host.c_str(), 
                 user.c_str(), passwd.c_str(), db.c_str(), 
                 port, socket.c_str(), flag)) == nullptr)
                throw mysql_error("Error: cannot connect MySQL.");
        }

        MySQLdb(const char* host, const char* user,
                const char* passwd, const char* db,
                const unsigned int& port, 
                const char* socket,
                const unsigned int& flag
                )
        {
            if ((mysql = mysql_init(nullptr)) == nullptr)
                throw mysql_error("Error: cannot open MySQL.");
            if ((mysql = mysql_real_connect(mysql, host, 
                 user, passwd, db, port, socket, flag)) == nullptr)
                throw mysql_error("Error: cannot connect MySQL.");
        }

        ~MySQLdb()
        {
            if (!mysql)
                mysql_close(mysql);
        }

/*
 * operations.
 */
        /* @fn query()
         *
         * Executes a MySQL query.
         * Returns false on failure, true otherwise.
         */
        bool query(const std::string& q)
        {
            /*
             * mysql_query() returns 0 on success, nonzero otherwise.
             */
            if (mysql_query(mysql, q.c_str()))
                return false;

            return true;
        }

        /* @fn get_result()
         *
         * Fetch the result of the latest query, and store this result
         * in the res field.
         * Return true on success, false otherwise.
         */
        bool get_result(void)
        {
            // res != nullptr denotes a select action
            // has been successfully done, and the result
            // have been stored in res field.
            if (res = mysql_store_result(mysql))
            {
                return true;
            }
            // res == nullptr denotes action other than
            // select has been done.
            else
            {
                // mysql_field_count() == 0 denotes the non-select
                // action has been successfully done.
                if (!mysql_field_count(mysql))
                    return true;
                else
                    return false;
            }
        }

        /* @fn read_nodes()
         *
         * Create nodes according the information read from
         * database.
         * Return true on success, false otherwise.
         */
/*        bool read_nodes(Nodes& nds)
        {
            if (!query("SELECT * FROM nodes ORDER BY id"))
                return false;
            if (!get_result())
                return false;

            int cols = mysql_num_fields(res);
            int rows = mysql_num_rows(res);

            for (int i = 0; i < rows; ++i)
            {
                if ((row = mysql_fetch_row(res)) < 0)
                    return false;
                coordinate_type x, y, radius;
                int id, type;
                for (int j = 0; j < cols; ++j)
                {
                    switch(j)
                    {
                        case 0:
                            id = atoi(row[j]);
                            break;
                        case 1:
                            type = atoi(row[j]);
                            break;
                        case 2:
                            radius = strtod(row[j], nullptr);
                            break;
                        case 3:
                            x = strtod(row[j], nullptr);
                            break;
                        case 4:
                            y = strtod(row[j], nullptr);
                            break;
                        default:
                            return false;
                    }
                }
                switch(type)
                {
                    case 0:
                        nds.push_back(new Sensor(Coordinate(x, y, 0.0), radius,
                                        10, id));
                        break;
                    case 1:
                        nds.push_back(new Relay(Coordinate(x, y, 0.0), radius, 
                                        10, id));
                        break;
                    case 2:
                        nds.push_back(new Sink(Coordinate(x, y, 0.0), radius, 
                                        10, id));
                        break;
                    default:
                        return false;
                }
            }
            return true;
        }
*/
    bool write_adjacency_list(AdjacencyList<Node*> al) {
        for (int i = 0; i < al.size(); ++i) {
            std::string q = "INSERT INTO graph VALUES (";
            std::string neis = "\"";
            q += std::to_string(al[i].data()->id()) + ", ";
            switch (al[i].data()->type()) {
                case NodeType::SENSOR:
                    q += std::to_string(0) + ", "; break;
                case NodeType::CDL:
                    q += std::to_string(1) + ", "; break;
                case NodeType::SINK:
                    q += std::to_string(2) + ", "; break;
                default:
                    return false;
            }
            q += std::to_string(al[i].data()->power()) + ", ";
            q += std::to_string(al[i].data()->coordinate().x()) + ", ";
            q += std::to_string(al[i].data()->coordinate().y()) + ", ";
            for (auto &e : al[i].neighbors())
                neis += std::to_string(e.end()->data()->id()) + ",";

            if (al[i].neighbors().size())
                neis[neis.size() - 1] = '\"';
            else
                neis += "\"";

            q += neis + ");";
            if (!query(q.c_str()))
                return false;
        }
        return true;
    }
    
    bool write_adjacency_list(AdjacencyList<Node*> al, const std::string table) {
        for (int i = 0; i < al.size(); ++i) {
            std::string q = "INSERT INTO " + table + " VALUES (";
            std::string neis = "\"";
            q += std::to_string(al[i].data()->id()) + ", ";
            switch (al[i].data()->type()) {
                case NodeType::SENSOR:
                    q += std::to_string(0) + ", "; break;
                case NodeType::CDL:
                    q += std::to_string(1) + ", "; break;
                case NodeType::SINK:
                    q += std::to_string(2) + ", "; break;
                default:
                    return false;
            }
            q += std::to_string(al[i].data()->power()) + ", ";
            q += std::to_string(al[i].data()->coordinate().x()) + ", ";
            q += std::to_string(al[i].data()->coordinate().y()) + ", ";
            for (auto &e : al[i].neighbors())
                neis += std::to_string(e.end()->data()->id()) + ",";

            if (al[i].neighbors().size())
                neis[neis.size() - 1] = '\"';
            else
                neis += "\"";

            q += neis + ");";
            if (!query(q.c_str()))
                return false;
        }
        return true;
    }

    private:
/*
 * data fields.
 */
        // handler for MySQL database connection.
        MYSQL*        mysql;
        // result of a query that returns rows.
        MYSQL_RES*    res;
        // a type-safe representation of one row of data.
        // It is currently implemented as an array of counted
        // byte strings.
        MYSQL_ROW     row;
    };
}

#endif
