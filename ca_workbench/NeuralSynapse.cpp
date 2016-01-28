#include "NeuralSynapse.hpp"

NeuralSynapse::NeuralSynapse() : SiteConnection() {
}

NeuralSynapse::NeuralSynapse(
	unsigned int sourceSiteId,
	unsigned int destinationSiteId,
	const std::vector<float>& color,
	float connectionStrengthWeight,
	bool shouldRender
) : SiteConnection(sourceSiteId, destinationSiteId, color, shouldRender)
{
	this->connectionStrengthWeight = connectionStrengthWeight;
}
