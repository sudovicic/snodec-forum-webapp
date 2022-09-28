#include <iostream>

#include <express/legacy/in/WebApp.h>
#include <express/tls/in/WebApp.h>
#include <express/middleware/StaticMiddleware.h>
#include "database/mariadb/MariaDBClient.h"
#include "database/mariadb/MariaDBCommandSequence.h"
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <string>
#include "utils.h"
#include "dbsetup.h"

#define SERVERROOT "/home/student/snodec-forum-webapp/public"
#define PORT 1337

std::string getUserIdFromSession(database::mariadb::MariaDBClient &mariaDbClient, std::string sessionID);
bool checkUserName(database::mariadb::MariaDBClient &mariaDbClient, std::string username);
std::string getUsernameById(database::mariadb::MariaDBClient &mariaDbClient, std::string userId);

int main(int argc, char *argv[]) {
    express::legacy::in::WebApp::init(argc, argv);

    database::mariadb::MariaDBClient mariaDbClient = DBSetup::InitDB();

    express::legacy::in::WebApp staticServer("ForumApp");
    std::hash<std::string> hasher;

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
                          std::size_t password = std::hash<std::string>{}(Utils::GetFieldByName(req.body.data(), "password"));


                          if(checkUserName(mariaDbClient, Utils::GetFieldByName(req.body.data(), "username"))){
                              res.send("Username already in use");
                          } else {
                              mariaDbClient.exec(
                              "INSERT INTO `users`(`username`, `password`) VALUES ('" + Utils::GetFieldByName(req.body.data(), "username") + "','" + std::to_string(password) + "');",
                              [&mariaDbClient, &res, &req](void) -> void {
                                  VLOG(0) << "********** OnQuery 1: ";
                                  mariaDbClient.affectedRows(
                                  [&res, &mariaDbClient, &req](my_ulonglong affectedRows) -> void {
                                      VLOG(0) << "********** AffectedRows 2: " << affectedRows;
                                      std::string sessionCookie = Utils::Gen_random(16);
                                      res.cookie("sessionCookie", sessionCookie, {{"Max-Age", "60"}} );

                                      mariaDbClient.query(
                                      "SELECT user_id FROM users WHERE username = '" + Utils::GetFieldByName(req.body.data(), "username") + "';",
                                      [&res, &mariaDbClient, sessionCookie](const MYSQL_ROW row) -> void {
                                          std::string userId;
                                          if (row != nullptr) {
                                              userId = row[0];
                                              VLOG(0) << "Row Result 3: " << row[0];
                                          } else {
                                              VLOG(0) << "Row Result 3: " << userId;
                                              mariaDbClient.exec(
                                              "INSERT INTO `sessions`(`session_id`, `userid`) VALUES ('" + sessionCookie + "','" + "2" + "')",
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
                                          }
                                      },
                                      [&res](const std::string& errorString, unsigned int errorNumber) -> void {
                                          VLOG(0) << "Error 3: " << errorString << " : " << errorNumber;
                                          res.sendStatus(500);
                                      });

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
                          }
                      });

    staticServer.post("/login", [&mariaDbClient] APPLICATION(req, res) {
                          req.body.push_back(0);
                          std::cout << req.body.data() << std::endl;

                          std::string sessionCookie = std::string(Utils::Gen_random(16));
                          std::string userID;

                          std::size_t password = std::hash<std::string>{}(Utils::GetFieldByName(req.body.data(), "password"));
                          nlohmann::json* usersJson = new nlohmann::json;

                          mariaDbClient.query(
                          "SELECT * FROM users WHERE username = '" + Utils::GetFieldByName(req.body.data(), "username") + "'AND password='" + std::to_string(password) + "';",
                          [&res, usersJson, sessionCookie, &userID, &mariaDbClient](const MYSQL_ROW row) -> void {
                              if (row != nullptr) {
                                  usersJson->push_back({{"user_id", row[0]}, {"username", row[1]}});
                                  userID = row[0];
                                  VLOG(0) << "Output 1: " << userID;
                              } else {
                                  if(!usersJson->is_null()){
                                      userID = usersJson[0][0]["user_id"];
                                      res.cookie("sessionCookie", sessionCookie, {{"Max-Age", "60"}} );
                                      mariaDbClient.exec(
                                      "INSERT INTO `sessions`(`session_id`, `userid`) VALUES ('" + sessionCookie + "','" + "1" + "')",
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
                                      res.send(usersJson->dump());
                                  } else {
                                      res.send("No valid User found with this credentials!");
                                  }
                                  delete usersJson;
                              }
                          },
                          [&res](const std::string& errorString, unsigned int errorNumber) -> void {
                              VLOG(0) << "Error 3: " << errorString << " : " << errorNumber;
                              res.sendStatus(500);
                          });

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
                          std::string sessionCookie = req.cookie("sessionCookie");

                          if(sessionCookie.empty()){
                              res.sendStatus(500);
                          } else {
                              std::string userid = getUserIdFromSession(mariaDbClient, sessionCookie);

                              std::cout << userid << std::endl;
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
                          }
                      });

    staticServer.get("/subtopic/:subtopicid/threads", [&mariaDbClient] APPLICATION(req, res) {
                         nlohmann::json* threadsJson = new nlohmann::json;
                         mariaDbClient.query(
                         "SELECT * FROM threads WHERE subtopicid = '" + req.params["subtopicid"] + "';",
                         [&res, threadsJson](const MYSQL_ROW row) -> void {
                             if (row != nullptr) {
                                 threadsJson->push_back({{"thread_id", row[0]},{"title", row[1]}, {"content", row[2]}, {"created_at", row[3]}, {"userid", row[4]}, {"subtopicid", row[5]}});
                                 VLOG(0) << "Row Result 3: " << row[0] << " : " << row[1];
                             } else {
                                 VLOG(0) << "Row Result 3: " << threadsJson->dump();
                                 res.send(threadsJson->dump());
                                 delete threadsJson;
                             }
                         },
                         [&res](const std::string& errorString, unsigned int errorNumber) -> void {
                             VLOG(0) << "Error 3: " << errorString << " : " << errorNumber;
                             res.sendStatus(500);
                         });

                     });

    staticServer.post("/subtopic/:subtopicid/threads/new", [&mariaDbClient] APPLICATION(req, res) {
                          req.body.push_back(0);
                          std::cout << req.body.data() << std::endl;
                          mariaDbClient.exec(
                          "INSERT INTO `threads`(`title`, `content`,`userid`, `subtopicid`) VALUES ('" + Utils::GetFieldByName(req.body.data(), "title") + "','" + Utils::GetFieldByName(req.body.data(), "content") + "','1','" + req.params["subtopcid"] + "');",
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

    staticServer.get("/subtopic/:subtopicid/threads/:threadid/posts", [&mariaDbClient] APPLICATION(req, res) {
                         nlohmann::json* postsJson = new nlohmann::json;
                         mariaDbClient.query(
                         "SELECT * FROM posts WHERE threadid = '" + req.params["threadid"] + "';",
                         [&res, postsJson](const MYSQL_ROW row) -> void {
                             if (row != nullptr) {
                                 postsJson->push_back({{"post_id", row[0]}, {"content", row[1]}, {"created_at", row[2]}, {"userid", row[3]}, {"threadid", row[4]}});
                                 VLOG(0) << "Row Result 3: " << row[0] << " : " << row[1];
                             } else {
                                 VLOG(0) << "Row Result 3: " << postsJson->dump();
                                 res.send(postsJson->dump());
                                 delete postsJson;
                             }
                         },
                         [&res](const std::string& errorString, unsigned int errorNumber) -> void {
                             VLOG(0) << "Error 3: " << errorString << " : " << errorNumber;
                             res.sendStatus(500);
                         });

                     });

    staticServer.post("/subtopic/:subtopicid/threads/:threadid/posts/new", [&mariaDbClient] APPLICATION(req, res) {
                          req.body.push_back(0);
                          std::cout << req.body.data() << std::endl;
                          mariaDbClient.exec(
                          "INSERT INTO `posts`(`content`,`userid`,`subtopicid`) VALUES ('" + Utils::GetFieldByName(req.body.data(), "content") + "','1','" + req.params["threadid"] + "');",
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

    staticServer.post("/logout", [&mariaDbClient] APPLICATION(req, res) {
                          std::string sessionCookie = req.cookie("sessionCookie");
                          if(sessionCookie.empty()){
                              res.sendStatus(500);
                          } else {
                              mariaDbClient.exec(           "DELETE FROM `sessions` WHERE session_id ='" + sessionCookie + "';",
                              [&mariaDbClient](void) -> void {
                                  VLOG(0) << "********** OnQuery 0;";
                                  mariaDbClient.affectedRows(
                                  [](my_ulonglong affectedRows) -> void {
                                      VLOG(0) << "********** AffectedRows 1: " << affectedRows;
                                  },
                                  [](const std::string& errorString, unsigned int errorNumber) -> void {
                                      VLOG(0) << "Error 1: " << errorString << " : " << errorNumber;
                                  });
                              },
                              [](const std::string& errorString, unsigned int errorNumber) -> void {
                                  VLOG(0) << "********** Error 0: " << errorString << " : " << errorNumber;
                              });
                          }
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

std::string getUserIdFromSession(database::mariadb::MariaDBClient &mariaDbClient, std::string sessionID){
    std::string userId;
    mariaDbClient.query(
                "SELECT userid FROM sessions WHERE session_id = '" + sessionID + "';",
                [&userId](const MYSQL_ROW row) -> void {
        if (row != nullptr) {
            VLOG(0) << "Row Result 3: " << row[0];
            userId = row[0];
        } else {
            VLOG(0) << "Row Result 3: " << userId;
        }
    },
    [](const std::string& errorString, unsigned int errorNumber) -> void {
        VLOG(0) << "Error 3: " << errorString << " : " << errorNumber;
    });

    return userId;

}


bool checkUserName(database::mariadb::MariaDBClient &mariaDbClient, std::string username){
    bool found = false;
    mariaDbClient.query(
                "SELECT * FROM users WHERE username = '" + username + "';",
                [&found](const MYSQL_ROW row) -> void {
        if (row != nullptr) {
            VLOG(0) << "Row Result 3: " << row[0];
            found = true;
        } else {
            VLOG(0) << "Row Result 3: ";
        }
    },
    [](const std::string& errorString, unsigned int errorNumber) -> void {
        VLOG(0) << "Error 3: " << errorString << " : " << errorNumber;
    });
    return found;
}

std::string getUsernameById(database::mariadb::MariaDBClient &mariaDbClient, std::string userId){
    std::string username;
    mariaDbClient.query(
                "SELECT username FROM users WHERE user_id = '" + userId + "';",
                [&username](const MYSQL_ROW row) -> void {
        if (row != nullptr) {
            VLOG(0) << "Row Result 3: " << row[0];
            username = row[0];
        } else {
            VLOG(0) << "Row Result 3: ";
        }
    },
    [](const std::string& errorString, unsigned int errorNumber) -> void {
        VLOG(0) << "Error 3: " << errorString << " : " << errorNumber;
    });
    return username;
}

