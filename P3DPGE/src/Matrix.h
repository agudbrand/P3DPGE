#pragma once

struct Matrix {
	int rows;
	int columns;
};

struct Matrix4x4 : public Matrix {
	float a[4][4] = {0};

	Matrix4x4() {
		rows = 4;
		columns = 4;
	}

	Matrix4x4(float a00, float a01, float a02, float a03,
			float a10, float a11, float a12, float a13, 
			float a20, float a21, float a22, float a23, 
			float a30, float a31, float a32, float a33){
		rows = 4; columns = 4;

		a[0][0] = a00; a[0][1] = a01; a[0][2] = a02; a[0][3] = a03;
		a[1][0] = a10; a[1][1] = a11; a[1][2] = a12; a[1][3] = a13;
		a[2][0] = a20; a[2][1] = a21; a[2][2] = a22; a[2][3] = a23;
		a[3][0] = a30; a[3][1] = a31; a[3][2] = a32; a[3][3] = a33;
	}


};