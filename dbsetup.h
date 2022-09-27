#ifndef DBSETUP_H
#define DBSETUP_H

#include "database/mariadb/MariaDBClient.h"
#include "database/mariadb/MariaDBCommandSequence.h"
#include <iostream>
#include <express/legacy/in/WebApp.h>
#include <express/tls/in/WebApp.h>
#include <express/middleware/StaticMiddleware.h>

class DBSetup
{
public:
    DBSetup();
    static database::mariadb::MariaDBClient InitDB();
};

#endif // DBSETUP_H
