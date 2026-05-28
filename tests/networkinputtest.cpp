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
// #include <steam/isteamnetworkingsockets.h>
#include <steam/steamnetworkingsockets_flat.h>

// ====================== GLOBALS ======================
std::mutex g_inputMutex;
std::queue<std::string> g_inputQueue;
std::atomic<bool> g_appRunning(true);

ISteamNetworkingSockets* g_pInterface = nullptr;
HSteamNetConnection g_hConnection = k_HSteamNetConnection_Invalid;
HSteamListenSocket g_hListenSocket = k_HSteamListenSocket_Invalid;

// ====================== CALLBACKS ======================
class NetworkStatusCallbacks {
public:
    static void OnConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* pInfo) {
        switch (pInfo->m_info.m_eState) {
            case k_ESteamNetworkingConnectionState_Connecting:
                if (g_hListenSocket != k_HSteamListenSocket_Invalid) {
                    // Accept incoming connection
                    g_pInterface->AcceptConnection(pInfo->m_hConn);
                }
                break;

            case k_ESteamNetworkingConnectionState_Connected:
                if (g_hListenSocket != k_HSteamListenSocket_Invalid) {
                    std::cout << "\n[Net] Client connected! Connection ID: " << pInfo->m_hConn << "\n> " << std::flush;
                    // You can store multiple connections if you want a real server
                    g_hConnection = pInfo->m_hConn;  // For simplicity, keep latest
                } else {
                    std::cout << "\n[Net] Successfully connected to server!\n> " << std::flush;
                }
                break;

            case k_ESteamNetworkingConnectionState_ClosedByPeer:
            case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
                std::cout << "\n[Net] Connection closed.\n> " << std::flush;
                if (pInfo->m_hConn == g_hConnection) {
                    g_hConnection = k_HSteamNetConnection_Invalid;
                }
                g_pInterface->CloseConnection(pInfo->m_hConn, 0, nullptr, false);
                break;

            default:
                break;
        }
    }
};

void ConsoleInputThread() {
    while (g_appRunning) {
        std::string line;
        if (std::getline(std::cin, line)) {
            std::lock_guard<std::mutex> lock(g_inputMutex);
            g_inputQueue.push(line);
        }
    }
}

int main() {
    std::cout << "--- GameNetworkingSockets Console Test ---\n";
    std::cout << "Commands: 'host', 'connect', 'send <msg>', 'quit'\n\n> ";

    SteamNetworkingErrMsg errMsg;
    if (!GameNetworkingSockets_Init(nullptr, errMsg)) {
        std::cerr << "Initialization failed: " << errMsg << std::endl;
        return 1;
    }
    g_pInterface = SteamNetworkingSockets();

    std::thread inputWorker(ConsoleInputThread);

    while (g_appRunning) {
        g_pInterface->RunCallbacks();

        // Receive messages
        if (g_hConnection != k_HSteamNetConnection_Invalid) {
            SteamNetworkingMessage_t* pMsg = nullptr;
            int numMsgs = g_pInterface->ReceiveMessagesOnConnection(g_hConnection, &pMsg, 1);
            if (numMsgs > 0 && pMsg) {
                std::cout << "\n[Received]: " << std::string((char*)pMsg->m_pData, pMsg->m_cbSize) 
                          << "\n> " << std::flush;
                pMsg->Release();
            }
        }

        // Process console input
        std::string command;
        {
            std::lock_guard<std::mutex> lock(g_inputMutex);
            if (!g_inputQueue.empty()) {
                command = g_inputQueue.front();
                g_inputQueue.pop();
            }
        }

        if (!command.empty()) {
            if (command == "quit") {
                g_appRunning = false;
            }
            else if (command == "host") {
                if (g_hListenSocket != k_HSteamListenSocket_Invalid) {
                    std::cout << "[Net] Already hosting.\n";
                } else {
                    SteamNetworkingIPAddr addr;
                    addr.Clear();
                    addr.SetIPv4(0, 27015);  // 0.0.0.0:27015

                    SteamNetworkingConfigValue_t cfg;
                    cfg.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, 
                              (void*)NetworkStatusCallbacks::OnConnectionStatusChanged);

                    g_hListenSocket = g_pInterface->CreateListenSocketIP(addr, 1, &cfg);
                    if (g_hListenSocket != k_HSteamListenSocket_Invalid) {
                        std::cout << "[Net] Server started on port 27015. Waiting for clients...\n";
                    } else {
                        std::cout << "[Error] Failed to create listen socket.\n";
                    }
                }
            }
            else if (command == "connect") {
                if (g_hListenSocket != k_HSteamListenSocket_Invalid) {
                    std::cout << "[Error] You are currently hosting. Use another terminal for client.\n";
                } else {
                    std::cout << "[Net] Connecting to 127.0.0.1:27015...\n";
                    SteamNetworkingIPAddr serverAddr;
                    serverAddr.Clear();
                    serverAddr.ParseString("127.0.0.1:27015");

                    SteamNetworkingConfigValue_t cfg;
                    cfg.SetPtr(k_ESteamNetworkingConfig_Callback_ConnectionStatusChanged, 
                              (void*)NetworkStatusCallbacks::OnConnectionStatusChanged);

                    g_hConnection = g_pInterface->ConnectByIPAddress(serverAddr, 1, &cfg);
                }
            }
            else if (command.rfind("send ", 0) == 0) {
                std::string payload = command.substr(5);
                if (g_hConnection != k_HSteamNetConnection_Invalid) {
                    g_pInterface->SendMessageToConnection(g_hConnection, payload.data(), 
                        payload.size(), k_nSteamNetworkingSend_Reliable, nullptr);
                    std::cout << "[Sent]: " << payload << "\n";
                } else {
                    std::cout << "[Error] Not connected. Use 'connect' or 'host' first.\n";
                }
            }
            else {
                std::cout << "[Unknown command]\n";
            }

            std::cout << "> " << std::flush;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // Cleanup
    if (g_hListenSocket != k_HSteamListenSocket_Invalid) {
        g_pInterface->CloseListenSocket(g_hListenSocket);
    }
    if (g_hConnection != k_HSteamNetConnection_Invalid) {
        g_pInterface->CloseConnection(g_hConnection, 0, "App shutdown", true);
    }

    GameNetworkingSockets_Kill();
    g_appRunning = false;
    if (inputWorker.joinable()) inputWorker.join();

    std::cout << "Terminated.\n";
    return 0;
}