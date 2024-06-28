#pragma once
#include <iostream>
using namespace std;

template <typename data>
void copy(data *&Arr1, data *&Arr2, int end, int start = 0)
{
    for (int i = 0; i < end - start; i++)
        Arr1[i] = Arr2[i + start];
}

template <typename data>
class MyVector
{
    data *array;
    int size;
    int max;
    char sorted;

public:
    MyVector() // * default constructor
    {
        sorted = 0;
        array = nullptr;
        size = 0;
        max = 0;
    }
    MyVector(const int capacity, const char sort = 0) // * parametrized contructor
    {
        sorted = sort;
        size = 0;
        max = capacity;
        array = new data[max];
    }
    MyVector(const MyVector &other)
    {
        max = other.getCapacity();
        size = other.getSize();
        array = new data[max];
        sorted = other.sorted;
        data *temp = other.getArray();
        copy(array, temp, size);
    }
    void operator=(const MyVector &other) // * operator overloading =
    {
        max = other.getCapacity();
        size = other.getSize();
        array = new data[max];
        data *temp = other.getArray();
        copy(array, temp, size);
    }
    ~MyVector() // * destructor
    {
        if (array == nullptr)
            return;
        delete[] array;
        size = 0;
        max = 0;
    }
    int getSize() const
    {
        return size;
    }
    int getCapacity() const
    {
        return max;
    }
    data *getArray() const // * returns the array
    {
        return array;
    }
    data getTop() const // * returns the last element
    {
        return array[size - 1];
    }
    void reserve(int n) // * preallocates memory
    {
        if (array != nullptr)
            delete[] array;
        array = new data[n];
        max = n;
    }
    void resize(int n) // * resize the array
    {
        if (array == nullptr)
        {
            array = new data[n];
            max = n;
            return;
        }
        data *temp = new data[n];
        copy(temp, array, size);
        delete[] array;
        array = temp;
        max = n;
    }
    bool isFull()
    {
        return size == max;
    }
    bool isEmpty()
    {
        return size == 0;
    }
    void sortasc() // * sorts in ascending order
    {
        for (int i = 0; i < size; i++)
            for (int j = i + 1; j < size; j++)
                if (array[i] > array[j])
                {
                    data temp = array[i];
                    array[i] = array[j];
                    array[j] = temp;
                }
    }
    void sortdesc() // * sorts in descending order
    {
        for (int i = 0; i < size; i++)
            for (int j = i + 1; j < size; j++)
                if (array[i] < array[j])
                {
                    data temp = array[i];
                    array[i] = array[j];
                    array[j] = temp;
                }
    }

    void display()
    {
        for (int i = 0; i < size; i++)
            cout << array[i] << "";
    }
    void leftShift(const int idx)
    {
        for (int i = idx; i < size; i++)
            array[i] = array[i + 1];
        size--;
    }
    void rightShift(const int idx)
    {
        for (int i = size + 1; i >= idx; i--)
            array[i] = array[i - 1];
        size++;
    }
    void pop() // * remove last element
    {
        size--;
    }
    int insert(const data val) // * insert element in the end
    {
        if (this->isFull())
            resize(max * 2 + 1);
        array[size] = val;
        size++;

        return size - 1;
    }
    int insertSorted(const data val) // * insert element in a sorted manner, ascending
    {
        if (this->isFull())
            resize(max * 2 + 1);
        if (sorted == 'a')
        {
            int i = size;
            while (i > 0)
            {
                if (array[i - 1] <= val)
                    break;
                else
                    array[i] = array[i - 1];
                i--;
            }
            array[i] = val;
            size++;
        }
        else
            array[size++] = val;

        return size - 1;
    }
    int search(data val)
    {
        for (int i = 0; i < size; i++)
            if (val == array[i])
                return i;
        return -1;
    }
    int remove(const int idx) // * remove at index
    {
        if (this->isEmpty())
            return -1;

        leftShift(idx);
        return 1;
    }
    const data &operator[](const int idx) const
    {
        return array[idx];
    }
    data &operator[](const int idx)
    {
        return array[idx];
    }
    void display(const MyVector<data> &vec)
    {
        for (int i = 0; i < vec.getSize(); i++)
            cout << vec[i] << "";
        cout << endl;
    }
};