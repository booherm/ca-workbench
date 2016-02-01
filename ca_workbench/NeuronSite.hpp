#ifndef NEURONSITE_HPP
#define NEURONSITE_HPP

#include "Site.hpp"
#include "NeuralSynapse.hpp"

class NeuronSite : public Site
{
public:
	bool freshActivation;
	float firingThreshold;
	float firingRate;
	unsigned int firingRateCalcActivations;
	std::vector<NeuralSynapse*> neuralSynapses;
};

#endif
