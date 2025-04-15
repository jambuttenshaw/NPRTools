#pragma once


#include "Subsystems/WorldSubsystem.h"

#include "NPRToolsWorldSubsystem.generated.h"


UCLASS()
class NPRTOOLS_API UNPRToolsWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	TSharedPtr<class FNPRToolsViewExtension, ESPMode::ThreadSafe> NPRToolsViewExtension;
};
