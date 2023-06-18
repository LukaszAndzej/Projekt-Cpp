#ifndef ITERATOR_H
#define ITERATOR_H

#include <vector>

template<typename T>
class Iterator {

    private:
        T **my_Ptr = nullptr;

    public:
        Iterator() {}
        Iterator(T **ptr) : my_Ptr(ptr) {};

        // Prefix ++ overload
        Iterator& operator++() {
            my_Ptr++;
            return *this;
        }

        // Postfix ++ overload
        Iterator operator++(int) {
            Iterator iterator = *this;
            ++(*this);
            return iterator;
        }

        Iterator& operator--() {
            my_Ptr--;
            return *this;
        }
        Iterator operator--(int) {
            Iterator iterator = *this;
            --(*this);
            return iterator;
        }

        T& operator[](int index) { return **(my_Ptr + index);}
        T* operator->() { return *my_Ptr;}
        T operator*() { return **my_Ptr;}

        bool operator==(const Iterator& other) const { return my_Ptr == other.my_Ptr;}
        bool operator!=(const Iterator& other) const { return !(*this == other);}
};
#endif