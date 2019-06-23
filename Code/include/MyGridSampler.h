#pragma once
#include<vector>
#include <zxing/common/Counted.h>
#include <zxing/common/BitMatrix.h>
#include <zxing/common/PerspectiveTransform.h>
namespace zxing {
	class MyGridSampler{
	public:
		MyGridSampler() {};
		std::vector<std::vector<int>>mySampleGrid(Ref<BitMatrix> image, int dimension, Ref<PerspectiveTransform> transform);
		static void checkAndNudgePoints(Ref<BitMatrix> image, std::vector<float> &points);
	};
}