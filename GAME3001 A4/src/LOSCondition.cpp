#include "LOSCondition.h"

LOSCondition::LOSCondition(const bool LOS)
{
	SetLOS(LOS);
	m_name = "LOS Condition";
}

LOSCondition::~LOSCondition()
= default;

void LOSCondition::SetLOS(const bool state)
{
	m_hasLOS = state;
}

bool LOSCondition::Condition()
{
	return m_hasLOS;
}
