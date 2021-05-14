#ifndef RESOURCEPOOL_H
#define RESOURCEPOOL_H

#include "action.h"
#include <list>
#include <vector>

template<typename Type>
struct Pool {

    class element {
    public:
        element(Pool<Type>* pool) :
            pool_(pool)
        {
            ptr_ = pool_->getInternal();
        }

        element(element&& other) {
            pool_ = other.pool_;
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }

        ~element() {
            if(!!ptr_) pool_->putInternal(ptr_);
        }

        Type* get() const {
            assert(!!ptr_);
            return ptr_;
        }

    private:
        Pool<Type>* pool_;
        Type* ptr_;

    };

    element get() {
        return element(this);
    }

private:
    
    Type* getInternal() {
        if(free.empty()) {
            objects.emplace_back();
            free.push_back(&objects.back());
        }
        Type* back = free.back();
        free.pop_back();
        return back;
    }

    void putInternal(Type* t) {
        free.push_back(t);
    }

    std::list<Type> objects;
    std::vector<Type*> free;
};

struct ResourcePool {

    Pool<ActionSet> actionsets;
    Pool<std::vector<double>> scores;
    Pool<std::vector<aspair>> actionscores;

};


#endif