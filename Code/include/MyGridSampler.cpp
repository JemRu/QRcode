#include "MyGridSampler.h"
#include <zxing/ReaderException.h>
#include <iostream>
#include <sstream>
std::vector<std::vector<int>> zxing::MyGridSampler::mySampleGrid(Ref<BitMatrix> image, int dimension, Ref<PerspectiveTransform> transform)
{
	std::vector<std::vector<int>> moduleCenters(image->getHeight());
	for (int y = 0; y < image->getHeight(); y++) moduleCenters[y].resize(image->getWidth(), -1);

	std::vector<float> points(dimension << 1, (const float)0.0f);
	for (int y = 0; y < dimension; y++) {
		int max = points.size();
		float yValue = (float)y + 0.5f;
		for (int x = 0; x < max; x += 2) {
			points[x] = (float)(x >> 1) + 0.5f;
			points[x + 1] = yValue;
		}
		transform->transformPoints(points);
		checkAndNudgePoints(image, points);
		for (int x = 0; x < max; x += 2) {	
			moduleCenters[points[x+1]][points[x]] = (y << 16) + (x >> 1);
		}
	}

	return moduleCenters;
}

void zxing::MyGridSampler::checkAndNudgePoints(Ref<BitMatrix> image, std::vector<float>& points)
{
	int width = image->getWidth();
	int height = image->getHeight();


	// The Java code assumes that if the start and end points are in bounds, the rest will also be.
	// However, in some unusual cases points in the middle may also be out of bounds.
	// Since we can't rely on an ArrayIndexOutOfBoundsException like Java, we check every point.

	for (size_t offset = 0; offset < points.size(); offset += 2) {
		int x = (int)points[offset];
		int y = (int)points[offset + 1];
		if (x < -1 || x > width || y < -1 || y > height) {
			std::ostringstream s;
			s << "Transformed point out of bounds at " << x << "," << y;
			throw zxing::ReaderException(s.str().c_str());
		}

		if (x == -1) {
			points[offset] = 0.0f;
		}
		else if (x == width) {
			points[offset] = float(width - 1);
		}
		if (y == -1) {
			points[offset + 1] = 0.0f;
		}
		else if (y == height) {
			points[offset + 1] = float(height - 1);
		}
	}

}
