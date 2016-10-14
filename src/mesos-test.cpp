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
#include <deque>
#include <list>
#include <map>
#include <memory>
#include <algorithm>

#include "util.h"


namespace jahn  // namespace for joon ahn
{

using namespace std;    // for ease of prototyping

typedef int     id_t;
typedef int     time_t;

class Simulator
{
public:
    bool    isEnd() const {return curTime_ >= endTime_;}
    void    setTime(time_t t) {curTime_ = t;}
    void    setEndTime(time_t t) {endTime_ = t;}
    err_t   advanceTime() {curTime_++; return 0;}
    time_t  time() const {return curTime_;}

private:
    time_t  curTime_;
    time_t  endTime_;
};

Simulator&
getSimulator()
{
    static Simulator sim;
    return sim;
}


class Request
{
public:
    Request(size_t s, time_t t) : units_(s), duration_(t) {}
    size_t  units() const {return units_;}
    time_t  duration() const {return duration_;}

private:
    size_t  units_;
    time_t  duration_;
};

class RequestContainer
{
public:
    virtual ~RequestContainer() {}
    virtual Request next() = 0;
    virtual void    restore(const Request&) = 0;
    virtual bool    empty() const = 0;
    virtual err_t   insert(const Request&) = 0;
};

class RequestQueue : public RequestContainer
{
public:
    bool    empty() const {return requests_.empty();}

    Request next() {
        Request req = move(requests_.front());
        requests_.pop_front();
        return move(req);
    }

    void    restore(const Request& req) {
        requests_.push_front(req);
    }

    err_t   insert(const Request&) {
        // todo: implement it
        return 0;
    }

private:
    deque<Request>  requests_;
};

class Task
{
public:
    Task(time_t s, time_t e, size_t u) : stime_(s), etime_(e), units_(u) {}
    time_t  startTime() const {return stime_;}
    time_t  endTime() const {return etime_;}
    size_t  units() const {return units_;}

private:
    time_t  stime_;
    time_t  etime_;
    size_t  units_;
};

class Resource
{
public:
    Resource(id_t n, size_t u) : node_(n), units_(u) {}

    id_t    node() const {return node_;}
    size_t  units() const {return units_;}
    void    setUnits(size_t u) {units_ = u;}

    size_t  unreservedUnits() const {
        time_t curtime = getSimulator().time();
        // todo: remove finshed reservations
        size_t resv = 0;
        for (auto it = reservations_.begin(); it != reservations_.end(); ++it) {
            if (curtime >= it->startTime() && curtime < it->endTime()) {
                resv += it->units();
            }
        }
        return units_ >= resv ? units_ - resv : 0;
    }

    err_t   reserve(const Request& req) {
        // assumption: there is no reservation that is started from future.
        if (unreservedUnits() < req.units()) {
            return ERR_NO_RESOURCE;
        }

        const Simulator& sim = getSimulator();
        reservations_.push_back(Task(sim.time(),
                                     sim.time() + req.duration(),
                                     req.units()));
        return 0;
    }

private:
    id_t    node_;
    size_t  units_;
    list<Task>  reservations_;
};

#if 0
class ResourceContainer
{
public:
    virtual ~ResourceContainer() = 0;
    virtual err_t   reserve(id_t, const Request&) = 0;
};
#endif

class ResourceSet // : public ResourceContainer
{
    typedef map<id_t, Resource> container_t;

public:
    container_t::iterator   begin() {return resources_.begin();}
    container_t::iterator   end() {return resources_.end();}

    err_t   reserve(id_t node, const Request& req) {
        auto it = resources_.find(node);
        if (it == resources_.end()) {
            return ERR_NOT_EXIST;
        }
        Resource& res = it->second;
        if (res.unreservedUnits() < req.units()) {
            return ERR_NO_RESOURCE;
        }
        return res.reserve(req);
    }

    err_t   update(const Resource& res) {
        auto it = resources_.find(res.node());
        if (it == resources_.end()) {
            resources_.insert(make_pair(res.node(), res));
            return 0;
        }
        it->second.setUnits(res.units());
        return 0;
    }

private:
    container_t  resources_;
};

#if 0
class Assignment
{
public:
    Assignment(id_t, Request&&);
};
#endif


class Scheduler
{
public:
    Scheduler() : ptrRequests_(new RequestQueue), ptrResources_(new ResourceSet),
        requests_(*ptrRequests_), resources_(*ptrResources_) {}
    RequestContainer&   requestContainer() {return requests_;}
    ResourceSet&  resourceContainer() {return resources_;}

    err_t   scheduleNext();
    id_t    bestNode(const Request& req);

private:
    unique_ptr<RequestContainer>    ptrRequests_;
    unique_ptr<ResourceSet>   ptrResources_;
    RequestContainer&   requests_;
    ResourceSet&  resources_;
};

id_t
Scheduler::bestNode(const Request& req)
{
    // attempt 1: get any node who has at least the needed units
    for (auto it = resources_.begin(); it != resources_.end(); ++it) {
        if (it->second.unreservedUnits() >= req.units()) {
            return it->second.node();
        }
    }

    // no avaiable resource:
    return 0;
}


err_t
Scheduler::scheduleNext()
{
    // get next request
    if (requests_.empty()) {
        return ERR_NOJOB;
    }

//    err_t err = 0;
    Request req = requests_.next();
    id_t node = bestNode(req);
    if (node == 0) {
        // no available resources
        requests_.restore(req);
        return ERR_NO_RESOURCE;
    }

    // reserve the resource for the needed time
    resources_.reserve(node, req);

    // recommend the assignment
//    Assignment(node, move(req));
    return 0;
}


class ResourceTracker
{
public:
    explicit ResourceTracker(ResourceSet& s) : resources_(s) {}
    err_t   update() {
        // todo: make it random.

        time_t t = getSimulator().time();
        // only for initialization, for now.

        if (t == 0) {
            id_t nnodes = 10;

            for (id_t i = 1; i <= nnodes; ++i) {
                Resource res(i, 10 + i);
                resources_.update(res);
            }
        }
        return 0;
    }

private:
    ResourceSet&    resources_;
};

class RequestTaker
{
public:
    explicit RequestTaker(RequestContainer& r) : requests_(r) {}
    err_t   update() {
        time_t t = getSimulator().time();
        // todo: randomize it

        int nreqs = 7;

        for (int i = 0; i < nreqs ;++i) {
            Request req(t+i+1, 7);
            requests_.insert(req);
        }

        return 0;
    }

private:
    RequestContainer& requests_;
};

err_t
simulate()
{
    int duration = 10;
    err_t err = 0;

    Simulator sim;

    // prepare a scheduler
    Scheduler sched;
    ResourceTracker rscTracker(sched.resourceContainer());
    RequestTaker reqTaker(sched.requestContainer());

    // run scheduler for the given time
    sim.setTime(0);
    sim.setEndTime(duration);

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

    // todo: print stat

    return err;
}


} // namespace jahn


int main()
{
    jahn::simulate();
    return 0;
}
