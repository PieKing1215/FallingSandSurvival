
#include "Networking.hpp"

bool Networking::init() {
    if(enet_initialize() != 0) {
        logError("An error occurred while initializing ENet.");
        return false;
    }
    atexit(enet_deinitialize);
    return true;
}

Server* Server::start(enet_uint16 port) {
    Server* server = new Server();
    server->address = ENetAddress();
    server->address.host = ENET_HOST_ANY;
    //enet_address_set_host_ip(&server->address, "172.23.16.150");
    server->address.port = port;

    server->server = enet_host_create(&server->address,
        32, // max client count
        2,  // number of channels
        0,  // unlimited incoming bandwidth
        0); // unlimited outgoing bandwidth

    if(server->server == NULL) {
        logError("An error occurred while trying to create an ENet server host.");
        delete server;
        return NULL;
    } else {
        char ch[20];
        enet_address_get_host_ip(&server->server->address, ch, 20);
        logInfo("[SERVER] Started on {}:{0:d}.", ch, server->server->address.port);
    }


    return server;
}

void Server::tick() {
    ENetEvent event;
    // poll for events
    while(enet_host_service(server, &event, 0) > 0) {
        switch(event.type) {
        case ENET_EVENT_TYPE_CONNECT:
            logInfo("[SERVER] A new client connected from {0:x}:{0:d}.",
                event.peer->address.host,
                event.peer->address.port);

            // arbitrary client data
            event.peer->data = (void*)"Client data";
            enet_peer_timeout(event.peer, 16, 3000, 10000);

            break;
        case ENET_EVENT_TYPE_RECEIVE:
            logDebug("[SERVER] A packet of length {0:d} containing {} was received from {} on channel {0:d}.",
                event.packet->dataLength,
                event.packet->data,
                event.peer->data,
                event.channelID);

            // done using packet
            enet_packet_destroy(event.packet);

            break;
        case ENET_EVENT_TYPE_DISCONNECT:
            logDebug("[SERVER] {} disconnected.", event.peer->data);

            // clear arbitrary data
            event.peer->data = NULL;
        }
    }
    //logDebug("[SERVER] done tick");
}

Server::~Server() {
    if(server != NULL) enet_host_destroy(server);
}

Client* Client::start() {

    Client* client = new Client();

    client->client = enet_host_create(NULL, // NULL means to make a client
        1,  // number of connections
        2,  // number of channels
        0,  // unlimited incoming bandwidth
        0); // unlimited outgoing bandwidth

    if(client->client == NULL) {
        logError("An error occurred while trying to create an ENet client host.");
        delete client;
        return NULL;
    }

    return client;
}

bool Client::connect(const char* ip, enet_uint16 port) {
    ENetEvent event;

    enet_address_set_host(&address, ip);
    //address.host = ENET_HOST_BROADCAST;
    address.port = port;

    peer = enet_host_connect(client, &address, 2, 0);
    if(peer == NULL) {
        logError("[CLIENT] No available peers for initiating an ENet connection.");

        return false;
    }

    // wait for connection to succeed
    if(enet_host_service(client, &event, 2000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT) {
        char ch[20];
        enet_address_get_host_ip(&peer->address, ch, 20);
        logInfo("[CLIENT] Connection to {}:{0:d} succeeded.", ch, peer->address.port);

        return true;
    } else {
        /* Either the 5 seconds are up or a disconnect event was */
        /* received. Reset the peer in the event the 5 seconds   */
        /* had run out without any significant event.            */
        enet_peer_reset(peer);
        char ch[20];
        enet_address_get_host_ip(&address, ch, 20);
        logError("[CLIENT] Connection to {}:{0:d} failed.", ch, address.port);
    }

    return false;
}

void Client::disconnect() {
    if(peer != NULL) enet_peer_disconnect(peer, 0);
}

void Client::tick() {
    ENetEvent event;

    // poll for events
    while(enet_host_service(client, &event, 0) > 0) {
        switch(event.type) {
        case ENET_EVENT_TYPE_CONNECT:
            logInfo("[CLIENT] Connected to server at {0:x}:{0:d}.",
                event.peer->address.host,
                event.peer->address.port);

            // arbitrary client data
            event.peer->data = (void*)"Client information";

            break;
        case ENET_EVENT_TYPE_RECEIVE:
            logDebug("[CLIENT] A packet of length {0:d} containing {} was received from {} on channel {0:d}.",
                event.packet->dataLength,
                event.packet->data,
                event.peer->data,
                event.channelID);

            // done with packet
            enet_packet_destroy(event.packet);

            break;
        case ENET_EVENT_TYPE_DISCONNECT:
            logDebug("[CLIENT] {} disconnected.\n", event.peer->data);

            // clear arbitrary data
            event.peer->data = NULL;
        }
    }
}

Client::~Client() {
    if(client != NULL) enet_host_destroy(client);
}
