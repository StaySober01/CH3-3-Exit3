#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Exit3Character.generated.h"

class UInputAction;
class UInputMappingContext;
class UCameraComponent;
struct FInputActionValue;

UCLASS()
class EXIT3_API AExit3Character : public ACharacter
{
	GENERATED_BODY()

public:
	AExit3Character();

	UFUNCTION(BlueprintPure, Category = "Exit3|Camera")
	UCameraComponent* GetFirstPersonCamera() const { return FirstPersonCamera; }

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void StartSprint();
	void StopSprint();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Exit3|Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	UPROPERTY(EditDefaultsOnly, Category = "Exit3|Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Exit3|Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Exit3|Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Exit3|Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Exit3|Input")
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditDefaultsOnly, Category = "Exit3|Movement")
	float WalkSpeed = 400.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Exit3|Movement")
	float SprintSpeed = 700.0f;
};
