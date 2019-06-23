#include "MyDetector.h"

#include <zxing/qrcode/detector/FinderPatternFinder.h>
#include <zxing/qrcode/detector/FinderPattern.h>
#include <zxing/qrcode/detector/AlignmentPattern.h>
#include <zxing/qrcode/detector/AlignmentPatternFinder.h>
#include <zxing/qrcode/Version.h>
#include <zxing/common/GridSampler.h>
#include <zxing/DecodeHints.h>
#include <zxing/common/detector/MathUtils.h>
#include <sstream>
#include <cstdlib>
#include <algorithm>  // vs12, std::min und std:max
#include<math.h>
#include "MyGridSampler.h"





std::pair<std::vector<std::vector<int>>, zxing::Ref<zxing::BitMatrix>> zxing::qrcode::MyDetector::processFinderPatternInfo(DecodeHints const& hints)
{
	zxing::qrcode::FinderPatternFinder finder(image, hints.getResultPointCallback());

	Ref<FinderPatternInfo> info(finder.find(hints));

	Ref<FinderPattern> topLeft(info->getTopLeft());
	Ref<FinderPattern> topRight(info->getTopRight());
	Ref<FinderPattern> bottomLeft(info->getBottomLeft());

	moduleSize = calculateModuleSize(topLeft, topRight, bottomLeft);//每个module大小
	if (moduleSize < 1.0f) {
		throw zxing::ReaderException("bad module size");
	}
	int dimension = computeDimension(topLeft, topRight, bottomLeft, moduleSize);//行列module数量
	
	Version *provisionalVersion = Version::getProvisionalVersionForDimension(dimension);
	int modulesBetweenFPCenters = provisionalVersion->getDimensionForVersion() - 7;

	Ref<AlignmentPattern> alignmentPattern;
	// Anything above version 1 has an alignment pattern
	if (provisionalVersion->getAlignmentPatternCenters().size() > 0) {


		// Guess where a "bottom right" finder pattern would have been
		float bottomRightX = topRight->getX() - topLeft->getX() + bottomLeft->getX();
		float bottomRightY = topRight->getY() - topLeft->getY() + bottomLeft->getY();


		// Estimate that alignment pattern is closer by 3 modules
		// from "bottom right" to known top left location
		float correctionToTopLeft = 1.0f - 3.0f / (float)modulesBetweenFPCenters;
		int estAlignmentX = (int)(topLeft->getX() + correctionToTopLeft * (bottomRightX - topLeft->getX()));
		int estAlignmentY = (int)(topLeft->getY() + correctionToTopLeft * (bottomRightY - topLeft->getY()));


		// Kind of arbitrary -- expand search radius before giving up
		for (int i = 4; i <= 16; i <<= 1) {
			try {
				alignmentPattern = findAlignmentInRegion(moduleSize, estAlignmentX, estAlignmentY, (float)i);
				break;
			}
			catch (zxing::ReaderException const& re) {
				(void)re;
				// try next round
			}
		}
		if (alignmentPattern == 0) {
			// Try anyway
		}

	}

	Ref<PerspectiveTransform> transform = createTransform(topLeft, topRight, bottomLeft, alignmentPattern, dimension);
	Ref<BitMatrix> bits(sampleGrid(image, dimension, transform));
	MyGridSampler* sampler=new MyGridSampler();
	std::vector<std::vector<int>>moduleCenters =sampler->mySampleGrid(image, dimension, transform);
	return make_pair(moduleCenters,bits);
}
