#include <iostream>

#include <express/legacy/in/WebApp.h>
#include <express/tls/in/WebApp.h>
#include <express/middleware/StaticMiddleware.h>
#include "database/mariadb/MariaDBClient.h"
#include "database/mariadb/MariaDBCommandSequence.h"

#define SERVERROOT "/home/student/Documents/Code/projects/forum-webapp/public"
#define PORT 1337

int main(int argc, char *argv[]) {
    express::legacy::in::WebApp::init(argc, argv);

    database::mariadb::MariaDBConnectionDetails details = {
        .hostname = "localhost",
        .username = "snodec",
        .password = "pentium5",
        .database = "snodec",
        .port = 3306,
        .socket = "/run/mysqld/mysqld.sock",
        .flags = 0,
    };

    database::mariadb::MariaDBClient db1(details);

    express::legacy::in::WebApp staticServer("ForumApp");

    staticServer.use(express::middleware::StaticMiddleware(SERVERROOT));

    staticServer.listen(PORT, [] (const express::legacy::in::WebApp::SocketAddress &socketAddress, int err) -> void {
        if (err != 0) {
            std::cerr << "Listen failed on port " << PORT << std:: endl;
        } else {
            std::cout << "ForumApp is listening on " << socketAddress.toString() << std::endl;
        }
    });

//    express::legacy::in::WebApp restAPI("Forum Rest API");

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
