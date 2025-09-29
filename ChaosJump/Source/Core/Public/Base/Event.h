#pragma once
#include <functional>
#include <vector>

#include <functional>
#include <vector>
#include <utility>

template<typename Return, typename... Args>
class TEvent
{
    using FuncType = std::function<Return(Args...)>;
    using Entry = std::pair<size_t, FuncType>;

    std::vector<Entry> mSubscribedFunctions;
    size_t mNextId = 0;

public:
    
    size_t subscribe(FuncType function)
    {
        size_t id = mNextId++;
        mSubscribedFunctions.emplace_back(id, std::move(function));
        return id;
    }

    void unsubscribe(size_t id)
    {
        auto it = std::remove_if(mSubscribedFunctions.begin(), mSubscribedFunctions.end(),
            [id](const Entry& e) { return e.first == id; });
        mSubscribedFunctions.erase(it, mSubscribedFunctions.end());
    }

    void broadcast(Args... args)
    {
        for (auto& [id, func] : mSubscribedFunctions)
        {
            func(args...);
        }
    }
};
