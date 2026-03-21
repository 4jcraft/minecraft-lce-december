#include "Build/stdafx.h"

#include "DummyCriteria.h"

DummyCriteria::DummyCriteria(const std::wstring &name)
{
	this->name = name;
	ObjectiveCriteria::CRITERIA_BY_NAME[name] = this;
}

std::wstring DummyCriteria::getName()
{
	return name;
}

int DummyCriteria::getScoreModifier(vector<shared_ptr<Player> > *players)
{
	return 0;
}

bool DummyCriteria::isReadOnly()
{
	return false;
}