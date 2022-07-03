#ifndef RISCVCPUSIMULATOR_TOOLS_HPP
#define RISCVCPUSIMULATOR_TOOLS_HPP


int sext(int num, int bit) {
    if (num >> (bit - 1) == 1) {
        return num ^ (-1 << bit);
    } else return num;
}

template<class T, int size>
class CircularQueue {
public:
    class iterator;

    friend class iterator;

private:
    T queue[size];
    int head = 0;
    int tail = 0;
    int count = 0;

public:
    class iterator {
    private:
        int pos;
        CircularQueue *master;

        iterator(int _pos, CircularQueue *_master) : master(_master) { pos = _pos; }

    public:
        iterator &operator++() {
            pos = (pos + 1) % size;
            return *this;
        }

        iterator operator++(int) {
            int _pos = pos;
            pos = (pos + 1) % size;
            return iterator(_pos, master);
        }

        T operator*() { return master->queue[pos]; }

        bool operator==(const iterator &rhs) { return pos == rhs.pos; }
    };

    CircularQueue() = default;

    T &operator[](int x) {return queue[x];}

    int push(const T &obj) {
        queue[tail] = obj;
        int ans = tail;
        tail = (tail + 1) % size, ++count;
        return ans;
    }

    T front() { return queue[head]; }

    void pop() { head = (head + 1) % size, --count; }

    bool full() { return count == size; }

    bool empty() { return !count; }

    iterator begin() { return iterator(head, this); }

    iterator end() { return iterator(tail, this); }

    void clear() {count=head=tail=0;}

};

#endif //RISCVCPUSIMULATOR_TOOLS_HPP
