#include "SpartaPawn.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "SpartaPlayerController.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

ASpartaPawn::ASpartaPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	RootComponent = CapsuleComponent;
	CapsuleComponent->InitCapsuleSize(40.0f, 90.0f);
	CapsuleComponent->SetSimulatePhysics(false);

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(RootComponent);
	SkeletalMeshComponent->SetSimulatePhysics(false);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 400.0f;
	SpringArmComponent->bUsePawnControlRotation = false;

	SpringArmComponent->bInheritPitch = true;
	SpringArmComponent->bInheritYaw = true;
	SpringArmComponent->bInheritRoll = false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;

	MoveSpeed = 600.0f;
	RotationSpeed = 150.0f;
	SprintSpeedMultiplier = 2.4f;
	SprintSpeed = MoveSpeed * SprintSpeedMultiplier;

	Gravity = -200.0f;
	AirControlMultiplier = 0.5f;
	bIsGrounded = false;
	ZVelocity = 0.0f;
	UpwardInput = 0.0f;

	bIsSprinting = false;

	bIsRolling = false;
	AccumulatedRollAngle = 0.0f;
	RollDirection = 1.0f;
	BarrelRollSpeed = 1080.0f;
}

void ASpartaPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (ASpartaPlayerController* PlayerController = Cast<ASpartaPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaPawn::Move
				);
			}

			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaPawn::StartJump
				);

				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Completed,
					this,
					&ASpartaPawn::StopJump
				);
			}

			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaPawn::StartSprint
				);

				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Completed,
					this,
					&ASpartaPawn::StopSprint
				);
			}

			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaPawn::Look
				);
			}
		}
	}
}

void ASpartaPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation + FVector(0.0f, 0.0f, -42.0f);

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	bIsGrounded = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, CollisionParams);

	if (UpwardInput > 0.0f)
	{
		ZVelocity = 0.1f;
	}
	else if (bIsGrounded)
	{
		ZVelocity = 0.0f;
	}
	else
	{
		ZVelocity += Gravity * DeltaTime;
		AddActorWorldOffset(FVector(0.0f, 0.0f, ZVelocity * DeltaTime), true);
	}

	if (UpwardInput != 0.0f)
	{
		float CurrentSpeed = bIsGrounded ? MoveSpeed : MoveSpeed * AirControlMultiplier;

		if (bIsSprinting)
		{
			CurrentSpeed *= SprintSpeedMultiplier;
		}

		FVector UpOffset = FVector(0.0f, 0.0f, UpwardInput) * CurrentSpeed * DeltaTime;
		AddActorLocalOffset(UpOffset, true);
	}

	if (bIsRolling)
	{
		float AngleStep = BarrelRollSpeed * DeltaTime;
		AccumulatedRollAngle += AngleStep;

		if (AccumulatedRollAngle >= 360.0f)
		{
			AngleStep -= (AccumulatedRollAngle - 360.0f);
			bIsRolling = false;
			AccumulatedRollAngle = 0.0f;
		}

		FRotator DeltaRotation = FRotator(0.0f, AngleStep * RollDirection, 0.0f);
		SkeletalMeshComponent->AddLocalRotation(DeltaRotation);
	}

}

void ASpartaPawn::Move(const FInputActionValue& value)
{
	if (!Controller) return;

	const FVector2D MoveInput = value.Get<FVector2D>();
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	float CurrentSpeed = bIsGrounded ? MoveSpeed : MoveSpeed * AirControlMultiplier;

	if (bIsSprinting)
	{
		CurrentSpeed *= SprintSpeedMultiplier;
	}

	FVector DeltaLocation = FVector(MoveInput.X, MoveInput.Y, 0.0f) * CurrentSpeed * DeltaTime;
	AddActorLocalOffset(DeltaLocation, true);
}

void ASpartaPawn::StartJump(const FInputActionValue& value)
{
	UpwardInput = 1.0f;
}

void ASpartaPawn::StopJump(const FInputActionValue& value)
{
	UpwardInput = 0.0f;
}

void ASpartaPawn::StartSprint(const FInputActionValue& value)
{
	bIsSprinting = true;
}

void ASpartaPawn::StopSprint(const FInputActionValue& value)
{
	bIsSprinting = false;
}

void ASpartaPawn::Look(const FInputActionValue& value)
{
	if (!Controller) return;

	const FVector2D LookInput = value.Get<FVector2D>();
	float DeltaTime = GetWorld()->GetDeltaSeconds();
	
	float PitchInput = LookInput.Y * RotationSpeed * DeltaTime * -1.0f;
	float YawInput = LookInput.X * RotationSpeed * DeltaTime;

	FRotator DeltaRotation = FRotator(PitchInput, YawInput, 0.0f);
	AddActorLocalRotation(DeltaRotation);
}

void ASpartaPawn::Roll(const FInputActionValue& value)
{
	if (!Controller || bIsRolling) return;

	float RollInput = value.Get<float>();

	if (FMath::IsNearlyZero(RollInput)) return;

	bIsRolling = true;
	AccumulatedRollAngle = 0.0f;
	RollDirection = FMath::Sign(RollInput);
}


//void ASpartaPawn::Look(const FInputActionValue& value)
//{
//	if (!Controller) return;
//
//	const FVector2D LookInput = value.Get<FVector2D>();
//	float DeltaTime = GetWorld()->GetDeltaSeconds();
//
//	float YawInput = LookInput.X * RotationSpeed * DeltaTime;
//	AddActorLocalRotation(FRotator(0.0f, YawInput, 0.0f));
//
//	if (SpringArmComponent)
//	{
//		float PitchInput = LookInput.Y * RotationSpeed * DeltaTime * -1.0f;
//		FRotator CurrentArmRotation = SpringArmComponent->GetRelativeRotation();
//		float NewPitch = FMath::Clamp(CurrentArmRotation.Pitch + PitchInput, -80.0f, 80.0f);
//
//		SpringArmComponent->SetRelativeRotation(FRotator(NewPitch, CurrentArmRotation.Yaw, CurrentArmRotation.Roll));
//	}
//}

