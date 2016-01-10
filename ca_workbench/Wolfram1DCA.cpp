#include "Wolfram1DCA.hpp"

Wolfram1DCA::Wolfram1DCA(unsigned int rows, unsigned int cols)
{
	this->rows = rows;
	this->cols = cols;

	// Wolfram elementary 1D CA
	for (unsigned int rc = 0; rc < 256; rc++) {
		unsigned char r = (unsigned char)rc;

		for (unsigned int jc = 0; jc <= 7; jc++) {
			unsigned char j = (unsigned char)jc;
			unsigned char shifted = r >> j;
			rules[rc][7 - jc] = shifted & 1;
		}
	}

	initialized = false;
	resetCellStates();
	initialized = true;
}

void Wolfram1DCA::resetCellStates()
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

void Wolfram1DCA::setActiveRuleNumber(unsigned int ruleNumber)
{
	activeRuleNumber = ruleNumber;
}

unsigned int Wolfram1DCA::getActiveRuleNumber()
{
	return activeRuleNumber;
}

bool Wolfram1DCA::iterate()
{
	bool halt = false;
	bool* activeRule = rules[activeRuleNumber];
	unsigned int prevIteration = iteration - 1;
	bool* prevIterationRow = cellStates[prevIteration];

	for (unsigned int i = 0; i < cols; i++)
	{
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
	}

	if (iteration >= rows - 1)
		halt = true;

	iteration++;

	return halt;
}

bool** Wolfram1DCA::getCellStates()
{
	return cellStates;
}

void Wolfram1DCA::cleanUp()
{
	if (initialized) {
		for (unsigned int i = 0; i < rows; i++)
			delete[] cellStates[i];
		delete[] cellStates;
	}
}

Wolfram1DCA::~Wolfram1DCA()
{
	cleanUp();
}
