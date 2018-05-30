#ifndef ENGINE_MYSQL_MYSQL_H_NDB
#define ENGINE_MYSQL_MYSQL_H_NDB

#include <ndb/engine/basic.hpp>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

namespace ndb
{
    class mysql_connection
    {
    public:
        mysql_connection(int id)
        {

        }
        ~mysql_connection()
        {
        }


    private:

    };

    class mysql : public basic_engine<mysql>
    {
    public:
        mysql() :
            instance_{ get_driver_instance() }
        {}

        template<class Database>
        void connect()
        {
            //instance_->connect("tcp://127.0.0.1:3306", "root", "root");
        }

    private:
        sql::Driver* instance_;
        //std::unordered_map<int, std::unique_ptr<sql_connection>> connection_list_;
    };
} // db

#endif // ENGINE_MYSQL_MYSQL_H_NDB

