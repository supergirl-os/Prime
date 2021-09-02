//----------------------------
//    DB_FIRST_HOMEWORK_P0
//    Coding By WangYaxuan
//   LAST_UPDATE:Sept2/2021
//----------------------------
#include "p0_starter.h"
template <typename T>
class Matrix {
protected:
	Matrix(int rows, int cols) {
		rows_ = rows;
		cols_ = cols;
		linear_ = new T[rows * cols];
		memset(linear_, 0, sizeof(T) * rows * cols);
	}
	int rows_;
	int cols_;

	T* linear_;

public:
	virtual int GetRowCount() const = 0;
	virtual int GetColumnCount() const = 0;
	virtual T GetElement(int i, int j) const = 0;
	virtual void SetElement(int i, int j, T val) = 0;
	virtual void FillFrom(const std::vector<T>& source) = 0;
	virtual ~Matrix() {
		delete[] linear_;
	}
};


template <typename T>
class RowMatrix : public Matrix<T>
{
public:
	RowMatrix(int rows, int cols) : Matrix<T>(rows, cols) {
		data_ = new T * [rows];
		for (int i = 0; i < rows; i++) {
			data_[i] = &this->linear_[i * cols];
		}
	}

	int GetRowCount() const override { return this->rows_; }

	int GetColumnCount() const override { return this->cols_; }

	T GetElement(int i, int j) const override { return data_[i][j]; }

	void SetElement(int i, int j, T val) override {
		data_[i][j] = val;
	}


	void FillFrom(const std::vector<T>& source) override {
		for (int i = 0; i < this->rows_; i++) {
			for (int j = 0; j < this->cols_; j++) {
				data_[i][j] = source[i * this->cols_ + j];
			}
		}
	}

	~RowMatrix()
	{
		delete[] data_;
	}
private:
	T** data_;
};


template <typename T>
class RowMatrixOperations {
public:
	static std::unique_ptr<RowMatrix<T>> Add(const RowMatrix<T>* matrixA, const RowMatrix<T>* matrixB) {

		if (matrixA->GetRowCount() != matrixB->GetRowCount() ||
			matrixA->GetColumnCount() != matrixB->GetColumnCount()) {
			return std::unique_ptr<RowMatrix<T>>(nullptr);
		}
		int row = matrixA->GetRowCount();
		int col = matrixA->GetColumnCount();
		std::unique_ptr<RowMatrix<T>> matrixAB(new RowMatrix<T>(row, col));
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				matrixAB->SetElement(i, j, matrixA->GetElement(i, j) +
					matrixB->GetElement(i, j));
			}
		}
		return matrixAB;
	}

	static std::unique_ptr<RowMatrix<T>> Multiply(const RowMatrix<T>* matrixA, const RowMatrix<T>* matrixB) {
		if (matrixA->GetColumnCount() != matrixB->GetRowCount()) {
			return std::unique_ptr<RowMatrix<T>>(nullptr);
		}

		int row = matrixA->GetRowCount();
		int col = matrixB->GetColumnCount();
		std::unique_ptr<RowMatrix<T>> matrixC(new RowMatrix<T>(row, col));
		for (int i = 0; i < row; i++) {
			for (int j = 0; j < col; j++) {
				for (int k = 0; k < matrixA->GetColumnCount(); k++) {
					T tmp = matrixC->GetElement(i, j);
					matrixC->SetElement(i, j, tmp + matrixA->GetElement(i, k) *
						matrixB->GetElement(k, j));
				}
			}
		}
		return matrixC;
	}

	static std::unique_ptr<RowMatrix<T>> GEMM(const RowMatrix<T>* matrixA, const RowMatrix<T>* matrixB,
		const RowMatrix<T>* matrixC) {
		std::unique_ptr<RowMatrix<T>> tmp = Multiply(matrixA, matrixB);
		if (tmp.get() == nullptr) {
			return std::unique_ptr<RowMatrix<T>>(nullptr);
		}
		int row = matrixA->GetRowCount();
		int col = matrixA->GetColumnCount();
		std::unique_ptr<RowMatrix<int>> matrixAB(new RowMatrix<T>(row, col));
		for (int i = 0;i < tmp->GetRowCount();i++)
		{
			for (int j = 0;j < tmp->GetColumnCount();j++)
			{

				matrixAB->SetElement(i, j, tmp->GetElement(i, j) + matrixC->GetElement(i, j));

			}
		}

		return matrixAB;
	}
};
