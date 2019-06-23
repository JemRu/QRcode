#pragma once
#include <vector>
#include<map>
#include<utility>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <zxing/LuminanceSource.h>
#include <zxing/common/Counted.h>
#include <zxing/Reader.h>
#include <zxing/aztec/AztecReader.h>
#include <zxing/common/GlobalHistogramBinarizer.h>
#include <zxing/DecodeHints.h>
#include <zxing/datamatrix/DataMatrixReader.h>
#include <zxing/MultiFormatReader.h>
#include <zxing/qrcode/decoder/BitMatrixParser.h>
#include <zxing/qrcode/ErrorCorrectionLevel.h>
#include <zxing/qrcode/Version.h>
#include <zxing/qrcode/decoder/DataBlock.h>
#include <zxing/qrcode/decoder/DecodedBitStreamParser.h>
#include <zxing/ReaderException.h>
#include <zxing/ChecksumException.h>
#include <zxing/common/reedsolomon/ReedSolomonException.h>
#include "MatSource.h"

#include "Version.h"
#include "Level.h"
#include "Mask.h"
#include "Pixel.h"
#include "CodeWord.h"
#include "Plan.h"


#include "MyDetector.h"
namespace info {
	class QRImageInfo {

	private:
		
		std::vector<std::vector<qrgen::Pixel>> modules;
		std::vector<qrgen::CodeWord> codewords;
		std::vector<int> blocks;
		std::vector<std::vector<int>>pixels; //如果是model的中心点存 modelindex 如果不是 -1

		void invertCodeWord(int y, int x) { codewords[modules[y][x].getOffset() / 8].setTrue(); }

		

		bool getCodeWordStatus(int y, int x) { return codewords[modules[y][x].getOffset() / 8].getStatus(); }
	public:
		int moduleSize;
		qrgen::Version *version;
		qrgen::LEVEL level;

		QRImageInfo() {};
		QRImageInfo(cv::Mat &image);

		~QRImageInfo(){
			modules.swap(std::vector<std::vector<qrgen::Pixel>>{});
			codewords.swap(std::vector<qrgen::CodeWord>{});
			blocks.swap(std::vector<int>{});
			pixels.swap(std::vector < std::vector<int>>{});
		}

		qrgen::Pixel::PixelRole getModuleRole(int y, int x) { return modules[y][x].getPixelRole();}
		
		int getBlockIndex(int y, int x) { return modules[y][x].getBlockIndex(); }

		int getBlock(int i) { return blocks[i]; }
		int getBlockSize() { return blocks.size(); }
		int getModuleSize() { return modules.size(); }
		std::pair<int,int> getPixel(int py, int px) {
			if (pixels[py][px] != -1)
				return std::make_pair(pixels[py][px] >> 16, pixels[py][px] & 0xFFFF);
			else
				return std::make_pair(-1, -1);
		}

		std::vector<std::tuple<bool,int,int>> tryModule(const std::vector<std::vector<int>> &pos){
			auto cwd = codewords;
			auto blk = blocks;
			std::vector<std::tuple<bool, int, int>> results(pos.size());
			for (int i = 0; i < pos.size(); i++) {
				int y = pos[i][0];
				int x = pos[i][1];
				if (cwd[modules[y][x].getOffset() / 8].getStatus()) {
					results.emplace_back(false, getBlockIndex(y, x), blk[getBlockIndex(y, x)]);
				}
				else {
					cwd[modules[y][x].getOffset() / 8].setTrue();
					blk[getBlockIndex(y, x)]--;
					results.emplace_back(true, getBlockIndex(y, x), blk[getBlockIndex(y, x)]);
				}
			}
			return results;
		}

		void invertModule(const std::vector<std::vector<int>> &pos) {
			for (int i = 0; i < pos.size(); i++) {
				int y = pos[i][0];
				int x = pos[i][1];
				modules[y][x].setPixel(qrgen::Pixel::INVERT.getPixel());
				if (!getCodeWordStatus(y, x)) {
					invertCodeWord(y, x);
					blocks[getBlockIndex(y, x)]--;
				}
			}	
		}
		



	};
}