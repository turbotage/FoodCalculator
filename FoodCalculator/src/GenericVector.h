#include <vector>
#include <iostream>
#include <ostream>

template<class T>
class GenericVector {
private:
	std::vector<T> data;
public:

	GenericVector();

	GenericVector(const std::vector<T>& v);

	int size() {
		return data.size();
	}

	void setSize(int size) {
		data.resize(size);
	}

	std::vector<T> getData() {
		return data;
	}


	GenericVector<T>& operator=(std::vector<T>& v);

	friend std::ostream& operator<<(std::ostream& out, const GenericVector& obj) {
		for (int i = 0; i < obj.data.size(); ++i) {
			out << obj.data[i];
			out << ", ";
		}
		out << std::endl;
		return out;
	}

	T& operator[](int x);

	GenericVector<T>& mul(const GenericVector<T>& other);
	GenericVector<T>& operator*=(const GenericVector<T>& other);
	friend GenericVector<T> operator*(GenericVector<T> left, const GenericVector<T>& right) {
		return left.mul(right);
	}

	GenericVector<T>& mul(const T& other);
	GenericVector<T>& operator*=(const T& other);
	friend GenericVector<T> operator*(GenericVector<T> left, const T& right) {
		return left.mul(right);
	}
	friend GenericVector<T> operator*(const T& left, GenericVector<T> right) {
		return right.mul(left);
	}

	GenericVector<T>& div(const GenericVector<T>& other);
	GenericVector<T>& operator/=(const GenericVector<T>& other);
	friend GenericVector<T> operator/(GenericVector<T> left, const GenericVector<T>& right) {
		return left.div(right);
	}

	GenericVector<T>& div(const T& other);
	GenericVector<T>& operator/=(const T& other);
	friend GenericVector<T> operator/(GenericVector<T> left, const T& right) {
		return left.div(right);
	}
	friend GenericVector<T> operator/(const T& left, GenericVector<T> right) {
		std::vector<T> divt(right.size(), left);
		GenericVector<T> divtt = divt;
		return divtt.div(right);
	}

	GenericVector<T>& add(const GenericVector<T>& other);
	GenericVector<T>& operator+=(const GenericVector<T>& other);
	friend GenericVector<T> operator+(GenericVector<T> left, const GenericVector<T>& right) {
		return left.add(right);
	}

	GenericVector<T>& add(const T& other);
	GenericVector<T>& operator+=(const T& other);
	friend GenericVector<T> operator+(GenericVector<T> left, const T& right) {
		return left.add(right);
	}
	friend GenericVector<T> operator+(const T& left, GenericVector<T> right) {
		return right.add(left);
	}

	GenericVector<T>& sub(const GenericVector<T>& other);
	GenericVector<T>& operator-=(const GenericVector<T>& other);
	friend GenericVector<T> operator-(GenericVector<T> left, const GenericVector<T>& right) {
		return left.sub(right);
	}

	GenericVector<T>& sub(const T& other);
	GenericVector<T>& operator-=(const T& other);
	friend GenericVector<T> operator-(GenericVector<T> left, const T& right) {
		return left.sub(right);
	}
	friend GenericVector<T> operator-(const T& left, GenericVector<T> right) {
		std::vector<T> subt(right.size(), left);
		GenericVector<T> subtt = subt;
		return subtt.sub(right);
	}

	T sum();
	friend T sum(const GenericVector<T>& v) {
		T ret = v.data[0];
		for (int i = 1; i < v.data.size(); ++i) {
			ret += v.data[i];
		}
		return ret;
	}

	GenericVector<T> getSmoothed() {
		GenericVector<T> ret;
		ret.setSize(size());
		for (int i = 1; i < ret.size() - 1; ++i) {
			ret[i] += data[i - 1];
			ret[i] += data[i];
			ret[i] += data[i + 1];
		}
		ret /= 3.0f;
		return ret;
	}

};


template<class T>
GenericVector<T>::GenericVector()
{
}

template<class T>
GenericVector<T>::GenericVector(const std::vector<T>& v)
{
	data = v;
}

template<class T>
GenericVector<T>& GenericVector<T>::operator=(std::vector<T>& v)
{
	data = v;
}

template<class T>
T& GenericVector<T>::operator[](int x)
{
	return data[x];
}

template<class T>
GenericVector<T>& GenericVector<T>::mul(const GenericVector<T>& other)
{
	if (data.size() != other.data.size()) {
		std::cout << "Badly formated vector" << std::endl;
		return *this;
	}

	for (int i = 0; i < other.data.size(); ++i) {
		data[i] *= other.data[i];
	}
	return *this;
}

template<class T>
GenericVector<T>& GenericVector<T>::operator*=(const GenericVector<T>& other)
{
	return dot(other);
}

template<class T>
inline GenericVector<T>& GenericVector<T>::mul(const T& other)
{
	std::vector<T> mult(data.size(), other);
	return mul(mult);
}

template<class T>
inline GenericVector<T>& GenericVector<T>::operator*=(const T& other)
{
	return mul(other);
}

template<class T>
inline GenericVector<T>& GenericVector<T>::div(const GenericVector<T>& other)
{
	if (data.size() != other.data.size()) {
		std::cout << "Badly formated vector" << std::endl;
		return *this;
	}

	for (int i = 0; i < other.data.size(); ++i) {
		data[i] /= other.data[i];
	}
	return *this;
}

template<class T>
inline GenericVector<T>& GenericVector<T>::operator/=(const GenericVector<T>& other)
{
	return div(other);
}

template<class T>
inline GenericVector<T>& GenericVector<T>::div(const T& other)
{
	std::vector<T> divt(data.size(), other);
	return div(divt);
}

template<class T>
inline GenericVector<T>& GenericVector<T>::operator/=(const T& other)
{
	return div(other);
}

template<class T>
GenericVector<T>& GenericVector<T>::add(const GenericVector<T>& other)
{
	if (this->data.size() != other.data.size()) {
		std::cout << "Badly formated vector" << std::endl;
		return *this;
	}

	for (int i = 0; i < other.data.size(); ++i) {
		this->data[i] += other.data[i];
	}
	return *this;
}

template<class T>
GenericVector<T>& GenericVector<T>::operator+=(const GenericVector<T>& other)
{
	return add(other);
}

template<class T>
inline GenericVector<T>& GenericVector<T>::add(const T& other)
{
	std::vector<T> addt(data.size(), other);
	return add(addt);
}

template<class T>
inline GenericVector<T>& GenericVector<T>::operator+=(const T& other)
{
	return add(other);
}

template<class T>
GenericVector<T>& GenericVector<T>::sub(const GenericVector<T>& other)
{
	if (this->data.size() != other.data.size()) {
		std::cout << "Badly formated vector" << std::endl;
		return *this;
	}

	for (int i = 0; i < other.data.size(); ++i) {
		this->data[i] -= other.data[i];
	}
	return *this;
}

template<class T>
GenericVector<T>& GenericVector<T>::operator-=(const GenericVector<T>& other)
{
	return sub(other);
}

template<class T>
inline GenericVector<T>& GenericVector<T>::sub(const T& other)
{
	std::vector<T> subt(data.size(), other);
	return sub(subt);
}

template<class T>
inline GenericVector<T>& GenericVector<T>::operator-=(const T& other)
{
	return sub(other);
}

template<class T>
T GenericVector<T>::sum()
{
	T ret = data[0];
	for (int i = 1; i < data.size(); ++i) {
		ret += data[i];
	}
	return ret;
}
