#include "dbsetup.h"


DBSetup::DBSetup()
{
}

database::mariadb::MariaDBClient DBSetup::InitDB(){
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

    mariaDbClient.exec(
                "CREATE TABLE IF NOT EXISTS `users` (user_id int(11) auto_increment, username varchar(255), password varchar(255), primary key(user_id));",
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

    mariaDbClient.exec(
                "CREATE TABLE IF NOT EXISTS `subtopics` (subtopic_id int(11) auto_increment, title varchar(255), created_at timestamp default current_timestamp, userid int(11), primary key(subtopic_id), foreign key(userid) references users(user_id));",
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

    mariaDbClient.exec(
                "CREATE TABLE IF NOT EXISTS `threads` (thread_id int(11) auto_increment, title varchar(255), content varchar(255), created_at timestamp default current_timestamp, userid int(11), subtopicid int(11), primary key(thread_id), foreign key(userid) references users(user_id), foreign key(subtopicid) references subtopics(subtopic_id));",
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

    mariaDbClient.exec(
                "CREATE TABLE IF NOT EXISTS `posts` (post_id int(11) auto_increment, content varchar(255), created_at timestamp default current_timestamp, userid int(11), threadid int(11), primary key(post_id), foreign key(userid) references users(user_id), foreign key(threadid) references threads(thread_id));",
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

    mariaDbClient.exec(
                "CREATE TABLE IF NOT EXISTS `sessions` (session_id varchar(255), userid int(11), created_at timestamp default current_timestamp, primary key(userid), foreign key(userid) references users(user_id));",
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

    return mariaDbClient;
}

