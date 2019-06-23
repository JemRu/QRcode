#include "QRImageInfo.h"
#include <zxing/common/reedsolomon/ReedSolomonDecoder.h>
#include <zxing/ChecksumException.h>
#include <zxing/common/reedsolomon/ReedSolomonException.h>

info::QRImageInfo::QRImageInfo(cv::Mat & image)
{
	using namespace zxing;
	using zxing::Ref;
	using zxing::ArrayRef;
	using zxing::BitMatrix;
	ReedSolomonDecoder rsDecoder(GenericGF::QR_CODE_FIELD_256);
	Ref<LuminanceSource> source = MatSource::create(image);
	int width = source->getWidth();
	int height = source->getHeight();
	//fprintf(stderr, "image width: %d, height: %d\n", width, height);

	Ref<Binarizer> binarizer(new GlobalHistogramBinarizer(source));
	Ref<zxing::BinaryBitmap> bitmap(new BinaryBitmap(binarizer));
	qrcode::MyDetector detector(bitmap->getBlackMatrix());
	auto imageInfo=detector.processFinderPatternInfo(DecodeHints(DecodeHints::QR_CODE_HINT));
	moduleSize = detector.getModuleSize();
	//initialize pixels
	pixels = imageInfo.first;

	Ref<BitMatrix> bits=imageInfo.second;


	qrcode::BitMatrixParser parser(bits);
	

	//initialize version,error collection level,mask;

	auto ver = parser.readVersion();
	auto ecLevel = parser.readFormatInformation()->getErrorCorrectionLevel();
	auto mask = parser.readFormatInformation()->getDataMask();

	version = new qrgen::Version(ver->getVersionNumber());
	level = qrgen::LEVEL(ecLevel.ordinal());

	//initialize modules
	qrgen::Version *version = new qrgen::Version(ver->getVersionNumber());
	qrgen::Plan *plan = qrgen::Plan::newPlan(version, level, new qrgen::Mask(static_cast<int>(mask&0xFF)));
	modules = plan->getPixels();

	//initialize codewords
	qrgen::VerInfo verinfo = qrgen::Version::VERSION_INFOS[version->getVersion()];
	codewords.resize(verinfo.bytes, qrgen::CodeWord());

	//initialize blocks
	int blockNumber = verinfo.lvlInfos[level].num_of_block;
	int checkByteNumber = verinfo.lvlInfos[level].cbytes_pre_block;
	blocks.resize(blockNumber, static_cast<int>(checkByteNumber/2));


	// check result
	ArrayRef<char> cwords(parser.readCodewords());
	// Separate into data blocks
	std::vector<Ref<qrcode::DataBlock> > dataBlocks(qrcode::DataBlock::getDataBlocks(cwords, ver, ecLevel));
	std::vector<std::vector<bool>> rawDataBlocks(dataBlocks.size());
	std::vector<std::vector<bool>> rawCheckBlocks(dataBlocks.size());

	// Count total number of data bytes
	int totalBytes = 0;
	for (size_t i = 0; i < dataBlocks.size(); i++) {
		totalBytes += dataBlocks[i]->getNumDataCodewords();
	}
	ArrayRef<char> resultBytes(totalBytes);
	int resultOffset = 0;
	
	auto correctErrors = [&](ArrayRef<char> &codewordBytes, int numDataCodewords) {
		int numCodewords = codewordBytes->size();
		ArrayRef<int> codewordInts(numCodewords);
		for (int i = 0; i < numCodewords; i++) {
			codewordInts[i] = codewordBytes[i] & 0xff;
		}
		int numECCodewords = numCodewords - numDataCodewords;

		try {
			rsDecoder.decode(codewordInts, numECCodewords);
		}
		catch (ReedSolomonException const& ignored) {
			(void)ignored;
			throw ChecksumException();
		}

		for (int i = 0; i < numCodewords; i++) {
			codewordBytes[i] = (char)codewordInts[i];
		}
	};
	int dataBlockMax = 0, checkBlockMax = 0;
	// Error-correct and copy data blocks together into a stream of bytes
	for (size_t j = 0; j < dataBlocks.size(); j++) {

		Ref<qrcode::DataBlock> dataBlock(dataBlocks[j]);

		ArrayRef<char> codewordBytes = dataBlock->getCodewords();
		std::vector<char> rawCodeWordBytes(codewordBytes->size());
		for (int i = 0; i < codewordBytes->size(); i++) rawCodeWordBytes[i] = codewordBytes[i];

		int numDataCodewords = dataBlock->getNumDataCodewords();
		correctErrors(codewordBytes, numDataCodewords);
		dataBlockMax = dataBlockMax > numDataCodewords ? dataBlockMax : numDataCodewords;
		checkBlockMax = checkBlockMax > (codewordBytes->size() - numDataCodewords) ? 
			checkBlockMax : (codewordBytes->size() - numDataCodewords);
		//check blocks
		for (int i = 0; i < codewordBytes->size(); i++) {
			if (i < numDataCodewords) {
				if (codewordBytes[i] != rawCodeWordBytes[i]) {
					rawDataBlocks[j].push_back(true);
					blocks[j]--;
				}	
				else
					rawDataBlocks[j].push_back(false);
			}
			else {
				if (codewordBytes[i] != rawCodeWordBytes[i]) {
					rawCheckBlocks[j].push_back(true);
					blocks[j]--;
				}	
				else
					rawCheckBlocks[j].push_back(false);
			}
		}
	}

	//check codewords
	int bitsIndex = 0;
	for (int i = 0; i < dataBlockMax; i++) {
		for (int j = 0; j < rawDataBlocks.size(); j++) {
			if (i < rawDataBlocks[j].size()) {
				if (rawDataBlocks[j][i]) codewords[bitsIndex].setTrue();
				bitsIndex++;
			}
		}
	}

	for (int i = 0; i < checkBlockMax; i++) {
		for (int j = 0; j < rawCheckBlocks.size(); j++) {
			if (i < rawCheckBlocks[j].size()) {
				if (rawCheckBlocks[j][i]) codewords[bitsIndex].setTrue();
				bitsIndex++;
			}	
		}
	}

}
