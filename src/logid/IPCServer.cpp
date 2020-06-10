#include <stdexcept>
#include <thread>

#include "logid.h"
#include "util.h"
#include "ipc/dbus_server.h"
#include "IPCServer.h"

using namespace logid;
using namespace pizza::pixl;
using namespace pizza::pixl::logiops;

IPCServer::IPCServer()
{
    dispatcher = new DBus::BusDispatcher;

    DBus::default_dispatcher = dispatcher;
    auto _bus = DBus::Connection::SessionBus();
    bus = &_bus;

    bus->request_name("pizza.pixl.logiops");
    _root = new IPC::Root(*bus);

    ipc_thread = nullptr;
}

void IPCServer::start()
{
    ipc_thread = new std::thread {[=]() { dispatcher->enter(); } };
}

void IPCServer::stop()
{
    if(ipc_thread != nullptr)
    {
        dispatcher->leave();
        ipc_thread->join();
    }
}

void IPCServer::addDevice(Device* device)
{
    std::vector<std::string> devices = _root->Devices();
    std::string device_path = device->path + ":" + std::to_string(device->index);
    devices.push_back(device_path);
    _root->Devices = devices;
}

void IPCServer::removeDevice(std::string path, HIDPP::DeviceIndex index) {
    std::vector<std::string> devices = _root->Devices();
    std::string device_path = path + ":" + std::to_string(index);
    for(auto it = devices.begin(); it != devices.end(); it++) {
        if(*it == device_path) {
            devices.erase(it);
            break;
        }
    }
    _root->Devices = devices;
}