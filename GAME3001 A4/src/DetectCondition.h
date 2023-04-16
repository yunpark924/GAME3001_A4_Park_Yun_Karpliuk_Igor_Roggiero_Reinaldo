#pragma once
#ifndef __DETECT_CONDITION__
#define __DETECT_CONDITION__
#include "ConditionNode.h"

class DetectCondition : public ConditionNode
{
public:
	DetectCondition(bool within_radius = false);
	virtual ~DetectCondition();

	// Getters and Setters
	void SetIsWithinRadius(bool state);

	virtual bool Condition() override;
private:
	bool m_isWithinRadius;
};

#endif /* defined (__DETECT_CONDITION__) */