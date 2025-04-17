#pragma once


#include "NPRToolsParameters.h"

#include "Subsystems/WorldSubsystem.h"

#include "NPRToolsWorldSubsystem.generated.h"


UCLASS()
class NPRTOOLS_API UNPRToolsWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// Transfers the latest subsystem state to the render thread
	void TransferState();

private:
	// Create a new proxy from the current parameters to be copied into the render thread state
	FNPRToolsParametersProxyPtr CreateProxyFromAsset(const UNPRToolsParametersDataAsset* ParamsAsset) const;

private:
	TSharedPtr<class FNPRToolsViewExtension, ESPMode::ThreadSafe> ViewExtension;

	// Render thread copy of the parameter state
	// Only access from render thread!
	FNPRToolsParametersProxyPtr ParametersProxy;

public:
	friend class FNPRToolsViewExtension;
};
