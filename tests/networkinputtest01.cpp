

// ADD THIS AT THE ABSOLUTE TOP OF YOUR MAIN CPP FILE
#define STEAMNETWORKINGSOCKETS_STATIC_LINK

#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <queue>
#include <chrono>
#include <atomic>

// GameNetworkingSockets headers
#include <steam/steamnetworkingsockets.h>
#include <steam/isteamnetworkingsockets.h>
#include <steam/steamnetworkingsockets_flat.h>

// Global structures for console input extraction
std::mutex g_inputMutex;
std::queue<std::string> g_inputQueue;
std::atomic<bool> g_appRunning(true);

// Thread function dedicated strictly to catching user command-line strings
void ConsoleInputThread() {
    while (g_appRunning) {
        std::string line;
        // This blocks the background thread until user hits 'Enter'
        if (std::getline(std::cin, line)) { 
            std::lock_guard<std::mutex> lock(g_inputMutex);
            g_inputQueue.push(line);
        }
    }
}

// Global reference pointers for the Valve network interface
ISteamNetworkingSockets* g_pInterface = nullptr;
HSteamNetConnection g_hConnection = k_HSteamNetConnection_Invalid;

// Callback class to intercept network state mutations
class NetworkStatusCallbacks {
public:
    static void OnConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pInfo) {
        switch (pInfo->m_info.m_eState) {
            case k_ESteamNetworkingConnectionState_None: 
                break;
            case k_ESteamNetworkingConnectionState_Connected:
                std::cout << "\n[Net] Successfully connected to server!\n> " << std::flush;
                break;
            case k_ESteamNetworkingConnectionState_ClosedByPeer:
            case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
                std::cout << "\n[Net] Disconnected from remote host.\n> " << std::flush;
                g_pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
                break;
            default:
                break;
        }
    }
};

int main() {
    std::cout << "--- GameNetworkingSockets Console Input Test ---\n";
    std::cout << "Commands available: 'connect', 'send <msg>', 'quit'\n\n> ";

    // 1. Initialize the library infrastructure
    SteamNetworkingErrMsg errMsg;
    if (!GameNetworkingSockets_Init(nullptr, errMsg)) {
        std::cerr << "Initialization failed: " << errMsg << std::endl;
        return 1;
    }
    g_pInterface = SteamNetworkingSockets();

    // 2. Spawn the isolated background keyboard thread
    std::thread inputWorker(ConsoleInputThread);

    // 3. Main Non-Blocking Network & Input Dispatcher Loop
    while (g_appRunning) {
        // Run network callbacks and flush internal packet buffers
        g_pInterface->RunCallbacks();

        // Check for incoming packets non-blockingly
        if (g_hConnection != k_HSteamNetConnection_Invalid) {
            SteamNetworkingMessage_t* pIncomingMsg = nullptr;
            int numMsgs = g_pInterface->ReceiveMessagesOnConnection(g_hConnection, &pIncomingMsg, 1);
            if (numMsgs > 0) {
                std::cout << "\n[Received]: " << std::string((char*)pIncomingMsg->m_pData, pIncomingMsg->m_cbSize) << "\n> " << std::flush;
                pIncomingMsg->Release();
            }
        }

        // Check for non-blocking local console text entries
        std::string command = "";
        {
            std::lock_guard<std::mutex> lock(g_inputMutex);
            if (!g_inputQueue.empty()) {
                command = g_inputQueue.front();
                g_inputQueue.pop();
            }
        }

        // Parse extracted inputs
        if (!command.empty()) {
            if (command == "quit") {
                g_appRunning = false;
            } 
            else if (command == "connect") {
                std::cout << "[Net] Connecting to localhost:27015...\n";
                SteamNetworkingIPAddr serverAddr;
                serverAddr.Clear();
                serverAddr.ParseString("127.0.0.1:27015");
                
                SteamNetworkingConfigValue_t cfg;
                cfg.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, (void*)NetworkStatusCallbacks::OnConnectionStatusChanged);
                g_hConnection = g_pInterface->ConnectByIPAddress(serverAddr, 1, &cfg);
            } 
            else if (command.rfind("send ", 0) == 0) {
                std::string payload = command.substr(5);
                if (g_hConnection != k_HSteamNetConnection_Invalid) {
                    g_pInterface->SendMessageToConnection(g_hConnection, payload.data(), payload.size(), k_nSteamNetworkingSend_Reliable, nullptr);
                    std::cout << "[Sent]: " << payload << "\n";
                } else {
                    std::cout << "[Error] Not connected. Type 'connect' first.\n";
                }
            }
            std::cout << "> " << std::flush;
        }

        // Prevent high-frequency CPU spinning
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // Clean up connections
    if (g_hConnection != k_HSteamNetConnection_Invalid) {
        g_pInterface->CloseConnection(g_hConnection, 0, "App exit", true);
    }

    std::cout << "Terminating app context..." << std::endl;
    GameNetworkingSockets_Kill();

    // Safely detach or join the execution thread 
    g_appRunning = false;
    inputWorker.detach(); 

    return 0;
}
