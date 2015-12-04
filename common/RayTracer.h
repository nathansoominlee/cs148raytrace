#pragma once

#include "common/common.h"
#include "assignment6/Assignment6.h"

class RayTracer {
public:
    RayTracer(std::unique_ptr<class Assignment6> app);

    void Run();
private:
    std::unique_ptr<class Assignment6> storedApplication;
};
