#include <iostream>
#include <cstring>
#include <cstdint>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

#pragma comment(lib, "Ws2_32.lib")

// Define the telemetry packet structure
struct TelemetryPacket {
    char apiMode[4];
    uint32_t version;
    char game[50];
    char vehicleName[50];
    char location[50];
    float speed;
    float rpm;
    float maxRpm;
    int gear;
    float pitch;
    float roll;
    float yaw;
    float lateralVelocity;
    float lateralAcceleration;
    float verticalAcceleration;
    float longitudinalAcceleration;
    float suspensionTravelFrontLeft;
    float suspensionTravelFrontRight;
    float suspensionTravelRearLeft;
    float suspensionTravelRearRight;
    uint32_t wheelTerrainFrontLeft;
    uint32_t wheelTerrainFrontRight;
    uint32_t wheelTerrainRearLeft;
    uint32_t wheelTerrainRearRight;
};

int main() {
    // Initialize Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed with error: " << result << std::endl;
        return 1;
    }

    // Constants
    const char API_MODE[] = "api";
    const uint32_t API_VERSION = 102;
    const char game[] = "Project Cars 2";
    const char vehicle[] = "Lamborghini Huracan";
    const char location[] = "Circuit Gilles-Villeneuve";

    // Socket setup
    SOCKET udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udpSocket == INVALID_SOCKET) {
        std::cerr << "Error: Could not create socket\n";
        WSACleanup();
        return 1;
    }

    BOOL broadcastEnable = TRUE;
    if (setsockopt(udpSocket, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<const char*>(&broadcastEnable), sizeof(broadcastEnable)) == SOCKET_ERROR) {
        std::cerr << "Error: Could not enable broadcast\n";
        closesocket(udpSocket);
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(33001);
    serverAddress.sin_addr.s_addr = htonl(INADDR_BROADCAST); // Broadcast address

    // Variables initialization
    float maxRpm = 8000;
    double speed = 0;
    float rpm = 0;
    int gear = 0;
    float pitch = 20;
    float roll = -20;
    float yaw = 0;
    float lateralVelocity = 0;
    float lateralAcceleration = 0;
    float verticalAcceleration = 0;
    float longitudinalAcceleration = 0;
    float suspensionTravelFrontLeft = 0;
    float suspensionTravelFrontRight = 0;
    float suspensionTravelRearLeft = 0;
    float suspensionTravelRearRight = 0;
    uint32_t wheelTerrainFrontLeft = 0;
    uint32_t wheelTerrainFrontRight = 0;
    uint32_t wheelTerrainRearLeft = 0;
    uint32_t wheelTerrainRearRight = 0;
    double speedIncrement = 0.2;

    while (true) {
        if (speed > 200) {
            speed = 200;
            speedIncrement = -0.2;
        }

        if (speed < 0) {
            speed = 0;
            speedIncrement = 0.2;
        }

        if (rpm > maxRpm)
            rpm = 0;

        if (gear > 9)
            gear = 0;

        // Populate telemetry packet
        TelemetryPacket tp;
        strncpy_s(tp.apiMode, API_MODE, sizeof(tp.apiMode));
        tp.version = API_VERSION;
        strncpy_s(tp.game, game, sizeof(tp.game));
        strncpy_s(tp.vehicleName, vehicle, sizeof(tp.vehicleName));
        strncpy_s(tp.location, location, sizeof(tp.location));
        tp.speed = speed;
        tp.rpm = rpm;
        tp.maxRpm = maxRpm;
        tp.gear = gear;
        tp.pitch = pitch;
        tp.roll = roll;
        tp.yaw = yaw;
        tp.lateralVelocity = lateralVelocity;
        tp.lateralAcceleration = lateralAcceleration;
        tp.verticalAcceleration = verticalAcceleration;
        tp.longitudinalAcceleration = longitudinalAcceleration;
        tp.suspensionTravelFrontLeft = suspensionTravelFrontLeft;
        tp.suspensionTravelFrontRight = suspensionTravelFrontRight;
        tp.suspensionTravelRearLeft = suspensionTravelRearLeft;
        tp.suspensionTravelRearRight = suspensionTravelRearRight;
        tp.wheelTerrainFrontLeft = wheelTerrainFrontLeft;
        tp.wheelTerrainFrontRight = wheelTerrainFrontRight;
        tp.wheelTerrainRearLeft = wheelTerrainRearLeft;
        tp.wheelTerrainRearRight = wheelTerrainRearRight;

        // Send telemetry packet
        int bytesSent = sendto(udpSocket, reinterpret_cast<const char*>(&tp), sizeof(tp), 0, reinterpret_cast<const sockaddr*>(&serverAddress), sizeof(serverAddress));
        if (bytesSent == SOCKET_ERROR) {
            std::cerr << "Error: sendto failed with error: " << WSAGetLastError() << std::endl;
            closesocket(udpSocket);
            WSACleanup();
            return 1;
        }

        std::cout << "sending... " << tp.apiMode << " " << tp.version << " " << tp.rpm << " " << tp.speed << std::endl;

        // Increment variables
        gear++;
        rpm += 50;
        speed += speedIncrement;

        // Sleep for a short duration
        Sleep(30);
    }

    closesocket(udpSocket);
    WSACleanup();

    return 0;
}
