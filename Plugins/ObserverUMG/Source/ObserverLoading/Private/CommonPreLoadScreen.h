#pragma once
#include "PreLoadScreenBase.h"

class SWidget;

class FCommonPreLoadScreen : public FPreLoadScreenBase
{
public:
	
	virtual void Init() override;
    virtual EPreLoadScreenTypes GetPreLoadScreenType() const override { return EPreLoadScreenTypes::EngineLoadingScreen; }
    virtual TSharedPtr<SWidget> GetWidget() override { return EngineLoadingWidget; }
	
private:

    TSharedPtr<SWidget> EngineLoadingWidget;
};
