#pragma once

#include "FlumenCore/Container/Pool.hpp"

namespace world::settlement
{
    class Job;

    class JobSet
    {
    public:
        container::Pool <Job> &GetJobs() {return jobs;}

        const container::Pool <Job> &GetJobs() const {return jobs;}

    protected:
        container::Pool <Job> jobs;
    };
}