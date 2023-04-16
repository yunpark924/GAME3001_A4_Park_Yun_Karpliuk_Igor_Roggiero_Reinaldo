#include "DetectCondition.h"

DetectCondition::DetectCondition(const bool within_radius)
{
	SetIsWithinRadius(within_radius);
	m_name = "Detect Condition";
}

DetectCondition::~DetectCondition()
= default;

void DetectCondition::SetIsWithinRadius(const bool state)
{
	m_isWithinRadius = state;
}

bool DetectCondition::Condition()
{
	return m_isWithinRadius;
}
