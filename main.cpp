#include <iostream>

#include <express/legacy/in/WebApp.h>
#include <express/tls/in/WebApp.h>
#include <express/middleware/StaticMiddleware.h>
#include "database/mariadb/MariaDBClient.h"
#include "database/mariadb/MariaDBCommandSequence.h"
#include <nlohmann/json.hpp>
#include "utils.h"

#define SERVERROOT "/home/student/snodec-forum-webapp/public"
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
                    "CREATE TABLE IF NOT EXISTS `users` (username varchar(255), password varchar(255))",
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


    staticServer.get("/users", [&mariaDbClient] APPLICATION(req, res) {
                        nlohmann::json* usersJson = new nlohmann::json;
                         mariaDbClient.query(
                             "SELECT * FROM users",
                             [&res, usersJson](const MYSQL_ROW row) -> void {
                                 if (row != nullptr) {
                                 usersJson->push_back({{"username", row[0]}, {"password", row[1]}});
                                 VLOG(0) << "Row Result 3: " << row[0] << " : " << row[1];

                                 } else {
                                        VLOG(0) << "Row Result 3: " << usersJson->dump();
                                       res.send(usersJson->dump());
                                       delete usersJson;
                                 }
                             },
                             [&res](const std::string& errorString, unsigned int errorNumber) -> void {
                                 VLOG(0) << "Error 3: " << errorString << " : " << errorNumber;
                             res.sendStatus(500);
                             });

                     });

    staticServer.post("/register", [&mariaDbClient] APPLICATION(req, res) {
                          req.body.push_back(0);
                          std::cout << req.body.data() << std::endl;
                          std::cout << Utils::GetFieldByName(req.body.data(), "username") << std::endl;
                          std::cout << Utils::GetFieldByName(req.body.data(), "password") << std::endl;

                          mariaDbClient.exec(
                              "INSERT INTO `users`(`username`, `password`) VALUES ('" + Utils::GetFieldByName(req.body.data(), "username") + "','" + Utils::GetFieldByName(req.body.data(), "password") + "')",
                              [&mariaDbClient, &res](void) -> void {
                                  VLOG(0) << "********** OnQuery 1: ";
                                  mariaDbClient.affectedRows(
                                      [&res](my_ulonglong affectedRows) -> void {
                                          VLOG(0) << "********** AffectedRows 2: " << affectedRows;
                                           res.cookie("sessionCookie", Utils::Gen_random(16), {{"Max-Age", "60"}} );
                                           res.sendStatus(200);
                                      },
                                      [&res](const std::string& errorString, unsigned int errorNumber) -> void {
                                          VLOG(0) << "********** Error 2: " << errorString << " : " << errorNumber;
                                          res.sendStatus(500);

                                      });
                              },
                              [&res](const std::string& errorString, unsigned int errorNumber) -> void {
                                  VLOG(0) << "********** Error 1: " << errorString << " : " << errorNumber;
                                    res.sendStatus(500);
                              });


                      });

    staticServer.post("/login", [&mariaDbClient] APPLICATION(req, res) {
                          req.body.push_back(0);
                          std::cout << req.body.data() << std::endl;

                          nlohmann::json* usersJson = new nlohmann::json;
                          mariaDbClient.query(
                              "SELECT * FROM users WHERE username = '" + Utils::GetFieldByName(req.body.data(), "username") + "'AND password='" + Utils::GetFieldByName(req.body.data(), "password") + "';",
                              [&res, usersJson](const MYSQL_ROW row) -> void {
                                  if (row != nullptr) {
                                    usersJson->push_back({{"username", row[0]}, {"password", row[1]}});

                                  } else {
                                    res.cookie("sessionCookie", Utils::Gen_random(16), {{"Max-Age", "60"}} );
                                    res.send(usersJson->dump());
                                    delete usersJson;
                                  }
                              },
                              [&res](const std::string& errorString, unsigned int errorNumber) -> void {
                                  VLOG(0) << "Error 3: " << errorString << " : " << errorNumber;
                              res.sendStatus(500);
                              });

                      });


    staticServer.use(express::middleware::StaticMiddleware(SERVERROOT));

    staticServer.listen(8080, [] (const express::legacy::in::WebApp::SocketAddress &socketAddress, int err) -> void {
        if (err != 0) {
            std::cerr << "Listen failed on port 8080 " << std:: endl;
        } else {
            std::cout << "ForumApp is listening on " << socketAddress.toString() << std::endl;
        }
    });

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
