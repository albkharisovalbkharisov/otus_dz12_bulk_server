#pragma once

#include <thread>
#include <string>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <list>
#include <atomic>
#include <vector>
#include <utility>    // std::move
#include <sstream>

template<bool count_lines_too = false>
class Counter
{
    std::atomic<size_t> line_counter;
    std::atomic<size_t> cmd_counter;
    std::atomic<size_t> bulk_counter;
public:
    Counter(void) : line_counter(0), cmd_counter(0), bulk_counter(0) {}
    void line_inc(size_t i = 1) { line_counter += i; }
    void cmd_inc(size_t i = 1)  { cmd_counter  += i; }
    void bulk_inc(size_t i = 1) { bulk_counter += i; }

    Counter(const Counter &that);
    void print_counters(const std::string &thread_name) const;
};

struct Worker
{
    std::thread thread;
    std::string name;
    Counter<false> dbg;
    Worker(std::thread t, const char *s) : thread(std::move(t)), name(s) {}
    Worker(const char *s) : name(s) {}
};

class ThreadManager
{
public:
    using type_to_handle = struct {
        const std::vector<std::string> vs;
        const std::time_t t;
    };
private:
    std::condition_variable cv;
    // cv_m is mutex to protect qMsg - queue to send data from main thread to Worker threads
    std::mutex cv_m;
    std::queue<type_to_handle> qMsg;
    std::list<Worker> vThread;
    std::atomic<bool> exit;

    void work(struct Worker &w);

public:
    virtual void handle(const type_to_handle &ht) = 0;

    template<typename ...Names>
    ThreadManager(Names... names) : exit(false) {
        const char * dummy[] = {(const char*)(names)...};
        for (auto &s : dummy)
            vThread.emplace_back(s);
    }

    void start_threads(void);
    void stop_threads(void);
    virtual ~ThreadManager(void);
    void notify(type_to_handle &ht);
};


class Bulk : public Counter<true>
{
    const size_t bulk_size;
    std::vector<std::string> vs;
    std::list<std::shared_ptr<ThreadManager>> observers;
    std::atomic<size_t> brace_cnt;
    std::time_t time_first_chunk;
    // m is mutex to protect concurent access to one Bulk
    std::mutex m;
//    std::stringstream ss;
    std::string ss;

    void flush(void);
    bool is_full(void);
    bool is_empty(void);
    void add(const std::string &s);
    void parse_line(const std::string &line);

public:
    Bulk(size_t size);
    void collect_and_parse(const char * data, size_t size);
    ~Bulk();
};

