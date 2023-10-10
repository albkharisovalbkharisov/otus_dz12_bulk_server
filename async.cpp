#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>
#include <list>
#include <fstream>
#include <ctime>
#include <string>
#include <shared_mutex>
#include <map>

#include <utility>
#include <memory>
#include <mutex>
#include "async.h"
#include "bulkmt.h"

namespace async {

std::size_t default_bulk_size;
static int handle_cnt = 0;
static std::map<async::handle_t, std::shared_ptr<Bulk>> bulks;
// smutex is mutex to protect access to bulks.
// It is shared because we can feed 2 different bulks
// simultaniously. For protecting one certain bulk from
// 2 simultaious accesses there is mutex inside
// Bulk class.
static std::shared_timed_mutex smutex;

async::handle_t connect(std::size_t bulk_size) {
    std::unique_lock<std::shared_timed_mutex> l{smutex};
    ++handle_cnt;    // numeration starts from 1
    async::handle_t handle = reinterpret_cast<async::handle_t>(handle_cnt);
    bulks.emplace(std::make_pair(handle, std::make_shared<Bulk>(bulk_size)));
    return handle;
}

void receive(async::handle_t handle, const char *data, std::size_t size) {
    std::shared_lock<std::shared_timed_mutex> l{smutex};
    // no exception handling. Should be handled higher
    auto bulk = bulks.at(handle);
    bulk->collect_and_parse(data, size);
}

void disconnect(async::handle_t handle) {
    std::unique_lock<std::shared_timed_mutex> l{smutex};
    bulks.erase(handle);
}

}

