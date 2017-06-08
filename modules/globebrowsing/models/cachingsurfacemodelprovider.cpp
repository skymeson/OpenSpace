/*****************************************************************************************
*                                                                                       *
* OpenSpace                                                                             *
*                                                                                       *
* Copyright (c) 2014-2017                                                               *
*                                                                                       *
* Permission is hereby granted, free of charge, to any person obtaining a copy of this  *
* software and associated documentation files (the "Software"), to deal in the Software *
* without restriction, including without limitation the rights to use, copy, modify,    *
* merge, publish, distribute, sublicense, and/or sell copies of the Software, and to    *
* permit persons to whom the Software is furnished to do so, subject to the following   *
* conditions:                                                                           *
*                                                                                       *
* The above copyright notice and this permission notice shall be included in all copies *
* or substantial portions of the Software.                                              *
*                                                                                       *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,   *
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A         *
* PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT    *
* HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF  *
* CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE  *
* OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                         *
****************************************************************************************/

#include <modules/globebrowsing/models/cachingsurfacemodelprovider.h>
#include <modules/globebrowsing/other/threadpool.h>
#include <ghoul/logging/logmanager.h>
#include <ghoul/io/texture/texturereader.h>

namespace {
	const std::string _loggerCat = "CachingSurfaceModelProvider";
}

namespace openspace {
namespace globebrowsing {

CachingSurfaceModelProvider::CachingSurfaceModelProvider(Renderable* parent)
	: _parent(parent)
{
	double cacheSize = 20;
	_asyncSurfaceModelProvider = std::make_shared<AsyncSurfaceModelProvider>(parent);
	_modelCache = std::make_shared<ModelCache>(static_cast<size_t>(cacheSize));
}

std::vector<std::shared_ptr<SubsiteModels>> CachingSurfaceModelProvider::getModels(const std::vector<std::shared_ptr<Subsite>> Subsites,
	const int level) {

	// Check if the chunk is already in the cache. If it is in the cache, loop through the corresponding vector
	// and look if the model is already loaded. If the model is already loaded, return the vector. If the model is
	// not already loaded, enqueue the model.
	std::vector<std::shared_ptr<SubsiteModels>> vectorOfSubsiteModels;

	// Temporary solution to not load LOD 0 models
	if(level > 0) {
		for (auto subsite : Subsites) {			
			bool requestedExistsInCache = true;
			ProviderSubsiteKey key = { level, subsite->site, subsite->drive };
			if (_modelCache->exist(key)) {
				vectorOfSubsiteModels.push_back(_modelCache->get(key));
			}
			else {
				for(const int& tempLevel : subsite->availableLevels){
					// Only enqueue the model if the LOD exists
					if(tempLevel == level)
						_asyncSurfaceModelProvider->enqueueModelIO(subsite, level);
				}
			
				// Check for available LODs above the requested.
				std::vector<int> levelsAbove = getLevelsAbove(subsite->availableLevels, level);
				for (int levelAbove = levelsAbove.size() + 1; levelAbove-- > 1; ) {
					ProviderSubsiteKey keyLowerLevel = { levelAbove, subsite->site, subsite->drive };
					// If the cache holds the correct model but with lower resultion than requested,
					// return highest available resolution.
					if (_modelCache->exist(keyLowerLevel)) {
						vectorOfSubsiteModels.push_back(_modelCache->get(keyLowerLevel));
						// Breaks the loop that goes through levelsAbove
						break;
					}
					// If the cache doesn't hold the LOD above, enqueue it. The LOD above is much smaller
					// in filesize and will be loaded more quickly. This is done to have something to render
					// when waiting for the rquested level to load.
					else {
						for(const int& availableLevel : subsite->availableLevels) {
							if(availableLevel == levelAbove) {
								_asyncSurfaceModelProvider->enqueueModelIO(subsite, levelAbove);
								// Breaks the loop that goes trough available levels for the subsite
								break;
							}
						}
						// Breaks the loop that goes through levelsAbove
						break;
					}
				}
			}
		}
	}

	return vectorOfSubsiteModels;
}

void CachingSurfaceModelProvider::update(Renderable* parent) {
	initModelsFromLoadedData(parent);
}

void CachingSurfaceModelProvider::setLevel(const int level) {
	//if (level != _previousLevel && level >= 2) {
		//clearQueuesAndJobs();
	//}
	_previousLevel = level;
}

void CachingSurfaceModelProvider::clearQueuesAndJobs() {
	_asyncSurfaceModelProvider->clearQueuesAndJobs();
}

std::vector<int> CachingSurfaceModelProvider::getLevelsAbove(const std::vector<int> availableLevels, const int requestedLevel) {
	std::vector<int> levelsAbove;
	for (int i = 0; i < requestedLevel - 1; i++) {
		if (i >= availableLevels.size()){
			return levelsAbove;
		}
		levelsAbove.push_back(availableLevels.at(i));
	}
	return levelsAbove;
}

void CachingSurfaceModelProvider::initModelsFromLoadedData(Renderable* parent) {
	std::vector<std::shared_ptr<SubsiteModels>> vectorOfSubsiteModels =
		_asyncSurfaceModelProvider->getLoadedModels();

	//TODO: Remove this loop when the asynctex uploading is implemented
	for (auto subsiteModels : vectorOfSubsiteModels) {
		std::vector<std::shared_ptr<Model>> theModels = subsiteModels->models;
		subsiteModels->models = theModels;
		
		//uint64_t key = CachingSurfaceModelProvider::hashKey(subsiteModels->site, subsiteModels->drive, subsiteModels->level);
		ProviderSubsiteKey key = { subsiteModels->level, subsiteModels->site, subsiteModels->drive };

		_modelCache->put(key, subsiteModels);
	}
}

uint64_t CachingSurfaceModelProvider::hashKey(const std::string site, const std::string drive, const int level) {
	uint64_t key = 0LL;

	int siteNumber = std::stoi(site);
	int driveNumber = std::stoi(drive);

	key |= level;
	key |= siteNumber << 5;
	key |= ((uint64_t)driveNumber) << 35;
	
	return key;
}

} // namespace globebrowsing
} // namespace openspace