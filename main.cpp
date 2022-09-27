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

                          bool* found = new bool;
                          *found = false;
                          mariaDbClient.query(
                          "SELECT * FROM users WHERE username = '" + Utils::GetFieldByName(req.body.data(), "username") + "';",
                          [found, &mariaDbClient, &res, &req, password](const MYSQL_ROW row) -> void {
                              if (row != nullptr) {
                                  VLOG(0) << "Row Result 3: " << row[0];
                                  *found = true;
                              } else {
                                  VLOG(0) << "Row Result 3: ";
                                  if(*found){
                                      VLOG(0) << "Username already in use";
                                      res.send("Username already in use");
                                      delete found;
                                  } else {
                                      mariaDbClient.exec(
                                      "INSERT INTO `users`(`username`, `password`) VALUES ('" + Utils::GetFieldByName(req.body.data(), "username") + "','" + std::to_string(password) + "');",
                                      [&mariaDbClient, &res, &req](void) -> void {
                                          VLOG(0) << "********** OnQuery 1: ";
                                          mariaDbClient.affectedRows(
                                          [&res, &mariaDbClient, &req](my_ulonglong affectedRows) -> void {
                                              VLOG(0) << "********** AffectedRows 2: " << affectedRows;
                                              std::string sessionCookie = Utils::Gen_random(16);
                                              std::string* userId = new std::string();
                                              mariaDbClient.query(
                                              "SELECT user_id FROM users WHERE username = '" + Utils::GetFieldByName(req.body.data(), "username") + "';",
                                              [&res, &mariaDbClient, sessionCookie, userId](const MYSQL_ROW row) -> void {
                                                  if (row != nullptr) {
                                                      *userId= row[0];
                                                      VLOG(0) << "Row Result 3: " << *userId;
                                                  } else {
                                                      VLOG(0) << "User ID: " << *userId;
                                                      mariaDbClient.exec(
                                                      "INSERT INTO `sessions`(`session_id`, `userid`) VALUES ('" + sessionCookie + "','" + *userId + "')",
                                                      [&mariaDbClient, &res, sessionCookie, userId](void) -> void {
                                                          VLOG(0) << "********** OnQuery 1: ";
                                                          mariaDbClient.affectedRows(
                                                          [&res, sessionCookie, userId](my_ulonglong affectedRows) -> void {
                                                              VLOG(0) << "********** AffectedRows 2: " << affectedRows;
                                                              res.cookie("sessionCookie", sessionCookie, {{"Max-Age", "3600"}} );
                                                              res.sendStatus(200);
                                                              delete userId;
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
                                      delete found;
                                  }
                              }
                          },
                          [](const std::string& errorString, unsigned int errorNumber) -> void {
                              VLOG(0) << "Error 3: " << errorString << " : " << errorNumber;
                          });



                      });

    staticServer.post("/login", [&mariaDbClient] APPLICATION(req, res) {
                          req.body.push_back(0);
                          std::cout << req.body.data() << std::endl;

                          std::string sessionCookie = std::string(Utils::Gen_random(16));
                          std::size_t password = std::hash<std::string>{}(Utils::GetFieldByName(req.body.data(), "password"));
                          nlohmann::json* usersJson = new nlohmann::json;
                          std::string* userId = new std::string();
                          mariaDbClient.query(
                          "SELECT * FROM users WHERE username = '" + Utils::GetFieldByName(req.body.data(), "username") + "'AND password='" + std::to_string(password) + "';",
                          [&res, usersJson, sessionCookie, &mariaDbClient, userId](const MYSQL_ROW row) -> void {
                              if (row != nullptr) {
                                  usersJson->push_back({{"user_id", row[0]}, {"username", row[1]}});
                                  VLOG(0) << "UserId: " << usersJson->dump();
                                  *userId = row[0];
                                  VLOG(0) << "UserId: " << row[0];
                              } else {
                                  if(!usersJson->is_null()){
                                      VLOG(0) << "UserId: " << *userId;
                                      mariaDbClient.exec(
                                      "INSERT INTO `sessions`(`session_id`, `userid`) VALUES ('" + sessionCookie + "','" + *userId + "')",
                                      [&mariaDbClient, &res, sessionCookie, userId, usersJson](void) -> void {
                                          VLOG(0) << "********** OnQuery 1: ";
                                          mariaDbClient.affectedRows(
                                          [&res, sessionCookie, userId, usersJson](my_ulonglong affectedRows) -> void {
                                              VLOG(0) << "********** AffectedRows 2: " << affectedRows;
                                              res.cookie("sessionCookie", sessionCookie, {{"Max-Age", "3600"}} );
                                              res.send(usersJson->dump());
                                              delete usersJson;
                                              delete userId;

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
                                  } else {
                                      VLOG(0) << "No valid User found";
                                      res.send("No valid User found with this credentials!");
                                  }

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
                         "SELECT subtopics.subtopic_id, subtopics.title, subtopics.created_at, subtopics.userid, users.username FROM subtopics LEFT JOIN users ON subtopics.userid = users.user_id",
                         [&res, subTopicsJson, &mariaDbClient](const MYSQL_ROW row) -> void {
                             if (row != nullptr) {
                                 VLOG(0) << "Username: " << row[4];
                                 std::string subtopic_id = row[0];
                                 std::string title = row[1];
                                 std::string created_at = row[2];
                                 std::string userId = row[3];

                                 subTopicsJson->push_back({{"subtopic_id", subtopic_id},{"title", title}, {"created_at", created_at}, {"userid", userId}, {"username", row[4]}});

                                 VLOG(0) << "userId: " << subTopicsJson->dump(4);

                             } else {
                                 res.send(subTopicsJson->dump(4));
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
                              res.sendStatus(403);
                          } else {
                              std::string* userId = new std::string();
                              mariaDbClient.query(
                              "SELECT userid FROM sessions WHERE session_id = '" + sessionCookie + "';",
                              [userId, &req, &res, &mariaDbClient](const MYSQL_ROW row) -> void {
                                  if (row != nullptr) {
                                      VLOG(0) << "Row Result 3: " << row[0];
                                      *userId = row[0];
                                  } else {
                                      VLOG(0) << "Row Result 3: " << *userId;
                                      std::cout << *userId << std::endl;
                                      req.body.push_back(0);
                                      std::cout << req.body.data() << std::endl;
                                      std::cout << Utils::GetFieldByName(req.body.data(), "title") << std::endl;

                                      mariaDbClient.exec(
                                      "INSERT INTO `subtopics`(`title`, `userid`) VALUES ('" + Utils::GetFieldByName(req.body.data(), "title") + "','" + *userId + "')",
                                      [&mariaDbClient, &res, userId](void) -> void {
                                          VLOG(0) << "********** OnQuery 1: ";
                                          mariaDbClient.affectedRows(
                                          [&res, userId](my_ulonglong affectedRows) -> void {
                                              VLOG(0) << "********** AffectedRows 2: " << affectedRows;
                                              res.sendStatus(200);
                                              delete userId;
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
                              [](const std::string& errorString, unsigned int errorNumber) -> void {
                                  VLOG(0) << "Error 3: " << errorString << " : " << errorNumber;
                              });

                          }
                      });

    staticServer.get("/subtopic/:subtopicid/threads", [&mariaDbClient] APPLICATION(req, res) {
                         nlohmann::json* threadsJson = new nlohmann::json;
                         mariaDbClient.query(
                         "SELECT threads.thread_id, threads.title, threads.content, threads.created_at, threads.userid, threads.subtopicid, users.username FROM threads LEFT JOIN users ON threads.userid = users.user_id WHERE subtopicid = '" + req.params["subtopicid"] + "';",
                         [&res, threadsJson](const MYSQL_ROW row) -> void {
                             if (row != nullptr) {
                                 threadsJson->push_back({{"thread_id", row[0]},{"title", row[1]}, {"content", row[2]}, {"created_at", row[3]}, {"userid", row[4]}, {"subtopicid", row[5]}, {"username", row[6]}});
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
                          std::string sessionCookie = req.cookie("sessionCookie");
                          if(sessionCookie.empty()){
                              res.sendStatus(403);
                          } else {
                              std::string* userId = new std::string();
                              mariaDbClient.query(
                              "SELECT userid FROM sessions WHERE session_id = '" + sessionCookie + "';",
                              [userId, &req, &res, &mariaDbClient](const MYSQL_ROW row) -> void {
                                  if (row != nullptr) {
                                      VLOG(0) << "Row Result 3: " << row[0];
                                      *userId = row[0];
                                  } else {
                                      VLOG(0) << "Row Result 3: " << *userId;
                                      std::cout << *userId << std::endl;
                                      req.body.push_back(0);
                                      std::cout << req.body.data() << std::endl;

                                      mariaDbClient.exec(
                                      "INSERT INTO `threads`(`title`, `content`,`userid`, `subtopicid`) VALUES ('" + Utils::GetFieldByName(req.body.data(), "title") + "','" + Utils::GetFieldByName(req.body.data(), "content") + "','" + *userId + "','" + req.params["subtopicid"] + "');",
                                      [&mariaDbClient, &res, userId](void) -> void {
                                          VLOG(0) << "********** OnQuery 1: ";
                                          mariaDbClient.affectedRows(
                                          [&res, userId](my_ulonglong affectedRows) -> void {
                                              VLOG(0) << "********** AffectedRows 2: " << affectedRows;
                                              res.sendStatus(200);
                                              delete userId;
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
                              [](const std::string& errorString, unsigned int errorNumber) -> void {
                                  VLOG(0) << "Error 3: " << errorString << " : " << errorNumber;
                              });
                          }
                      });

    staticServer.get("/subtopic/:subtopicid/threads/:threadid/posts", [&mariaDbClient] APPLICATION(req, res) {
                         nlohmann::json* postsJson = new nlohmann::json;
                         mariaDbClient.query(
                         "SELECT posts.post_id, posts.content, posts.created_at, posts.userid, posts.threadid, users.username FROM posts LEFT JOIN users ON posts.userid=users.user_id WHERE threadid = '" + req.params["threadid"] + "';",
                         [&res, postsJson](const MYSQL_ROW row) -> void {
                             if (row != nullptr) {
                                 postsJson->push_back({{"post_id", row[0]}, {"content", row[1]}, {"created_at", row[2]}, {"userid", row[3]}, {"threadid", row[4]}, {"username", row[5]}});
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
                          std::string sessionCookie = req.cookie("sessionCookie");
                          if(sessionCookie.empty()){
                              res.sendStatus(403);
                          } else {
                              std::string* userId = new std::string();
                              mariaDbClient.query(
                              "SELECT userid FROM sessions WHERE session_id = '" + sessionCookie + "';",
                              [userId, &req, &res, &mariaDbClient](const MYSQL_ROW row) -> void {
                                  if (row != nullptr) {
                                      VLOG(0) << "Row Result 3: " << row[0];
                                      *userId = row[0];
                                  } else {
                                      VLOG(0) << "Row Result 3: " << *userId;
                                      std::cout << *userId << std::endl;
                                      req.body.push_back(0);
                                      std::cout << req.body.data() << std::endl;

                                      mariaDbClient.exec(
                                      "INSERT INTO `posts`(`content`,`userid`,`threadid`) VALUES ('" + Utils::GetFieldByName(req.body.data(), "content") + "','" + *userId + "','" + req.params["threadid"] + "');",
                                      [&mariaDbClient, &res, userId](void) -> void {
                                          VLOG(0) << "********** OnQuery 1: ";
                                          mariaDbClient.affectedRows(
                                          [&res, userId](my_ulonglong affectedRows) -> void {
                                              VLOG(0) << "********** AffectedRows 2: " << affectedRows;
                                              res.sendStatus(200);
                                              delete userId;
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
                              [](const std::string& errorString, unsigned int errorNumber) -> void {
                                  VLOG(0) << "Error 3: " << errorString << " : " << errorNumber;
                              });
                          }

                      });

    staticServer.get("/logout", [&mariaDbClient] APPLICATION(req, res) {
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



