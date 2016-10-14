/*
 * Copyright (c) 2016 Joon Ahn (green@weahns.net). All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <assert.h>
#include <memory>

#include "util.h"


namespace jahn  // namespace for joon ahn
{

using namespace std;    // for ease of prototyping

class RequestContainer
{
};

class ResourceContainer
{
};

class Scheduler
{
public:
    Scheduler() : ptrRequests_(new RequestContainer), ptrResources_(new ResourceContainer),
        requests_(*ptrRequests_), resources_(*ptrResources_) {}
    err_t   scheduleNext();
    RequestContainer&   requestContainer();
    ResourceContainer&  resourceContainer();

private:
    unique_ptr<RequestContainer>    ptrRequests_;
    unique_ptr<ResourceContainer>   ptrResources_;
    RequestContainer&   requests_;
    ResourceContainer&  resources_;
};

err_t
Scheduler::scheduleNext()
{
    // get next request

    // find a proper resource

    // reserve the resource for the needed time

    // recommend the assignment
}


class ResourceTracker
{
public:
    explicit ResourceTracker(ResourceContainer&);
    err_t   update();
};

class RequestTaker
{
public:
    explicit RequestTaker(RequestContainer&);
    err_t   update();
};

class Simulator
{
public:
    bool    isEnd() const;
    err_t   advanceTime();
};

err_t
simulate()
{
    err_t err = 0;

    Simulator sim;

    // prepare a scheduler
    Scheduler sched;
    ResourceTracker rscTracker(sched.resourceContainer());
    RequestTaker reqTaker(sched.requestContainer());

    // run scheduler for the given time
    while (!sim.isEnd()) {

        // update resources for this time slot
        rscTracker.update();

        // get requests for this time slot
        reqTaker.update();

        do {
            err = sched.scheduleNext();
            if (err == ERR_NOJOB) {
                break;
            }
            else if (err) {
                assert(false);  // todo: error handling
            }
        } while (true);

        // todo: update stat

        sim.advanceTime();
    }

    return err;
}


} // namespace jahn


int main()
{
    jahn::simulate();
    return 0;
}
