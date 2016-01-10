#ifndef WOLFRAM1DCA_HPP
#define WOLFRAM1DCA_HPP

class Wolfram1DCA
{
public:
	Wolfram1DCA(unsigned int rows, unsigned int cols);
	void resetCellStates();
	unsigned int getActiveRuleNumber();
	void setActiveRuleNumber(unsigned int ruleNumber);
	bool iterate();
	bool** getCellStates();
	~Wolfram1DCA();

private:
	bool initialized;
	bool rules[256][8];
	bool** cellStates;
	unsigned int rows;
	unsigned int cols;
	unsigned int iteration = 1;
	unsigned int activeRuleNumber = 0;
	void cleanUp();
};

#endif
