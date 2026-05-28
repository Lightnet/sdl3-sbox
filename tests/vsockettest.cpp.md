#include <stdio.h>
#include <steam/steamnetworkingsockets.h>
#include <steam/steamnetworkingsockets_flat.h>

int main() {
    SteamDatagramErrMsg errMsg;
    
    // Initialize the library
    if (!GameNetworkingSockets_Init(NULL, errMsg)) {
        printf("Failed to init sockets: %s\n", errMsg);
        return 1;
    }

    // Grab a pointer to the global flat interface
    ISteamNetworkingSockets* sockets_interface = SteamNetworkingSockets_v2();
    
    if (sockets_interface) {
        printf("Valve Network Sockets running successfully in pure C!\n");
        
        // Example C call structure:
        // SteamAPI_ISteamNetworkingSockets_CreateListenSocketIP(sockets_interface, ...)
    }

    // Clean up before exit
    GameNetworkingSockets_Kill();
    return 0;
}
