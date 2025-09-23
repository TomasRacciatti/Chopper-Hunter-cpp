#pragma once
#include <vector>
#include <functional>

template <class T>
class Pool
{
public:
    explicit Pool(size_t capacity, std::function<void(T&)> onConstruct = {})
    {
        _items.resize(capacity);
        _free.reserve(capacity);
        for (auto& item : _items) 
        {
            if (onConstruct) 
                onConstruct(item);

            _free.push_back(&item);
        }
    }

    T* Spawn()
    {
        if (_free.empty()) return nullptr;
        T* obj = _free.back(); 
        _free.pop_back();
        return obj;
    }

    void Despawn(T* obj)
    {
        _free.push_back(obj);
    }

    void Reset()
    {
        _free.clear();
        for (auto& item : _items)
        {
            item.Deactivate();
            _free.push_back(&item);
        }
    }

    std::vector<T>& Items() { return _items; }
    const std::vector<T>& Items() const { return _items; }

private:
    std::vector<T> _items;
    std::vector<T*> _free;
};
