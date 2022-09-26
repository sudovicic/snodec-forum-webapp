#include <iostream>

#include <express/legacy/in/WebApp.h>
#include <express/tls/in/WebApp.h>
#include <express/middleware/StaticMiddleware.h>
#include "database/mariadb/MariaDBClient.h"
#include "database/mariadb/MariaDBCommandSequence.h"

#define SERVERROOT "/home/student/forum-webapp/public"
#define PORT 1337

int main(int argc, char *argv[]) {
    express::legacy::in::WebApp::init(argc, argv);

    const database::mariadb::MariaDBConnectionDetails details = {
        "localhost",
        "snodec",
        "password",
        "WebForumDB",
        3307,
        "/var/run/mysqld/mysqld.sock",
        0,
    };

    database::mariadb::MariaDBClient mariaDbClient(details);

    std::string sqlString = "Select * from User;";

    mariaDbClient.query(sqlString, [&](const MYSQL_ROW &rows) {
           if (rows != nullptr) {
               for (int i = 0; rows[i + 1]; ++i) {
                   std::cout << rows[i] << std::endl;

               }
           }

       }, [](const std::string &, int) {
       })

    .exec(
                    "CREATE TABLE IF NOT EXISTS `Users` (name varchar(255), password varchar(255))",
                    [&mariaDbClient](void) -> void {
            VLOG(0) << "**** OnQuery 0;";
            mariaDbClient.affectedRows(
                        [](my_ulonglong affectedRows) -> void {
                VLOG(0) << "**** AffectedRows 1: " << affectedRows;
            },
            [](const std::string& errorString, unsigned int errorNumber) -> void {
                VLOG(0) << "Error 1: " << errorString << " : " << errorNumber;
            });
        },
        [](const std::string& errorString, unsigned int errorNumber) -> void {
            VLOG(0) << "**** Error 0: " << errorString << " : " << errorNumber;
        });


    express::legacy::in::WebApp staticServer("ForumApp");

        staticServer.use(express::middleware::StaticMiddleware(SERVERROOT));

        staticServer.listen(8080, [] (const express::legacy::in::WebApp::SocketAddress &socketAddress, int err) -> void {
            if (err != 0) {
                std::cerr << "Listen failed on port 8080 " << std:: endl;
            } else {
                std::cout << "ForumApp is listening on " << socketAddress.toString() << std::endl;
            }
        });

    express::legacy::in::WebApp restAPI("ForumRestAPI");




//    restAPI.get(
//        "/api/subtopics",
//        [&db1] APPLICATION(req, res) {
//            res.cookie("TestCookie", "CookieValue", {{"Max-Age", "3600"}});

//            db1.query(
//                "SELECT * FROM snodec",
//                [](const MYSQL_ROW row) -> void {
//                    if (row != nullptr) {

//                    }
//                },
//                [](const std::string& errorString, unsigned int errorNumber) -> void {
//                    VLOG(0) << "********** Error 2: " << errorString << " : " << errorNumber;
//                });
//            //res.send();
//        }
//    );

    return express::WebApp::start();
}

