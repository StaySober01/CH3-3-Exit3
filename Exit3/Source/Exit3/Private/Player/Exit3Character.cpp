#include "Player/Exit3Character.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"

AExit3Character::AExit3Character()
{
	PrimaryActorTick.bCanEverTick = false;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	FirstPersonCamera->SetRelativeLocation(FVector(-10.0f, 0.0f, 64.0f));
	FirstPersonCamera->bUsePawnControlRotation = true;

	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AExit3Character::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	if (!DefaultMappingContext)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s has no Default Mapping Context. Assign IMC_Player on the character Blueprint."), *GetName());
		return;
	}

	if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s has no player controller during BeginPlay; input mapping was not added."), *GetName());
	}
}

void AExit3Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)
		{
			EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AExit3Character::Move);
		}
		if (LookAction)
		{
			EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AExit3Character::Look);
		}
		if (JumpAction)
		{
			EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
			EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		}
		if (SprintAction)
		{
			EnhancedInput->BindAction(SprintAction, ETriggerEvent::Started, this, &AExit3Character::StartSprint);
			EnhancedInput->BindAction(SprintAction, ETriggerEvent::Completed, this, &AExit3Character::StopSprint);
		}

		if (!MoveAction || !LookAction || !JumpAction || !SprintAction)
		{
			UE_LOG(LogTemp, Warning,
				TEXT("%s is missing one or more Input Actions. Assign Move, Look, Jump, and Sprint on the character Blueprint."),
				*GetName());
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s requires an Enhanced Input Component."), *GetName());
	}
}

void AExit3Character::Move(const FInputActionValue& Value)
{
	const FVector2D Movement = Value.Get<FVector2D>();
	AddMovementInput(GetActorForwardVector(), Movement.Y);
	AddMovementInput(GetActorRightVector(), Movement.X);
}

void AExit3Character::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxis = Value.Get<FVector2D>();
	AddControllerYawInput(LookAxis.X);
	AddControllerPitchInput(LookAxis.Y);
}

void AExit3Character::StartSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AExit3Character::StopSprint()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}
