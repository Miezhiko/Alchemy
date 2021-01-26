#pragma once

#include "llmath.h"
#include "llrect.h"
#include "llvector4a.h"
#include "llmatrix4a.h"

namespace ALGLMath
{
	inline LLMatrix4a genRot(const float a, const LLVector4a& axis)
	{
		F32 r = a * DEG_TO_RAD;

		F32 c = cosf(r);
		F32 s = sinf(r);

		F32 ic = 1.f - c;

		const LLVector4a add1(c, axis[VZ] * s, -axis[VY] * s);	//1,z,-y
		const LLVector4a add2(-axis[VZ] * s, c, axis[VX] * s);	//-z,1,x
		const LLVector4a add3(axis[VY] * s, -axis[VX] * s, c);	//y,-x,1

		LLVector4a axis_x;
		axis_x.splat<0>(axis);
		LLVector4a axis_y;
		axis_y.splat<1>(axis);
		LLVector4a axis_z;
		axis_z.splat<2>(axis);

		LLVector4a c_axis;
		c_axis.setMul(axis, ic);

		LLMatrix4a rot_mat;
		rot_mat.getRow<0>().setMul(c_axis, axis_x);
		rot_mat.getRow<0>().add(add1);
		rot_mat.getRow<1>().setMul(c_axis, axis_y);
		rot_mat.getRow<1>().add(add2);
		rot_mat.getRow<2>().setMul(c_axis, axis_z);
		rot_mat.getRow<2>().add(add3);
		rot_mat.setRow<3>(LLVector4a(0, 0, 0, 1));

		return rot_mat;
	}

	inline LLMatrix4a genRot(const float a, const float x, const float y, const float z) { return genRot(a, LLVector4a(x, y, z)); }

	inline bool projectf(const LLVector3& object, const LLMatrix4a& modelview, const LLMatrix4a& projection, const LLRect& viewport, LLVector3& windowCoordinate)
	{
		//Begin SSE intrinsics

		// Declare locals
		const LLVector4a obj_vector(object.mV[VX], object.mV[VY], object.mV[VZ]);
		const LLVector4a one(1.f);
		LLVector4a temp_vec;								//Scratch vector
		LLVector4a w;										//Splatted W-component.

		modelview.affineTransform(obj_vector, temp_vec);	//temp_vec = modelview * obj_vector;

		//Passing temp_matrix as v and res is safe. res not altered until after all other calculations
		projection.rotate4(temp_vec, temp_vec);				//temp_vec = projection * temp_vec

		w.splat<3>(temp_vec);								//w = temp_vec.wwww

		//If w == 0.f, use 1.f instead.
		LLVector4a div;
		div.setSelectWithMask(w.equal(_mm_setzero_ps()), one, w);	//float div = (w[N] == 0.f ? 1.f : w[N]);
		temp_vec.div(div);									//temp_vec /= div;

		//Map x, y to range 0-1
		temp_vec.mul(.5f);
		temp_vec.add(.5f);

		LLVector4Logical mask = temp_vec.equal(_mm_setzero_ps());
		if (mask.areAllSet(LLVector4Logical::MASK_W))
			return false;

		//End SSE intrinsics

		//Window coordinates
		windowCoordinate[0] = temp_vec[VX] * viewport.getWidth() + viewport.mLeft;
		windowCoordinate[1] = temp_vec[VY] * viewport.getHeight() + viewport.mBottom;
		//This is only correct when glDepthRange(0.0, 1.0)
		windowCoordinate[2] = temp_vec[VZ];

		return true;
	}

	inline bool unprojectf(const LLVector3& windowCoordinate, const LLMatrix4a& modelview, const LLMatrix4a& projection, const LLRect& viewport, LLVector3& object)
	{
		//Begin SSE intrinsics

		// Declare locals
		static const LLVector4a one(1.f);
		static const LLVector4a two(2.f);
		LLVector4a norm_view(
			((windowCoordinate.mV[VX] - (F32)viewport.mLeft) / (F32)viewport.getWidth()),
			((windowCoordinate.mV[VY] - (F32)viewport.mBottom) / (F32)viewport.getHeight()),
			windowCoordinate.mV[VZ],
			1.f);

		LLMatrix4a inv_mat;								//Inverse transformation matrix
		LLVector4a temp_vec;							//Scratch vector
		LLVector4a w;									//Splatted W-component.

		inv_mat.setMul(projection, modelview);			//inv_mat = projection*modelview

		float det = inv_mat.invert();

		//Normalize. -1.0 : +1.0
		norm_view.mul(two);								// norm_view *= vec4(.2f)
		norm_view.sub(one);								// norm_view -= vec4(1.f)

		inv_mat.rotate4(norm_view, temp_vec);			//inv_mat * norm_view

		w.splat<3>(temp_vec);							//w = temp_vec.wwww

		//If w == 0.f, use 1.f instead. Defer return if temp_vec.w == 0.f until after all SSE intrinsics.
		LLVector4a div;
		div.setSelectWithMask(w.equal(_mm_setzero_ps()), one, w);	//float div = (w[N] == 0.f ? 1.f : w[N]);
		temp_vec.div(div);								//temp_vec /= div;

		LLVector4Logical mask = temp_vec.equal(_mm_setzero_ps());
		if (mask.areAllSet(LLVector4Logical::MASK_W))
			return false;

		//End SSE intrinsics

		if (det == 0.f)
			return false;

		object.set(temp_vec.getF32ptr());

		return true;
	}
}