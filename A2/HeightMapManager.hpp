#ifndef HEIGHTMAPMANAGER_H
#define HEIGHTMAPMANAGER_H

class HeightMapManager {

public:
	HeightMapManager();
	~HeightMapManager();

	void setSkipSize(int& skipSize) { m_skipSize = skipSize; };

private:
	int m_skipSize;
	float m_stepSize;
};

#endif