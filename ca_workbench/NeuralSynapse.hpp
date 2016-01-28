#ifndef NEURALSYNAPSE_HPP
#define NEURALSYNAPSE_HPP

#include <vector>
#include "SiteConnection.hpp"

class NeuralSynapse : public SiteConnection
{
public:
	NeuralSynapse();
	NeuralSynapse(
		unsigned int sourceSiteId,
		unsigned int destinationSiteId,
		const std::vector<float>& color,
		float connectionStrengthWeight,
		bool shouldRender
	);

	float connectionStrengthWeight;
};

#endif
