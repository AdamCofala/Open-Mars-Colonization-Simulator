#pragma once
#include <string>

class Structure {
	private:
		std::string textureId;

	public:
	virtual void init();
	virtual void update(float dt) = 0;
	virtual std::string getTextureId();
};
