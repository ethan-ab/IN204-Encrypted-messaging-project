#include <iostream>

#include "server/BoostServer.h"

int main() {

    BoostServer server = BoostServer();
    server.Listen( 8080 );
    server.Run( );

    return 0;
}

