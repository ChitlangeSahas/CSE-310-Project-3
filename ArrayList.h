/*
* No vectors? I made an Arraylist class.
*/
template <class T>
class  ArrayList {
public:

    typedef T* Iterator;

    ArrayList();
    ArrayList(unsigned int size);
    ArrayList(unsigned int size, const T & initial);
    ArrayList(const ArrayList<T>& v);
    ~ArrayList();

    unsigned int capacity() const;
    unsigned int size() const;
    bool empty() const;
    Iterator begin();
    Iterator end();
    T& front();
    T& back();
    void push_back(const T& value);
    void pop_back();
    T  pop();

    bool has(int n);

    void reserve(unsigned int capacity);
    void resize(unsigned int size);

    T & operator[](unsigned int index);
    ArrayList<T> & operator = (const ArrayList<T> &);
    void clear();
private:
    unsigned int _size;
    unsigned int _capacity;
    unsigned int Log;
    T* buffer;
};

template<class T>
ArrayList<T>::ArrayList() {
    _capacity = 0;
    _size = 0;
    buffer = 0;
    Log = 0;
}


template<class T>
ArrayList<T>& ArrayList<T>::operator = (const ArrayList<T> & v) {
    delete[] buffer;
    _size = v._size;
    Log = v.Log;
    _capacity = v._capacity;
    buffer = new T [_capacity];
    for (unsigned int i = 0; i < _size; i++)
        buffer[i] = v.buffer[i];
    return *this;
}

template<class T>
void ArrayList<T>::push_back(const T & v) {
    /*
        Incidentally, one common way of regrowing an array is to double the size as needed.
        This is so that if you are inserting n items at most only O(log n) regrowths are performed
        and at most O(n) space is wasted.
    */
    if (_size >= _capacity) {
        reserve(1 << Log);
        Log++;
    }
    buffer [_size++] = v;
}

template <class T>
T  ArrayList<T>::pop()
{
    T first = buffer[0];
    buffer++;
    _size--;
    return first;
}

template<class T>
void ArrayList<T>::pop_back() {
    _size--;
}

template<class T>
void ArrayList<T>::reserve(unsigned int capacity) {
    T * newBuffer = new T[capacity];

    for (unsigned int i = 0; i < _size; i++)
        newBuffer[i] = buffer[i];

    _capacity = capacity;
    delete[] buffer;
    buffer = newBuffer;
}

template<class T>
unsigned int ArrayList<T>::size() const {
    return _size;
}

template<class T>
T& ArrayList<T>::operator[](unsigned int index) {
    return buffer[index];
}

template<class T>
ArrayList<T>::~ArrayList() {
    delete[] buffer;
}

template<class T>
bool ArrayList<T>::has(int n) {
    for (int i = 0; i < _size; ++i)
    {
        if (buffer[i] == n)
        {
            return true;
        }
    }
    return false;
}
