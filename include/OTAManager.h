#ifndef OTAManager_h
#define OTAManager_h

class OTAManager_
{
private:
    OTAManager_() = default;

public:
    static OTAManager_ &getInstance();
    void setup();
    void tick();
};

extern OTAManager_ &OTAManager;

#endif