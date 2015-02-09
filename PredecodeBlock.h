// This file contains code from NVSim, (c) 2012-2013,  Pennsylvania State University 
//and Hewlett-Packard Company. See LICENSE_NVSim file in the top-level directory.
//No part of DESTINY Project, including this file, may be copied,
//modified, propagated, or distributed except according to the terms
//contained in the LICENSE file.


#ifndef PREDECODEBLOCK_H_
#define PREDECODEBLOCK_H_

#include "FunctionUnit.h"
#include "RowDecoder.h"
#include "BasicDecoder.h"

class PredecodeBlock: public FunctionUnit {
public:
	PredecodeBlock();
	virtual ~PredecodeBlock();

	/* Functions */
	void PrintProperty();
	void Initialize(int _numAddressBit, double _capLoad, double _resLoad);
	void CalculateArea();
	void CalculateRC();
	void CalculateLatency(double _rampInput);
	void CalculatePower();
	PredecodeBlock & operator=(const PredecodeBlock &);

	/* Properties */
	bool initialized;	/* Initialization flag */
	RowDecoder * rowDecoderStage1A;
	RowDecoder * rowDecoderStage1B;
	RowDecoder * rowDecoderStage1C;
	int numNandInputStage1A, numNandInputStage1B, numNandInputStage1C;
	int numAddressBitStage1A, numAddressBitStage1B, numAddressBitStage1C;
	RowDecoder * rowDecoderStage2;
	BasicDecoder * basicDecoderA1;
	BasicDecoder * basicDecoderA2;
	BasicDecoder * basicDecoderB;
	BasicDecoder * basicDecoderC;
	double capLoad;		/* Load capacitance Unit: F */
	double resLoad;     /* Load resistance Unit: ohm */
	int numAddressBit;   /* Number of Address Bits assigned to the block */
	int numOutputAddressBit;
	int numDecoder12;          /* Number of 1 to 2 Decoders */
	int numDecoder24;          /* Number of 2 to 4 Decoders */
	int numDecoder38;          /* Number of 3 to 8 Decoders */
	int numBasicDecoderA1, numBasicDecoderA2;
	double capLoadBasicDecoderA1, capLoadBasicDecoderA2, capLoadBasicDecoderB, capLoadBasicDecoderC;
	double rampInput, rampOutput;
	/* TO-DO: Predecoder so far does not take OptPriority input because the output driver is already quite fixed in this module */
};

#endif /* PREDECODEBLOCK_H_ */
