#pragma once
#include <string>

// TODO : structures may have resources that they produce, we need to include this!

class Structure {
private:
		std::string textureId;
		// TODO : add more members here, such as production data, resources etc.

public:
	virtual void init();
	virtual void update(float dt) = 0;
	virtual std::string getTextureId();
};
