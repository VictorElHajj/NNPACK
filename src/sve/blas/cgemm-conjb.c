#include <stddef.h>
#include <stdint.h>
#include <arm_sve.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <nnpack/hwinfo.h>



void nnp_cVLgemm_conjb_only_2x2__sve(
	size_t k, size_t update,
	const float A[restrict static 1],
	const float B[restrict static 1],
	float C[restrict static 1],
	size_t row_stride_c)
{

	svfloat32_t a0r, a1r, b0r, b1r;
	svfloat32_t a0i, a1i, b0i, b1i;

	svfloat32_t svAcc00r = svdup_f32(0.0f);
	svfloat32_t svAcc01r = svdup_f32(0.0f);
	svfloat32_t svAcc10r = svdup_f32(0.0f);
	svfloat32_t svAcc11r = svdup_f32(0.0f);

	svfloat32_t svAcc00i = svdup_f32(0.0f);
	svfloat32_t svAcc01i = svdup_f32(0.0f);
	svfloat32_t svAcc10i = svdup_f32(0.0f);
	svfloat32_t svAcc11i = svdup_f32(0.0f);

	const size_t simd_width = nnp_hwinfo.simd_width; //4;						  // svcntw(); //todo fix
	const svbool_t p0 = svwhilelt_b32_s32(0, simd_width); // svptrue_b32();

	for (int i = 0; i < k; i++)
	{

		a0r = svld1(p0, A + i * simd_width * 4 + simd_width * 0);
		a0i = svld1(p0, A + i * simd_width * 4 + simd_width * 1);

		a1r = svld1(p0, A + i * simd_width * 4 + simd_width * 2);
		a1i = svld1(p0, A + i * simd_width * 4 + simd_width * 3);

		b0r = svld1(p0, B + i * simd_width * 4 + simd_width * 0);
		b0i = svld1(p0, B + i * simd_width * 4 + simd_width * 1);

		b1r = svld1(p0, B + i * simd_width * 4 + simd_width * 2);
		b1i = svld1(p0, B + i * simd_width * 4 + simd_width * 3);

		svAcc00r = svmla_m(p0, svAcc00r, a0r, b0r);
		svAcc00r = svmla_m(p0, svAcc00r, a0i, b0i);
		svAcc00i = svmla_m(p0, svAcc00i, a0i, b0r);
		svAcc00i = svmls_m(p0, svAcc00i, a0r, b0i);

		svAcc01r = svmla_m(p0, svAcc01r, a0r, b1r);
		svAcc01r = svmla_m(p0, svAcc01r, a0i, b1i);
		svAcc01i = svmla_m(p0, svAcc01i, a0i, b1r);
		svAcc01i = svmls_m(p0, svAcc01i, a0r, b1i);

		svAcc10r = svmla_m(p0, svAcc10r, a1r, b0r);
		svAcc10r = svmla_m(p0, svAcc10r, a1i, b0i);
		svAcc10i = svmla_m(p0, svAcc10i, a1i, b0r);
		svAcc10i = svmls_m(p0, svAcc10i, a1r, b0i);

		svAcc11r = svmla_m(p0, svAcc11r, a1r, b1r);
		svAcc11r = svmla_m(p0, svAcc11r, a1i, b1i);
		svAcc11i = svmla_m(p0, svAcc11i, a1i, b1r);
		svAcc11i = svmls_m(p0, svAcc11i, a1r, b1i);
	}

	if (update != 0)
	{

		svst1(p0, C + simd_width * 0, svadd_m(p0, svAcc00r, svld1(p0, C + simd_width * 0)));
		svst1(p0, C + simd_width * 1, svadd_m(p0, svAcc00i, svld1(p0, C + simd_width * 1)));
		svst1(p0, C + simd_width * 2, svadd_m(p0, svAcc01r, svld1(p0, C + simd_width * 2)));
		svst1(p0, C + simd_width * 3, svadd_m(p0, svAcc01i, svld1(p0, C + simd_width * 3)));
		C += row_stride_c;
		svst1(p0, C + simd_width * 0, svadd_m(p0, svAcc10r, svld1(p0, C + simd_width * 0)));
		svst1(p0, C + simd_width * 1, svadd_m(p0, svAcc10i, svld1(p0, C + simd_width * 1)));
		svst1(p0, C + simd_width * 2, svadd_m(p0, svAcc11r, svld1(p0, C + simd_width * 2)));
		svst1(p0, C + simd_width * 3, svadd_m(p0, svAcc11i, svld1(p0, C + simd_width * 3)));
	}
	else
	{

		svst1(p0, C + simd_width * 0, svAcc00r);
		svst1(p0, C + simd_width * 1, svAcc00i);
		svst1(p0, C + simd_width * 2, svAcc01r);
		svst1(p0, C + simd_width * 3, svAcc01i);
		C += row_stride_c;
		svst1(p0, C + simd_width * 0, svAcc10r);
		svst1(p0, C + simd_width * 1, svAcc10i);
		svst1(p0, C + simd_width * 2, svAcc11r);
		svst1(p0, C + simd_width * 3, svAcc11i);
	}
}

