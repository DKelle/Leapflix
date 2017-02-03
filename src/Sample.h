#define LEFT 0
#define RIGHT 1


#include <iostream>
#include <cstring>
#include "Leap.h"
#include <vector>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

using namespace Leap;

class SampleListener : public Listener {
  public:
    virtual void onInit(const Controller&);
    virtual void onConnect(const Controller&);
    virtual void onDisconnect(const Controller&);
    virtual void onExit(const Controller&);
    virtual void onFrame(const Controller&);
    virtual void onFocusGained(const Controller&);
    virtual void onFocusLost(const Controller&);
    virtual void onDeviceChange(const Controller&);
    virtual void onServiceConnect(const Controller&);
    virtual void onServiceDisconnect(const Controller&);
    std::vector<int> digits;

  private:
    bool print_leap_stats = true;


};
