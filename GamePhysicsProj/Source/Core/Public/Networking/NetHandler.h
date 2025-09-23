#pragma once

class NetHandler
{
    
public:

    NetHandler();
    virtual ~NetHandler() = default;

    void HostLocal();
    void ConnectLocalInstance();
    
};
