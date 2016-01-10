#include "Simplified1DCA.hpp"

Simplified1DCA::Simplified1DCA(unsigned int rows, unsigned int cols)
{
	this->rows = rows;
	this->cols = cols;

	// R: Rule Count
	// K: Number of States
	// N: Size of Neigborhood
	// R = K^(K^N)

	/*
	// R = 2:  2^(2^0)
	rules[0][0] = true;
	rules[1][0] = false;
	*/

	// R = 4:  2^(2^1)
	for (unsigned int rc = 0; rc < 4; rc++) {
		unsigned char r = (unsigned char)rc;

		for (unsigned int jc = 0; jc <= 1; jc++) {
			unsigned char j = (unsigned char)jc;
			unsigned char shifted = r >> j;
			rules[rc][1 - jc] = shifted & 1;
		}
	}

	/*
	// R = 16:  2^(2^2)
	for (unsigned int rc = 0; rc < 16; rc++) {
		unsigned char r = (unsigned char)rc;

		for (unsigned int jc = 0; jc <= 3; jc++) {
			unsigned char j = (unsigned char)jc;
			unsigned char shifted = r >> j;
			rules[rc][3 - jc] = shifted & 1;
		}
	}
	*/

	/*
	// R = 256:  2^(2^3)
	for (unsigned int rc = 0; rc < 256; rc++) {
		unsigned char r = (unsigned char)rc;

		for (unsigned int jc = 0; jc <= 7; jc++) {
			unsigned char j = (unsigned char)jc;
			unsigned char shifted = r >> j;
			rules[rc][7 - jc] = shifted & 1;
		}
	}
	*/

	initialized = false;
	resetCellStates();
	initialized = true;
}

void Simplified1DCA::resetCellStates()
{
	iteration = 1;
	cleanUp();

	// initialize cell states
	cellStates = new bool*[rows];
	for (unsigned int i = 0; i < rows; i++) {
		cellStates[i] = new bool[cols];

		for (unsigned int j = 0; j < cols; j++)
			cellStates[i][j] = false;
	}

	// one cell on initial condition
	cellStates[0][cols / 2] = true;
}

void Simplified1DCA::setActiveRuleNumber(unsigned int ruleNumber)
{
	activeRuleNumber = ruleNumber;
}

unsigned int Simplified1DCA::getActiveRuleNumber()
{
	return activeRuleNumber;
}

bool Simplified1DCA::iterate()
{
	bool halt = false;
	bool* activeRule = rules[activeRuleNumber];
	unsigned int prevIteration = iteration - 1;
	bool* prevIterationRow = cellStates[prevIteration];

	for (unsigned int i = 0; i < cols; i++)
	{
		/*
		// R = 2
		if ((activeRule[0])) {
			cellStates[iteration][i] = true;
		}
		*/

		// R = 4
		bool pir_n0;

		pir_n0 = prevIterationRow[i];

		if (
			(pir_n0  && activeRule[0])
		 || (!pir_n0 && activeRule[1])
		) {
			cellStates[iteration][i] = true;
		}

		/*
		// R = 16
		bool pir_n0;
		bool pir_n1;

		// wrap-around array
		pir_n0 = prevIterationRow[i];
		pir_n1 = (i == cols - 1 ? prevIterationRow[0] : prevIterationRow[i + 1]);

		if (((pir_n0 &&  pir_n1) && activeRule[0])
			|| ((pir_n0 &&  !pir_n1) && activeRule[1])
			|| ((!pir_n0 && pir_n1) && activeRule[2])
			|| ((!pir_n0 && !pir_n1) && activeRule[3])
		) {
			cellStates[iteration][i] = true;
		}
		*/

		/* R = 256
		bool pir_n0;
		bool pir_n1;
		bool pir_n2;

		// wrap-around array
		pir_n0 = (i == 0 ? prevIterationRow[cols - 1] : prevIterationRow[i - 1]);
		pir_n1 = prevIterationRow[i];
		pir_n2 = (i == cols - 1 ? prevIterationRow[0] : prevIterationRow[i + 1]);

		if (((pir_n0 &&  pir_n1 &&  pir_n2) && activeRule[0])
			|| ((pir_n0 &&  pir_n1 && !pir_n2) && activeRule[1])
			|| ((pir_n0 && !pir_n1 &&  pir_n2) && activeRule[2])
			|| ((pir_n0 && !pir_n1 && !pir_n2) && activeRule[3])
			|| ((!pir_n0 &&  pir_n1 &&  pir_n2) && activeRule[4])
			|| ((!pir_n0 &&  pir_n1 && !pir_n2) && activeRule[5])
			|| ((!pir_n0 && !pir_n1 &&  pir_n2) && activeRule[6])
			|| ((!pir_n0 && !pir_n1 && !pir_n2) && activeRule[7])
			) {

			cellStates[iteration][i] = true;

		}
		*/

	}

	if (iteration >= rows - 1)
		halt = true;

	iteration++;

	return halt;
}

bool** Simplified1DCA::getCellStates()
{
	return cellStates;
}

void Simplified1DCA::cleanUp()
{
	if (initialized) {
		for (unsigned int i = 0; i < rows; i++)
			delete[] cellStates[i];
		delete[] cellStates;
	}
}

Simplified1DCA::~Simplified1DCA()
{
	cleanUp();
}
