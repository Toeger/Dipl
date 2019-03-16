#pragma once

#include "tags.hpp"

namespace TSSTP {
    template <class T, Own>
    struct Thread_private;

    template <class T>
    struct Thread_private<T, Non_owner> {
        Thread_private(T &value)
            : t{&value} {}

        T &operator*() {
            return *t;
        }

        private:
        T *t;
    };
} // namespace TSSTP