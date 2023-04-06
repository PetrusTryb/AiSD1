#pragma once
template <class T>
class Array {
private:
	const int ARRAY_BLOCK_SIZE = 32;
	T* data = nullptr;
	int currentLenght;
	int maxLenght;
public:
	Array() {
		data = new T[ARRAY_BLOCK_SIZE]{};
		maxLenght = ARRAY_BLOCK_SIZE;
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

	void Add(const T& c) {
		if (currentLenght == maxLenght) {
			T* temp = new T[maxLenght + ARRAY_BLOCK_SIZE]{};
			for (int i = 0; i < currentLenght; i++) {
				temp[i] = data[i];
			}
			delete[] data;
			data = temp;
			maxLenght += ARRAY_BLOCK_SIZE;
		}
		data[currentLenght] = c;
		currentLenght++;
	}

	T operator[](const int index) const {
		return data[index];
	}

	bool operator==(Array& str) const {
		if (currentLenght != str.Length())
			return false;
		for (int i = 0; i < currentLenght; i++)
			if (str[i] != data[i]) {
				return false;
			}
		return true;
	}

	int ToInt(bool detectErrors = false) const {
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

	char* ToValidString() {
		return (char*)data;
	}

	void RemoveTrailingSpaces() {
		while (data[currentLenght - 1] <= ' ' && currentLenght > 0) {
			data[currentLenght - 1] = '\0';
			currentLenght--;
		}
	}

	int Length() const {
		return currentLenght;
	}

	void Clear() {
		delete[] data;
		data = new T[ARRAY_BLOCK_SIZE]{};
		maxLenght = ARRAY_BLOCK_SIZE;
		currentLenght = 0;
	}

};

typedef Array<char> String;