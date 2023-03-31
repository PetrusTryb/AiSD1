#pragma once
#define BLOCK_LENGHT 64
template <class T>
class Array {
private:
    T* data;
    int currentLenght;
    int maxLenght;
public:
    Array() {
        data = new T[BLOCK_LENGHT]{};
        maxLenght = BLOCK_LENGHT;
        currentLenght = 0;
    }
    Array(const Array& arr) {
        data = new T[arr.maxLenght]{};
        maxLenght = arr.maxLenght;
        currentLenght = arr.currentLenght;
        for (int i = 0; i < currentLenght; i++) {
            data[i] = arr.data[i];
        }
    }
    Array& operator=(const Array& arr) {
        if (this == &arr)
            return *this;
        delete[] data;
        data = new T[arr.maxLenght]{};
        maxLenght = arr.maxLenght;
        currentLenght = arr.currentLenght;
        for (int i = 0; i < currentLenght; i++) {
            data[i] = arr.data[i];
        }
        return *this;
    }

    ~Array() {
        delete[] data;
    }

    void operator+=(const T& c) {
        if (currentLenght == maxLenght) {
            T* temp = new T[maxLenght + BLOCK_LENGHT]{};
            for (int i = 0; i < currentLenght; i++) {
                temp[i] = data[i];
            }
            delete[] data;
            data = temp;
            maxLenght += BLOCK_LENGHT;
        }
        data[currentLenght] = c;
        currentLenght++;
    }

    bool hasElement(Array<char>& el) const {
        for (int i = 0; i < currentLenght; i++)
            if (data[i] == el)
                return true;
        return false;
    }

    T operator[](const int index) const {
        return data[index];
    }

    bool operator==(Array& str)
        const {
        if (currentLenght != str.length())
            return false;
        for (int i = 0; i < currentLenght; i++)
            if (str[i] != data[i]) {
                return false;
            }
        return true;
    }

    int toInt(bool detectErrors = false) const {
        int x = 0;
        for (int i = 0; i < currentLenght; i++) {
            int temp = data[i] - '0';
            if ((temp < 0 || temp>9) && detectErrors)
                return -1;
            x *= 10;
            x += temp;
        }
        return x;
    }

    char* toValidString() {
        char* str = new char[currentLenght + 2] {};
        int start = 0;
        if (data[0] == ' ')
            start = 1;
        for (int i = start; i < currentLenght; i++) {
            str[i - start] = data[i];
        }
        if (data[currentLenght - 1] == ' ')
            str[currentLenght - 1] = '\0';
        return str;
    }

    void removeTrailingSpaces() {
        while (data[currentLenght - 1] <= ' ') {
            data[currentLenght - 1] = '\0';
            currentLenght--;
            if (currentLenght == -1) {
                currentLenght = 0;
                return;
            }
        }
    }

    int length() const {
        return currentLenght;
    }

    void clear() {
        delete[] data;
        data = new T[BLOCK_LENGHT]{};
        maxLenght = BLOCK_LENGHT;
        currentLenght = 0;
    }

};

typedef Array<char> String;