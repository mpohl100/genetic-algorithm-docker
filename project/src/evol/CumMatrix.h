#pragma once

#include <vector>
#include <numeric>
#include <stdexcept>

namespace evol {

template<class T>
class CumVector {
	std::vector<T> val_;
	T cum_ = {};
public:
	CumVector(std::vector<T> values)
		: val_(std::move(values))
		, cum_(std::accumulate(val_.begin(), val_.end(), T{}))
	{}
	CumVector(size_t size)
		: val_(std::vector<T>(size))
	{}
	CumVector() = default;
	CumVector(CumVector const&) = default;
	CumVector& operator=(CumVector const&) = default;
	CumVector(CumVector&&) = default;
	CumVector& operator=(CumVector&&) = default;

	const T& operator[](size_t index) const
	{
		if(index < val_.size())
			return val_[index];
		throw std::runtime_error("CumVector index out of bounds in []");
	}

	void set(size_t index, T const& val)
	{
		if (index <= val_.size()) {
			if (index == val_.size())
				val_.push_back(T{});
			cum_ += val - val_[index];
			if (index < val_.size())
				val_[index] = val;
			else
				val_.push_back(val);
		}
		else
			throw std::runtime_error("CumVector index out of bounds in set");
	}

	T cum() const { return cum_; }
	size_t size() const { return val_.size(); }
};

template<class T>
class CumMatrix {
	std::vector<CumVector<T>> val_;
	CumVector<T> cum_;
public:
	CumMatrix(std::vector<CumVector<T>> values)
		: val_(std::move(values))
	{
		for (int i = 0; i < values.size(); ++i)
			for (int j = 0; j < values[i].size(); ++j)
				cum_.set(j, cum_[j] + values[i][j]);
	}
	CumMatrix(size_t size)
	{
		val_ = std::vector<CumVector<T>>(size, CumVector<T>(size));
		cum_ = CumVector<T>(size);
	}
	CumMatrix() = default;
	CumMatrix(CumMatrix const&) = default;
	CumMatrix& operator=(CumMatrix const&) = default;
	CumMatrix(CumMatrix&&) = default;
	CumMatrix& operator=(CumMatrix&&) = default;

	const CumVector<T>& operator[](size_t index) const
	{
		if (index < val_.size())
			return val_[index];
		throw std::runtime_error("CumMatrix index out of bounds in []");
	}

	void set(size_t row, size_t col, T const& val)
	{
		if (row < val_.size()) {
			T delta = val - val_[row][col];
			cum_.set(col, cum_[col] + delta);
			val_[row].set(col, val);
		}
		else
			throw std::runtime_error("CumMatrix index out of bounds in set");
	}

	void set(size_t row, CumVector<T> const& val)
	{
		if (row <= val_.size()) {
			if (row == val_.size())
				val_.push_back(val);
			for (size_t i = 0; i < val.size(); ++i) 
				cum_.set(i, val[i] - val_[row][i]);
			val_[row] = val;
		}
		else
			throw std::runtime_error("CumMatrix index out of bounds in set");
	}

	const CumVector<T>& cum() const { return cum_; }
	
	T cum(size_t index) const 
	{
		if (index < val_.size())
			return cum_[index];
		throw std::runtime_error("CumMatrix out of bounds in cum(size_t)");
	}
	
	size_t size() const { return val_.size(); }
};

}