void nnp_cVLgemm_conjb_upto_2x2__sve(
	uint32_t mr, uint32_t nr,
	size_t k, size_t update,
	const float A[restrict static 1],
	const float B[restrict static 1],
	float C[restrict static 1],
	size_t row_stride_c)
{

	svfloat32_t a0r, a1r, b0r, b1r;
	svfloat32_t a0i, a1i, b0i, b1i;

	svfloat32_t svAcc00r = svdup_f32(0.0f);
	svfloat32_t svAcc01r = svdup_f32(0.0f);
	svfloat32_t svAcc10r = svdup_f32(0.0f);
	svfloat32_t svAcc11r = svdup_f32(0.0f);

	svfloat32_t svAcc00i = svdup_f32(0.0f);
	svfloat32_t svAcc01i = svdup_f32(0.0f);
	svfloat32_t svAcc10i = svdup_f32(0.0f);
	svfloat32_t svAcc11i = svdup_f32(0.0f);

	const size_t simd_width = nnp_hwinfo.simd_width; //4;						  // svcntw(); //todo fix
	const svbool_t p0 = svwhilelt_b32_s32(0, simd_width); // svptrue_b32();

	int lenA = mr * 2; // 2 for complex
	int lenB = nr * 2; 

	for (int i = 0; i < k; i++)
	{

		a0r = svld1(p0, A + i * simd_width * lenA + simd_width * 0);
		a0i = svld1(p0, A + i * simd_width * lenA + simd_width * 1);

		b0r = svld1(p0, B + i * simd_width * lenB + simd_width * 0);
		b0i = svld1(p0, B + i * simd_width * lenB + simd_width * 1);

		svAcc00r = svmla_m(p0, svAcc00r, a0r, b0r);
		svAcc00r = svmla_m(p0, svAcc00r, a0i, b0i);
		svAcc00i = svmla_m(p0, svAcc00i, a0i, b0r);
		svAcc00i = svmls_m(p0, svAcc00i, a0r, b0i);

		if (nr > 1)
		{

			b1r = svld1(p0, B + i * simd_width * lenB + simd_width * 2);
			b1i = svld1(p0, B + i * simd_width * lenB + simd_width * 3);

			svAcc01r = svmla_m(p0, svAcc01r, a0r, b1r);
			svAcc01r = svmla_m(p0, svAcc01r, a0i, b1i);
			svAcc01i = svmla_m(p0, svAcc01i, a0i, b1r);
			svAcc01i = svmls_m(p0, svAcc01i, a0r, b1i);
		}

		if (mr > 1)
		{

			a1r = svld1(p0, A + i * simd_width * lenA + simd_width * 2);
			a1i = svld1(p0, A + i * simd_width * lenA + simd_width * 3);

			svAcc10r = svmla_m(p0, svAcc10r, a1r, b0r);
			svAcc10r = svmla_m(p0, svAcc10r, a1i, b0i);
			svAcc10i = svmla_m(p0, svAcc10i, a1i, b0r);
			svAcc10i = svmls_m(p0, svAcc10i, a1r, b0i);

			if (nr > 1)
			{

				svAcc11r = svmla_m(p0, svAcc11r, a1r, b1r);
				svAcc11r = svmla_m(p0, svAcc11r, a1i, b1i);
				svAcc11i = svmla_m(p0, svAcc11i, a1i, b1r);
				svAcc11i = svmls_m(p0, svAcc11i, a1r, b1i);
			}
		}
	}

	if (update != 0)
	{

		svst1(p0, C + simd_width * 0, svadd_m(p0, svAcc00r, svld1(p0, C + simd_width * 0)));
		svst1(p0, C + simd_width * 1, svadd_m(p0, svAcc00i, svld1(p0, C + simd_width * 1)));
		if (nr > 1)
		{
			svst1(p0, C + simd_width * 2, svadd_m(p0, svAcc01r, svld1(p0, C + simd_width * 2)));
			svst1(p0, C + simd_width * 3, svadd_m(p0, svAcc01i, svld1(p0, C + simd_width * 3)));
		}
		if (mr > 1)
		{
			C += row_stride_c;
			svst1(p0, C + simd_width * 0, svadd_m(p0, svAcc10r, svld1(p0, C + simd_width * 0)));
			svst1(p0, C + simd_width * 1, svadd_m(p0, svAcc10i, svld1(p0, C + simd_width * 1)));
			if (nr > 1)
			{
				svst1(p0, C + simd_width * 2, svadd_m(p0, svAcc11r, svld1(p0, C + simd_width * 2)));
				svst1(p0, C + simd_width * 3, svadd_m(p0, svAcc11i, svld1(p0, C + simd_width * 3)));
			}
		}
	}else{

		svst1(p0, C + simd_width * 0, svAcc00r);
		svst1(p0, C + simd_width * 1, svAcc00i);
		if (nr > 1)
		{
			svst1(p0, C + simd_width * 2, svAcc01r);
			svst1(p0, C + simd_width * 3, svAcc01i);
		}
		if (mr > 1)
		{
			C += row_stride_c;
			svst1(p0, C + simd_width * 0, svAcc10r);
			svst1(p0, C + simd_width * 1, svAcc10i);
			if (nr > 1)
			{
				svst1(p0, C + simd_width * 2, svAcc11r);
				svst1(p0, C + simd_width * 3, svAcc11i);
			}
		}
	}
}


//----------------------------------------------------------------------

void nnp_cgemm_conjb_only_2x2__scalar(
	size_t k, size_t update,
	const float A[restrict static 1],
	const float B[restrict static 1],
	float C[restrict static 1],
	size_t row_stride_c)
{
	nnp_cVLgemm_conjb_only_2x2__sve(k, update, A, B, C, row_stride_c);
}

void nnp_cgemm_conjb_upto_2x2__scalar(
	uint32_t mr, uint32_t nr,
	size_t k, size_t update,
	const float A[restrict static 1],
	const float B[restrict static 1],
	float c[restrict static 1],
	size_t row_stride_c)
{
	nnp_cVLgemm_conjb_upto_2x2__sve(mr, nr, k, update, A, B, c, row_stride_c);
}