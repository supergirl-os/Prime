//----------------------------
//    DB_FIRST_HOMEWORK_P0
//    Coding By WangYaxuan
//   LAST_UPDATE:Sept2/2021
//----------------------------
#include <memory>
#include <stdexcept>
#include <vector>
#include "p0_starter.cpp"
#include <iostream>
using namespace std;
//测试RowMatrix各个成员函数的功能
void test_rowmat()
{
	cout << "-----testing RowMatrix----" << endl;
	RowMatrix<int> test(3, 3);
	vector<int> source(9, 1);
	test.FillFrom(source);
	cout << "the row of matrix: " << test.GetRowCount() << endl;
	cout << "the col of matrix: " << test.GetColumnCount() << endl;
	cout << "mat[2,1] = " << test.GetElement(2, 1) << endl;
	cout << "modify mat[2,1] = 2 " << endl;
	test.SetElement(2, 1, 2);
	cout << "modified_mat[2,1] = " << test.GetElement(2, 1) << endl;
}

//矩阵加法
void test_add()
{
	RowMatrix<int> A(2, 2);
	vector<int> sourceA(9, 1);
	A.FillFrom(sourceA);

	RowMatrix<int> B(2, 2);
	vector<int> sourceB(9, 1);
	B.FillFrom(sourceB);
	RowMatrixOperations<int> op;
	std::unique_ptr<RowMatrix<int>> adder = op.Add(&A, &B);
	cout << "the result of Add:" << endl;
	int out;
	for (int i = 0;i < adder->GetRowCount();i++)
	{
		for (int j = 0;j < adder->GetColumnCount();j++)
		{
			out = adder->GetElement(i, j);
			cout << out << " ";
		}
		cout << endl;
	}
}

//矩阵乘法
void test_mul()
{
	RowMatrix<int> A(2, 2);
	vector<int> sourceA(9, 1);
	A.FillFrom(sourceA);

	RowMatrix<int> B(2, 2);
	vector<int> sourceB(9, 1);
	B.FillFrom(sourceB);

	RowMatrixOperations<int> op;
	std::unique_ptr<RowMatrix<int>> muler = op.Multiply(&A, &B);
	cout << "the result of Multiply:" << endl;
	int out1;
	for (int i = 0;i < muler->GetRowCount();i++)
	{
		for (int j = 0;j < muler->GetColumnCount();j++)
		{
			out1 = muler->GetElement(i, j);
			cout << out1 << " ";
		}
		cout << endl;
	}
}

//矩阵乘法后加法
void test_gemm()
{
	RowMatrix<int> A(2, 2);
	vector<int> sourceA(9, 1);
	A.FillFrom(sourceA);

	RowMatrix<int> B(2, 2);
	vector<int> sourceB(9, 1);
	B.FillFrom(sourceB);

	RowMatrixOperations<int> op;
	std::unique_ptr<RowMatrix<int>> g = op.GEMM(&A, &B, &A);
	cout << "the result of GEMM:" << endl;
	int out2;
	for (int i = 0;i < g->GetRowCount();i++)
	{
		for (int j = 0;j < g->GetColumnCount();j++)
		{
			out2 = g->GetElement(i, j);
			cout << out2 << " ";
		}
		cout << endl;
	}
}

int main()
{
	//测试RowMatrix各个成员函数的功能
	test_rowmat();

	cout << "----testing operations of matrix-----" << endl;

	//矩阵加法
	test_add();

	//矩阵乘法
	test_mul();

	//矩阵乘法&加法
	test_gemm();

	return 0;
}