#pragma once

#include <condition_variable>
#include <mutex>

#include "tags.hpp"

namespace TSSTP {
    template <class T, Own owning = Non_owner, Wait waitable = Waitable>
    struct Mutex_protected;

    template <class T>
    struct Lock{
        T *operator->(){
            return t;
        }
        private:
        T *t;
        std::unique_lock<std::mutex> lock;
    };

    template <class T>
    struct Mutex_protected<T, Non_owner, Non_waitable> {
        Lock<T> lock() {
            return t;
        }
        private:
        T *t;
    };

    template <class T>
    struct Mutex_protected<T, Non_owner, Waitable> {
        Mutex_protected()

        Lock<T> lock() {
            return t;
        }
        private:
        T *t;
    };
} // namespace TSSTP