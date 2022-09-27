#include <iostream>

#include <express/legacy/in/WebApp.h>
#include <express/tls/in/WebApp.h>
#include <express/middleware/StaticMiddleware.h>
#include "database/mariadb/MariaDBClient.h"
#include "database/mariadb/MariaDBCommandSequence.h"
#include <nlohmann/json.hpp>
#include "utils.h"
#include "dbsetup.h"

#define SERVERROOT "/home/student/snodec-forum-webapp/public"
#define PORT 1337

int main(int argc, char *argv[]) {
    express::legacy::in::WebApp::init(argc, argv);

    database::mariadb::MariaDBClient mariaDbClient = DBSetup::InitDB();

    express::legacy::in::WebApp staticServer("ForumApp");

    staticServer.get("/users", [&mariaDbClient] APPLICATION(req, res) {
                         std::cout << "Cookie-Value of \"SessionCookie\": "<< req.cookie("sessionCookie")<<std::endl;
                         nlohmann::json* usersJson = new nlohmann::json;
                         mariaDbClient.query(
                         "SELECT user_id, username, password FROM users",
                         [&res, usersJson](const MYSQL_ROW row) -> void {
                             if (row != nullptr) {
                                 usersJson->push_back({{"user_id", row[0]},{"username", row[1]}, {"password", row[2]}});
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
                          std::string sessionCookie = req.cookie("sessionCookie");

                          /*
                           * Idea: If no cookie is present then create a SessionID
                           * and save sessionID with userID in sessions table
                           *
                           * Else if cookie is present, then check if sessionId is in table
                           * if so then user is logged in if not user is logged out
                           *
                           * If user is in sessions table but sessionId does not correspond
                           * then delete entry in sessions table
                           */

                          if(sessionCookie.empty()) {
                              req.body.push_back(0);
                              std::cout << req.body.data() << std::endl;

                              std::string sessionID = std::string(Utils::Gen_random(16));

                              std::string userID = "-1";

                              nlohmann::json* usersJson = new nlohmann::json;

                              mariaDbClient.query(
                              "SELECT * FROM users WHERE username = '" + Utils::GetFieldByName(req.body.data(), "username") + "'AND password='" + Utils::GetFieldByName(req.body.data(), "password") + "';",
                              [&res, usersJson, sessionID, &userID](const MYSQL_ROW row) -> void {
                                  if (row != nullptr) {
                                      usersJson->push_back({{"user_id", row[0]}, {"username", row[1]}});

                                      // Expected output: 1, Result: /���
                                      userID = row[0];
                                      VLOG(0) << "Output 1: " << userID;

                                  } else {
                                      if(!usersJson->is_null()){
                                          userID = usersJson[0][0]["user_id"];
                                          res.cookie("sessionCookie", sessionID, {{"Max-Age", "2"}} );
                                          res.send(usersJson->dump());
                                      } else {
                                          res.send("No valid User found with this credentials!");
                                      }
                                      // Expected output: 1, Result: /���
                                      VLOG(0) << "Output 2: " << userID;
                                      delete usersJson;
                                  }
                              },
                              [&res](const std::string& errorString, unsigned int errorNumber) -> void {
                                  VLOG(0) << "Error 3: " << errorString << " : " << errorNumber;
                                  res.sendStatus(500);
                              });

                                // Expected output: 1, Result: -1 (default value)
                              VLOG(0) << "Output 3: " << userID;
/*
                              mariaDbClient.exec(
                              "INSERT INTO `sessions`(`session_id`, `userid`) VALUES ('" + sessionID + "', '" + userID + "' )",
                              [&mariaDbClient, &res, sessionID, userID](void) -> void {

                                  VLOG(0) << "Error 3: " << "INSERT INTO `sessions`(`session_id`, `userid`) VALUES ('" + sessionID + "', '" + userID + "' )";
                                  VLOG(0) << "********** OnQuery 1: ";
                                  mariaDbClient.affectedRows(
                                  [&res](my_ulonglong affectedRows) -> void {
                                      VLOG(0) << "********** AffectedRows 2: " << affectedRows;
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
                                                        */
                          }


                      });

    staticServer.get("/subtopics", [&mariaDbClient] APPLICATION(req, res) {
                         nlohmann::json* subTopicsJson = new nlohmann::json;
                         mariaDbClient.query(
                         "SELECT * FROM subtopics",
                         [&res, subTopicsJson](const MYSQL_ROW row) -> void {
                             if (row != nullptr) {
                                 subTopicsJson->push_back({{"subtopic_id", row[0]},{"title", row[1]}, {"created_at", row[2]}, {"userid", row[3]}});
                                 VLOG(0) << "Row Result 3: " << row[0] << " : " << row[1];

                             } else {
                                 VLOG(0) << "Row Result 3: " << subTopicsJson->dump();
                                 res.send(subTopicsJson->dump());
                                 delete subTopicsJson;
                             }
                         },
                         [&res](const std::string& errorString, unsigned int errorNumber) -> void {
                             VLOG(0) << "Error 3: " << errorString << " : " << errorNumber;
                             res.sendStatus(500);
                         });

                     });

    staticServer.post("/subtopics/new", [&mariaDbClient] APPLICATION(req, res) {

                          req.body.push_back(0);
                          std::cout << req.body.data() << std::endl;
                          std::cout << Utils::GetFieldByName(req.body.data(), "title") << std::endl;

                          mariaDbClient.exec(
                          "INSERT INTO `subtopics`(`title`, `userid`) VALUES ('" + Utils::GetFieldByName(req.body.data(), "title") + "','1')",
                          [&mariaDbClient, &res](void) -> void {
                              VLOG(0) << "********** OnQuery 1: ";
                              mariaDbClient.affectedRows(
                              [&res](my_ulonglong affectedRows) -> void {
                                  VLOG(0) << "********** AffectedRows 2: " << affectedRows;
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


    staticServer.use(express::middleware::StaticMiddleware(SERVERROOT));

    staticServer.listen(8080, [] (const express::legacy::in::WebApp::SocketAddress &socketAddress, int err) -> void {
        if (err != 0) {
            std::cerr << "Listen failed on port 8080 " << std:: endl;
        } else {
            std::cout << "ForumApp is listening on " << socketAddress.toString() << std::endl;
        }
    });

    return express::WebApp::start();
}
