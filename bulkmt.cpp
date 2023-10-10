#include <chrono>
#include <iostream>
#include <fstream>
#include "bulkmt.h"


template<bool count_lines_too>
Counter<count_lines_too>::Counter(const Counter<count_lines_too> &that)
{
    this->line_counter.store(that.line_counter);
    this->cmd_counter.store(that.cmd_counter);
    this->bulk_counter.store(that.bulk_counter);
}

template<bool count_lines_too>
void Counter<count_lines_too>::print_counters(const std::string &thread_name) const
{
    std::cout << "thread " << thread_name << ": ";
    if (count_lines_too)
        std::cout << line_counter << " lines, ";
    std::cout << cmd_counter << " commands, ";
    std::cout << bulk_counter << " bulks" << std::endl;
}

void ThreadManager::work(struct Worker &w)
{
    while(1) {
        std::unique_lock<std::mutex> lk(cv_m);
        cv.wait(lk, [this](){ return !this->qMsg.empty() || exit; });
        if (exit && qMsg.empty()) break;
        auto m = qMsg.front();
        qMsg.pop();
        lk.unlock();
        w.dbg.bulk_inc();
        w.dbg.cmd_inc(m.vs.size());
        this->handle(m);
    }
}

void ThreadManager::start_threads(void)
{
    for (auto &w : vThread)
        w.thread = std::thread(&ThreadManager::work, this, std::ref(w));   // moving thread is OK
}

void ThreadManager::stop_threads(void)
{
    exit = true;
    cv.notify_all();
    for (auto &a : vThread) {
        if (a.thread.joinable())
            a.thread.join();
        else {
            std::cout << "stop_threads() : join..." << a.name << " can't!" << std::endl;
            throw "ERROR: thread can't be joined";
        }
    }
}

ThreadManager::~ThreadManager(void)
{
    for (const auto &a : vThread)
        a.dbg.print_counters(a.name);
}

void ThreadManager::notify(type_to_handle &ht)
{
    {
        std::unique_lock<std::mutex> lk(cv_m);
        qMsg.push(ht);
    }
    cv.notify_one();
}

class Saver : public ThreadManager
{
public:
    template<typename ...Names>
    Saver(Names... names) : ThreadManager(names...) {}
    void handle(const type_to_handle &ht) override {
        using namespace std::chrono;

        // invent name
        std::hash<std::thread::id> hash_thread_id;
        size_t hash = hash_thread_id(std::this_thread::get_id()) ^ std::hash<int>()(std::rand());
        std::string filename = "bulk" + std::to_string(ht.t) + "_" + std::to_string(hash) + ".log";

        std::fstream fs;
        fs.open (filename, std::fstream::in | std::fstream::out | std::fstream::app);
        for (auto &a : ht.vs) {
            fs << a;
            fs << '\n';
        }
        fs.close();
    }
};

class Printer : public ThreadManager
{
public:
    template<typename ...Names>
    Printer(Names... names) : ThreadManager(names...) {}
    void handle(const type_to_handle &ht) override {
        std::cout << output_string_make(ht.vs);
    }

private:
    std::string output_string_make(const std::vector<std::string> &vs)
    {
        bool first = true;
        std::string s("bulk: ");
        for (const auto &si : vs) {
            if (!first)
                s += ", ";
            else
                first = false;
            s += si;
        }
        s += '\n';
        return s;
    }
};

Bulk::~Bulk(){
    flush();
    for (const auto &h : observers) {
        h->stop_threads();
    }
    print_counters("main");
}

void Bulk::flush(void) {
    if (vs.size() == 0)
        return;

    bulk_inc();
    ThreadManager::type_to_handle ht = {vs, time_first_chunk};
    for (const auto &h : observers) {
        h->notify(ht);
    }
    vs.clear();
    time_first_chunk = 0;
}

Bulk::Bulk(size_t size) : bulk_size(size), brace_cnt(0), time_first_chunk(0) {
    vs.reserve(bulk_size);
    auto save_ptr = std::make_shared<Saver> ("file1", "file2");
    observers.push_back(save_ptr);
    save_ptr->start_threads();
    auto print_ptr = std::make_shared<Printer> ("log");
    observers.push_back(print_ptr);
    print_ptr->start_threads();
}

void Bulk::collect_and_parse(const char * data, size_t size)
{
    std::unique_lock<std::mutex> l{m};
    ss.append(data, size);
    for (std::string::size_type pos; (pos = ss.find('\n')) != std::string::npos ; )
    {
        parse_line(std::string(ss, 0, pos));
        ss.erase(0, pos+1);
        if (ss.empty()) {
            break;
        }
    }
}

void Bulk::parse_line(const std::string &line)
{
    line_inc();
    if (line == "{") {
        if (!is_empty() && (brace_cnt == 0))
            flush();
        ++brace_cnt;
        return;
    }
    else if (line == "}") {
        if (brace_cnt > 0) {
            --brace_cnt;
            if (brace_cnt == 0) {
                flush();
                return;
            }
        }
    }
    else
        add(line);

    if (is_full() && !brace_cnt)
        flush();
}

void Bulk::add(const std::string &s)
{
    // protected by m mutex
    cmd_inc();
    if (time_first_chunk == 0)
        time_first_chunk = std::time(0);
    vs.push_back(s);
}

bool Bulk::is_full(void)
{
    return vs.size() >= bulk_size;
}

bool Bulk::is_empty(void)
{
    return vs.size() == 0;
}

